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

#include "system.h"

#if 0
#define sys_printf printf
#else
#define sys_printf(...)
#endif

static struct {
    // active queue (handlers in active state)
    u8 active;

    // ready queue (handlers that have pending messages)
    u8 ready;

    // pending message for each handler
    u32 pending[TASK_NUM];
} sys;

u8 sys_ffs(u32 t)
{
    u8 n = 0;

    if (t == 0) return 0;

    if (!(0xffff & t))      n += 16;
    if (!((0xff << n) & t)) n += 8;
    if (!((0xf << n) & t))  n += 4;
    if (!((0x3 << n) & t))  n += 2;
    if (!((0x1 << n) & t))  n += 1;

    return n + 1;
}

void sys_dumphex(char* msg, const u8* v, u16 len)
{
    int i;

    printf("%s: [", msg);
    for (i=0; i<len; i++) printf("%02x ", v[i]);
    printf("]\n");
}

void sys_setup( void )
{
    sys_memset(&sys, 0, sizeof(sys));
}

void sys_set_event(u8 task_id, u8 event_id)
{
    sys_printf("setevt <%d,%d>\n", task_id, event_id);

    sys.pending[task_id] |= (1 << event_id);
    sys.ready |= (1 << task_id);
}

int sys_schedule( void )
{
    u8 tmp;
    u8 task_id;
    u8 event_id;

    // find a task to schedule
    tmp = sys_ffs(sys.ready & sys.active);

    if (tmp == 0) {
        return 0;
    }

    task_id = tmp - 1;

    // find an event
    tmp = sys_ffs(sys.pending[task_id]);

    event_id = tmp - 1;

    // clear event
    sys.pending[task_id] &= ~(1 << event_id);
    if (sys.pending[task_id] == 0) {
        sys.ready &= ~(1 << task_id);
    }

    sys_printf("schedule <%d,%d>\n", task_id, event_id);
    task_handlers[task_id](event_id);

    return 1;
}

void sys_resume(u8 task_id)
{
    sys.active |= (1 << task_id);
}

void sys_pause(u8 task_id)
{
    sys.active &= ~(1 << task_id);
}
