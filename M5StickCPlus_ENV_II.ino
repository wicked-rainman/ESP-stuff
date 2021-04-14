// The M5 ENVII grove add-on contains a BMP280 and SHT30.
// Read the pressure from the BMP280 and the temperature and humidity from the SHT30
// Write these values out to the built in screen

#include <M5StickCPlus.h>
#include "SHT3X.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>

SHT3X sht3x;
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
  if (sht3x.get() == 0) {
    M5.Lcd.setCursor(0, 20, 4);
    M5.Lcd.printf("Temp: %2.1f", sht3x.cTemp);
    M5.Lcd.setCursor(0, 50, 4);
    M5.Lcd.printf("Humi: %2.0f", sht3x.humidity);
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
