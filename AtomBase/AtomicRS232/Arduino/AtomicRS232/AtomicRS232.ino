/*When the serial port sends or receives messages, the LED lights up,Press button to send character*/


#include <M5Atom.h>


void setup() {

  M5.begin(true, false, true);
  // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
  Serial2.begin(115200, SERIAL_8N1, 22, 19);
}

void loop() {
 if(M5.Btn.wasPressed()){
    M5.dis.drawpix(0, 0xff00ff);
    M5.update();
    Serial2.write('a');
    delay(50);
 }else{
    M5.dis.drawpix(0, 0x000000);
 }
 if(Serial2.available()) {
   int ch = Serial2.read();
   Serial.write(ch);
   M5.dis.drawpix(0, 0x00ffff);
   M5.update();
   delay(50);
 }else{
    M5.dis.drawpix(0, 0x000000);
 }
 M5.update();
}
