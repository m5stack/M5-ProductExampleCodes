/*
  Example for DHT12 Sensor
*/

#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h> //The DHT12 uses I2C comunication.

void setup() {
    M5.begin();
    Wire.begin();
    M5.Lcd.setBrightness(10);
    Serial.println(F("ENV Unit(DHT12 and BMP280) test..."));
}

void loop() {
    float tmp = dht12.readTemperature();
    float hum = dht12.readHumidity();
    Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%\r\n", tmp, hum);

    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\n", tmp, hum);

    delay(100);
}