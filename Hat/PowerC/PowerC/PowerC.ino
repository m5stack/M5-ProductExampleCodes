#include <M5StickC.h>
#include <Wire.h>

void setup() {
  M5.begin();
  Wire.begin(0, 26, 400000);
  M5.Lcd.setRotation(1);
  while(Wire.endTransmission() !=0){
     M5.Lcd.setCursor(20,0);
     M5.Lcd.print("Not found PowerC");
     Wire.beginTransmission(0x75);
  }
  M5.Lcd.fillRect(0, 0, 160, 80, BLACK);
}

uint16_t readBatV(uint8_t Reg)
{
    uint8_t dataV[2] = {0, 0};
    M5.I2C.readBytes(0x75, Reg, 2, dataV);
    return ((dataV[1]<<8)&0xff00) | dataV[0];
}

uint16_t readBatI(uint8_t Reg)
{
    uint8_t dataI[2] = {0, 0};
    M5.I2C.readBytes(0x75, Reg, 2, dataI);
    return ((dataI[1]<<8)&0xff00) | dataI[0];
}


void loop() {
  float v = readBatV(0xA2);
  float i = readBatI(0XA4);
  v = v * 0.00026855 + 2.6;
  i = i * 0.745985;
  M5.Lcd.setCursor(0,10);
  M5.Lcd.printf("%1.2f,V",v);
  Serial.printf("%1.2f",v);
  Serial.println("");
  M5.Lcd.setCursor(0,40);
  M5.Lcd.printf("%1.0f,mAh",i);
  Serial.printf("%1.0f",i);
  Serial.println("");
}
