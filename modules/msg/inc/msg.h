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

#ifndef _MSG_H_
#define _MSG_H_

#include "system.h"

/*
 * max number of handlers : 8
 * msg id range: 0 to 31
 */

// message handler function pointer
typedef void (*msg_handler_t)(u8 msg_id);

/* post a message
 */
void msg_post(u8 handler_id, u8 msg_id);

/* schedule a message
 * return value:
 *   0 -> no message
 *   1 -> got a message
 */
int msg_schedule(u8* handler_id, u8* msg_id );

/* pause a message handler
 */
void msg_pause(u8 handler_id);

/* resume a message handler
 */
void msg_resume(u8 handler_id);

#endif // _MSG_H_
