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

#ifndef _SDP_SPEC_H_
#define _SDP_SPEC_H_

// Data Element Types
enum {
    SDP_DE_NIL = 0,
    SDP_DE_UINT,
    SDP_DE_INT,
    SDP_DE_UUID,
    SDP_DE_STRING,
    SDP_DE_BOOL,
    SDP_DE_DES,
    SDP_DE_DEA,
    SDP_DE_URL
};

// Data Element Size
enum {
    SDP_DE_SIZE_8 = 0,
    SDP_DE_SIZE_16,
    SDP_DE_SIZE_32,
    SDP_DE_SIZE_64,
    SDP_DE_SIZE_128,
    SDP_DE_SIZE_VAR_8,
    SDP_DE_SIZE_VAR_16,
    SDP_DE_SIZE_VAR_32
};

#define SDP_UUID_16(v)                          \
    ((SDP_DE_UUID << 3) | SDP_DE_SIZE_16),      \
        ((v) >> 8),                             \
        ((v) & 0xff)

#define SDP_UINT_8(v)                           \
    ((SDP_DE_UINT << 3) | SDP_DE_SIZE_8),       \
        (v)

#define SDP_UINT_16(v)                          \
    ((SDP_DE_UINT << 3) | SDP_DE_SIZE_16),      \
        ((v) >> 8),                             \
        ((v) & 0xff)

#define SDP_DES(v)                              \
    ((SDP_DE_DES << 3) | SDP_DE_SIZE_VAR_8),    \
        (v)

#define SDP_STRING(v)                           \
    ((SDP_DE_STRING << 3) | SDP_DE_SIZE_VAR_8), \
        (v)

// SDP PDU ID
enum {
    SDP_ERROR_RESPONSE = 1,
    SDP_SERVICE_SEARCH_REQUEST,
    SDP_SERVICE_SEARCH_RESPONSE,
    SDP_SERVICE_ATTRIBUTE_REQUEST,
    SDP_SERVICE_ATTRIBUTE_RESPONSE,
    SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST,
    SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE,
};

// UNIVERSAL ATTRIBUTE DEFINITIONS
#define SDP_SERVICE_RECORD_HANDLE                  0x0000
#define SDP_SERVICE_CLASS_ID_LIST                  0x0001
#define SDP_SERVICE_RECORD_STATE                   0x0002
#define SDP_SERVICE_ID                             0x0003
#define SDP_PROTOCOL_DESCRIPTOR_LIST               0x0004
#define SDP_BROWSE_GROUP_LIST                      0x0005
#define SDP_LANGUAGE_BASE_ATTRIBUTE_ID_LIST        0x0006
#define SDP_SERVICE_INFO_TIME_TO_LIVE              0x0007
#define SDP_SERVICE_AVAILABILITY                   0x0008
#define SDP_PROFILE_DESCRIPTOR_LIST                0x0009
#define SDP_DOCUMENTATION_URL                      0x000A
#define SDP_CLIENT_EXECUTABLE_URL                  0x000B
#define SDP_ICON_URL                               0x000C
#define SDP_ADDITIONAL_PROTOCOL_DESCRIPTOR_LIST    0x000D

#define SDP_SERVICE_NAME                           0x0100
#define SDP_SERVICE_DESCRIPTION                    0x0101
#define SDP_PROVIDER_NAME                          0x0102

#define SDP_SUPPORTED_FORMATS_LIST                 0x0303

// SERVICE CLASSES
#define SDP_SERIAL_PORT_UUID       0x1101
#define SDP_OPP                    0x1105
#define SDP_FTP                    0x1106
#define SDP_PUBLIC_BROWSE_GROUP    0x1002

// PROTOCOLS
#define SDP_SDP_UUID           0x0001
#define SDP_UDP_UUID           0x0002
#define SDP_RFCOMM_UUID        0x0003
#define SDP_OBEX_UUID          0x0008
#define SDP_L2CAP_UUID         0x0100

#endif // _SDP_SPEC_H_
