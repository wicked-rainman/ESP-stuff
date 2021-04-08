// NodeMCU (Amica) 
// Broadcasts UDP packets containing the values read from a BME280 connected on I2C
//

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SCL  5
#define SDA  4
#define MISO 12
#define MOSI 13
#define SCLK 14
#define CS   15
#define LED  16
#define UDP_PORT 5679

const char *ssid =  "??????";    
const char *pass =  "???????";
char packet[200];
char pressure[10];
char temperature[10];
char humidity[10];
int packetSize;
WiFiUDP udp;
Adafruit_BME280 bme;
IPAddress SendIP(192,168,1,255);
bool status; 
void setup() {

  pinMode(LED, OUTPUT); //Big LED near USB socket
  Serial.begin(9600);
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
        while (1){ //Could not find the sensor
          digitalWrite(LED,HIGH);
          delay(150);
          digitalWrite(LED,LOW);
          delay(150);
        }
    }
  packetSize = 0;
  udp.begin(UDP_PORT);
}

void loop() {
  memset(packet,0,200);
  memset(pressure,0,10);
  memset(temperature,0,10);
  memset(humidity,0,10);
  snprintf(pressure, sizeof(pressure),"%.1f,", (bme.readPressure()/100)); 
  strcpy(packet,pressure);
  snprintf(humidity,sizeof(humidity),"%.0f,", bme.readHumidity());
  strcat(packet,humidity);
  snprintf(temperature,sizeof(temperature),"%.1f",bme.readTemperature());
  strcat(packet,temperature);
  strcat(packet,"\n");
  udp.beginPacket(SendIP,UDP_PORT);
  udp.write(packet);
  udp.endPacket();
  delay(5000);
}
