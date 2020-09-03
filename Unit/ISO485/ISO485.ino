/*
 neo pixel test

   hardwware:  M5StackFire 
   Pressed ButtonA :send "hello world"
   Pressed ButtonB :send "ON";
   Pressed ButtonB :send "OFF";

   received "ON" : turn on ledbar
   received "OFF" : turn on ledbar

 please install the Adafruit library first!
 September 2018, ChrisMicro
*/

#include <Adafruit_NeoPixel.h>
#include <M5Stack.h>

#include "TFT_Terminal.h"

#define M5STACK_FIRE_NEO_NUM_LEDS 10
#define M5STACK_FIRE_NEO_DATA_PIN 15

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(M5STACK_FIRE_NEO_NUM_LEDS, M5STACK_FIRE_NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);


String str = "";

void Light_ON(){
    for(int pixelNumber = 0; pixelNumber < 9; pixelNumber++){
        pixels.setPixelColor(pixelNumber, pixels.Color(255, 255, 255));
    }
    pixels.show();
    delay(10);
}

void Light_OFF(){
    for(int pixelNumber = 0; pixelNumber < 9; pixelNumber++){
        pixels.setPixelColor(pixelNumber, pixels.Color(0, 0, 0));
    }
    pixels.show();
    delay(10);
}

void setup() {

    M5.begin();
    termInit();
    pixels.begin(); 
    M5.Lcd.drawString("ISO485", 120, 0, 2);
  
    // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() {
    
   if(M5.BtnA.wasPressed()){
      Serial2.write("HelloWorld\r\n");
   }
   if(M5.BtnB.wasPressed()){
      Serial2.write("ON\r\n");
   }
   if(M5.BtnC.wasPressed()){
      Serial2.write("OFF\r\n");
   }
   if(Serial2.available()) {                                       
     char ch = Serial2.read();
     str += ch;
     termPutchar(ch);
     
     if(str.indexOf("ON\r\n") != -1){
      Serial.println("on");

     }else if(str.indexOf("OFF\r\n") != -1){
      Light_OFF();
      Serial.println("off");
     }
     if(str.endsWith("\r\n")){
      Serial.println(str);
      str = "";      
     }
   }
   M5.update();
}
