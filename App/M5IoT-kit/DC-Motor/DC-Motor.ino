/*
    Name:       DC-Motor.ino
    Created:    2019/5/17 10:06:15
    Author:     shaoxiang
*/

#include <M5Stack.h>
// The setup() function runs once each time the micro-controller starts
void setup() {
  // init lcd, serial, but don't init sd card
  M5.begin(true, false, true);
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(65, 10);
  M5.Lcd.println("DC motor example");
  M5.Lcd.setCursor(30, 220);
  M5.Lcd.println("Forward");
  M5.Lcd.setCursor(140, 220);
  M5.Lcd.println("stop");
  M5.Lcd.setCursor(220, 220);
  M5.Lcd.println("Reverse");
  
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(13, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
}

// Add the main program code into the continuous loop() function
void loop() {
  M5.update();
 
  if (M5.BtnA.wasReleased()) {
    M5.Lcd.print('A');
    digitalWrite(22, LOW);
    digitalWrite(21, HIGH);
  } else if (M5.BtnB.wasReleased()) {
    M5.Lcd.print('B');
    digitalWrite(21, LOW);
    digitalWrite(22, LOW);
  } else if (M5.BtnC.wasReleased()) {
    M5.Lcd.print('C');
    digitalWrite(21, LOW);
    digitalWrite(22, HIGH);
  } 
}
