//----------------------------------------------------------------------------
//M5Atom lite connected via Grove connector (I2c) to an ENV II sensor.
//Connection made to a time server via Internet to establish date time stamp.
//Data broadcasted to local area network using UDP
//----------------------------------------------------------------------------
#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_BMP280.h>
#include "UNIT_ENV.h"
#include "time.h"
#define UDP_PORT 5670
#define RED 0xff0000
#define ORANGE 0xff9900
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define WHITE 0xffffff

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
struct tm timeinfo;
char tstr[30];
char packet[50];
IPAddress SendIP(192, 168, 1, 255);     //UDP broadcast address
WiFiUDP udp;
SHT3X sht30;
Adafruit_BMP280 bme;
const char *ssid =  "xxxxxxx";
const char *pass =  "xxxxxxx";
bool status;

void setup() {
  M5.begin(true, false, true);
  delay(50);
  Wire.begin(26, 32);                   //Re-define the default I2C pins for Atom lite
  if (!bme.begin(0x76)) {
    while (1) FlashPix(200, WHITE);
  }
  WiFi.begin(ssid, pass);
  M5.dis.setBrightness(5);               //Max is 60, but actually seems to make no difference
  while (WiFi.status() != WL_CONNECTED) {
    FlashPix(200, RED);
  }
  M5.dis.drawpix(0, GREEN);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  udp.begin(UDP_PORT);
}

void loop() {
  sht30.get();
  memset(packet, 0, 50);
  if (getLocalTime(&timeinfo)) {
    strftime(tstr, 25, "%F %X", &timeinfo);
    snprintf(packet, 50, "#1 %s %2.1fC %2.0f%% %2.2f\r\n", tstr, sht30.cTemp, sht30.humidity, (bme.readPressure() / 100));
    udp.beginPacket(SendIP, UDP_PORT);
    udp.printf("%s",packet);
    udp.endPacket();
    if(sht30.cTemp >=25) FlashPix(1000,RED);
    else if(sht30.cTemp >=20) FlashPix(1000,ORANGE);
    else if(sht30.cTemp >=10) FlashPix(1000,GREEN);
    else if(sht30.cTemp >=5) FlashPix(1000,WHITE);
    else FlashPix(1000, BLUE);
    delay(3000);                    //With the LED flash, this gives a delay of 5 secs for each UDP packet
  }
}

void FlashPix(int flashtime, int rgbcolor) {
  M5.dis.drawpix(0, rgbcolor);
  delay(flashtime);
  M5.dis.clear();
  delay(flashtime);
}
