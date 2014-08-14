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

#ifndef _BOARD_H_
#define _BOARD_H_

void board_setup(void);
void bt_reset( void );
void bt_uart_setup(uint32_t baudrate);
void bt_uart_tx(const uint8_t* buffer, uint16_t len);
void bt_uart_tx_done(void);
void bt_uart_rx(uint8_t* buffer, uint16_t len);
void bt_uart_rx_done(void);
void delayms(uint16_t time);

#endif
