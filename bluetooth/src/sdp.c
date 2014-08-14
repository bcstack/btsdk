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

#if DEBUG_SDP
#define sdp_printf printf
#else
#define sdp_printf(...)
#endif

static struct {
    struct {
        u8 id;
        u8 tid;
    } pdu;

    u8 handle;

    union {
        u8 all;
        struct {
            u8 service_record_handle: 1;
            u8 service_class_id: 1;
            u8 protocol_descriptor_list: 1;
            u8 profile_descriptor_list: 1;
            u8 service_name: 1;
        } bits;
    } attrs;

} sdp;

static void sdp_input_ssa(u8* input, u16 isize);
static void sdp_parse_uuid_list(u8* input, u16 isize);
static void sdp_parse_attr_list(u8* input, u16 isize);
static void sdp_read_des(u8** input, u8** data, u32* length);
static void sdp_read_uuid(u8** input, u8* uuid);
static void sdp_read_attr(u8** input, u16* attr_range);
static void sdp_read_header(u8**input, u8* type, u32* length, u8** data);

extern const sdp_record_t spp_record;

#define SPP_RECORD_HANDLE 1
static const sdp_record_t* records[] = {
    &spp_record,
};

void sdp_setup( void )
{
    memset(&sdp, 0, sizeof(sdp));
}

void sdp_input(u8* input, u16 isize)
{
    u8 has_output = 0;
	u16 length;

    sdp.pdu.id = input[0];
    sdp.pdu.tid = sdp_read_u16(input + 1);
    length = sdp_read_u16(input + 3);

    sdp_printf("SDP-IN id:%d, tid:%d, plen:%d\n",
               sdp.pdu.id, sdp.pdu.tid, length);

    switch (sdp.pdu.id) {
    case SDP_ERROR_RESPONSE:
        break;
    case SDP_SERVICE_SEARCH_REQUEST:
        has_output = 1;
        break;
    case SDP_SERVICE_ATTRIBUTE_REQUEST:
        has_output = 1;
        break;
    case SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
        sdp_input_ssa(input + 5, length);
        has_output = 1;
        break;
    }

    if (has_output) {
        sys_set_event(L2CAP_TASK_ID, L2CAP_SEND_SDP_EVENT_ID);
    }
}

static void sdp_input_ssa(u8* input, u16 isize)
{
    u8* uuid_list_data;
    u32 uuid_list_length;
    u8* attr_list_data;
    u32 attr_list_length;
	u16 max_attr_data;
	u16 ct_length;
    u8* p = input;

    // service uuid list
    sdp_read_des(&p, &uuid_list_data, &uuid_list_length);

    sdp_printf("SSA UUID List len:%d\n", uuid_list_length);

    // max amount of attribute data
    max_attr_data = sdp_read_u16(p);
    p += 2;

    sdp_printf("SSA max amount of attr data:%d\n", max_attr_data);

    // attribute list
    sdp_read_des(&p, &attr_list_data, &attr_list_length);

    sdp_printf("SSA ATTR List len:%d\n", attr_list_length);

    // continuation
    ct_length = p[0];
    p++;

    // parse service uuid list
    sdp_parse_uuid_list(uuid_list_data, uuid_list_length);

    // parse attribute list
    sdp_parse_attr_list(attr_list_data, attr_list_length);
}

static void sdp_parse_uuid_list(u8* input, u16 isize)
{
    u8* p = input;
    u8 uuid[16];

    sdp.handle = 0;

    while (p-input < isize) {
        sdp_read_uuid(&p, uuid);
        sdp_printf("UUID: %02x%02x\n", uuid[0], uuid[1]);

        if ((sdp_read_u16(uuid) == SDP_SERIAL_PORT_UUID) ||
            (sdp_read_u16(uuid) == SDP_L2CAP_UUID)) {
            sdp.handle = SPP_RECORD_HANDLE;
        }
    }
}

static void sdp_parse_attr_list(u8* input, u16 isize)
{
    u8* p = input;
    u16 attr_range[2];

    sdp.attrs.all = 0;

    while (p-input < isize) {
        sdp_read_attr(&p, attr_range);
        sdp_printf("ATTR: %02x~%02x\n", attr_range[0], attr_range[1]);

        if ((SDP_SERVICE_RECORD_HANDLE >= attr_range[0]) &&
            (SDP_SERVICE_RECORD_HANDLE <= attr_range[1])) {
            sdp.attrs.bits.service_record_handle = 1;
        }

        if ((SDP_SERVICE_CLASS_ID_LIST >= attr_range[0]) &&
            (SDP_SERVICE_CLASS_ID_LIST <= attr_range[1])) {
            sdp.attrs.bits.service_class_id = 1;
        }

        if ((SDP_PROTOCOL_DESCRIPTOR_LIST >= attr_range[0]) &&
            (SDP_PROTOCOL_DESCRIPTOR_LIST <= attr_range[1])) {
            sdp.attrs.bits.protocol_descriptor_list = 1;
        }

        if ((SDP_PROFILE_DESCRIPTOR_LIST >= attr_range[0]) &&
            (SDP_PROFILE_DESCRIPTOR_LIST <= attr_range[1])) {
            sdp.attrs.bits.profile_descriptor_list = 1;
        }
        if ((SDP_SERVICE_NAME >= attr_range[0]) &&
            (SDP_SERVICE_NAME <= attr_range[1])) {
            sdp.attrs.bits.service_name = 1;
        }
    }

    sdp_printf("attrs.bits = %x\n", sdp.attrs.all);
}

