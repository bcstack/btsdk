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

static struct {
    const u8* curr_cmd;
    u8 acl_credits;

    struct {
        u16 hconn;
    } le;

#if !DISABLE_BT_CLASSICAL
    struct {
        u16 hconn;
        u8  bdaddr[6];
    } edr;
#endif

    struct {
        u8 visible : 1;
        u8 cmd_credit : 1;
        u8 cmd_buf_avail : 1;
    } state;
} gap;

extern const u8 radio_init_cmds[];

#if DEBUG_GAP
#define gap_printf printf
#define gap_dumphex sys_dumphex
#else
#define gap_printf(...)
#define gap_dumphex(...)
#endif

static void hci_cmd_cmplt( u8* data, u8 len );
static void hci_event_input(u8* input, u16 isize);
static void hci_acl_input(u8* input, u16 isize);

void gap_setup( void )
{
    memset(&gap, 0, sizeof(gap));
    sys_resume(HCI_TASK_ID);
    sys_resume(GAP_TASK_ID);
    sys_resume(L2CAP_TASK_ID);

    hci_receive(BT_EVENT_CHANNEL);
    hci_receive(BT_ACL_IN_CHANNEL);
}

void gap_reset(void)
{
    sys_set_event(GAP_TASK_ID, GAP_INIT_RADIO_EVENT_ID);
    gap.curr_cmd = radio_init_cmds;

#if !DISABLE_BLE
    sys_set_event(GAP_TASK_ID, GAP_SET_ADV_DATA_EVENT_ID);
#endif
#if !DISABLE_BT_CLASSICAL
    sys_set_event(GAP_TASK_ID, GAP_WRITE_LOCAL_NAME_EVENT_ID);
    sys_set_event(GAP_TASK_ID, GAP_WRITE_EIR_EVENT_ID);
#endif
}

void gap_set_visible(int v)
{
    gap.state.visible = v;

#if !DISABLE_BLE
    sys_set_event(GAP_TASK_ID, GAP_SET_ADV_EN_EVENT_ID);
#endif
#if !DISABLE_BT_CLASSICAL
    sys_set_event(GAP_TASK_ID, GAP_WRITE_SCAN_EN_EVENT_ID);
#endif
}

void hci_task_handler(u8 event_id)
{
    u8* buffer;
    u16 length;

    switch (event_id) {
    case HCI_COMMAND_SENT_EVENT_ID:
        gap.state.cmd_buf_avail = 1;

        if (gap.state.cmd_buf_avail && gap.state.cmd_credit) {
            sys_resume(GAP_TASK_ID);
        }
        break;
    case HCI_EVENT_RECEIVED_EVENT_ID:
        hci_get_buffer(BT_EVENT_CHANNEL, &buffer, &length);
        hci_event_input(buffer, length);
        break;
    case HCI_ACL_RECEIVED_EVENT_ID:
        hci_get_buffer(BT_ACL_IN_CHANNEL, &buffer, &length);
        hci_acl_input(buffer, length);
        break;
    case HCI_ACL_SENT_EVENT_ID:
        if (gap.acl_credits) {
            sys_resume(L2CAP_TASK_ID);
        }
        break;
    }
}

