#include <M5Stack.h>

bool ButtonA = false;
bool ButtonC = false;
// the setup routine runs once when M5StickC starts up
void setup(){
  // Initialize the M5StickC object
  M5.begin();
  pinMode(15, OUTPUT);
  pinMode(12, OUTPUT);
  M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.drawString("BASE26 RELAY", 160, 0, 4);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(0, 100, 2);
  M5.Lcd.println("Press control the RELAYs BY ButtonA and ButtonC");
  M5.Lcd.fillRect(40, 195, 20, 20, WHITE);
  M5.Lcd.fillRect(45, 200, 10, 10, RED); 
  M5.Lcd.fillRect(245, 195, 20, 20, WHITE);
  M5.Lcd.fillRect(250, 200, 10, 10, RED); 
}

// the loop routine runs over and over again forever
void loop() {
   if (M5.BtnA.wasPressed()){      
       if (ButtonA == false){
           ButtonA = true;
            M5.Lcd.fillRect(40, 195, 20, 20, WHITE);
            M5.Lcd.fillRect(45, 200, 10, 10, GREEN);
            digitalWrite(15, HIGH);
            Serial.println("green"); 
       }else{
           ButtonA = false;
            M5.Lcd.fillRect(40, 195, 20, 20, WHITE);
            M5.Lcd.fillRect(45, 200, 10, 10, RED); 
            digitalWrite(15, LOW);
            Serial.println("red"); 
       }
   }
   if (M5.BtnC.wasPressed()){      
       if (ButtonC == false){
          ButtonC = true;
          M5.Lcd.fillRect(245, 195, 20, 20, WHITE);
          M5.Lcd.fillRect(250, 200, 10, 10, GREEN);
          digitalWrite(12, HIGH);
          Serial.println("green"); 
   }else{
          ButtonC = false;
          M5.Lcd.fillRect(245, 195, 20, 20, WHITE);
          M5.Lcd.fillRect(250, 200, 10, 10, RED);
          digitalWrite(15, LOW);
          Serial.println("red"); 
   }
}
   M5.update();
}
