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

#ifndef _L2CAP_SPEC_H_
#define _L2CAP_SPEC_H_

#define L2CAP_SIG_CID           0x0001
#define L2CAP_ATT_CID           0x0004
#define L2CAP_LESIG_CID         0x0005

#define L2CAP_SDP_PSM           0x0001
#define L2CAP_RFCOMM_PSM        0x0003
#define L2CAP_BNEP_PSM          0x000F
#define L2CAP_HIDC_PSM          0x0011
#define L2CAP_HIDI_PSM          0x0013

#define L2CAP_CONNECTION_SUCCESSFUL    0x0000
#define L2CAP_CONNECTION_PENDING       0x0001
#define L2CAP_PSM_NOT_SUPPORTED        0x0002
#define L2CAP_SECURITY_BLOCK           0x0003
#define L2CAP_NO_RESOURCES_AVAILABLE   0x0004

#define L2CAP_COMMAND_REJECT              0x01
#define L2CAP_CONNECTION_REQUEST          0x02
#define L2CAP_CONNECTION_RESPONSE         0x03
#define L2CAP_CONFIGURE_REQUEST           0x04
#define L2CAP_CONFIGURE_RESPONSE          0x05
#define L2CAP_DISCONNECTION_REQUEST       0x06
#define L2CAP_DISCONNECTION_RESPONSE      0x07
#define L2CAP_ECHO_REQUEST                0x08
#define L2CAP_ECHO_RESPONSE               0x09
#define L2CAP_INFORMATION_REQUEST         0x0A
#define L2CAP_INFORMATION_RESPONSE        0x0B

#endif // _L2CAP_SPEC_H_
