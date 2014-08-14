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

void bt_setup(void)
{
    hci_setup();
    gap_setup();
    l2cap_setup();
    sdp_setup();
    rfcomm_setup();
}

void bt_loop( void )
{
    hci_loop();
}

void bt_shutdown(void)
{
    hci_shutdown();
}
