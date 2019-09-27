#include "esp32-hal-ledc.h"
#include <M5StickC.h>
#define COUNT_HIGH 8000
#define COUNT_LOW  3000
#define COUNT_MID 5000
#define TIMER_WIDTH 16
#define CHANNEL 0
#define FREQUANCE 50
#define PIN 26


String comData = "";
String lastContent = "";
bool flag;
void setup() {
  M5.begin();
  // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
  Serial2.begin(115200, SERIAL_8N1, 33, 32);
  ledcSetup(CHANNEL, FREQUANCE, TIMER_WIDTH);
  ledcAttachPin(PIN, CHANNEL);
}

void loop() {
  delay(20);
  if(Serial2.available()){
    comData = "";
    while(Serial2.available()){
    char ch = Serial2.read();
      if(ch != '\r'){
        comData += ch;
      }
    }
    if(comData == "clear"){
       M5.Lcd.fillScreen(BLACK);
       ledcWrite(CHANNEL, 1800);
       return;     
    }
     if(lastContent != comData){
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.print(comData);
      if(comData == "M5Stick"){
        ledcWrite(CHANNEL, COUNT_LOW);    
      }else if(comData == "M5StickC"){
        ledcWrite(CHANNEL, COUNT_HIGH);
      }else if(comData == "M5Stack Fire"){
        ledcWrite(CHANNEL, COUNT_MID);
      }
    }
  }
}    
