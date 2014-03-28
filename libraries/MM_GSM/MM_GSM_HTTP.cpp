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
#include "MM_GSM_HTTP.h"
#include "MM_GSM_IP.h"
#include "MM_GSM_Serial.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

// DEBUG_SMS_NOSEND - When defined, SMS's will be sent to the Serial debug 
//                     console, rather than being sent to save SMS credits.
//#define DEBUG_SMS_NOSEND

/*********************************************************************************/
// GSM_HTTP
static char * GSM_HTTP_strnstr (const char * str1, const char * str2, int Length);

static const char _GSM_HTTP_DefaultContentType[] = "application/json";



GSM_HTTP::GSM_HTTP(GSM_IP* _gsmip)
{
  _MMIP = _gsmip;
  _path = NULL;
  _querystr = NULL;
  _postdata = NULL;
  _postsize = 0;
  _returnedstatuscode = 0;
  _contenttype = _GSM_HTTP_DefaultContentType;
}

/*

GSM_HTTP::GSM_HTTP()
{
 // _host = NULL;
 // _port = 80;
  _path = NULL;
  _querystr = NULL;
  _postdata = NULL;
  _postsize = 0;
  _returnedstatuscode = 0;
  _contenttype = _GSM_HTTP_DefaultContentType;
}

*/

void GSM_HTTP::begin()
{
  
}

int8_t GSM_HTTP::PORInit()
{
  // Nothing to do here
  
  return 0;
}


int8_t GSM_HTTP::open()
{
  int resp = -1;
  int attemptCount = 5;
  
  if ( _MMIP->getAPN() != NULL)
  {
    do
    {
        resp = this->open(_MMIP->getAPN() ,"","");
        attemptCount--;
        delay(100);
    }
    while ((resp) && attemptCount > 0);
  }
  else
  {
     return resp  ;
  }
  return resp ;
}

