//------------------------------------------------------------------------------------
//Heltec board with built in oled display
//UDP client to diplay weather values sent from a remote esp32 with attached BME280 
//(Temp, pressure and humidity). See ESP8266-bm280-weather-stn.c in this git
//------------------------------------------------------------------------------------
#include "Arduino.h"
#include "heltec.h"
#include <WiFi.h>
#include <WiFiUdp.h>
const char ssid[] = "an ssid";
const char pass[] = "a password";
WiFiUDP udp;
//----------------------------------
// Setup
//----------------------------------
void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->display();
  WiFi.disconnect();
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Heltec.display->drawString(0, 0, "Connecting");
    Heltec.display->display();
  }
  Heltec.display->clear();
  Heltec.display->drawString(0, 5, IpAddress2String(WiFi.localIP()));
  Heltec.display->display();
  udp.begin(5679);
}
//-----------------------
// Main loop
//-----------------------
void loop() {
  char weatherChars[1024];
  char outTemp[10];
  char outHumi[10];
  char RelPress[10];
  char *weatherptr;
  int pos;
  int packetSize = 0;
  int payloadSize = 0;
  packetSize = udp.parsePacket();
  if (packetSize) {
    payloadSize = udp.read(weatherChars, 255);
    if (payloadSize > 0) {
      weatherChars[payloadSize] = 0;
      weatherptr=weatherChars;
      Serial.println(weatherChars);
      
      //Jump over the Epoc date field
      pos = strpos(" ",weatherptr);
      weatherptr += (pos+1);

      //Extract temp
      outTemp[0] = '\0';
      pos=strpos(" ",weatherptr);
      strncpy(outTemp, weatherptr,pos);
      outTemp[pos] = '\0';
      strcat(outTemp, "c");
      weatherptr+=(pos+1);

      //Extract Humidity
      outHumi[0] = '\0';
      pos = strpos(" ", weatherptr);
      strncpy(outHumi, weatherptr, pos);
      outHumi[pos] = 0;
      strcat(outHumi, "%");
      weatherptr += (pos+1);
      
      //Extract pressure
      RelPress[0] = '\0';
      strcpy(RelPress, weatherptr);
      RelPress[strlen(RelPress)]='\0';
      strcat(RelPress,"mb");
      
      //Write the values to the builtin screen
      Heltec.display->clear();
      Heltec.display->setFont(ArialMT_Plain_24);
      Heltec.display->drawString(2, 8, outTemp);
      Heltec.display->drawString(75, 8, outHumi);
      Heltec.display->drawString(5, 40, RelPress);
      Heltec.display->display();

    }
  }
}

int strpos(char *needle, char *haystack) {
  char *offset;
  offset = strstr(haystack, needle);
  if (offset != NULL) {
    return offset - haystack;
  }
  return -1;
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") + \
         String(ipAddress[1]) + String(".") + \
         String(ipAddress[2]) + String(".") + \
         String(ipAddress[3])  ;
}