void gap_task_handler(u8 event_id)
{
    u8 send_cmd = 0;
    u8* buffer;
    u16 length;

    //gap_printf("gap event %d\n", event_id);

    hci_get_buffer(BT_COMMAND_CHANNEL, &buffer, &length);

    switch(event_id) {
    case GAP_INIT_RADIO_EVENT_ID:
    {
        u8 cmdlen;

        cmdlen = gap.curr_cmd[2] + 3;
        memcpy(buffer, gap.curr_cmd, cmdlen);

        gap.curr_cmd += cmdlen;
        send_cmd = 1;

        if ((gap.curr_cmd[0] == 0) &&
            (gap.curr_cmd[1] == 0)) {
            gap_printf("radio initialized\n");
        } else {
            sys_set_event(GAP_TASK_ID, GAP_INIT_RADIO_EVENT_ID);
        }
    }
    break;
    case GAP_SET_ADV_DATA_EVENT_ID:
        // opcode
        buffer[0] = 8;
        buffer[1] = 0x20;

        // parameter length
        buffer[2] = 32;

        memset(buffer + 3, 0, 32);
        buffer[3] = sizeof(CFG_DEVICE_NAME) + 3; // adv data len
        buffer[4] = sizeof(CFG_DEVICE_NAME) + 2; // adv element len
        buffer[5] = 9; // type: complete name
        memcpy(buffer + 6, CFG_DEVICE_NAME, sizeof(CFG_DEVICE_NAME));

        send_cmd = 1;
        break;
    case GAP_SET_ADV_EN_EVENT_ID:
        // opcode
        buffer[0] = 0xA;
        buffer[1] = 0x20;

        // parameter length
        buffer[2] = 0x1;

        if ((gap.le.hconn == 0) &&
            (gap.state.visible == 1)) {
            buffer[3] = 1;
            gap_printf("LE Advertising ON\n");
        } else {
            buffer[3] = 0;
            gap_printf("LE Advertising OFF\n");
        }

        send_cmd = 1;
        break;
#if !DISABLE_BT_CLASSICAL
    case GAP_WRITE_LOCAL_NAME_EVENT_ID:
        // opcode
        buffer[0] = 0x13;
        buffer[1] = 0xC;

        // parameter length
        buffer[2] = 248;

        strncpy((char*)buffer + 3, CFG_DEVICE_NAME, 248);

        send_cmd = 1;
        break;
    case GAP_WRITE_EIR_EVENT_ID:
        // opcode
        buffer[0] = 0x52;
        buffer[1] = 0xC;

        // parameter length
        buffer[2] = 241;

        buffer[3] = 1; // FEC required

        memset(buffer + 4, 0, 240);
        // EIR Element: complete local name
        buffer[4] = sizeof(CFG_DEVICE_NAME) + 1;
        buffer[5] = 9;
        memcpy(buffer + 6, CFG_DEVICE_NAME, sizeof(CFG_DEVICE_NAME));

        send_cmd = 1;
        break;
    case GAP_WRITE_SCAN_EN_EVENT_ID:
        // opcode
        buffer[0] = 0x1A;
        buffer[1] = 0xC;

        // parameter length
        buffer[2] = 1;

        if ((gap.edr.hconn == 0) &&
            (gap.state.visible == 1)) {
            buffer[3] = 3;
            gap_printf("PSCAN&ISCAN ON\n");
        } else {
            buffer[3] = 0;
            gap_printf("PSCAN&ISCAN OFF\n");
        }
        send_cmd = 1;

        break;
    case GAP_ACCEPT_CONN_EVENT_ID:
        // opcode
        buffer[0] = 9;
        buffer[1] = 4;

        // parameter length
        buffer[2] = 7;

        // BDADDR
        memcpy(buffer + 3, gap.edr.bdaddr, 6);
        buffer[9] = 1;

        send_cmd = 1;
        gap_printf("accept conn\n");
        break;
    case GAP_IO_CAP_REPLY_EVENT_ID:
        // opcode
        buffer[0] = 0x2B;
        buffer[1] = 4;

        // parameter length
        buffer[2] = 9;

        // BDADDR
        memcpy(buffer + 3, gap.edr.bdaddr, 6);
        buffer[9] = 3; // No IO
        buffer[10] = 0; // No OOB data
        buffer[11] = 0;

        send_cmd = 1;
        break;
    case GAP_USER_CFM_REPLY_EVENT_ID:
        // opcode
        buffer[0] = 0x2C;
        buffer[1] = 4;

        // parameter length
        buffer[2] = 6;

        // BDADDR
        memcpy(buffer + 3, gap.edr.bdaddr, 6);

        send_cmd = 1;
    }
#endif

    if (send_cmd) {
        u8 pktlen;

        pktlen = buffer[2] + 3;

        hci_send(BT_COMMAND_CHANNEL, buffer, pktlen);

        gap_dumphex("cmd", buffer, pktlen);

        gap.state.cmd_credit = 0;
        gap.state.cmd_buf_avail = 0;
        sys_pause(GAP_TASK_ID);
    }
}

