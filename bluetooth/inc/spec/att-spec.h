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

#ifndef BT_ATT_SPEC_H
#define BT_ATT_SPEC_H

/* ATT Methods */
#define ERROR_RSP                  0x01
#define EXCH_MTU_REQ               0x02
#define EXCH_MTU_RSP               0x03
#define FIND_INF_REQ               0x04
#define FIND_INF_RSP               0x05
#define FIND_BY_TYPE_VAL_REQ       0x06
#define FIND_BY_TYPE_VAL_RSP       0x07
#define READ_BY_TYPE_REQ           0x08
#define READ_BY_TYPE_RSP           0x09
#define READ_REQ                   0x0A
#define READ_RSP                   0x0B
#define READ_BLOB_REQ              0x0C
#define READ_BLOB_RSP              0x0D
#define READ_MULTI_REQ             0x0E
#define READ_MULTI_RSP             0x0F
#define READ_BY_GRP_TYPE_REQ       0x10
#define READ_BY_GRP_TYPE_RSP       0x11
#define WRITE_REQ                  0x12
#define WRITE_RSP                  0x13
#define WRITE_CMD                  0x52
#define PREP_WRITE_REQ             0x16
#define PREP_WRITE_RSP             0x17
#define EXEC_WRITE_REQ             0x18
#define EXEC_WRITE_RSP             0x19
#define HANDLE_VAL_NOTIF           0x1B
#define HANDLE_VAL_IND             0x1D
#define HANDLE_VAL_CFM             0x1E
#define SIGN_WRITE_CMD             0xD2

/* Error Codes */

#define INVALID_HANDLE             0x01
#define READ_NOT_PERMITTED         0x02
#define WRITE_NOT_PERMITTED        0x03
#define INVALID_PDU                0x04
#define INSUFF_AUTHENTICATION      0x05
#define REQ_NOT_SUPPORTED          0x06
#define INVALID_OFFSET             0x07
#define INSUFF_AUTHORIZATION       0x08
#define PREPARE_QUEUE_FULL         0x09
#define ATTR_NOT_FOUND             0x0A
#define ATTR_NOT_LONG              0x0B
#define INSUFF_ENCRYPT_KEY_SIZE    0x0C
#define INVALID_ATTR_VAL_LEN       0x0D
#define UNLIKELY_ERROR             0x0E
#define INSUFF_ENCRYPT             0x0F
#define UNSUPP_GRP_TYPE            0x10
#define INSUFF_RESOURCES           0x11

/* Permission */

#define BT_ATT_READABLE                   0x01
#define BT_ATT_WRITEABLE                  0x02
#define BT_ATT_AUTHENTICATION_REQUIRED    0x04
#define BT_ATT_AUTHORIZATION_REQUIRED     0x08

#endif // BT_ATT_SPEC_H
