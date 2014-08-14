/*
  Copyright 2013-2014 bcstack.org

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "bluetooth.h"

#define RFCOMM_SEND_UA_FLAG      0x01
#define RFCOMM_SEND_PNR_FLAG     0x02
#define RFCOMM_SEND_MSCC_FLAG    0x04
#define RFCOMM_SEND_MSCR_FLAG    0x08
#define RFCOMM_SEND_USER_FLAG    0x10

#if DEBUG_RFCOMM
#define rfcomm_printf printf
#else
#define rfcomm_printf(...)
#endif

static struct {
    u8 output_mask;
    u8 initiator;
    u8 new_credit;
    u8 mycredits;

    struct {
        u8 flags;
    } channels[CFG_RFCOMM_NUM_CHANNELS];

    struct {
        u8 dlci;
    } mcc;
} rfcomm;

#define RFCOMM_MARK_OUTPUT(ch, flag)            \
    do {                                        \
        rfcomm.channels[(ch)].flags |= (flag);  \
        rfcomm.output_mask |= (1 << (ch));      \
    } while(0)

#define RFCOMM_CLEAR_OUTPUT(ch, flag)           \
    do {                                        \
        rfcomm.channels[(ch)].flags &= ~(flag); \
        if (!rfcomm.channels[(ch)].flags)       \
            rfcomm.output_mask &= ~(1 << (ch)); \
    } while(0)

/*
 *  bit0 : End of Address (always 1)
 *  bit1 : Command/Response
 *  bit2 : Direction (we are server on non initiating device, so always 0)
 *  bit3-7: Server Channel
 */
#define RFCOMM_ADDRESS(cr, ch)                  \
    (0x1 | ((cr) << 1) | ((ch << 3)))

/*
 * bit0: EA (1 for short length)
 * bit1-7: length
 */
#define RFCOMM_LENGTH_SHORT(L)                  \
    (1 | ((L) << 1))

static void rfcomm_input_sabm(u8* input, u16 isize);
static void rfcomm_input_ua(u8* input, u16 isize);
static void rfcomm_input_disc(u8* input, u16 isize);
static void rfcomm_input_dm(u8* input, u16 isize);
static void rfcomm_input_uih(u8* input, u16 isize);
static void mcc_input(u8 ch, u8* input, u16 isize);
static void mcc_input_pn(u8 ch, u8 cr, u8* input, u16 isize);
static void mcc_input_msc(u8 ch, u8 cr, u8* input, u16 isize);
static void user_input(u8 ch, u8* input, u16 isize);

static void rfcomm_output_ua(u8 ch, u8* output, u16* osize);
static void rfcomm_output_pn(u8 ch, u8* output, u16* osize);
static void rfcomm_output_msc(u8 ch, u8 cr, u8* output, u16* osize);
static void rfcomm_output_user(u8 ch, u8* output, u16* osize);

