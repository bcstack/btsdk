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

#if DEBUG_H4
#define h4_printf printf
#else
#define h4_printf(...)
#endif

#define H4_COMMAND_PACKET        1
#define H4_ACL_PACKET            2
#define H4_SCO_PACKET            3
#define H4_EVENT_PACKET          4

enum {
    H4_RX_PACKET_ID = 1,
    H4_RX_ACL_HEADER,
    H4_RX_ACL_PAYLOAD,
    H4_RX_EVENT_HEADER,
    H4_RX_EVENT_PAYLOAD,
};

static struct {
    u8 tx_dma_event;
    u8 rx_dma_event;
    u8 rx_state;
    u16 rx_length;
    u8 txbuf[CFG_HCI_UART_MTU_H2C + 1];
    u8 rxbuf[CFG_HCI_UART_MTU_C2H + 1];
} h4;

static void start_rx(void);

void hci_setup(void)
{
    memset(&h4, 0, sizeof(h4));
    start_rx();
}

void hci_shutdown(void)
{
}

void hci_receive(u8 channel)
{
}

void hci_get_buffer(u8 channel, u8** buffer, u16* length)
{
    switch (channel) {
    case BT_COMMAND_CHANNEL:
    case BT_ACL_OUT_CHANNEL:
        *buffer = h4.txbuf + 1;
        *length = CFG_HCI_UART_MTU_H2C;
        break;
    case BT_EVENT_CHANNEL:
    case BT_ACL_IN_CHANNEL:
        *buffer = h4.rxbuf + 1;
        *length = h4.rx_length;
        break;
    }
}

static void start_rx(void)
{
    h4.rx_state = H4_RX_PACKET_ID;
    bt_uart_rx(h4.rxbuf, 1);
}

void hci_send(u8 channel, u8* buffer, u16 size)
{
    switch (channel) {
    case BT_COMMAND_CHANNEL:
        h4.txbuf[0] = H4_COMMAND_PACKET;
        break;
    case BT_ACL_OUT_CHANNEL:
        h4.txbuf[0] = H4_ACL_PACKET;
        break;
    }

    bt_uart_tx(h4.txbuf, size + 1);
}

void hci_loop(void)
{
    if (h4.tx_dma_event) {
        h4.tx_dma_event = 0;

        switch (h4.txbuf[0]) {
        case H4_COMMAND_PACKET:
            sys_set_event(HCI_TASK_ID, HCI_COMMAND_SENT_EVENT_ID);
            break;
        case H4_ACL_PACKET:
            sys_set_event(HCI_TASK_ID, HCI_ACL_SENT_EVENT_ID);
            break;
        }
    }

    if (h4.rx_dma_event) {
        h4.rx_dma_event = 0;
        switch (h4.rxbuf[0]) {
        case H4_EVENT_PACKET:
            sys_set_event(HCI_TASK_ID, HCI_EVENT_RECEIVED_EVENT_ID);
            break;
        case H4_ACL_PACKET:
            sys_set_event(HCI_TASK_ID, HCI_ACL_RECEIVED_EVENT_ID);
            break;
        }
    }
}

void bt_uart_tx_done()
{
    h4.tx_dma_event = 1;
}

void bt_uart_rx_done()
{
    u16 payload_len;

    switch (h4.rx_state) {
    case H4_RX_PACKET_ID:
        switch (h4.rxbuf[0]) {
        case H4_ACL_PACKET:
            h4.rx_state = H4_RX_ACL_HEADER;
            bt_uart_rx(h4.rxbuf + 1, 4);
            break;
        case H4_EVENT_PACKET:
            h4.rx_state = H4_RX_EVENT_HEADER;
            bt_uart_rx(h4.rxbuf + 1, 2);
            break;
        }
        break;
    case H4_RX_ACL_HEADER:
        h4.rx_state = H4_RX_ACL_PAYLOAD;
        payload_len = bt_read_u16(h4.rxbuf + 3);
        bt_uart_rx(h4.rxbuf + 5, payload_len);
        break;
    case H4_RX_ACL_PAYLOAD:
        payload_len = bt_read_u16(h4.rxbuf + 3);
        h4.rx_length = payload_len + 4;
        h4.rx_dma_event = 1;
        start_rx();
        break;
    case H4_RX_EVENT_HEADER:
        h4.rx_state = H4_RX_EVENT_PAYLOAD;
        payload_len = h4.rxbuf[2];
        bt_uart_rx(h4.rxbuf + 3, payload_len);
        break;
    case H4_RX_EVENT_PAYLOAD:
        payload_len = h4.rxbuf[2];
        h4.rx_length = payload_len + 2;
        h4.rx_dma_event = 1;
        start_rx();
        break;
    }
}
