/*

Copyright (c) 2014, Mobile Minds LTD.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __MM_GSM_HTTP_H__
#define __MM_GSM_HTTP_H__

#include "Arduino.h"
#include "MM_GSM_Serial.h"
#include "MM_GSM_IP.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>


#define GSM_HTTP_TIMEOUT_CONNECTION                         20000
#define GSM_HTTP_DELAY_COMMAND                              100
#define GSM_HTTP_TIMEOUT_GPRS_CONTEXT                       20000
#define GSM_HTTP_TIMEOUT_SOCKET_OPEN                        10000
#define GSM_HTTP_TIMEOUT_SEND_START                         10000
#define GSM_HTTP_TIMEOUT_SEND_ACTION                        60000

#define GSM_HTTP_CONTENT_TEXT                               0
#define GSM_HTTP_CONTENT_JSON                               1

typedef enum {
  RequestGET,
  RequestPOST,
  RequestPUT,
  RequestDELETE
} RequestType_t;

class GSM_HTTP
{
  public:
    GSM_HTTP(GSM_IP*);
    //GSM_HTTP();
    void begin();
    static int8_t PORInit();
    int8_t open(const char* APN, const char* Username, const char* Password);
    int8_t open();
    int8_t close();
    
    //void setHost(const char* Host);
    //void setPort(uint16_t Port);
    void setPath(const char* Path);
    void setQueryString(const char* QueryStr);
    void setExtraHeaders(const char* ExtraHeaders);
    void setPostData(const char * POSTData, uint16_t POSTSize);
    void setContentType(const char * ContentType);
    
    int16_t HTTPRequest(RequestType_t RequestType, char * ContentResponse, int16_t ContentResponseLen, uint8_t returnHeader=false, uint32_t Timeout=GSM_HTTP_TIMEOUT_CONNECTION);
    
    uint16_t getStatusCode();
    
  private:
    //const char* _host;
    //uint16_t _port;
    const char* _path;
    const char* _querystr;
    const char* _extraheaders;
    const char* _postdata;
    uint16_t _postsize;
    uint16_t _returnedstatuscode;
    const char* _contenttype;
    
    int16_t getTCPData(char * buffer, int16_t bytes);
    
    // Take a pointer to MM_GSM_IP instance
    GSM_IP*  _MMIP;
  
};

#endif
