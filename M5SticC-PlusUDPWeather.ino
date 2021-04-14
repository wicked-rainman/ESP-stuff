#include <M5StickCPlus.h>

//UDP Weather client for M5stickc 
//Displays the content of UDP packets sent from an ESP connected to a BME280
//
#include <SPI.h>
#include <WiFiClient.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <WiFiMulti.h>
#include <WiFi.h>
#include <ETH.h>
#include <WiFiAP.h>
#include <WiFiType.h>
#include <WiFiGeneric.h>
#include <WiFiSTA.h>

int status = WL_IDLE_STATUS;
WiFiUDP Udp;
const char* ssid= "?????";
const char* pass= "?????";
char packetBuffer[255];
int clientCount = 0;
const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
struct tm timeinfo;

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(3);
  M5.Lcd.printf("Connecting ");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.printf(".");
    delay(1000);
  }
  M5.Lcd.setCursor(1, 50);
  M5.Lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());
  M5.Lcd.setTextSize(3);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Udp.begin(5679);
}

void loop() {
  char weatherChars[1024];
  char outTemp[10];
  char outHumi[10];
  char RelPress[10];
  float pres, baro, temp;
  int pos;
  char mmRain[10];
  int packetSize = 0;
  int payloadSize = 0;
  char *weatherptr;
  packetSize = Udp.parsePacket();
  if (packetSize) {
    payloadSize = Udp.read(weatherChars, 255);
    if (payloadSize > 0) {
      weatherChars[payloadSize] = 0;
      weatherptr = weatherChars;
      RelPress[0] = '\0';
      outHumi[0] = '\0';
      outTemp[0] = '\0';
      
      getLocalTime(&timeinfo);

      M5.Lcd.fillScreen(BLACK);
      //Draw a stupid frame round the values that get printed
      M5.Lcd.drawLine(8, 8, 230, 8, GREEN); //top horizontal line
      M5.Lcd.drawLine(8, 130, 230, 130, GREEN); //bottom horizontal line
      M5.Lcd.drawLine(8, 45, 230, 45, GREEN); //Second horizontal line
      M5.Lcd.drawLine(8, 82, 230, 82, GREEN); //Third horizontal line
      M5.Lcd.drawLine(8, 8, 8, 130, GREEN); //left
      M5.Lcd.drawLine(230, 8, 230, 130, GREEN); //right
      M5.Lcd.drawLine(120, 45, 120, 82, GREEN); //Break between humidity and temp

      //Extract and print pressure
      pos = strpos(",", weatherptr);
      strncpy(RelPress, weatherptr, pos);
      RelPress[pos] = 0;
      weatherptr += (pos + 1);
      M5.Lcd.setCursor(45, 14);
      pres = strtof(RelPress, NULL);
      if (pres > 1000) M5.Lcd.setTextColor(GREEN);
      else M5.Lcd.setTextColor(ORANGE);
      strcat(RelPress, "mb");
      M5.Lcd.print(RelPress);

      //Extract and print Humidity
      pos = strpos(",", weatherptr);
      strncpy(outHumi, weatherptr, pos);
      outHumi[pos] = 0;
      strcat(outHumi, "%");
      weatherptr += (pos + 1);
      M5.Lcd.setCursor(40, 53);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.print(outHumi);

      //Extract and print temp
      strcpy(outTemp, weatherptr);
      outTemp[strlen(outTemp) - 1] = '\0';
      strcat(outTemp, "C");
      temp = strtof(outTemp, NULL);
      if (temp > 24) M5.Lcd.setTextColor(RED);
      else if (temp > 10) M5.Lcd.setTextColor(GREEN);
      else if (temp > 5) M5.Lcd.setTextColor(WHITE);
      else M5.Lcd.setTextColor(BLUE);
      M5.Lcd.setCursor(125, 53);
      M5.Lcd.print(outTemp);

      //Print time
      M5.Lcd.setCursor(50, 95);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.print(&timeinfo, "%H:%M:%S");
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