int8_t GSM_HTTP::open(const char* APN, const char* Username, const char* Password)
{
  char cmd[64];
  
  // Configure the PDP context
  sprintf(cmd,"AT+CGDCONT=1,\"IP\",\"%s\"\r", APN);
  if (GSMSerial.SendCommand(cmd, "OK") < 0)
  {
    return -1;
  }
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Enable the GPRS context - may take a while, so increase the timeout a bit
  if (GSMSerial.SendCommand("AT+CGATT=1\r", "OK", NULL, -1, GSM_HTTP_TIMEOUT_GPRS_CONTEXT) < 0)
  {
    return -1;
  }
  
  /*
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Set the NetStat light to flash fast when GPRS is active
  if (serialObj->SendCommand("AT+CSGS=1\r", "OK") < 0)
  {
    // not critical if this isn't set
    continue;
  }
  */
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Shutdown any existing GPRS connections - likely to happen if the module is still on after a reset
  if (GSMSerial.SendCommand("AT+CIPSHUT\r", "SHUT OK") < 0)
  {
    // Not critical if this fails (no connections may be active)
  }
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Set notification on new bytes. We need to do this as our buffers are so small
  if (GSMSerial.SendCommand("AT+CIPRXGET=1\r", "OK") < 0)
  {
    return -1;
  }
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
    
  // Set the APN, username and password
  sprintf(cmd,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r", APN, Username, Password);
  if (GSMSerial.SendCommand(cmd, "OK") < 0)
  {
    return -1;
  }

  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Bring up the GPRS connection
  if (GSMSerial.SendCommand("AT+CIICR\r", "OK", NULL, -1, GSM_HTTP_TIMEOUT_GPRS_CONTEXT) < 0)
  {
    return -1;
  }
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Get the local IP address
  //  Note: Seems neccessary otherwise the module won't respond correctly
  GSMSerial.SendCommand("AT+CIFSR\r", NULL, cmd, sizeof(cmd));
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  /*
  // Add number of bytes received at the start of each incoming data stream
  if (GSMSerial.SendCommand("AT+CIPHEAD=1\r", "OK") < 0)
  {
    return -1;
  }

  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  */
  
  return 0;
}

int8_t GSM_HTTP::close()
{
  // Wait a moment for any previous commands to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  // Shutdown any existing GPRS connections
  if (GSMSerial.SendCommand("AT+CIPSHUT\r", "SHUT OK") < 0)
  {
    return -1;
  }
  
  // Wait a moment for the command to take effect
  delay(GSM_HTTP_DELAY_COMMAND);
  
  return 0;
}

/*
void GSM_HTTP::setHost(const char* Host)
{
  this->_host = Host;
}


void GSM_HTTP::setPort(uint16_t Port)
{
  this->_port = Port;
}
*/

void GSM_HTTP::setPath(const char* Path)
{
  this->_path = Path;
}

void GSM_HTTP::setQueryString(const char* QueryStr)
{
  this->_querystr = QueryStr;
}

void GSM_HTTP::setExtraHeaders(const char* ExtraHeaders)
{
  this->_extraheaders = ExtraHeaders;
}

void GSM_HTTP::setPostData(const char* POSTData, uint16_t POSTSize)
{
  this->_postdata = POSTData;
  this->_postsize = POSTSize;
}

void GSM_HTTP::setContentType(const char* ContentType)
{
  if (ContentType == NULL)
    this->_contenttype = _GSM_HTTP_DefaultContentType;
  else
    this->_contenttype = ContentType;
}

// HTTPRequest
// Notes: 
//   - Must always have a ContentResponse string passed in
//   - If returnHeader is false, but the ContentResponse buffer isn't large enough to fit 
//      the whole header, then the header will be returned.
//     
int16_t GSM_HTTP::HTTPRequest(RequestType_t RequestType, char* ContentResponse, int16_t ContentResponseLen, uint8_t returnHeader, uint32_t Timeout)
{
  char     buff[64];
  char *   headerEndPtr;
  const char *   host;
  uint16_t port;
  int16_t  buff_bytes=-1;
  int16_t  content_bytes=-1;
  int16_t  header_bytes=-1;
  
  
  //make sure the host and port are set
  if ( _MMIP->getHost() != NULL)
  {
    host = _MMIP->getHost();
  }
  else
  {
    return -1;
  }
  
  if ( _MMIP->getPort() != 0)
  {
    port = _MMIP->getPort();
  }
  else
  {
    return -1;
  }
  
  // Make sure any existing connection is closed
  GSMSerial.SendCommand("AT+CIPCLOSE=0\r","SHUT OK");
    
  sprintf(buff, "AT+CIPSTART=\"TCP\",\"%s\",%d\r", host, port);
  
  // Connect out to the server
  if (GSMSerial.SendCommand(buff, "CONNECT OK", NULL, -1, GSM_HTTP_TIMEOUT_SOCKET_OPEN) < 0)
  {
    return -1;
  }
   
  // Start packet transfer
  // We do this in one hit to prevent any packet fragmentation 
  //   (and thus increasing incurred data costs)
  // Also the send is done as open ended. This makes processing easier/quicker this side,
  //   but has the disadvantage that we can't have any data that contains the SUB ascii (0x1A) character
  if (GSMSerial.SendCommand("AT+CIPSEND\r", ">", NULL, -1, GSM_HTTP_TIMEOUT_SEND_START) < 0)
  {
    return -1;
  }
  
  // Send the HTTP request header
  // format e.g.: GET /path HTTP/1.1
  // Host: www.iotstr.com
  switch (RequestType)
  {
    case RequestGET: GSMSerial.Write("GET "); break;
    case RequestPOST: GSMSerial.Write("POST "); break;
    case RequestPUT: GSMSerial.Write("PUT "); break;
    case RequestDELETE: GSMSerial.Write("DELETE "); break;
    default: return -1;
  }
  
  GSMSerial.Write(this->_path);
  if (this->_querystr != NULL)
  {
    GSMSerial.Write("?");
    GSMSerial.Write(this->_querystr);
  }
  GSMSerial.Write(" HTTP/1.1\r\nHost: ");
  GSMSerial.Write(host);
  GSMSerial.Write("\r\n");
  //GSMSerial.Write("Connection: Keep-Alive\r\n");
  
  // Got data to send out?
  if (this->_postdata != NULL)
  {
    // Now send how much data we're about to chuck out
    sprintf(buff, "Content-Length: %d\r\n", this->_postsize);
    GSMSerial.Write(buff);
    // Send out the content type too
    sprintf(buff, "Content-Type: %s\r\n", this->_contenttype);
    GSMSerial.Write(buff);
  }
  
  // headers to send?
  if (this->_extraheaders != NULL)
    GSMSerial.Write(this->_extraheaders,strlen(this->_extraheaders));
  
  // Finish headers
  GSMSerial.Write("\r\n");
  
  // If we have data to send out, do it now
  if (this->_postdata != NULL)
    GSMSerial.Write(_postdata, _postsize);

  
  // All headers have been now sent
  // Terminate and wait till send completes
  if (GSMSerial.SendCommand("\x1A", "SEND OK", NULL, -1, GSM_HTTP_TIMEOUT_SEND_ACTION) < 0)
  {
    if (GSMSerial.SendCommand("AT+CIPCLOSE=1\r", "CLOSE OK", NULL, -1, GSM_HTTP_TIMEOUT_SEND_ACTION) < 0)
      return -1;
    else
      return -2;
  }
  
  // Receive the response
  buff_bytes = 0;
  //uint32 LoopTimeOutTimer=0;
  //while(LoopTimeOutTimer)
  boolean ReceivedHeader=false;
  boolean gotStatusCode=false;
  int16_t outBufferPos=0;
  int16_t i=0;
  uint32_t TimeoutEnd = millis()+Timeout;
  
  while (millis() < TimeoutEnd)
  {
    // buffer full?
    if (outBufferPos >= ContentResponseLen)
      // get out of here, as we can't store any more data anyway
      break;
    
    // got all data that we can (based on the content-length field)
    if (content_bytes > -1)
    {
      if (content_bytes >= (outBufferPos-(outBufferPos-header_bytes)))
      {
        break;
      }
    }
    
    // Process any received commands from the module
    if (GSMSerial.IsDataAvailable())
      buff_bytes += GSMSerial.Read(buff+buff_bytes, sizeof(buff)-buff_bytes);
    
    // Data to process?
    if (buff_bytes > 0)
    {
      // Is there a newline at the end?
      if (GSM_HTTP_strnstr(buff,"\r",buff_bytes) != NULL)
      {
        if (GSM_HTTP_strnstr(buff,"+CIPRXGET:1",buff_bytes) != NULL)
        {
          // New data arrived in buffer
          // Pull it in
          if (outBufferPos < ContentResponseLen)
          {
            outBufferPos += this->getTCPData(ContentResponse+outBufferPos, (ContentResponseLen-outBufferPos));
          }
          else
          {
            // Buffer full, close the connection and get out of here
            break;
          }
          
          // Process the new data that's come in
          
          // have we got the status code?
          if (!gotStatusCode)
          {
            // this will be on the first line after the HTTP command
            // e.g. HTTP/1.0 200 OK
            char * statusCodeStr = strchr(ContentResponse, ' ');
            if (statusCodeStr != NULL)
            {
              statusCodeStr+=1;
              this->_returnedstatuscode = atoi(statusCodeStr);
              
              gotStatusCode=true;
            }
          }
          
          if (!ReceivedHeader)
          {
            // Have we had the header come in yet?
            headerEndPtr = GSM_HTTP_strnstr(ContentResponse,"\r\n\r\n",outBufferPos);
            if (headerEndPtr != NULL)
            {
              headerEndPtr+=4;
              
              // Pull out the total bytes to receive from the content-length field
              char *contentLengthField = GSM_HTTP_strnstr(ContentResponse,"Content-Length: ",outBufferPos);
              if (contentLengthField != NULL)
              {
                // Skip off the field itself
                contentLengthField += 16;
                
                // Retrieve the number of bytes of the content size
                content_bytes = atoi(contentLengthField);
                header_bytes = headerEndPtr - ContentResponse;
                
                // we got any more bytes to come in?
                if (content_bytes >= (outBufferPos-header_bytes))
                {
                  // If not, trigger a timeout next time around
                  TimeoutEnd = millis();
                }
              }
              
              // Stripping out the header?
              if (!returnHeader)
              {
                // Move everything from the end of the header into the start of the buffer
                //   to strip out the header.
                // But only do this if we have the header size (otherwise we won't know 
                //   what to strip off). This should always be the case though, otherwise we 
                //   won't have the header end characters (\r\n\r\n)
                if (header_bytes > 0)
                {
                  i=0;
                  while (i < (outBufferPos-header_bytes))
                  {
                    ContentResponse[i] = headerEndPtr[i];
                    i++;
                  }
                  outBufferPos=i;
                }
                
                // Room for more bytes?
                // We do this again, as we may have bytes from the last receive packet left over
                if (outBufferPos < ContentResponseLen)
                {
                  // Add more data to the response buffer
                  outBufferPos += this->getTCPData(ContentResponse+outBufferPos, (ContentResponseLen-outBufferPos));
                }
                
              }
              
              ReceivedHeader = true;
            }
          }
        }
        else if(GSM_HTTP_strnstr(buff,"CLOSED",buff_bytes) != NULL)
        {
          // connection closed, we're finished
          break;
        }
        else if(GSM_HTTP_strnstr(buff,"ERROR",buff_bytes) != NULL)
        {
          // got a problem
          
        }
        buff_bytes=0;
      }
    }
  }
  
  // Close the connection
  GSMSerial.SendCommand("AT+CIPCLOSE=1\r","SHUT OK");
  
  // return the number of bytes we've received
  return outBufferPos;
}

int16_t GSM_HTTP::getTCPData(char * buffer, int16_t bytes)
{
  char cmd[32];
  int16_t bufferBytes=0;
  uint8_t gotHeader=0;
  int16_t bytesonModuleLeft=0;
  int16_t i;
  
  // Clear the buffers
  memset(buffer, 0x00, bytes);
  GSMSerial.Flush();
  
  // Tell the module to give us some bytes of data
  sprintf(cmd, "AT+CIPRXGET=2,%d\r", bytes);
  GSMSerial.Write(cmd);
  
  // Read the first chunk in
  while (bufferBytes < bytes)
  {
    if (GSMSerial.IsDataAvailable())
    {
      // If not got the header yet, then pull small bits of data in first
      if ((!gotHeader) && ((bytes-bufferBytes)>64))
        bufferBytes += GSMSerial.Read(buffer, 64);
      else
        bufferBytes += GSMSerial.Read(buffer+bufferBytes, bytes-bufferBytes);
      
      if (bufferBytes > 0)
      {
        if (!gotHeader)
        {
          // Process any of the received header data
          char* cmdPos = GSM_HTTP_strnstr(buffer, "+CIPRXGET:2,", bufferBytes);
          if ((cmdPos != NULL) && (strchr(cmdPos,'\r') != NULL))
          {
            // skip the command off to be read in
            cmdPos+=12;
            // find how many bytes we need to pull in
            bytes = atoi(cmdPos);
            
            // find out how many bytes left on the module
            cmdPos = strchr(cmdPos, ',');
            cmdPos+=1;
            bytesonModuleLeft = atoi(cmdPos);
            
            // find the end of the line
            cmdPos = strchr(cmdPos, '\n');
            cmdPos+=1;
            /*if (cmdPos[0] == '\n')
              cmdPos+=1;*/
            
            // move all characters from the end to the start of the buffer
            i=0;
            while(cmdPos[i])
            {
              buffer[i] = cmdPos[i];
              i++;
            }
            
            // Reset the buffer size
            bufferBytes=i;
            
            // Set out flag that the header has been pulled out
            gotHeader = true;
          }
        }
      }
    }
  }
  
  // Return number of bytes read.
  // This will be less than the original buffer size if we've read all data
  //  in the module buffer.
  return bufferBytes;
}

uint16_t GSM_HTTP::getStatusCode()
{
  return this->_returnedstatuscode;
}

static char * GSM_HTTP_strnstr (const char * str1, const char * str2, int Length)
{
  char *cp = (char *) str1;
  char *s1, *s2;

  if ( !*str2 )
    return((char *)str1);

  while (*cp && Length-->0)
  {
    s1 = cp;
    s2 = (char *) str2;

    while ( *s1 && *s2 && !(*s1-*s2) && Length>0)
    {
      s1++, s2++;
      Length--;
    }

    if(Length==0)
      return NULL;

    if (!*s2)
      return(cp);

    cp++;
  }

  return(NULL);
}