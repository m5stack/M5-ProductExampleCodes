// --------------------------------------
// i2c_scanner (Fork from https://github.com/tackelua/I2C_Scanner/blob/master/I2C_Scanner.ino)
//
// Version 1
//    This program (or code that looks like it)
//    can be found in many places.
//    For example on the Arduino.cc forum.
//    The original author is not know.
// Version 2, Juni 2012, Using Arduino 1.0.1
//     Adapted to be as simple as possible by Arduino.cc user Krodal
// Version 3, Feb 26  2013
//    V3 by louarnold
// Version 4, March 3, 2013, Using Arduino 1.0.3
//    by Arduino.cc user Krodal.
//    Changes by louarnold removed.
//    Scanning addresses changed from 0...127 to 1...119,
//    according to the i2c scanner by Nick Gammon
//    http://www.gammon.com.au/forum/?id=10896
// Version 5, March 28, 2013
//    As version 4, but address scans now to 127.
//    A sensor seems to use address 120.
// Version 6, November 27, 2015.
//    Added waiting for the Leonardo serial communication.
//
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
//

#include <M5Stack.h>
#include <Wire.h>

void setup()
{
  M5.begin();
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
  M5.Lcd.println("\nI2C Scanner");
}


void loop()
{
  Wire.begin();
  byte error, address;
  int nDevices;

  static int s = 0;
  Serial.print(s++);
  Serial.println(". Scanning...");
  M5.Lcd.print(s++);
  M5.Lcd.println(". Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      M5.Lcd.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
        Serial.print("0");
      }
      Serial.print(address, HEX);
      M5.Lcd.print(address, HEX);
      Serial.println("  !");
      M5.Lcd.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      M5.Lcd.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
        Serial.print("0");
      }
      Serial.print(address, HEX);
      M5.Lcd.print(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
    M5.Lcd.println("No I2C devices found\n");
  }

  else
  {
    Serial.print(">>>> Found total ");;
    Serial.print(nDevices);
    Serial.println(" devices\n");
  }

  delay(2000);           // wait 5 seconds for next scan
}
