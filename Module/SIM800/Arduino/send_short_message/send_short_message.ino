#include <M5Stack.h>

//
String cmd_at = "AT";
// check the firmware version of SIM800
String cmd_check_firmware_version = "AT+GMR";

void setup() {
  M5.begin();
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000);// delay for lorawan power on
  /* LoRaWAN Init */
  Serial2.println(cmd_at);
  delay(500);
  Serial2.println(cmd_check_firmware_version);
  delay(500);
}

void loop() {
  if(Serial2.available()) {
    int ch = Serial2.read();
    M5.Lcd.print((char)ch);
    Serial.write(ch);
  }
}