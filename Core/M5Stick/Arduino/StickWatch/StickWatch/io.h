#ifndef _IO_H_
#define _IO_H_

#define BtnPin 35
#define LedPin 19
#define BuzzerPin 26

void setPinModes() {
  // pinMode(BtnPin, INPUT_PULLDOWN);
  pinMode(BtnPin, INPUT_PULLUP);
  pinMode(LedPin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
}

void buzzer() {
  for (int i = 0; i < 200; i++) {
    digitalWrite(BuzzerPin, HIGH);
    delayMicroseconds(20);
    digitalWrite(BuzzerPin, LOW);
    delayMicroseconds(980);
  }
}

void led(boolean on, int milliseconds) {
  digitalWrite(LedPin, on ? 1 : 0);
  delay(milliseconds);
}

void flashLed(int milliseconds) {
  for (int i = 0; i < milliseconds / 200; i++) {
    digitalWrite(LedPin, 1);
    delay(100);
    digitalWrite(LedPin, 0);
    delay(100);
  }
}


#endif // _IO_H_
