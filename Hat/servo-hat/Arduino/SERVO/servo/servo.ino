#define COUNT_LOW 1500
#define COUNT_HIGH 8500
#define TIMER_WIDTH 16
#include "esp32-hal-ledc.h"
#include <M5StickC.h>

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  ledcSetup(1, 50, TIMER_WIDTH);
  ledcAttachPin(26, 1);
  M5.Lcd.setCursor(20, 80, 2);
  M5.Lcd.print("SERVO");

}

void loop() {
  // put your main code here, to run repeatedly:
      ledcWrite(1, 8500);
    delay(50);
}
