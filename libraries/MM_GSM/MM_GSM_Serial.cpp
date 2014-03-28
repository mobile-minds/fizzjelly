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
#include "MM_GSM_Serial.h"

#include <stdint.h>
#include <stdio.h>

//#define GSMSERIAL_DEBUG_ON

_GSMSerial GSMSerial;

void _GSMSerial::begin(uint32_t baudRate)
{
  Serial1.setTimeout(GSMSERIAL_DEFAULT_TIMEOUT);
  Serial1.begin(baudRate);
}

int16_t _GSMSerial::SendCommand(const char * command, const char * waitForResponseStr, char * responseStr, int16_t responseStrSize, uint32_t timeout)
{
  uint16_t ResponseCountMatch=0;
  uint16_t ResponseStartPosition=0;
  int16_t ResponseStrIndex=0;
  int16_t ReceivedByte=0;
  uint32_t TimeoutEnd = millis()+timeout;
  
  Flush();
  
  // Send the specified command
  #ifdef GSMSERIAL_DEBUG_ON
  Serial.print("SendCmd=");
  Serial.println(command);
  #endif
  while (*command != '\0')
    Serial1.write(*(command++));
  
  #ifdef GSMSERIAL_DEBUG_ON
  Serial.print("  Resp=");
  #endif

  // Wait for the specified response or until the timeout happens
  while (millis() < TimeoutEnd)
  {
    if (Serial1.available())
    {
      ReceivedByte = Serial1.read();
      #ifdef GSMSERIAL_DEBUG_ON
      Serial.write(ReceivedByte);
      #endif
      // Write the response string away if asked for
      if ((responseStr != NULL) && (ResponseStrIndex < responseStrSize))
      {
        responseStr[ResponseStrIndex] = ReceivedByte;
      }
      ResponseStrIndex++;
      
      if ((waitForResponseStr != NULL) && (waitForResponseStr[ResponseCountMatch] == ReceivedByte))
      {
        if (ResponseCountMatch == 0)
        { 
          if ((responseStr != NULL) && (ResponseStrIndex < responseStrSize))
            ResponseStartPosition = ResponseStrIndex;
          else 
            ResponseStartPosition = 0;
        }
        
        ResponseCountMatch++;
        if (ResponseCountMatch >= strlen(waitForResponseStr))
        {
          if ((responseStr != NULL) && (ResponseStrIndex < responseStrSize))
            responseStr[ResponseStrIndex] = '\0';
          
          #ifdef GSMSERIAL_DEBUG_ON
          Serial.println(" SUCCESS");
          #endif
          
          Flush();
          return ResponseStartPosition;
        }
      }
      /*
      else if ((waitForResponseStr == NULL) && ((ReceivedByte == '\r') || (ReceivedByte == '\n')))
      {
        break;
      }*/
      else
      {
        ResponseCountMatch = 0;
        ResponseStartPosition = 0;
      }
    }
  }
  
  if ((responseStr != NULL) && (ResponseStrIndex < responseStrSize))
    responseStr[ResponseStrIndex] = '\0';
  
  #ifdef GSMSERIAL_DEBUG_ON
  Serial.println(" FAIL");
  #endif
  
  Flush();
  
  return -1;
}

int16_t _GSMSerial::IsDataAvailable()
{
  if (Serial1.available())
    return Serial1.available();
  else
    return -1;
}

int16_t _GSMSerial::Read(char * data, int16_t bufferSize, uint16_t timeout)
{
  Serial1.setTimeout(timeout);
  int16_t bytesReceived = Serial1.readBytes(data, bufferSize);
  
  #ifdef GSMSERIAL_DEBUG_ON
  Serial.print("ReadBytes=");
  Serial.print(bytesReceived);
  Serial.print("  Data=");
  for (int i=0;i<bytesReceived;i++)
    Serial.write(data[i]);
  Serial.println();
  #endif
  
  return bytesReceived;
}

void _GSMSerial::Write(const char * data, int16_t bufferSize)
{
  if (bufferSize < 0)
    bufferSize = strlen(data);
  
  #ifdef GSMSERIAL_DEBUG_ON
    Serial.print("WriteBytes=");
    Serial.print(bufferSize);
    Serial.print(" Data=");
  #endif
  
  for (int i=0;i<bufferSize;i++)
  {
    Serial1.write(data[i]);
    
    #ifdef GSMSERIAL_DEBUG_ON
      Serial.write(data[i]);
    #endif
  }
  
  #ifdef GSMSERIAL_DEBUG_ON
    Serial.println();
  #endif
}

void _GSMSerial::Flush()
{
  char data;
  
  Serial1.flush();
  while (Serial1.available())
    data = Serial1.read();
}
