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

#ifndef _BTCONFIG_H_
#define _BTCONFIG_H_

/*** General Configurations ***/

#define DISABLE_BLE 1

#define CFG_DEVICE_NAME "BT-SDK"

/*** Debugging ***/
#define DEBUG_USER       0
#define DEBUG_BCSP       0
#define DEBUG_USB        0
#define DEBUG_GAP        0
#define DEBUG_GATT       0
#define DEBUG_L2CAP      0

#define DEBUG_ANY (                             \
        DEBUG_USER ||                           \
        DEBUG_BCSP ||                           \
        DEBUG_USB ||                            \
        DEBUG_HCI ||                            \
        DEBUG_GATT ||                           \
        DEBUG_L2CAP)

/*** L2CAP configurations ***/

// Low Energy link MTU
#define CFG_L2CAP_MTU_LE      30

// EDR link MTU
#define CFG_L2CAP_MTU_EDR     315

/*** UART transport configurations ***/

// Host 2 Controller MTU
#define CFG_HCI_UART_MTU_H2C 256

// Controller 2 Host MTU
#define CFG_HCI_UART_MTU_C2H 256

/*** USB transport configurations ***/

#define CFG_HCI_USB_COMMAND_MTU    256
#define CFG_HCI_USB_EVENT_MTU      256
#define CFG_HCI_USB_ACL_MTU        1024

/*** RFCOMM configurations ***/

#define CFG_RFCOMM_NUM_CHANNELS    2

/*** Serial Port configurations ***/
#define CFG_SPP_MTU 20
#define CFG_SPP_TX_RING_DEPTH 20
#define CFG_SPP_RX_RING_DEPTH 60

#endif // _BTCONFIG_H_
