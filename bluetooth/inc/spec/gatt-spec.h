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

#ifndef BT_GATT_SPEC_H
#define BT_GATT_SPEC_H

/* Standard UUIDs */

#define UUID_GENERIC_ACCESS_PROFILE       0x1800
#define UUID_GENERIC_ATTR_PROFILE         0x1801
#define UUID_PRIMARY_SERVICE              0x2800
#define UUID_SECONDARY_SERVICE            0x2801
#define UUID_INCLUDE                      0x2802
#define UUID_CHAR                         0x2803
#define UUID_CHAR_EXT_PROP                0x2900
#define UUID_CHAR_USER_DESC                0x2901
#define UUID_CLIENT_CHAR_CFG              0x2902
#define UUID_SERVER_CHAR_CFG              0x2903
#define UUID_CHAR_FORMAT                  0x2904
#define UUID_CHAR_AGGR_FORMAT             0x2905
#define UUID_DEVICE_NAME                  0x2A00
#define UUID_APPEARANCE                   0x2A01
#define UUID_PERI_PRIVACY_FLAG            0x2A02
#define UUID_RECONN_ADDR                  0x2A03
#define UUID_PERI_PREF_CONN_PARAM         0x2A04
#define UUID_SERVICE_CHANGED              0x2A05

/* Characteristic Properties */
#define BT_GATT_CHAR_BROADCAST                0x01
#define BT_GATT_CHAR_READ                     0x02
#define BT_GATT_CHAR_WRITE_WITHOUT_RESPONSE   0x04
#define BT_GATT_CHAR_WRITE                    0x08
#define BT_GATT_CHAR_NOTIFY                   0x10
#define BT_GATT_CHAR_INDICATE                 0x20
#define BT_GATT_CHAR_SIGNED_WRITE             0x40
#define BT_GATT_CHAR_EXT_PROPERTIES           0x80

#endif // BT_GATT_SPEC_H
