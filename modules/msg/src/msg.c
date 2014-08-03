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

#include "msg.h"

static struct {
    // active queue (handlers in active state)
    u8 active;

    // ready queue (handlers that have pending messages)
    u8 ready;

    // pending message for each handler
    u32 pending[CFG_MSG_HANDLER_NUM];
} msg;

void msg_setup( void )
{
    sys_memset(&msg, 0, sizeof(msg));
}

void msg_post(u8 handler_id, u8 msg_id)
{
    msg.pending[handler_id] |= (1 << msg_id);
    msg.ready |= (1 << handler_id);
}

int msg_schedule( u8* handler_id, u8* msg_id )
{
    u8 tmp;

    // find a handler to invoke
    tmp = sys_ffs(msg.ready & msg.active);

    if (tmp == 0) {
        return 0;
    }

    *handler_id = tmp - 1;

    // find a pending msg
    tmp = sys_ffs(msg.pending[*handler_id]);

    *msg_id = tmp - 1;

    // clear msg
    msg.pending[*handler_id] &= ~(1 << *msg_id);
    if (msg.pending[*handler_id] == 0) {
        msg.ready &= ~(1 << *handler_id);
    }

    return 1;
}

void msg_resume(u8 handler_id)
{
    msg.active |= (1 << handler_id);
}

void msg_pause(u8 handler_id)
{
    msg.active &= ~(1 << handler_id);
}
