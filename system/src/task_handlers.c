/* GENERATED BY update.py DO NOT EDIT */

#include "system.h"
void hci_task_handler(u8 event_id);
void gap_task_handler(u8 event_id);
void l2cap_task_handler(u8 event_id);

task_handler_t task_handlers[] = {
	hci_task_handler,
	gap_task_handler,
	l2cap_task_handler,
};
