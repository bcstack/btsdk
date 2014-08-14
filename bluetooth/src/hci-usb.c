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

#include <libusb.h>
#include "bluetooth.h"

static struct {
    u16 vid, pid;
} known_devices[] = {
    {0x05ac, 0x828c},
    {0x0a12, 0x0001},
};

static struct libusb_device_handle *devh = NULL;
static struct libusb_transfer *evt_xfer;
static struct libusb_transfer *acl_in_xfer;
static struct libusb_transfer *acl_out_xfer;
static struct libusb_transfer *cmd_xfer;

static int event_addr;
static int acl_in_addr;
static int acl_out_addr;

static uint8_t cmd_buffer[CFG_HCI_USB_COMMAND_MTU + LIBUSB_CONTROL_SETUP_SIZE];
static uint8_t evt_buffer[CFG_HCI_USB_EVENT_MTU];
static uint8_t acl_in_buffer[CFG_HCI_USB_ACL_MTU];
static uint8_t acl_out_buffer[CFG_HCI_USB_ACL_MTU];

#if DEBUG_HCI
#define hci_printf printf
#else
#define hci_printf(...)
#endif

static void LIBUSB_CALL usb_cb(struct libusb_transfer *transfer)
{
    int r;

	if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
		hci_printf("usb xfer incompleted ep=%02X, status=%d\n", transfer->endpoint, transfer->status);
		libusb_free_transfer(transfer);
		return;
	}

    if (transfer == cmd_xfer) {
        sys_set_event(HCI_TASK_ID, HCI_COMMAND_SENT_EVENT_ID);
    } else if (transfer == evt_xfer) {
        sys_set_event(HCI_TASK_ID, HCI_EVENT_RECEIVED_EVENT_ID);
    } else if (transfer == acl_in_xfer) {
        sys_set_event(HCI_TASK_ID, HCI_ACL_RECEIVED_EVENT_ID);
    } else if (transfer == acl_out_xfer) {
        sys_set_event(HCI_TASK_ID, HCI_ACL_SENT_EVENT_ID);
    } else {
        printf("error in usb_cb\n");
    }
}

void hci_setup(void)
{
	int r;

	r = libusb_init(NULL);
	if (r < 0)
		return;

	devh = libusb_open_device_with_vid_pid(NULL, known_devices[1].vid, known_devices[1].pid);//TODO: search for device
    if (devh == NULL) {
        r = -1;
        printf("Cannot open Bluetooth device\n");
        goto out;
    }

    printf("Opened Bluetooth Device\n");

    r = libusb_kernel_driver_active(devh, 0);
    if (r < 0) {
        goto out;
    }

    if (r == 1) {
        r = libusb_detach_kernel_driver(devh, 0);
        if (r < 0) {
            printf("Cannot detach kernel driver\n");
            goto out;
        }
    }
    printf("detached kernel driver\n");

    r = libusb_claim_interface(devh, 0);
    if (r < 0) {
        printf("Cannot claim interface\n");
        goto out;
    }
    printf("claimed interface\n");

    evt_xfer = libusb_alloc_transfer(0);
    acl_in_xfer  = libusb_alloc_transfer(0);
    acl_out_xfer = libusb_alloc_transfer(0);
    cmd_xfer = libusb_alloc_transfer(0);

    event_addr = 0x81; // EP1, IN interrupt
    acl_in_addr =   0x82; // EP2, IN bulk
    acl_out_addr =  0x02; // EP2, OUT bulk

out:

    if (r<0) {
        printf("usb error, exit\n");
        exit(-1);
    }
    return;
}

void hci_shutdown(void)
{
	libusb_exit(NULL);
}

void hci_get_buffer(u8 channel, u8** buffer, u16* length)
{
    switch (channel) {
    case BT_COMMAND_CHANNEL:
        *buffer = cmd_buffer + LIBUSB_CONTROL_SETUP_SIZE;
        *length = CFG_HCI_USB_COMMAND_MTU;
        break;
    case BT_EVENT_CHANNEL:
        *buffer = evt_buffer;
        *length = evt_xfer->actual_length;
        break;
    case BT_ACL_IN_CHANNEL:
        *buffer = acl_in_buffer;
        *length = acl_in_xfer->actual_length;
        break;
    case BT_ACL_OUT_CHANNEL:
        *buffer = acl_out_buffer;
        *length = CFG_HCI_USB_ACL_MTU;
        break;
    }
}

void hci_receive(u8 channel)
{
    int r;

    switch (channel) {
    case BT_EVENT_CHANNEL:
        libusb_fill_interrupt_transfer(evt_xfer, devh, event_addr, 
                                       evt_buffer, CFG_HCI_USB_EVENT_MTU,
                                       usb_cb, (void*)0, 0);
        r = libusb_submit_transfer(evt_xfer);
        if (r) {
            printf("Cannot submit event transfer");
        }
        break;
    case BT_ACL_IN_CHANNEL:
        libusb_fill_bulk_transfer(acl_in_xfer, devh, acl_in_addr, 
                                  acl_in_buffer, CFG_HCI_USB_ACL_MTU,
                                  usb_cb, (void*)0, 0) ;
 
        r = libusb_submit_transfer(acl_in_xfer);
        if (r) {
            printf("Cannot submit acl in transfer");
        }
        break;
    }
}

void hci_send(u8 channel, u8* buffer, u16 size)
{
    int r;

    switch (channel) {
    case BT_COMMAND_CHANNEL:
        libusb_fill_control_setup(cmd_buffer, LIBUSB_REQUEST_TYPE_CLASS |
                                  LIBUSB_RECIPIENT_INTERFACE, 0, 0, 0, size);

        libusb_fill_control_transfer(cmd_xfer, devh, cmd_buffer, usb_cb, 0, 0);
        cmd_xfer->flags = LIBUSB_TRANSFER_FREE_BUFFER;
        r = libusb_submit_transfer(cmd_xfer);

        if (r < 0) {
            printf("Error submitting control transfer %d", r);
        }
        break;
    case BT_ACL_OUT_CHANNEL:
        libusb_fill_bulk_transfer(acl_out_xfer, devh, acl_out_addr,
                                  acl_out_buffer, size,
                                  usb_cb, 0, 0);
        acl_out_xfer->type = LIBUSB_TRANSFER_TYPE_BULK;

        r = libusb_submit_transfer(acl_out_xfer);
        if (r < 0) {
            printf("Error submitting acl out transfer, %d", r);
        }
    }
}

void hci_loop(void)
{
    libusb_handle_events(NULL);
}
