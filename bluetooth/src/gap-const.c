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

const u8 radio_init_cmds[] = {
    0x03, 0x0C, 0x00, // reset
    //0x03, 0x10, 0x00, // read local supported features
    //0x01, 0x10, 0x00, // read local version
    //0x09, 0x10, 0x00, // read BD ADDR
    0x05, 0x10, 0x00, // read buffer size
#if !DISABLE_BT_CLASSICAL
    //0x23, 0x0C, 0x00, // read class of device
    //0x25, 0x0C, 0x00, // read voice setting
#endif
    0x05, 0x0C, 0x01, 0x00, // set event filter (clear all)
    0x16, 0x0C, 0x02, 0x00, 0x7D, // write connection accept timeout (32000)
#if !DISABLE_BT_CLASSICAL
    //0x1B, 0x0C, 0x00, // read page scan activity
    //0x46, 0x0C, 0x00, // read page scan type
#endif
#if !DISABLE_BLE
    0x02, 0x20, 0x00, // le read buffer size
#endif
    //0x03, 0x20, 0x00, // le read local supported features
    //0x07, 0x20, 0x00, // le read advertising channel tx power
    //0x0F, 0x20, 0x00, // le read white list size
    //0x1C, 0x20, 0x00, // le read supported states
    0x01, 0x0C, 0x08, 0xFF, 0xFF, 0xFB, 0xFF, 0x07, 0xF8, 0xBF, 0x3D, // set event mask
#if !DISABLE_BLE
    0x01, 0x20, 0x08, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // le set event mask
#endif
#if !DISABLE_BT_CLASSICAL
    0x56, 0x0C, 0x01, 0x01, // write simple pairing mode (1)
    0x45, 0x0C, 0x01, 0x02, // write inquiry mode (2)
    //0x58, 0x0C, 0x00, // read inquiry response tx power
#endif
    //0x04, 0x10, 0x01, 0x01, // read local extended features (page 1)
#if !DISABLE_BT_CLASSICAL
    0x12, 0x0C, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, // delete stored key
    0x0F, 0x08, 0x02, 0x0F, 0x00, // write default link policy settings
#endif
    0x6D, 0x0C, 0x02, 0x01, 0x01, // write le host supported (1)
#if !DISABLE_BT_CLASSICAL
    0x1C, 0x0C, 0x04, 0x00, 0x08, 0x12, 0x00, // write page scan activity
    0x1E, 0x0C, 0x04, 0x00, 0x08, 0x12, 0x00, // write inquiry scan activity
#endif

#if !DISABLE_BLE
    6,0x20, 0xF, 0x20, 0, 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, // adv params
#endif
    0,0,0 // end of sequence
};
