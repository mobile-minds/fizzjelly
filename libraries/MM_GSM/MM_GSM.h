/*

Copyright (c) 2014, Mobile Minds LTD.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __MM_GSM_H__
#define __MM_GSM_H__

#include "Arduino.h"
#include "MM_GSM_Serial.h"
#include "MM_GSM_SMS.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <Time.h>


#define GSM_IO_POWERKEY               20
#define GSM_IO_POWERREG               19
#define GSM_BAUD_RATE                 57600 //57600//9600

#define GSM_SIM_READY                 0
#define GSM_SIM_PIN_REQUIRED          1
#define GSM_SIM_PUK_REQUIRED          2

#define GSM_NETSTAT_REGISTERED        0
#define GSM_NETSTAT_REGISTERING       1
#define GSM_NETSTAT_REGISTERFAILED    2

#define GSM_NETWORK_REGISTER_TIMEOUT  20000

class GSM
{
  public:
    int8_t begin(boolean powerOn = true, char * SIM_PINCode = NULL);
    int8_t SetModuleOn(uint8_t state);
    int8_t PORInit(char * SIM_PINCode = NULL);
    int8_t GetSignalStrength();
    int8_t SetSIMPIN(char * PINCode);
    int8_t GetSIMStatus();
    int8_t GetNetworkStatus();
    
  /*protected:*/
};

#endif