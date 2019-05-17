#include "driver/ledc.h"

int freq = 50;
int ledChannel = 0;
int resolution = 8;
 
void setup() {
 
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(15, ledChannel);
 
}
 
void loop() {
 
    ledcWrite(ledChannel, 6);//0°
    delay(1000);
    ledcWrite(ledChannel, 18);//90°
    delay(1000);
    ledcWrite(ledChannel, 30);//180°
    delay(1000);
    
}
