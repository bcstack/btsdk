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

#ifndef _L2CAP_H_
#define _L2CAP_H_

enum {
    L2CAP_SIG_CHANNEL = 0,
    L2CAP_GATT_CHANNEL,
    L2CAP_SDP_CHANNEL,
    L2CAP_RFCOMM_CHANNEL,
};

enum {
    L2CAP_SEND_SIG_EVENT_ID = 0,
    L2CAP_SEND_RFCOMM_EVENT_ID,
    L2CAP_SEND_GATT_EVENT_ID,
    L2CAP_SEND_SDP_EVENT_ID,
};

void l2cap_setup( void );

void l2cap_input(u8* input, u16 isize, u8 flags);
void l2cap_get_buffer(u8** buffer);
void l2cap_send( u8 channel, u8* payload, u16 size );

#endif //_L2CAP_H_