static void sdp_read_des(u8** input, u8** data, u32* length)
{
    u8 type;

    sdp_read_header(input, &type, length, data);

    if (type != SDP_DE_DES) {
        sdp_printf("SDP Error\n Expected DES, got %d\n", type);
    }
}

static void sdp_read_uuid(u8** input, u8* uuid)
{
    u8 type;
    u32 length;
    u8* data;

    sdp_read_header(input, &type, &length, &data);

    if (type != SDP_DE_UUID) {
        sdp_printf("SDP Error\n Expected UUID, got %d\n", type);
    }

    memset(uuid, 0, 16);

    if (length <= 16) {
        memcpy(uuid, data, length);
    } else {
        sdp_printf("SDP Error: UUID Length = %d\n", length);
    }
}

static void sdp_read_attr(u8** input, u16* attr_range)
{
    u8 type;
    u32 length;
    u8* data;

    sdp_read_header(input, &type, &length, &data);

    if (type != SDP_DE_UINT) {
        sdp_printf("SDP Error\n Expected UINT, got %d\n", type);
    }

    switch (length) {
    case 2:
        attr_range[0] = sdp_read_u16(data);
        attr_range[1] = attr_range[0];
        break;
    case 4:
        attr_range[0] = sdp_read_u16(data);
        attr_range[1] = sdp_read_u16(data + 2);
        break;
    default:
        sdp_printf("SDP Error, Unexpected UINT Size %d\n", length);
    }
}

static void sdp_read_header(u8**input, u8* type, u32* length, u8** data)
{
    u8* p = *input;
    u8 hdrlen;

    *type = p[0] >> 3;
    hdrlen = 1;
    switch (p[0] & 7) {
    case SDP_DE_SIZE_8:
        *length = 1;
        break;
    case SDP_DE_SIZE_16:
        *length = 2;
        break;
    case SDP_DE_SIZE_32:
        *length = 4;
        break;
    case SDP_DE_SIZE_64:
        *length = 8;
        break;
    case SDP_DE_SIZE_128:
        *length = 16;
        break;
    case SDP_DE_SIZE_VAR_8:
        *length = p[1];
        hdrlen += 1;
        break;
    case SDP_DE_SIZE_VAR_16:
        *length = sdp_read_u16(p + 1);
        hdrlen += 2;
        break;
    case SDP_DE_SIZE_VAR_32:
        *length = sdp_read_u32(p + 1);
        hdrlen += 4;
        break;
    }

    *data = *input + hdrlen;
    *input += hdrlen + *length;
}

void sdp_output(u8* output, u16* osize)
{
    const sdp_record_t* record;
    const sdp_attr_t* attr;
    u8 i;
    u8 offset;

    record = records[sdp.handle - 1];

    switch (sdp.pdu.id) {
    case SDP_SERVICE_SEARCH_REQUEST:
        output[0] = SDP_SERVICE_SEARCH_RESPONSE;
        sdp_write_u16(output + 1, sdp.pdu.tid);
        sdp_write_u16(output + 3, 7); // length
        sdp_write_u16(output + 5, 1); // total record count
        sdp_write_u16(output + 7, 1); // current record count
        sdp_write_u32(output + 9, 1); // record handle
        output[13] = 0;
        *osize = 10;
        break;
    case SDP_SERVICE_ATTRIBUTE_REQUEST:
        output[0] = SDP_SERVICE_ATTRIBUTE_RESPONSE;
        sdp_write_u16(output + 1, sdp.pdu.tid);
        sdp_write_u16(output + 3, 0); // length
        sdp_write_u16(output + 5, 0); // attribute list byte count
        // attribute list
        output[7] = (SDP_DE_DES << 3) + SDP_DE_SIZE_VAR_8;
        output[8] = 0; // length of attr id/value pair
        output[9] = (SDP_DE_UINT << 3) + SDP_DE_SIZE_16;
        //sdp_write_u16(output + 10, attr_id);
        // attr value
        // continuation
        break;
    case SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
        sdp_printf("attr: %x,%x\n", attr->id, attr->length);
        output[0] = SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE;
        sdp_write_u16(output + 1, sdp.pdu.tid);


        if (sdp.handle) {
            offset = 11;

            // record handle
            output[offset++] = (SDP_DE_UINT << 3) | SDP_DE_SIZE_16;
            sdp_write_u16(output + offset, SDP_SERVICE_RECORD_HANDLE);
            offset += 2;
            output[offset++] = (SDP_DE_UINT << 3) | SDP_DE_SIZE_16;
            sdp_write_u16(output + offset, 1);
            offset += 2;

            for (i=0; i<record[0].count; i++) {
                attr = &record->attrs[i];
                output[offset++] = (SDP_DE_UINT << 3) | SDP_DE_SIZE_16;
                sdp_write_u16(output + offset, attr->id);
                offset += 2;
                memcpy(output + offset, attr->value, attr->length);
                offset += attr->length;
            }

            // atribute list
            output[9] = (SDP_DE_DES << 3) + SDP_DE_SIZE_VAR_8;
            output[10] = offset - 11;
        } else {
            offset = 9;
        }
        // continuation
        output[offset] = 0;

        // attribute lists
        output[7] = (SDP_DE_DES << 3) + SDP_DE_SIZE_VAR_8;
        output[8] = offset - 9;

        // attr list byte count
        sdp_write_u16(output + 5, offset - 7);

        // length
        sdp_write_u16(output + 3, offset - 4);

        *osize = offset + 1;
        break;
    }
}
