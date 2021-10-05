#include <SPI.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <M5Stack.h>

WiFiUDP Udp;
char ssid[10] = {'A','B','C','D','E','\0'};           //SSID. Change length and value to suit.
char pass[20] = {'p','a','s','s','w','o','r','d','\0'}; //Password. Change length and value to suit.
char weatherChars[250];

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("Connecting ");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.printf(".");
    delay(1000);
  }
  M5.Lcd.clear();
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.print(WiFi.localIP());
  Udp.begin(5679);
  memset(weatherChars, 0, 250);
}

void loop() {
  char tempStr[10],humiStr[10],pressStr[10],epocStr[20],timeStr[10];
  float pressure,temperature;
  int humidity,pos,packetSize,payloadSize;
  time_t epoc;
  struct tm ts;

  packetSize = Udp.parsePacket();
  if (packetSize) {
    payloadSize = Udp.read(weatherChars, 200);
    Serial.print(weatherChars);
    if (payloadSize > 0) {
      weatherChars[payloadSize] = 0;

      //----------------------------------------------------------
      // Extract Epoc
      //----------------------------------------------------------
      epocStr[0] = '\0';
      pos = strpos(" ", weatherChars);
      strncpy(epocStr, weatherChars, pos);
      strcpy(weatherChars, &weatherChars[pos + 1]);
      epoc = atol(epocStr);
      ts = *localtime(&epoc);
      strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &ts);

      //----------------------------------------------------------
      // Extract temp
      //----------------------------------------------------------
      tempStr[0] = '\0'; //outTemp
      pos = strpos(" ", weatherChars);
      strncpy(tempStr, weatherChars, pos);
      strcpy(weatherChars, &weatherChars[pos + 1]);
      temperature = strtof(tempStr,NULL);

      //----------------------------------------------------------
      // Extract Humidity
      //----------------------------------------------------------
      humiStr[0] = '\0';
      pos = strpos(" ", weatherChars);
      strncpy(humiStr, weatherChars, pos);
      strcpy(weatherChars, &weatherChars[pos + 1]);
      humidity = atoi(humiStr);
      
      //----------------------------------------------------------
      //Extract pressure
      //----------------------------------------------------------
      pressStr[0] = '\0'; //RelPess
      pos = strpos(" ", weatherChars);
      strcpy(pressStr, weatherChars);
      pressStr[strlen(pressStr) - 1] = '\0';
      pressure = strtof(pressStr,NULL);

      //----------------------------------------------------------
      //
      // Update the display with wanted values
      //
      //----------------------------------------------------------
      M5.Lcd.clear();
      M5.Lcd.setTextSize(5);

      if(pressure <= 980) M5.Lcd.setTextColor(RED);
      else if(pressure <= 990) M5.Lcd.setTextColor(ORANGE);
      else if(pressure <= 1000) M5.Lcd.setTextColor(YELLOW);
      else M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10,10);
      M5.Lcd.printf("%2.2f", pressure);

      if(humidity >= 95) M5.Lcd.setTextColor(RED);
      else if(humidity >= 75) M5.Lcd.setTextColor(ORANGE);
      else M5.Lcd.setTextColor(GREEN);
      M5.Lcd.setCursor(10,60);
      M5.Lcd.printf("%d%%",humidity);

      //Display temperature
      if(temperature>=24) M5.Lcd.setTextColor(RED);
      else if(temperature >=20) M5.Lcd.setTextColor(ORANGE);
      else if(temperature >=15) M5.Lcd.setTextColor(YELLOW);
      else if(temperature >=10) M5.Lcd.setTextColor(GREEN);
      else if(temperature >=5) M5.Lcd.setTextColor(WHITE);
      else M5.Lcd.setTextColor(BLUE);
      M5.Lcd.setCursor(10, 110);
      M5.Lcd.printf("%2.1fC",temperature);

      //Display reading time
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setCursor(10,170);
      M5.Lcd.printf("%s",timeStr);
    }
  }
}


int strpos(const char *needle, const char *haystack) {
  char *offset;
  offset = strstr(haystack, needle);
  if (offset != NULL) {
    return offset - haystack;
  }
  return -1;
}
