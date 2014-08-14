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

#ifndef _BTUTILS_H
#define _BTUTILS_H

#if DEBUG_ANY
void dumphex(char* msg, const u8* v, u16 len);
#else
#define dumphex(...)
#endif

#define bt_read_u16(buffer)                     \
    ((buffer)[0] + ((buffer)[1] << 8))

#define bt_write_u16(buffer, value)             \
    do {                                        \
        (buffer)[0] = (value) & 0xFF;           \
        (buffer)[1] = (value) >> 8;             \
    } while (0)

#define bt_read_u32(buffer)                     \
    ((buffer)[0] + ((buffer)[1] << 8) +         \
     (buffer)[2] << 16 + ((buffer)[3] << 24))

#define bt_write_u32(buffer, value)             \
    do {                                        \
        (buffer)[0] = (value) & 0xFF;           \
        (buffer)[1] = ((value) >> 8) & 0xFF;    \
        (buffer)[2] = ((value) >> 16) & 0xFF;   \
        (buffer)[3] = ((value) >> 24) & 0xFF;   \
    } while (0)

#endif // _BTUTILS_H
