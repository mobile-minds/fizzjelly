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
#include "MM_GSM.h"
#include "MM_GSM_Serial.h"
#include "MM_GSM_SMS.h"
#include "MM_GSM_HTTP.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "../Time/Time.h"

/*********************************************************************************/
// GSM


int8_t GSM::begin(boolean powerOn, char * SIM_PINCode)
{
  int8_t resp;
  uint32_t timeout;
  
  pinMode(GSM_IO_POWERKEY, OUTPUT);
  pinMode(GSM_IO_POWERREG, OUTPUT);
  digitalWrite(GSM_IO_POWERKEY, HIGH);
  digitalWrite(GSM_IO_POWERREG, LOW);
  
  GSMSerial.begin(GSM_BAUD_RATE);
  
  if (powerOn)
  {
    // Power the module on.
    resp = this->SetModuleOn(true);
    if (resp) return resp;
    
    // Initalise the module into a known powered on state.
    resp = this->PORInit(SIM_PINCode);
    if (resp) return resp;
    
    // Wait for the network to connect
    timeout = millis() + GSM_NETWORK_REGISTER_TIMEOUT;
    
    while (timeout > millis())
    {
      if (this->GetNetworkStatus() == GSM_NETSTAT_REGISTERED)
        return 0;
    }
    
    return -1;
  }
  
  return 0;
}

int8_t GSM::SetModuleOn(uint8_t state)
{
  int attempts = 4;
  if (state)
  {
    // Switch module On
    
    // Turn the power to the module on first
    digitalWrite(GSM_IO_POWERREG, HIGH);
    
    // Wait a moment for the power to settle
    delay(100);
    
    // Turn the module on
    digitalWrite(GSM_IO_POWERKEY, LOW);
    delay(2500);
    digitalWrite(GSM_IO_POWERKEY, HIGH);
    
    // Wait a moment for the module to settle
    delay(2500);
    
    // Keep hitting the module with AT until it responds
    while(attempts > 0)
    {
      // Turn off command echo on the module.
      // Also the AT at the start of the command does a auto-baud rate
      //  sorting two things at once.
      if (GSMSerial.SendCommand("ATE0\r","OK") > -1)
      {
        // Wait a moment for the module to properly start-up
        delay(1000);

        return 0;
      }
      attempts--;
      
      // Give a moment before we try again.
      delay(1000);
    }
    return -1;
  }
  else
  {
    // attempt to shut down the module cleanly
    while(attempts > 0)
    {
      if (GSMSerial.SendCommand("AT+CPOWD=1\r","NORMAL POWER DOWN") > -1)
        break;
      attempts--;
      delay(2500);
    }
    
    // Did the last action fail?
    if (attempts <= 0)
    {
      // Switch module Off
      // Turn the module off
      digitalWrite(GSM_IO_POWERKEY, LOW);
      delay(1500);
      digitalWrite(GSM_IO_POWERKEY, HIGH);
    }
    
    // Wait a moment for the module to shut off
    delay(1000);
    
    // Remove the modules power
    digitalWrite(GSM_IO_POWERREG, LOW);
    
    return 0;
  }
}

int8_t GSM::PORInit(char * SIM_PINCode)
{

  // Check to see if the SIM is ready and needs a pin code
  switch (this->GetSIMStatus())
  {
    case GSM_SIM_READY:
      // Nothing to do, so continue
      break;
    case GSM_SIM_PIN_REQUIRED:
      // Have we been given a PIN code to use?
      if ((SIM_PINCode != NULL) && (strlen(SIM_PINCode) > 0))
      {
        // Send over the PIN Code
        if (this->SetSIMPIN(SIM_PINCode))
        {
          // failed
          return -1;
        }
      }
      else
      {
        // We haven't been given a pincode, so there isn't much we can do here
        return -1;
      }
      break;
    default:
      // Something gone wrong. Probably no SIM Card inserted.
      return -1;
  }
  
  // Allow the respective classes to init the module for themselves
  if (GSM_SMS::PORInit())
    return -1;
  if (GSM_HTTP::PORInit())
    return -1;
  
  return 0;
}

