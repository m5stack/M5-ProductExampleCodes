#include "M5Atom.h"

#define RX_PIN      32
#define TX_PIN      26

void setup() {
  M5.begin(true, false, true);
  Serial.begin(115200);   
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  if(M5.Btn.wasPressed()) {
    Serial2.write("hello");
    M5.dis.drawpix(0, 0x00f000);
  }
  while(Serial2.available()){
    char c = Serial2.read();
    Serial.print(c);
    M5.dis.drawpix(0, 0x0000f0);
  }
    delay(20);  
    M5.dis.drawpix(0, 0x000000);
    M5.update();
}
