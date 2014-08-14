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

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;

u8 sys_ffs(u32 t);
void sys_dumphex(char* msg, const u8* v, u16 len);

#define sys_memcpy memcpy
#define sys_memcmp memcmp
#define sys_memset memset

// task handler function pointer
typedef void (*task_handler_t)(u8 task_id);

/* setup system
 */
void sys_setup( void );

/* set an event
 */
void sys_set_event(u8 task_id, u8 event_id);

/* run scheduler. return 0 when idle
 */
int sys_schedule( void );

/* pause a task
 */
void sys_pause(u8 task_id);

/* resume a task
 */
void sys_resume(u8 task_id);

#include "task_handlers.h"

#endif // _SYSTEM_H_
