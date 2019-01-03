#include <Wire.h>
#include "MFRC522_I2C.h"
#include <M5Stack.h>

// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28);   // Create MFRC522 instance.

void setup() {
  M5.Lcd.begin();
  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);

  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("M5StackFire MFRC522");
  Serial.begin(115200);           // Initialize serial communications with the PC
  Wire.begin();                   // Initialize I2C
  M5.begin();
  mfrc522.PCD_Init();             // Init MFRC522
  ShowReaderDetails();            // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
  M5.Lcd.println("Scan PICC to see UID, type, and data blocks...");
}

void loop() {
  // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  // Dump UID
  Serial.print(F("Card UID:"));
  M5.Lcd.println(" ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}




#include <Wire.h>
#include "MAX30100.h"


#define SAMPLING_RATE                       MAX30100_SAMPRATE_100HZ
#define IR_LED_CURRENT                      MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT                     MAX30100_LED_CURR_27_1MA
// set HIGHRES_MODE to true only when setting PULSE_WIDTH to MAX30100_SPC_PW_1600US_16BITS
#define PULSE_WIDTH                         MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE                        true

// new a object
MAX30100 sensor;

void setup()
{
    Serial.begin(115200);
    Serial.print("Initializing MAX30100..");
    if (!sensor.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
    sensor.setMode(MAX30100_MODE_SPO2_HR);
    sensor.setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
    sensor.setLedsPulseWidth(PULSE_WIDTH);
    sensor.setSamplingRate(SAMPLING_RATE);
    sensor.setHighresModeEnabled(HIGHRES_MODE);
}

void loop()
{
    uint16_t ir, red;
    sensor.update();
    while (sensor.getRawValues(&ir, &red)) {
        Serial.print(ir);
        Serial.print('\t');
        Serial.println(red);
    }
}