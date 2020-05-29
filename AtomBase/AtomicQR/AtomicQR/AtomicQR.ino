/*Press button to scan, serial monitor will print information*/

#include  <M5Atom.h>

#define TRIG 23
#define DLED 33

void setup() {
  M5.begin(false, false, true);
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 22, 19);
  M5.dis.drawpix(0, 0x00f000);
  pinMode(TRIG, OUTPUT);
  pinMode(DLED, INPUT);
  digitalWrite(TRIG, HIGH);
}

void loop() {
  M5.update();
  if(digitalRead(39) == LOW){
    digitalWrite(TRIG, LOW);
  }else {
    digitalWrite(TRIG, HIGH);
  }
  if(digitalRead(DLED) == HIGH){
    while(Serial2.available() > 0){
      char ch = Serial2.read();
      Serial.print(ch);
    }
  }
}
