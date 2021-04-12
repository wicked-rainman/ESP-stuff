//Heltec board with built in oled display
//UDP client to diplay weather values off a remote BME280 (Temp, pressure and humidity)

#include "Arduino.h"
#include "heltec.h"
#include <WiFi.h>
#include <WiFiUdp.h>
const char ssid[]="ssid";
const char pass[]="pass";
WiFiUDP udp;

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
  Heltec.display->drawString(0, 0, IpAddress2String(WiFi.localIP()));
  Heltec.display->display();
  udp.begin(5679);

}

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
      
      //Extract pressure
      RelPress[0] = '\0';
      pos = strpos(",", weatherptr);
      strncpy(RelPress, weatherptr, pos);
      RelPress[pos]='\0';
      strcat(RelPress,"mb");
      weatherptr += (pos+1);

      //Extract Humidity
      outHumi[0] = '\0';
      pos = strpos(",", weatherptr);
      strncpy(outHumi, weatherptr, pos);
      outHumi[pos] = 0;
      strcat(outHumi, "%");
      weatherptr += (pos+1);

      //Extract temp
      outTemp[0] = '\0';
      //pos=strpos(",",weatherChars);
      strcpy(outTemp, weatherptr);
      outTemp[strlen(outTemp) - 1] = '\0';
      strcat(outTemp, "c");

      Heltec.display->clear();
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_24);

      Heltec.display->drawString(2, 5, outTemp);
      Heltec.display->drawString(75, 5, outHumi);
      Heltec.display->setFont(ArialMT_Plain_24);
      Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
      Heltec.display->drawString(55, 40, RelPress);
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
