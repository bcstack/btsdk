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

#ifndef _RFCOMM_H_
#define _RFCOMM_H_

enum {
    RFCOMM_SEND_RESPONSE_EVENT_ID = 0,
};

void rfcomm_setup( void );
void rfcomm_input(u8* input, u16 isize);
void rfcomm_output(u8* output, u16* osize);

#endif //_RFCOMM_H_
