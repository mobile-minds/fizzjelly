#include <MM_GSM.h>
#include <MM_GSM_SMS.h>

GSM gsm;
GSM_SMS sms;

// the setup routine runs once when you press reset:
void setup() 
{
  Serial.begin(115200);
  
  // While waiting for the Serial to connect, flash the LED
  while(!Serial)
  {
    digitalWrite(STATUSLED, 1);
    delay(500);
    digitalWrite(STATUSLED, 0);
    delay(1000);
  }
  
  digitalWrite(STATUSLED, 1);
  
  Serial.println("Fizzjelly Send SMS Example");
  Serial.println();
  
  // Start up the module
  Serial.print("Starting up the GSM module...");
  if (gsm.begin())
  {
    // Something went wrong here
    Serial.println("Unable initalising GSM module.");
    while (1);
  }
  Serial.println("Done.");
}

// the loop routine runs over and over again forever:
void loop() 
{
  Serial.print("Sending SMS...");
  sms.Send("07775581453", "Hello World!");
  Serial.println("Done.");
  
  Serial.println("Push the function button to send another SMS.");
  
  // Wait for the function button to be pushed
  while(digitalRead(0) == 1)
  {
    digitalWrite(STATUSLED, 1);
    delay(500);
    digitalWrite(STATUSLED, 0);
    delay(500);
  }  
}
