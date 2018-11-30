#include <M5Stack.h>

String cmd_test_mode = "AT+Mode=Test";
String cmd_rfconf = "AT+TEST=RFCFG,472.3,8,250,8,8,20";
String cmd_send_data = "AT+TEST=TXLRPKT,"00 00 01 00 00 AF 80 07 02 00 00 39"";

void setup() {
  
  M5.begin();

  Serial.begin(9600);

  // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  /* LoRaWAN Init */
  Serial2.print(cmd_test_mode);//entry test mode
  Serial2.print(cmd_rfconf);//Configure the modem,like Freq, SF, BW, Preamble length, TX output power

}

void loop() {

  // if(Serial.available()) {
  //   int ch = Serial.read();
  //   Serial2.write(ch);  
  // }

  Serial2.print(cmd_send_data);//send data as HEX format
  delay(200);

  if(Serial2.available()) {
    int ch = Serial2.read();
    Serial.write(ch);
  }
}
