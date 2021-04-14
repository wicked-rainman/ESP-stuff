// The M5 ENVII grove add-on contains a BMP280 and SHT30.
// Read the pressure from the BMP280 and the temperature and humidity from the SHT30
// Write these values out to the built in screen

#include <M5StickCPlus.h>
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;
bool bmp280_found;

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  Wire.begin();
  bmp280_found = bmp.begin(0x76);
}

void loop() {
  unsigned int data[6];
  float tempC,humidity;
  int i;
  //Wire talk to the SHT30 to get temp and humidity
  Wire.beginTransmission(0x44);
  Wire.write(0x2C);
  Wire.write(0x06);
  if (Wire.endTransmission() == 0) {
    delay(500);
    //Read byte data
    Wire.requestFrom(0x44, 6);
    for (i = 0; i < 6; i++) {
      data[i] = Wire.read();
    };
    delay(50);
    if (Wire.available() == 0) {
      //Got valid data
      tempC = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
      humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
      M5.Lcd.setCursor(0, 20, 4);
      M5.Lcd.printf("Temp: %2.1f", tempC);
      M5.Lcd.setCursor(0, 50, 4);
      M5.Lcd.printf("Humi: %2.0f", humidity);
    }
  }
  else {
    M5.Lcd.setCursor(0, 20, 4);
    M5.Lcd.print("Temp: Unknown");
    M5.Lcd.setCursor(0, 50, 4);
    M5.Lcd.print("Humi: Unknown");
  }

  if (bmp280_found) {
    M5.Lcd.setCursor(0, 80, 4);
    M5.Lcd.printf("Pressure: %2.1f", (bmp.readPressure() / 100.0));
  }
  else {
    M5.Lcd.setCursor(0, 70, 4);
    M5.Lcd.print("Pressure: Unknown");
  }
  delay(1000);
}


