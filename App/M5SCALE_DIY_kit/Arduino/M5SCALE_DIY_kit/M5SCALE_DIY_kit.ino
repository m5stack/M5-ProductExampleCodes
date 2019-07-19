#include"HX711.h"
#include<M5StickC.h>

int Weight = 0;
void setup() {
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.drawString("SCALE", 80, 0, 4);  
   Init_Hx711();
   Get_Gross();   //clear the weight
   M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);  
   Serial.begin(115200);
     
}
 
void loop() {  
   M5.update(); 
//   if (M5.BtnA.wasReleased()) {
//      Get_Maopi();
//    }
     Weight = Get_Weight();
     M5.Lcd.setCursor(40,30,4);
     M5.Lcd.fillRect(0, 30, 160, 30, TFT_BLACK);
     M5.Lcd.printf("%d g", Weight);
     M5.Lcd.fillRect(0, 70, 160, 10, TFT_BLACK);
     M5.Lcd.fillRect(0, 70, Weight*0.016, 10, TFT_YELLOW);
     delay(100);  

}