/* reversed, 8-bit, poly=0x07 */
static u8 rfcomm_crc_table[256] = {
	0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4, 0x75,
	0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b,
	0x1c, 0x8d, 0xff, 0x6e, 0x1b, 0x8a, 0xf8, 0x69,
	0x12, 0x83, 0xf1, 0x60, 0x15, 0x84, 0xf6, 0x67,

	0x38, 0xa9, 0xdb, 0x4a, 0x3f, 0xae, 0xdc, 0x4d,
	0x36, 0xa7, 0xd5, 0x44, 0x31, 0xa0, 0xd2, 0x43,
	0x24, 0xb5, 0xc7, 0x56, 0x23, 0xb2, 0xc0, 0x51,
	0x2a, 0xbb, 0xc9, 0x58, 0x2d, 0xbc, 0xce, 0x5f,

	0x70, 0xe1, 0x93, 0x02, 0x77, 0xe6, 0x94, 0x05,
	0x7e, 0xef, 0x9d, 0x0c, 0x79, 0xe8, 0x9a, 0x0b,
	0x6c, 0xfd, 0x8f, 0x1e, 0x6b, 0xfa, 0x88, 0x19,
	0x62, 0xf3, 0x81, 0x10, 0x65, 0xf4, 0x86, 0x17,

	0x48, 0xd9, 0xab, 0x3a, 0x4f, 0xde, 0xac, 0x3d,
	0x46, 0xd7, 0xa5, 0x34, 0x41, 0xd0, 0xa2, 0x33,
	0x54, 0xc5, 0xb7, 0x26, 0x53, 0xc2, 0xb0, 0x21,
	0x5a, 0xcb, 0xb9, 0x28, 0x5d, 0xcc, 0xbe, 0x2f,

	0xe0, 0x71, 0x03, 0x92, 0xe7, 0x76, 0x04, 0x95,
	0xee, 0x7f, 0x0d, 0x9c, 0xe9, 0x78, 0x0a, 0x9b,
	0xfc, 0x6d, 0x1f, 0x8e, 0xfb, 0x6a, 0x18, 0x89,
	0xf2, 0x63, 0x11, 0x80, 0xf5, 0x64, 0x16, 0x87,

	0xd8, 0x49, 0x3b, 0xaa, 0xdf, 0x4e, 0x3c, 0xad,
	0xd6, 0x47, 0x35, 0xa4, 0xd1, 0x40, 0x32, 0xa3,
	0xc4, 0x55, 0x27, 0xb6, 0xc3, 0x52, 0x20, 0xb1,
	0xca, 0x5b, 0x29, 0xb8, 0xcd, 0x5c, 0x2e, 0xbf,

	0x90, 0x01, 0x73, 0xe2, 0x97, 0x06, 0x74, 0xe5,
	0x9e, 0x0f, 0x7d, 0xec, 0x99, 0x08, 0x7a, 0xeb,
	0x8c, 0x1d, 0x6f, 0xfe, 0x8b, 0x1a, 0x68, 0xf9,
	0x82, 0x13, 0x61, 0xf0, 0x85, 0x14, 0x66, 0xf7,

	0xa8, 0x39, 0x4b, 0xda, 0xaf, 0x3e, 0x4c, 0xdd,
	0xa6, 0x37, 0x45, 0xd4, 0xa1, 0x30, 0x42, 0xd3,
	0xb4, 0x25, 0x57, 0xc6, 0xb3, 0x22, 0x50, 0xc1,
	0xba, 0x2b, 0x59, 0xc8, 0xbd, 0x2c, 0x5e, 0xcf
};

#define CRC8_INIT  0xFF

void rfcomm_setup( void )
{
    memset(&rfcomm, 0, sizeof(rfcomm));
}

static u8 rfcomm_crc(u8 *data, u16 len)
{
    u16 i;
    u8 crc = CRC8_INIT;

    for (i = 0; i < len; i++)
        crc = rfcomm_crc_table[crc ^ data[i]];
    return crc;
}

#define rfcomm_fcs(_data, _len)                 \
    (0xFF - rfcomm_crc(_data, _len))

void rfcomm_input(u8* input, u16 isize)
{
    u8 ch = input[0] >> 3;
    u8 control = input[1];

    rfcomm_printf("rfcomm-in ch=%d, ctrl=%x\n", ch, control);

    // we have only channel 0 and 1, so we ignore other channels
    if (ch > 1) return;

    // dispatch to handlers based on control field
    switch (control & 0xEF) {
    case RFCOMM_SABM: rfcomm_input_sabm(input, isize); break;
    case RFCOMM_UA:   rfcomm_input_ua(input, isize);   break;
    case RFCOMM_DISC: rfcomm_input_disc(input, isize); break;
    case RFCOMM_DM:   rfcomm_input_dm(input, isize);   break;
    case RFCOMM_UIH:  rfcomm_input_uih(input, isize);  break;
    }

    // request for a transmission if we have anything to tx
    if (rfcomm.output_mask) {
        sys_set_event(L2CAP_TASK_ID, L2CAP_SEND_RFCOMM_EVENT_ID);
    }
}

/*
 * SABM: Set Asynchronous Balanced Mode
 *
 * The SABM command shall be used to place the addressed station in the
 * Asynchronous Balanced Mode (ABM) where all control fields shall be one
 * octet in length. The station shall confirm acceptance of the SABM command
 * by transmission of a UA response at the first opportunity. Upon acceptance
 * of this command, the DLC send and receive state variables shall be set to
 * zero.
 */
static void rfcomm_input_sabm(u8* input, u16 isize)
{
    u8 ch = input[0] >> 3;

    // clear all state

    // request to send UA
    RFCOMM_MARK_OUTPUT(ch, RFCOMM_SEND_UA_FLAG);
}

static void rfcomm_input_ua(u8* input, u16 isize)
{
}

