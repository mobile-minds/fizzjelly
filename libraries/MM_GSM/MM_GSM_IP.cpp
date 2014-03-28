/*

Copyright (c) 2014, Mobile Minds LTD.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "Arduino.h"

#include "MM_GSM_IP.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>



GSM_IP::GSM_IP(const char* APN, const char* Username, const char* Password, const char* HostName, uint16_t Port )
{
  _host = HostName;
  _port = Port;
  _apn = APN;
  _username = Username;
  _password = Password;
  
}

GSM_IP::GSM_IP(const char* APN, const char* HostName, uint16_t Port )
{
  _host = HostName;
  _port = Port;
  _apn = APN;
  _username = NULL;
  _password = NULL;
  
}

GSM_IP::GSM_IP()
{
  _host = NULL;
  _port = 80;
  _username = NULL;
  _password = NULL;
  _apn = NULL;
}

const char* GSM_IP::getHost()
{
  return(this->_host);
}

uint16_t GSM_IP::getPort()
{
  return(this->_port);
}

const char* GSM_IP::getAPN()
{
  return(this->_apn);
}

const char* GSM_IP::getUserName()
{
  return(this->_username);
}

const char* GSM_IP::getPassword()
{
  return(this->_password);
}