static void hci_event_input(u8* input, u16 isize)
{
    u8 event_code;
    u8 param_len;
    u8 subcode;
    u8 *param;

    gap_dumphex("evt", input, isize);

    /*
      event format
      0: event code
      1: parameter total length
      *: parameters
      */
    
    event_code = input[0];
    param_len = input[1];
    param = input + 2;

    switch (event_code) {
#if !DISABLE_BT_CLASSICAL
    case HCI_CONN_CMPLT_EVT:
        if (param[0] == 0) {
            gap_printf("edr connected\n");

            gap.edr.hconn = bt_read_u16(param + 1);

            sys_set_event(GAP_TASK_ID, GAP_WRITE_SCAN_EN_EVENT_ID);
        }
        break;
    case HCI_CONN_REQ_EVT:
        memcpy(gap.edr.bdaddr, param, 6);
        sys_set_event(GAP_TASK_ID, GAP_ACCEPT_CONN_EVENT_ID);
        break;
#endif
    case HCI_DISCONN_CMPLT_EVT:
    {
        u16 handle;

        handle = bt_read_u16(param + 1);

#if !DISABLE_BT_CLASSICAL
        if (handle == gap.edr.hconn) {
            gap_printf("edr disconnected\n");
            gap.edr.hconn = 0;
            sys_set_event(GAP_TASK_ID, GAP_WRITE_SCAN_EN_EVENT_ID);
        } else
#endif
            if (handle == gap.le.hconn) {
                gap_printf("le disconnected\n");
                gap.le.hconn = 0;
                sys_set_event(GAP_TASK_ID, GAP_SET_ADV_EN_EVENT_ID);
            }
    }
    break;
    case HCI_CMD_CMPLT_EVT:
        gap.state.cmd_credit = 1;

        if (gap.state.cmd_buf_avail && gap.state.cmd_credit) {
            sys_resume(GAP_TASK_ID);
        }
        hci_cmd_cmplt(param, param_len);
        break;
    case HCI_CMD_STATUS_EVT:
        gap.state.cmd_credit = 1;

        if (gap.state.cmd_buf_avail && gap.state.cmd_credit) {
            sys_resume(GAP_TASK_ID);
        }
        break;
    case HCI_NUM_OF_CMPLT_PKTS_EVT:
    {
        u16 num = bt_read_u16(param + 3);

        gap.acl_credits += num;

        gap_printf("acl credits: %d\n", gap.acl_credits);

        sys_resume(L2CAP_TASK_ID);
    }
    break;
#if !DISABLE_BT_CLASSICAL
    case HCI_IO_CAP_REQ:
        sys_set_event(GAP_TASK_ID, GAP_IO_CAP_REPLY_EVENT_ID);
        break;
    case HCI_USER_CFM_REQ:
        sys_set_event(GAP_TASK_ID, GAP_USER_CFM_REPLY_EVENT_ID);
        break;
#endif
    case HCI_LE_META_EVT:
        subcode = input[2];
        switch(subcode) {
        case HCI_LE_CONN_CMPLT_EVT:
            gap_printf("le connected\n");

            gap.le.hconn = bt_read_u16(param + 2);
            sys_set_event(GAP_TASK_ID, GAP_SET_ADV_EN_EVENT_ID);
            break;
        }
        break;
    }

    hci_receive(BT_EVENT_CHANNEL);
}

static void hci_cmd_cmplt( u8* data, u8 len )
{
    u16 opcode;

    opcode = bt_read_u16(data + 1);

    switch (opcode) {
    case 0x1005: // read buffer size
        gap.acl_credits = data[7];
        gap_printf("acl credits: %d\n", gap.acl_credits);
    case 0x2002: // le read buffer size
        gap_printf("le pkt num %d\n", data[6]);
    }
}

static void hci_acl_input(u8* input, u16 isize)
{
    u8 handle;
    u8 flags;

    gap_dumphex("ACLI", input, isize);

    /* acl format
       bits 0-11   handle
       bits 12-13  pb flag
       bits 14-15  bc flag
       bits 16-31  input total length
    */

    handle = bt_read_u16(input) & 0xFFF;
    flags = input[1] >> 4;
    //acl_len = bt_read_u16(input +2);

    if ((gap.le.hconn != handle)
#if !DISABLE_BT_CLASSICAL
        && (gap.edr.hconn != handle)
#endif
        ) return;

    l2cap_input(input + 4, isize - 4, flags);

    hci_receive(BT_ACL_IN_CHANNEL);
}

void gap_get_buffer(u8** buffer, u16* size)
{
    u8 *acl_buffer;
    u16 length;

    hci_get_buffer(BT_ACL_OUT_CHANNEL, &acl_buffer, &length);
    *buffer = acl_buffer + 4;
    *size = length - 4;
}

void gap_send(u8* buffer, u16 size, u8 edr)
{
    u8* acl_buffer;

    acl_buffer = buffer - 4;

#if !DISABLE_BT_CLASSICAL
    if (edr) {
        bt_write_u16(acl_buffer, gap.edr.hconn | 0x2000);
    } else
#endif
    {
        bt_write_u16(acl_buffer, gap.le.hconn | 0x2000);
    }
    bt_write_u16(acl_buffer + 2, size);

    gap.acl_credits--;

    gap_dumphex("ACLO", acl_buffer, size + 4);

    hci_send(BT_ACL_OUT_CHANNEL, acl_buffer, size + 4);

    sys_pause(L2CAP_TASK_ID);
}