/* The DISC command shall be used to terminate an operational or
 * initialization mode previously set by a command.
 * It shall be used to inform one station that the other station is
 * suspending operation and that the station should assume a logically
 * disconnected mode. Prior to actioning the command, the receiving station
 * shall confirm the acceptance of the DISC command by the transmission
 * of a UA response.
 *
 * DISC command sent at DLCI 0 have the same meaning as the Multiplexer
 * Close Down command (see subclause 5.4.6.3.3).
 * See also subclause 5.8.2 for more information about the Close-down procedure.
*/
static void rfcomm_input_disc(u8* input, u16 isize)
{
    u8 ch = input[0] >> 3;

    // set state to disconnected

    RFCOMM_MARK_OUTPUT(ch, RFCOMM_SEND_UA_FLAG);
}

/*
 * The DM response shall be used to report a status where the station is
 * logically disconnected from the data link. When in disconnected mode no
 * commands are accepted until the disconnected mode is terminated by the
 * receipt of a SABM command. If a DISC command is received while in
 * disconnected mode a DM response should be sent.
 */
static void rfcomm_input_dm(u8* input, u16 isize)
{
}

/*
 * The UIH command/response shall be used to send information without
 * affecting the V(S) or V(R) variables at either station. UIH is used
 * where the integrity of the information being transferred is of lesser
 * importance than its delivery to the correct DLCI. For the UIH frame,
 * the FCS shall be calculated over only the address and control fields.
 * Reception of the UIH command/response is not sequence number verified
 * by the data link procedures; therefore, the UIH frame may be lost if a
 * data link exception occurs during transmission of the protected portion
 * of the command, or duplicated if an exception condition occurs during any
 * reply to the command. There is no specified response to the UIH
 * command/response.
 */
static void rfcomm_input_uih(u8* input, u16 isize)
{
    u8  ch = input[0] >> 3;
    u8  control = input[1];
    u16 length;
    u8* info = input + 3;
    u16 infolen = isize - 4;

    length = input[2] >> 1;

    if (0 == (input[2] & 1)) {
        length |= ((u16)input[3]) << 7;
        info++;
        infolen--;
    }

    if (ch == 0) {
        // multiplexer control
        mcc_input(ch, info, infolen);
    } else {
        if (control & RFCOMM_PF) {
            rfcomm.mycredits += info[0];
            info++;
            infolen--;
        }

        user_input(ch, info, infolen);
    }
}

static void mcc_input(u8 ch, u8* input, u16 isize)
{
    u8 type = input[0] >> 2;
    u16 length = input[1] >> 1;
    u8* param = input + 2;
    u8 cr = (input[0] >> 1) & 1;

    switch (type) {
    case RFCOMM_PN:
        mcc_input_pn(ch, cr, param, length);
        break;
    case RFCOMM_MSC:
        mcc_input_msc(ch, cr, param, length);
        break;
    }
}

static void mcc_input_pn(u8 ch, u8 cr, u8* input, u16 isize)
{
    rfcomm.mcc.dlci = input[0];

    rfcomm.mycredits = input[7] & 0xf;

    RFCOMM_MARK_OUTPUT(ch, RFCOMM_SEND_PNR_FLAG);
}

static void mcc_input_msc(u8 ch, u8 cr, u8* input, u16 isize)
{
    rfcomm_printf("MSC\n");

    rfcomm.mcc.dlci = input[0];

    if (cr) {
        RFCOMM_MARK_OUTPUT(ch, RFCOMM_SEND_MSCC_FLAG);
        RFCOMM_MARK_OUTPUT(ch, RFCOMM_SEND_MSCR_FLAG);
    }
}

static void user_input(u8 ch, u8* input, u16 isize)
{
    rfcomm_printf("USER\n");

    rfcomm.new_credit = 1;

    spp_recv(input, isize);

    RFCOMM_MARK_OUTPUT(ch, RFCOMM_SEND_USER_FLAG);
}

void rfcomm_output(u8* output, u16* osize)
{
    u8 ch;

    if (rfcomm.output_mask & 1) ch = 0;
    else ch = 1;

    if (rfcomm.channels[ch].flags & RFCOMM_SEND_UA_FLAG) {
        RFCOMM_CLEAR_OUTPUT(ch, RFCOMM_SEND_UA_FLAG);
        rfcomm_output_ua(ch, output, osize);
    } else if (rfcomm.channels[ch].flags & RFCOMM_SEND_PNR_FLAG) {
        RFCOMM_CLEAR_OUTPUT(ch, RFCOMM_SEND_PNR_FLAG);
        rfcomm_output_pn(ch, output, osize);
    } else if (rfcomm.channels[ch].flags & RFCOMM_SEND_MSCC_FLAG) {
        RFCOMM_CLEAR_OUTPUT(ch, RFCOMM_SEND_MSCC_FLAG);
        rfcomm_output_msc(ch, 1, output, osize);
    } else if (rfcomm.channels[ch].flags & RFCOMM_SEND_MSCR_FLAG) {
        RFCOMM_CLEAR_OUTPUT(ch, RFCOMM_SEND_MSCR_FLAG);
        rfcomm_output_msc(ch, 0, output, osize);
    } else if (rfcomm.channels[ch].flags & RFCOMM_SEND_USER_FLAG) {
        RFCOMM_CLEAR_OUTPUT(ch, RFCOMM_SEND_USER_FLAG);
        rfcomm_output_user(ch, output, osize);
    }

    if (rfcomm.output_mask) {
        sys_set_event(L2CAP_TASK_ID, L2CAP_SEND_RFCOMM_EVENT_ID);
    }
}

