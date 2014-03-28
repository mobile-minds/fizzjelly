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
#include "MM_GSM_SMS.h"
#include "MM_GSM_Serial.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "../Time/Time.h"

// DEBUG_SMS_NOSEND - When defined, SMS's will be sent to the Serial debug 
//                     console, rather than being sent to save SMS credits.
//#define DEBUG_SMS_NOSEND

/*********************************************************************************/
// GSM_SMS

void GSM_SMS::begin()
{

}

int8_t GSM_SMS::PORInit()
{
  // Set parameters useful to us when doing sms
  // at+cmgr=1 - Send SMS message length in the header
  if (GSMSerial.SendCommand("AT+CSDH=1\r", "OK") < 0)
  {
    return -1;
  }
  
  // at+cmgf=1 - Send SMS message data as Text (rather than the SMS standard PDU format)
  if (GSMSerial.SendCommand("AT+CMGF=1\r", "OK") < 0)
  {
    return -1;
  }
  
  return 0;
}

int8_t GSM_SMS::Send(const char * PhoneNumber, const char * MessageText)
{
  // Build the message up from the passed parameters
  GSM_SMS_Message message;
  
  // Copy the phone number across
  strcpy(message.PhoneNumber, PhoneNumber);
  message.PhoneNumberLen = strlen(PhoneNumber);
  
  // Copy the message across
  strcpy(message.Message, MessageText);
  message.MessageLen = strlen(MessageText);
  
  // Send the message
  return this->Send(&message);
}

int8_t GSM_SMS::Send(GSM_SMS_Message * Message)
{
  char command[64];
  char response[64];
  int responseLen=0;
  
  // Set the last character of the phone number to null space
  // This is to allow sprintf to work properly and not bomb out rubbish characters
  if (Message->PhoneNumberLen >= sizeof(Message->PhoneNumber))
    Message->PhoneNumber[sizeof(Message->PhoneNumber)-1]='\0';
  else
    Message->PhoneNumber[Message->PhoneNumberLen]='\0';
  
  sprintf(command,"AT+CMGS=\"%s\",%i\r",Message->PhoneNumber, (Message->PhoneNumber[0]=='+')?145:129);
  
  #ifdef DEBUG_SMS_NOSEND
    int i=0;
    Serial.print("*DEBUG* Sending (");
    for (i=0;i<Message->PhoneNumberLen;i++)
      Serial.print(Message->PhoneNumber[i]);
    Serial.print(") Message:");
    for (i=0;i<Message->MessageLen;i++)
      Serial.print(Message->Message[i]);
    Serial.println();
    return 0;
  #else
    if ((responseLen=GSMSerial.SendCommand(command, ">", response, sizeof(response))) > -1)
    {
      // Now send the message
      GSMSerial.Write(Message->Message, Message->MessageLen);
      
      // Finish the message off with the Ctrl-D
      char EOT[]={0x1A, 0x00};
      if ((responseLen=GSMSerial.SendCommand(EOT, "OK", response, sizeof(response), GSM_SMS_TIMEOUT_MESSAGESEND)) > -1)
      {
        return 0;
      }
    }
    return -1;
  #endif
}

