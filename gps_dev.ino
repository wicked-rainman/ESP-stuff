#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Atom.h>
#define MAX_INPUT_LINE_LENGTH 150
#define UDP_PORT 5680
WiFiUDP udp;
IPAddress SendIP(192,168,1,255);
const char *ssid =  "Virus";
const char *pass =  "50158980";
char packet[200];
HardwareSerial GPSRaw(2);
char ch;
int packetSize;

//--------------------------------------------------------------------
void setup() {
  M5.begin();
  WiFi.begin(ssid,pass);
  while(WiFi.status() !=WL_CONNECTED) {
    delay(100);
  }
  GPSRaw.begin(9600, SERIAL_8N1, 32, 26);
  Serial.begin(115200);
  memset(packet, 0, 200);
  packetSize = 0;
  ch = '\0';
  udp.begin(UDP_PORT);
}
//-------------------------------------------------------------------
void loop() {
  while (GPSRaw.available()) {
    ch = GPSRaw.read();
    //If char is a newline, then complete sentence received.
    //Output it as a UDP broadcast
    if (ch == '\n') {
      packet[packetSize]='\0';
      //Ditch any GPTXT sentences and output anything else
      if(memcmp("$GPTXT",packet,6)!=0) {
        udp.beginPacket(SendIP,UDP_PORT);
        udp.print(packet);
        udp.endPacket();
      }
      packetSize = 0;
      memset(packet,0,200);
    }
    //If Char is anything other than a newline, add it to the sentence
    else packet[packetSize++] = ch;

    //If the count of characters in the current line are too great,
    //then throw the line away
    if (packetSize >= MAX_INPUT_LINE_LENGTH) {
      memset(packet,0,200);
      packetSize = 0;
    }
  }
}
