/*
    Slaver.ino
*/

#include <M5Stack.h>

// entry test mode
String cmd_test_mode = "AT+Mode=Test";
// Configure the modem,like Freq, SF, BW, Preamble length
String cmd_rfconf = "AT+TEST=RFCFG,472.3,8,250,8,8,20";
// allow to receive data
String cmd_receive_data = "AT+TEST=RXLRPKT";

void setup() {
  M5.begin();
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  delay(1000);// delay for lorawan power on
  /* LoRaWAN Init */
  Serial2.println(cmd_test_mode);
  delay(500);
  Serial2.println(cmd_rfconf);
  delay(500);
  Serial2.println(cmd_receive_data);
  delay(500);
}

void loop() {
  if(Serial2.available()) {
    int ch = Serial2.read();
    M5.Lcd.print((char)ch);
    Serial.write(ch);
  }
}