int8_t GSM_SMS::Read(uint8_t index, GSM_SMS_Message * Message)
{
  char command[16];
  char response[300];
  int responseLen=0;
  int i=0;
  
  sprintf(command, "AT+CMGR=%i\r", index);
  responseLen=GSMSerial.SendCommand(command, "OK", response, sizeof(response), GSM_SMS_TIMEOUT_MESSAGEREAD);
  if (responseLen > -1)
  {
    // See if we got the +CMGR back in the command to tell us that we actually received an SMS
    if (strstr(response,"+CMGR"))
    {
      // Make sure that the given struct is blank, to prevent any string over runs
      for (i=0; i<sizeof(Message->Message); i++)
        Message->Message[i] = 0;
      for (i=0; i<sizeof(Message->PhoneNumber); i++)
        Message->PhoneNumber[i] = 0;
      Message->MessageLen = 0;
      Message->PhoneNumberLen = 0;
      
      /* dig out the following from the header:
       *   - phone number
       *   - phone number length
       *   - sent time
       *   - message size
       *   - message data
       */
      char * PhoneNumberStr = strstr(response,"\",\"")+3;
      int PhoneNumberLen = strstr(PhoneNumberStr,"\",\"") - PhoneNumberStr;
      
      char * SentTimeStr = strstr(strstr(PhoneNumberStr,"\",\"")+3,"\",\"")+3;
      //int SentTimeLen = strstr(SentTimeStr, "\",") - SentTimeStr;
      
      // for the message size field, its the 8th field (,) from the sent time
      //   - remember to include the commar in the time/date field!
      char * MessageSizeStr = SentTimeStr;
      for (i=0;i<8;i++)
        // using strchr instead of strstr for speed
        MessageSizeStr = strchr(MessageSizeStr, ',')+1;
      
      int MessageSize = atoi(MessageSizeStr);
      
      char * MessageData = strchr(MessageSizeStr,'\r')+1;
      // Is the next character a newline (i.e. cr+lf enabled)?
      if (MessageData[0] == '\n')
        MessageData += 1;
      
      // Copy all the data in
      memcpy(Message->PhoneNumber, PhoneNumberStr, PhoneNumberLen);
      Message->PhoneNumberLen=PhoneNumberLen;
      Message->PhoneNumber[Message->PhoneNumberLen] = '\0';
      
      memcpy(Message->Message, MessageData, MessageSize);
      Message->MessageLen = MessageSize;
      Message->Message[Message->MessageLen] = '\0';
      
      // Convert the time into something useful
      TimeElements tm;
      tm.Year = y2kYearToTm(atoi(&(SentTimeStr[0])));
      tm.Month = atoi(&(SentTimeStr[3]));
      tm.Day = atoi(&(SentTimeStr[6]));
      tm.Hour = atoi(&(SentTimeStr[9]));
      tm.Minute = atoi(&(SentTimeStr[12]));
      tm.Second = atoi(&(SentTimeStr[15]));
      int TimeZoneCorrection = atoi(&(SentTimeStr[17]));
      
      // Time Zone is in quarter hour (15 minute) chunks. So +4 = 1 hr
      //  We need to convert this into seconds to be useful anyway
      TimeZoneCorrection *= (60*15);
      
      // Save away the time into our message structure
      Message->SentTime = makeTime(tm);
      // Add our timezone difference to make it the actual time and date sent
      Message->SentTime += TimeZoneCorrection;
      
      return 0;
    }
    // No SMS in the location
    return -2;
  }
  // Error occured in the command
  return -1;
}

int8_t GSM_SMS::Delete(uint8_t index)
{
  char command[32];
  
  sprintf(command, "AT+CMGD=%i\r", index);
  
  if (GSMSerial.SendCommand(command, "OK", NULL, -1, GSM_SMS_TIMEOUT_MESSAGEDELETE) > -1)
  {
    return 0;
  }
  
  return -1;
}

int8_t GSM_SMS::FindNext(GSM_SMS_Message * Message, int8_t lastIndex)
{
  int i=0;
  int TotalMessages = this->TotalCount();
  
  for (i=lastIndex;i<=TotalMessages;i++)
  {
    if (!this->Read(i, Message))
    {
      // Found a valid message
      return i;
    }
  }
  
  // Not found a valid message
  return -1;
}


int8_t GSM_SMS::ReceivedCount()
{
  char response[64];
  int responseLen=0;
  
  if ((responseLen=GSMSerial.SendCommand("AT+CPMS=\"SM\"\r","OK",response,sizeof(response),GSM_SMS_TIMEOUT_MESSAGECOUNT)) > -1)
  {
    response[responseLen]='\0';
    char * MsgCntStr = NULL;
    MsgCntStr = strstr(response, "+CPMS: ");
    if (MsgCntStr != NULL)
    {
      MsgCntStr += (sizeof("+CPMS: ")-1);
      int MsgCnt = atoi(MsgCntStr);
      return MsgCnt;
    }
  }
  return -1;
}

int8_t GSM_SMS::TotalCount()
{
  char response[64];
  int responseLen=0;
  
  if ((responseLen=GSMSerial.SendCommand("AT+CPMS=\"SM\"\r","OK",response,sizeof(response),GSM_SMS_TIMEOUT_MESSAGECOUNT)) > -1)
  {
    response[responseLen]='\0';
    char * MsgCntStr = NULL;
    MsgCntStr = strstr(response, "+CPMS: ");
    if (MsgCntStr != NULL)
    {
      MsgCntStr += (sizeof("+CPMS: ")-1);
      MsgCntStr = strstr(MsgCntStr, ",");
      MsgCntStr += 1;
      int MsgTotalCnt = atoi(MsgCntStr);
      return MsgTotalCnt;
    }
  }
  return -1;
}
