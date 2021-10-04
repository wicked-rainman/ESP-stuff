//---------------------------------------------------------
// Generic ESP 8266 board connected to a BME280.
// Epoc time stamp,temperature, humidity and pressure output 
// on UDP port as a local broadcast
//---------------------------------------------------------
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <time.h>
#define LED  16
#define UDP_PORT 5679               //Whatever port you choose
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
const char *ssid =  "xxxxx";
const char *pass =  "xxxxx";
char packet[50];
char pressure[10];
char temperature[10];
char humidity[10];
int packetSize;
WiFiUDP udp;
Adafruit_BME280 bme;
IPAddress SendIP(192, 168, 1, 255); //Broadcast address for a LAN.
bool status;

void setup() {
  pinMode(LED, OUTPUT); //Big LED near USB socket
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
  digitalWrite(LED, LOW);
  status = bme.begin(0x76);
  if (!status) {
    while (1) { //Could not find the sensor
      digitalWrite(LED, HIGH);
      delay(150);
      digitalWrite(LED, LOW);
      delay(150);
    }
  }
  packetSize = 0;
  udp.begin(UDP_PORT);
  timeClient.begin();
}

void loop() {
  timeClient.update();
  digitalWrite(LED, LOW);
  memset(packet, 0, 50);
  snprintf(packet, 50, "%d %2.1f %2.0f %2.2f\r\n", timeClient.getEpochTime(), bme.readTemperature(), bme.readHumidity(), (bme.readPressure() / 100));
  udp.beginPacket(SendIP, UDP_PORT);
  udp.write(packet);
  udp.endPacket();
  delay(200);
  digitalWrite(LED, HIGH);
  delay(4800);
}
