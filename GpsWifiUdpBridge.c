//--------------------------------------------------------------------
// Program: GpsWifiBridge.c
// Hardware: M5Atom lite and M5 BDS GPS on I2c
// Purpose: Enables distribution of GPS sentences over 
// Wifi as a UDP broadcast
//--------------------------------------------------------------------
#include <WiFi.h>
#include <WiFiUdp.h>
#include <M5Atom.h>
#define MAX_INPUT_LINE_LENGTH 150
#define UDP_PORT 5680
#define ORANGE 0xff9900
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define WHITE 0xffffff
WiFiUDP udp;
IPAddress SendIP(192,168,1,255);
const char *ssid =  "xxxxxx";
const char *pass =  "xxxxxxx";
char packet[200];
HardwareSerial GPSRaw(2);
char ch;
int packetSize;

//--------------------------------------------------------------------
void setup() {
  M5.begin(true,true,true);
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
    if ((ch == '\n') && (packet[0] == '$')) {
      packet[packetSize]='\0';
      //Ditch any GPTXT sentences and output anything else
      if(memcmp("$GPTXT",packet,6)!=0) {
        FlashPix(8,WHITE);
        udp.beginPacket(SendIP,UDP_PORT);
        udp.print(packet);
        udp.endPacket();
        //Serial.println(packet);
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

void FlashPix(int flashtime, int rgbcolor) {
  M5.dis.drawpix(0, rgbcolor);
  delay(flashtime);
  M5.dis.clear();
  delay(flashtime);
}
