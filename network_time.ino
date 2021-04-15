#include <M5StickCPlus.h>
#include <WiFi.h>
#include "time.h"
const char* ssid="???";
const char* pass="???";
struct tm timeinfo;
const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void setup() {
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  getLocalTime(&timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
