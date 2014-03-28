/*

Copyright (c) 2014, Mobile Minds LTD.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __MM_GSM_SMS_H__
#define __MM_GSM_SMS_H__

#include "Arduino.h"
#include "MM_GSM_Serial.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <Time.h>


#define GSM_SMS_TIMEOUT_MESSAGESEND                        20000
#define GSM_SMS_TIMEOUT_MESSAGEREAD                        2500
#define GSM_SMS_TIMEOUT_MESSAGEDELETE                      2500
#define GSM_SMS_TIMEOUT_MESSAGECOUNT                       5000

#if !defined(__time_t_defined) // avoid conflict with newlib or other posix libc
  typedef unsigned long time_t;
#endif

typedef struct 
{
  char PhoneNumber[24];
  uint8_t PhoneNumberLen;
  char Message[161];
  uint8_t MessageLen;
  time_t SentTime;
} GSM_SMS_Message;

class GSM_SMS
{
  public:
    void begin();
    static int8_t PORInit();
    int8_t Send(GSM_SMS_Message * Message);
    int8_t Send(const char * PhoneNumber, const char * MessageText);
    int8_t Read(uint8_t index, GSM_SMS_Message * Message);
    int8_t Delete(uint8_t index);
    int8_t FindNext(GSM_SMS_Message * Message, int8_t lastIndex=-1);
    int8_t ReceivedCount();
    int8_t TotalCount();
};

#endif
