#include <M5Stack.h>

String cmd_test_mode = "AT+Mode=Test";
String cmd_rfconf = "AT+TEST=RFCFG,472.3,8,250,8,8,20";
String cmd_receive_data = "AT+TEST=RXLRPKT";

void setup() {
  
  M5.begin();

  Serial.begin(9600);

  // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  /* LoRaWAN Init */
  Serial2.print(cmd_test_mode);//entry test mode
  Serial2.print(cmd_rfconf);//Configure the modem,like Freq, SF, BW, Preamble length

}

void loop() {

  // if(Serial.available()) {
  //   int ch = Serial.read();
  //   Serial2.write(ch);  
  // }

  Serial2.print(cmd_receive_data);//allow for receiving data
                                  //actually it need to be send once you change rf configuration or reset device
  delay(200);

  if(Serial2.available()) {
    int ch = Serial2.read();
    Serial.write(ch);
  }
}
