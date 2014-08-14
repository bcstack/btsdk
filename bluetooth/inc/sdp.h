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

#ifndef _SDP_H_
#define _SDP_H_

typedef struct _sdp_attr_t {
    u16 id;
    const u8* value;
    u8 length;
} sdp_attr_t;

typedef struct _sdp_record_t {
    const sdp_attr_t* attrs;
    u8 count;
} sdp_record_t;

#define sdp_read_u16(buffer)                     \
    ((buffer)[1] + ((buffer)[0] << 8))

#define sdp_write_u16(buffer, value)             \
    do {                                        \
        (buffer)[1] = (value) & 0xFF;           \
        (buffer)[0] = (value) >> 8;             \
    } while (0)

#define sdp_read_u32(buffer)                     \
    ((buffer)[3] + ((buffer)[2] << 8) +         \
     (buffer)[1] << 16 + ((buffer)[0] << 24))

#define sdp_write_u32(buffer, value)             \
    do {                                        \
        (buffer)[3] = (value) & 0xFF;           \
        (buffer)[2] = ((value) >> 8) & 0xFF;    \
        (buffer)[1] = ((value) >> 16) & 0xFF;   \
        (buffer)[0] = ((value) >> 24) & 0xFF;   \
    } while (0)


enum {
    SDP_SEND_RESPONSE_EVENT_ID = 0,
};

void sdp_setup( void );
void sdp_input(u8* input, u16 isize);
void sdp_output(u8* output, u16* osize);

#endif // _SDP_H_
