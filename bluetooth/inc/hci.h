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

#ifndef _HCI_H_
#define _HCI_H_

enum {
    HCI_COMMAND_SENT_EVENT_ID,
    HCI_EVENT_RECEIVED_EVENT_ID,
    HCI_ACL_RECEIVED_EVENT_ID,
    HCI_ACL_SENT_EVENT_ID,
};

#define BT_COMMAND_CHANNEL 1
#define BT_EVENT_CHANNEL   2
#define BT_ACL_IN_CHANNEL  3
#define BT_ACL_OUT_CHANNEL 4

void hci_setup(void);
void hci_shutdown(void);
void hci_loop(void);

void hci_get_buffer(u8 channel, u8** buffer, u16* length);
void hci_receive(u8 channel);
void hci_send(u8 channel, u8* buffer, u16 size);

#endif //_HCI_H_
