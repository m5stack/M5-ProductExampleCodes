#include <M5Stack.h>

#include "Wire.h"

#define JOY_ADDR 0x52
void setup() {
    M5.begin();
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(40, 0);
    M5.Lcd.println("Dual Button example");
    M5.Lcd.setTextColor(WHITE);
    // disable the speak noise
    dacWrite(25, 0);

    Wire.begin(21, 22, 400000UL);
}

uint8_t x_data;
uint8_t y_data;
uint8_t button_data;
char data[100];
void loop() {
    Wire.requestFrom(JOY_ADDR, 3);
    if (Wire.available()) {
        x_data      = Wire.read();  // X(range: 10~250)
        y_data      = Wire.read();  // Y(range: 10~250)
        button_data = Wire.read();  // Z(0: released 1: pressed)
        sprintf(data, "x:%d y:%d button:%d\n", x_data, y_data, button_data);
        Serial.print(data);
        M5.Lcd.fillRect(10, 100, 100, 25, BLACK);
        M5.Lcd.setCursor(10, 100);
        M5.Lcd.print(data);
    }
    delay(200);
}
