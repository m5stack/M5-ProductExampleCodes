#include <M5Core2.h>
#include <Fonts/EVA_20px.h>
#include <stdio.h>

void showNum(short int X, short int Y)
{
  char Str[10];
  M5.Lcd.clear(WHITE);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("DISPPLAY Test!");
  M5.Lcd.setCursor(10, 26);
  sprintf(Str,"X:%d", X);
  M5.Lcd.printf(Str);
  M5.Lcd.setCursor(10, 42);
  sprintf(Str,"Y:%d", Y);
  M5.Lcd.printf(Str);
}

void touchsetup()
{
    showNum(0,0);
}

void touchflush()
{
    char X[4];
    char Y[4];
    M5.Lcd.setCursor(10, 10);
    TouchPoint_t pos= M5.Touch.getPressPoint();
    bool touchStateNow = ( pos.x == -1 ) ? false : true;
    if( touchStateNow )
    {
        showNum(pos.x,pos.y);
    }
}

void setup() {
  M5.begin(true, true, true, true);
  Serial.begin(115200);
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  touchsetup();
}

void loop() {
  TouchPoint_t pos= M5.Touch.getPressPoint();
  if(pos.y > 240)
    if(pos.x < 109)
      M5.Lcd.setTextColor(RED);
    else if(pos.x > 218)
      M5.Lcd.setTextColor(BLUE);
    else if(pos.x >= 109 && pos.x <= 218)
      M5.Lcd.setTextColor(GREEN);
  touchflush();
  delay(10);
}
