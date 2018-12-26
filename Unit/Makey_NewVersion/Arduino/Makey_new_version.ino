#include <M5Stack.h>
#include <Wire.h>

#define NOTE_D1 294

/* we just use Alto and Treble*/
// #define NOTE_DL1 147
// #define NOTE_DL2 165
// #define NOTE_DL3 175
// #define NOTE_DL4 196
// #define NOTE_DL5 221
// #define NOTE_DL6 248
// #define NOTE_DL7 278

#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 1120

// #define USE_M5CORE_BEEP

void setup() {
  // Power ON Stabilizing...
  delay(500);
  M5.begin();
  pinMode(21, INPUT);
  pinMode(22, INPUT);
  // Init I2C
  Wire.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.println("M5Stack Makey Piano.");
}
int Key1 = 0, Key2 = 0, Index = 0;
void showKey() {

  M5.Lcd.setCursor(20, 200);
  M5.Lcd.setTextColor(BLUE, BLACK);
  M5.Lcd.print(Index);
  M5.Lcd.print("   ");

  M5.Lcd.setCursor(5 + 0 * 16, 110);

  /* Alto */
  if ((Key1 & (0x01 << 0)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D1, 20);
#endif
  }
  M5.Lcd.print("1");                    //Alto 1
  M5.Lcd.setCursor(5 + 1 * 16, 110);


  if ((Key1 & (0x01 << 1)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D2, 20);
#endif
  }
  M5.Lcd.print("2");
  M5.Lcd.setCursor(5 + 2 * 16, 110);


  if ((Key1 & (0x01 << 2)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D3, 20);
#endif
  }
  M5.Lcd.print("3");
  M5.Lcd.setCursor(5 + 3 * 16, 110);


  if ((Key1 & (0x01 << 3)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D4, 20);
#endif
  }
  M5.Lcd.print("4");
  M5.Lcd.setCursor(5 + 4 * 16, 110);


  if ((Key1 & (0x01 << 4)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D5, 20);
#endif
  }
  M5.Lcd.print("5");
  M5.Lcd.setCursor(5 + 5 * 16, 110);


  if ((Key1 & (0x01 << 5)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D6, 20);
#endif
  }
  M5.Lcd.print("6");
  M5.Lcd.setCursor(5 + 6 * 16, 110);


  if ((Key1 & (0x01 << 6)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_D7, 20);
#endif
  }
  M5.Lcd.print("7");
  M5.Lcd.setCursor(10 + 7 * 16, 110);

  /* Treble */
  if ((Key1 & (0x01 << 7)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH1, 20);
#endif
  }

  if ((Key2 & (0x01 << 0)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH2, 20);
#endif
  }


  if ((Key2 & (0x01 << 1)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH3, 20);
#endif
  }


  if ((Key2 & (0x01 << 2)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH4, 20);
#endif
  }


  if ((Key2 & (0x01 << 3)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH5, 20);
#endif
  }


  if ((Key2 & (0x01 << 4)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH6, 20);
#endif
  }


  if ((Key2 & (0x01 << 5)) == 0x00)
    M5.Lcd.setTextColor(WHITE, BLACK);
  else {
    M5.Lcd.setTextColor(RED, BLACK);
#ifdef USE_M5CORE_BEEP
    M5.Speaker.tone(NOTE_DH7, 20);
#endif
  }

  // if ((Key2 & (0x01 << 6)) == 0x00)
  //   M5.Lcd.setTextColor(WHITE, BLACK);
  // else {
  //   M5.Lcd.setTextColor(RED, BLACK);
  //   M5.Speaker.tone(NOTE_DH1, 20);
  // }

  // if ((Key2 & (0x01 << 7)) == 0x00)
  //   M5.Lcd.setTextColor(WHITE, BLACK);
  // else {
  //   M5.Lcd.setTextColor(RED, BLACK);
  //   M5.Speaker.tone(NOTE_DH2, 20);
  // }
}

int CommandStatus = 0;
void loop() {
  Wire.requestFrom(0x51, 2);
  while (Wire.available()) {
    Key1 = Wire.read();
    Key2 = Wire.read();
    Index++;
  }
  showKey();
  // delay(10);
  M5.update();
}