/*
  The UA response shall be used by the station to acknowledge the receipt and
  acceptance of SABM and DISC commands.
*/
static void rfcomm_output_ua(u8 ch, u8* output, u16* osize)
{
    // Address
    // this is response to initiating device, so CR = 1
    output[0] = RFCOMM_ADDRESS(1, ch);

    // Control
	output[1] = RFCOMM_UA | RFCOMM_PF;

    // Length
	output[2] = RFCOMM_LENGTH_SHORT(0);

    // FCS
	output[3] = rfcomm_fcs(output, 3);

    *osize = 4;
}

static void rfcomm_output_pn(u8 ch, u8* output, u16* osize)
{
    // Address
    output[0] = RFCOMM_ADDRESS(1, 0);

    // Control
	output[1] = RFCOMM_UIH;

    // Length = 10 bytes
	output[2] = RFCOMM_LENGTH_SHORT(10);

    // PN RSP
    output[3] = 0x81;

    // length
    output[4] = 0x11; // 8 bytes

    // DLCI
    output[5] = rfcomm.mcc.dlci;

    // msb = 0xf (initial) or 0 (already open)
    // lsb = 0
    output[6] = 0xe0;

    // priority
    output[7] = 7;

    // ack timer
    output[8] = 0;

    // max frame size (64 bytes)
    output[9] = 64;
    output[10] = 0;

    // max number of retransmission
    output[11] = 0;

    // initial credit
    output[12] = 7;

    // FCS
	output[13] = rfcomm_fcs(output, 2);

    *osize = 14;
}

static void rfcomm_output_msc(u8 ch, u8 cr, u8* output, u16* osize)
{
    // Address
    output[0] = RFCOMM_ADDRESS(1, 0);

    // Control
	output[1] = RFCOMM_UIH;

    // Length
	output[2] = RFCOMM_LENGTH_SHORT(4);

    // Command
    // 1110, 00 CR EA
    output[3] = 0xe1 | (cr << 1);

    // Length
    output[4] = RFCOMM_LENGTH_SHORT(2);

    // DLCI
    output[5] = RFCOMM_ADDRESS(1, 1);

    // V24 Signals
    /*
      Bit 1. The EA bit is set to 1 in the last octet of the sequence; in other octets EA is set to 0. If only one octet is transmitted EA is set to 1.
      Bit 2.Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
      Bit 3. Ready To Communicate (RTC). The bit is set to 1 when the device is ready to communicate.
      Bit 4. Ready To Receive (RTR). The bit is set to 1 when the device is ready to receive data.
      Bit 5. Reserved for future use. Set to zero by the sender, ignored by the receiver.
      Bit 6. Reserved for future use. Set to zero by the sender, ignored by the receiver.
      Bit 7. Incoming call indicator (IC). The bit is set to 1 to indicate an incoming call. Bit
      8. Data Valid (DV). The bit is set to 1 to indicate that valid data is being sent.
     */
    output[6] = 0x8d; // 1000 1101

    // FCS
	output[7] = rfcomm_fcs(output, 2);

    *osize = 8;
}

static void rfcomm_output_user(u8 ch, u8* output, u16* osize)
{
    u16 len;

    // Address
    output[0] = RFCOMM_ADDRESS(1, ch);

    // Control
	output[1] = RFCOMM_UIH | RFCOMM_PF;

    // Credit
    output[3] = rfcomm.new_credit;
    rfcomm.new_credit = 0;
#if 1
    len = 0;
#else
    if (rfcomm.mycredits) {
        len = 0;
        rfcomm.mycredits--;
        printf("mycredits=%d\n", rfcomm.mycredits);
    } else {
        len = 0;
    }
#endif

    // Length
	output[2] = RFCOMM_LENGTH_SHORT(1 + len);

    // FCS
	output[4 + len] = rfcomm_fcs(output, 2);

    *osize = 5 + len;
}
