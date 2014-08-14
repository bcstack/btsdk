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

/*
 * this is a minimal implementation of ATT/GATT profile.
 * it contains only one simple profile for transfering data over BLE link
 */

static struct {
    u8  cmtu;
    u8  reqcode;
    u16 start_handle;
    u8  errcode;
} gatt;

/*
 * Attribute Table
 *
 * 0x0001, <<Primary Service>>, <<LESPP Service>>
 * 0x0002, <<Char>>, 0x0003, <<LESPP Char>>, GATT_READ | GATT_NOTIFY
 * 0x0003, <<LESPP Char>>
 */
const u8 lespp_service_uuid[16] =
{0xb4, 0xe4, 0x4b, 0x3a, 0x10, 0xcb, 0xbe, 0xe4,
 0xdc, 0x72, 0xa1, 0xcb, 0x1f, 0xc2, 0x4b, 0x00};

const u8 lespp_value_uuid[16] =
{0xca, 0xe8, 0xb3, 0x7b, 0x9e, 0x8e, 0xef, 0x39,
 0xe7, 0x2e, 0xe4, 0x81, 0xe3, 0xb6, 0x49, 0x0d};

#define LESPP_VALUE_HANDLE         3

void gatt_input(u8* input, u16 isize)
{
    gatt.reqcode = input[0];
    gatt.errcode = 0;

    switch (gatt.reqcode) {
    case EXCH_MTU_REQ:
        gatt.cmtu = bt_read_u16(input + 1);
        break;
    case FIND_INF_REQ:
        // we have no characteristic descriptors
        gatt.errcode = ATTR_NOT_FOUND;
        break;
    case FIND_BY_TYPE_VAL_REQ:
    {
        u16 end_handle = bt_read_u16(input + 3);
        u16 type = bt_read_u16(input + 5);
        u8* service_type = input + 7;
        gatt.start_handle = bt_read_u16(input + 1);

        if (!((gatt.start_handle <= 1) &&
              (end_handle >= 1) &&
              (type == UUID_PRIMARY_SERVICE) &&
              (0 == memcmp(service_type, lespp_service_uuid, 16)))) {
            gatt.errcode = ATTR_NOT_FOUND;
        }
    }
        break;
    case READ_BY_TYPE_REQ:
    {
        u16 end_handle = bt_read_u16(input + 3);
        u16 type = bt_read_u16(input + 5);
        gatt.start_handle = bt_read_u16(input + 1);

        if (!((gatt.start_handle <= 2) &&
              (end_handle >= 2) &&
              (type == UUID_CHAR))) {
            gatt.errcode = ATTR_NOT_FOUND;
        }
    }
        break;
    case READ_BY_GRP_TYPE_REQ:
    {
        u16 end_handle = bt_read_u16(input + 3);
        u16 type = bt_read_u16(input + 5);
        gatt.start_handle = bt_read_u16(input + 1);

        if (!((gatt.start_handle <= 1) &&
              (end_handle >= 1) &&
              (type == UUID_PRIMARY_SERVICE))) {
            gatt.errcode = ATTR_NOT_FOUND;
        }
    }
        break;
    case WRITE_CMD:
        gatt.start_handle = bt_read_u16(input + 1);
        if (gatt.start_handle == 3) {
            gatt.errcode = 0;
            spp_recv(input + 3, isize - 3);
        } else {
            gatt.errcode = ATTR_NOT_FOUND;
        }
    default:
        gatt.errcode = REQ_NOT_SUPPORTED;
    }

    if (!((gatt.reqcode == WRITE_CMD) ||
          (gatt.reqcode == SIGN_WRITE_CMD))) {
        sys_set_event(L2CAP_TASK_ID, L2CAP_SEND_GATT_EVENT_ID);
    }
}

void gatt_output(u8* output, u16* osize)
{
    if (gatt.reqcode) {
        if (gatt.errcode) {
            output[0] = ERROR_RSP;
            output[1] = gatt.reqcode;
            bt_write_u16(output + 2, gatt.start_handle);
            output[4] = gatt.errcode;
            *osize = 5;
        } else {
            switch (gatt.reqcode) {
            case EXCH_MTU_REQ:
                output[0] = EXCH_MTU_RSP;
                bt_write_u16(output + 1, ATT_MTU);
                *osize = 2;
                break;
            case FIND_BY_TYPE_VAL_REQ:
                    output[0] = FIND_BY_TYPE_VAL_RSP;
                    bt_write_u16(output + 1, 1);
                    bt_write_u16(output + 3, 3);
                    *osize = 5;
                break;
            case READ_BY_TYPE_REQ:
                    output[0] = READ_BY_TYPE_RSP;
                    output[1] = 21; // Length of each data
                    bt_write_u16(output + 2, 2); // Handle
                    output[4] = BT_GATT_CHAR_WRITE | BT_GATT_CHAR_NOTIFY; // char props
                    bt_write_u16(output + 5, 3);
                    memcpy(output + 7, lespp_value_uuid, 16);
                    *osize = 23;
                break;
            case READ_BY_GRP_TYPE_REQ:
                    output[0] = READ_BY_GRP_TYPE_RSP;
                    output[1] = 20; // Length of each data
                    bt_write_u16(output + 2, 1);
                    bt_write_u16(output + 4, 3);
                    memcpy(output + 6, lespp_service_uuid, 16);
                    *osize = 22;
                break;
            case WRITE_REQ:
                output[0] = WRITE_RSP;
                *osize = 1;
                break;
            }
        }

        gatt.reqcode = 0;
    } else {
        u8 len;
        // TODO: uplink data
        output[0] = HANDLE_VAL_NOTIF;
        bt_write_u16(output + 1, LESPP_VALUE_HANDLE);
        *osize = len + 3;
    }
}
