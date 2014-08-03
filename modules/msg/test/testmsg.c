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

#define FOO 0
#define BAR 1

void foo(u8 msg_id)
{
    printf("foo: received message %d\n", msg_id);

    if (msg_id < 31) {
        msg_post(BAR, msg_id + 1);
    }
}

void bar(u8 msg_id)
{
    printf("bar: received message %d\n", msg_id);

    if (msg_id < 31) {
        msg_post(FOO, msg_id + 1);
    }
}

static msg_handler_t handlers[] = {
    foo,
    bar,
};

void main(void)
{
    u8 handler_id, msg_id;

    printf("test message handler\n");

    msg_post(FOO, 0);
    msg_resume(FOO);
    msg_resume(BAR);

    for (;;) {
        if (msg_schedule(&handler_id, &msg_id)) {
            printf("schedule <%d,%d>\n", handler_id, msg_id);
            handlers[handler_id](msg_id);
        } else {
            break;
        }
    }
}
