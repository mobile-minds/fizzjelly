#include <Time.h>
#include <MM_GSM.h>
#include <MM_GSM_HTTP.h>
#include <MM_GSM_Serial.h>

GSM gsm;
GSM_HTTP gsm_http;

void setup()
{
  Serial.begin(115200);
  
  // Wait for the usb serial port to be ready
  while (!Serial);
  
  Serial.println("GSM HTTP Library Test");

  Serial.println();
  
  Serial.print(F("Init Library..."));
  gsm.begin();
  gsm_http.begin(&gsm.serial);
  Serial.println("done.");
  
  Serial.print(F("Powering on module..."));
  gsm.SetModuleOn(true);
  gsm.PORInit(NULL);
  Serial.println(F("done."));
  
  // Give a moment for the module to settle
  //delay(1000);
  
  Serial.print(F("Registering to network..."));
  while(gsm.GetNetworkStatus() != GSM_NETSTAT_REGISTERED);
  Serial.println(F("done."));
  
  network_connect();
}

void network_connect()
{
  int8_t resp;
  int8_t attemptCount;
  
  Serial.print(F("Connecting GPRS..."));
  attemptCount = 3;
  do
  {
    resp = gsm_http.open("JTM2M","","");
    attemptCount--;
  }
  while ((resp) && attemptCount > 0);
  
  if (resp)
  {
    Serial.println(F("Failed."));
    Serial.println();
    Serial.println(F("Hit space to restart"));
    while(Serial.read() != ' ');
    network_connect();
  }
  
  Serial.println("done.");
}


char receivedPacket[368];

void loop()
{
  //char receivedPacket[128];
  char host[] = "api.fizzjelly.com";
  uint16_t port = 80;
  char path[] = "/devices/5/sensors/4/datapoints";
  char headers[] = "X-API-KEY: 025ee969e80de4393045f54b7c30a4e0\r\n";
  char querystr[] = "";
  char postdata[128];
  int16_t receivedPacketLength;
  
  Serial.println();
  Serial.println(F("Hit space to continue"));
  while(Serial.read() != ' ');
  
  Serial.print(F("Creating dummy sensor data: "));
  sprintf(postdata, "{\"type\":\"integer\",\"value\":%d}", 44);
  Serial.println(postdata);
  
  Serial.print(F("Initalising HTTP call..."));
  gsm_http.setHost(host);
  gsm_http.setPort(port);
  gsm_http.setPath(path);
  gsm_http.setQueryString(querystr);
  gsm_http.setExtraHeaders(headers);
  gsm_http.setPostData(postdata, strlen(postdata));
  Serial.println(F("Done."));
  
  Serial.print(F("Making HTTP request..."));
  receivedPacketLength = gsm_http.HTTPRequest(RequestPOST, receivedPacket, sizeof(receivedPacket), false);
  Serial.println(F("Done."));
  
  Serial.println();
  Serial.println();
  if (receivedPacketLength > 0)
  {
    Serial.print(F("Received Packet (Size: "));
    Serial.print(receivedPacketLength,DEC);
    Serial.println(F(" bytes):"));
    uint8_t i;
    for (i=44;i>0;i--)
      Serial.print(F("-"));
      
    Serial.println();
    Serial.write((uint8_t *)receivedPacket, receivedPacketLength);
    Serial.println();
    
    for (i=44;i>0;i--)
      Serial.print(F("-"));
    Serial.println();
    Serial.println();
  }
}
