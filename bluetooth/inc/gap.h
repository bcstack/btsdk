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

#ifndef _GAP_H_
#define _GAP_H_

enum {
    GAP_INIT_RADIO_EVENT_ID,
    GAP_SET_ADV_DATA_EVENT_ID,
    GAP_SET_ADV_EN_EVENT_ID,
    GAP_WRITE_LOCAL_NAME_EVENT_ID,
    GAP_WRITE_EIR_EVENT_ID,
    GAP_WRITE_SCAN_EN_EVENT_ID,
    GAP_ACCEPT_CONN_EVENT_ID,
    GAP_IO_CAP_REPLY_EVENT_ID,
    GAP_USER_CFM_REPLY_EVENT_ID,
};

void gap_setup(void);
void gap_reset(void);
void gap_set_visible(int v);
void gap_get_buffer(u8** buffer, u16* size);
void gap_send(u8* buffer, u16 size, u8 edr);

#endif // _GAP_H_
