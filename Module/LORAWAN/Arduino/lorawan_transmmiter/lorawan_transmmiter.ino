/*
    Master.ino
*/

#include <M5Stack.h>

// entry test mode
String cmd_test_mode = "AT+Mode=Test";
// Configure the modem,like Freq, SF, BW, Preamble length, TX output power
String cmd_rfconf = "AT+TEST=RFCFG,472.3,8,250,8,8,20";
// send data as HEX format
String cmd_send_data = "AT+TEST=TXLRPKT,\"30 31 32 33 34 35\"";

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
}

void loop() {
  if(M5.BtnA.wasPressed()) {
    Serial2.println(cmd_send_data);
    Serial.println(cmd_send_data);
  }
  M5.update();
}
