#include <M5Stack.h>
#include <Wire.h>

#define TONE_D1 294

/* we just use Alto and Treble*/
/* Bass range */
// #define TONE_DL1 147
// #define TONE_DL2 165
// #define TONE_DL3 175
// #define TONE_DL4 196
// #define TONE_DL5 221
// #define TONE_DL6 248
// #define TONE_DL7 278

/* Alto range */
#define TONE_D1 294
#define TONE_D2 330
#define TONE_D3 350
#define TONE_D4 393
#define TONE_D5 441
#define TONE_D6 495
#define TONE_D7 556

/* Treble range */
#define TONE_DH1 589
#define TONE_DH2 661
#define TONE_DH3 700
#define TONE_DH4 786
#define TONE_DH5 882
#define TONE_DH6 990
#define TONE_DH7 1120

#define USE_M5CORE_BEEP 1
#define MAKEY_ADDR 0x51

uint16_t tone_array[] = {
  TONE_D1, TONE_D2, TONE_D3, TONE_D4, TONE_D5, TONE_D6, TONE_D7, TONE_DH1, TONE_DH2, TONE_DH3, TONE_DH4, TONE_DH5, TONE_DH6, TONE_DH7
};

char str;
uint16_t tone_key = 0x0000;

void setup() {
  // Power ON Stabilizing...
  delay(500);
  M5.begin();
  pinMode(21, INPUT); pinMode(22, INPUT);
  Wire.begin();  // Init I2C
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.println("M5Stack Makey Piano.");
}

int Key1 = 0, Key2 = 0, Index = 0;
void showKey() {

  M5.Lcd.setCursor(20, 200);
  M5.Lcd.setTextColor(BLUE, BLACK);

  M5.Lcd.setCursor(5 + 0 * 16, 110);

  for(int i=0; i<14; i++){//"14": for removing Treble1 and Treble2
    if((tone_key & (0x01 << i)) == 0x00){//handle data which's from MAKEY
      M5.Lcd.setTextColor(WHITE, BLACK);
    }
    else{//detectd which button was pressed
      M5.Lcd.setTextColor(RED, BLACK);
    #ifdef USE_M5CORE_BEEP
      M5.Speaker.tone(tone_array[i], 20);
    #endif
    }
    if(i<7){
      itoa((i+1), &str, 10);
      M5.Lcd.print(str);
      M5.Lcd.setCursor(5 + (i+1) * 16, 110);
    }
  }
}

//read data from MAKEY
void loop() {
  Wire.requestFrom(MAKEY_ADDR, 2);
  while (Wire.available()) {
    Key1 = Wire.read();
    Key2 = Wire.read();
    tone_key = (Key2<<8) | Key1;
    // Serial.println(tone_key, HEX);
    Index++;
  }
  showKey();
  M5.update();
}