int8_t GSM::GetSignalStrength()
{
  char response[64];
  int responseLen=0;
  
  if ((responseLen=GSMSerial.SendCommand("AT+CSQ\r","OK",response,sizeof(response))) > -1)
  {
    response[responseLen]='\0';
    char * RSSIStr = NULL;
    RSSIStr = strstr(response,"+CSQ: ");
    if (RSSIStr != NULL)
    {
      RSSIStr += (sizeof("+CSQ: ")-1);
      int RSSIStrength = atoi(RSSIStr);
      if ((RSSIStrength >= 0) && (RSSIStrength <= 31))
        return map(RSSIStrength,0,31,0,100);
      else
        return -1;
    }
  }
  return -1;
}

int8_t GSM::SetSIMPIN(char * PINCode)
{
  char command[32];
  
  sprintf(command, "AT+CPIN=%s\r", PINCode);
  if (GSMSerial.SendCommand(command,"OK") > -1)
  {
    return 0;
  }
  return -1;
}

int8_t GSM::GetSIMStatus()
{
  char response[64];
  int responseLen=0;
  
  if ((responseLen=GSMSerial.SendCommand("AT+CPIN?\r","OK",response,sizeof(response))) > -1)
  {
    if (strstr(response, "READY") > 0)
      return GSM_SIM_READY;
    if (strstr(response, "SIM PIN") > 0)
      return GSM_SIM_PIN_REQUIRED;
    if (strstr(response, "SIM PUK") > 0)
      return GSM_SIM_PUK_REQUIRED;
    else
      return -1;
  }
  
  return 0;
}

int8_t GSM::GetNetworkStatus()
{
  char response[64];
  int responseLen=0;
  
  if ((responseLen=GSMSerial.SendCommand("AT+CREG?\r","OK",response,sizeof(response))) > -1)
  {
    if (strstr(response, "+CREG") > 0)
    {
      char * responseCodeStr = strchr(response, ',');
      if (responseCodeStr != NULL)
      {
        switch (atoi(responseCodeStr+1))
        {
          case 1:
          case 5:
            return GSM_NETSTAT_REGISTERED;
          case 2:
            return GSM_NETSTAT_REGISTERING;
          case 3:
            return GSM_NETSTAT_REGISTERFAILED;
        }
      }
    }
  }
  
  return -1;
}




int8_t GSM::GetNetworkDateTime(GSM_NETWORK_TIME * dateTime )
{
  
  char response[64];
  int responseLen=0;
  int i = 0 ;
  
  
    for (i=0; i<sizeof(dateTime->NetworkDate); i++)
        dateTime->NetworkDate[i] = 0;
    
    dateTime->NetworkDateLen = 0;
    
    for (i=0; i<sizeof(dateTime->NetworkTime); i++)
        dateTime->NetworkTime[i] = 0;
        
    dateTime->NetworkTimeLen = 0;
  
  //enable the clock  
  if ( (responseLen=GSMSerial.SendCommand("AT+CLTS=1\r","OK",response,sizeof(response))) < 1)
    return -1;
   
  //get the date and time
  if ((responseLen=GSMSerial.SendCommand("AT+CCLK? \r","OK",response,sizeof(response))) > 1)
  {
    response[responseLen]='\0';
    
    char *DateStr = strchr(response, '\"')+1;
    char *TimeStr = strchr(response, ',')+1;
    
    if (DateStr != NULL)
    {
     memcpy(dateTime->NetworkDate, DateStr, 8);
     dateTime->NetworkDateLen = strlen(dateTime->NetworkDate);
     dateTime->NetworkDate[dateTime->NetworkDateLen] = '\0';
    } 
    
    
    if (TimeStr != NULL)
    {
     memcpy(dateTime->NetworkTime, TimeStr, 8);
     dateTime->NetworkTimeLen = strlen(dateTime->NetworkTime);
     dateTime->NetworkTime[dateTime->NetworkTimeLen] = '\0';
    }  
    
    return 0;
  
  }
  else
  {
    return -1;
  } 
  
}  
