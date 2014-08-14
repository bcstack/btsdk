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

static const u8 spp_service_class_id_list[] = {
    SDP_DES(3),
    SDP_UUID_16(SDP_SERIAL_PORT_UUID),
};

static const u8 spp_protocol_descriptor_list[] = {
    SDP_DES(10),

    // L2CAP
    SDP_DES(3),
    SDP_UUID_16(SDP_L2CAP_UUID),

    // RFCOMM
    SDP_DES(5),
    SDP_UUID_16(SDP_RFCOMM_UUID),
    SDP_UINT_8(1), // rfcomm channel
};

static const u8 spp_profile_descriptor_list[] = {
    SDP_DES(8),

    // serial port profile
    SDP_DES(6),
    SDP_UUID_16(SDP_SERIAL_PORT_UUID),
    SDP_UINT_16(0x102),
};

static const u8 spp_service_name[] = {
    SDP_STRING(3),

    'C', 'O', 'M',
};

static const sdp_attr_t spp_attrs[] = {
    {SDP_SERVICE_CLASS_ID_LIST,
     spp_service_class_id_list,
     sizeof(spp_service_class_id_list)},

    {SDP_PROTOCOL_DESCRIPTOR_LIST,
     spp_protocol_descriptor_list,
     sizeof(spp_protocol_descriptor_list)},

    {SDP_PROFILE_DESCRIPTOR_LIST,
     spp_profile_descriptor_list,
     sizeof(spp_profile_descriptor_list)},

    {SDP_SERVICE_NAME,
     spp_service_name,
     sizeof(spp_service_name)}
};

const sdp_record_t spp_record = {
    spp_attrs,
    sizeof(spp_attrs) / sizeof(spp_attrs[0]),
};
