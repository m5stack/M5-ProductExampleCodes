#include <M5Stack.h>
#include <Wire.h>

/***************************************************************************************
This example requires the device with the iic device to be inserted into the hub port, 
and the host will read the corresponding iic address.
****************************************************************************************/
#define PAHUB_ADDR 0X70

void portselectall(uint8_t ports) {  
  Wire.beginTransmission(PAHUB_ADDR);
  Wire.write(ports&0x3f);
  Wire.endTransmission(); 
}


//Hub range is 0 to 5
void portselect(uint8_t i) {
  if (i > 7) return;
  
  Wire.beginTransmission(PAHUB_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
}


void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  int error = 0;
  for(int i = 0; i < 6; i++){
     portselect(i);
     M5.Lcd.setCursor(0, i * 40, 2);
     M5.Lcd.print("TCA Port"); M5.Lcd.print(i);
     for(uint8_t address = 1; address < 127; address++){
       Wire.beginTransmission(address);
       error = Wire.endTransmission();
       if(error == 0){
        if((address == 0x70) || (address == 0x75)) continue;
          M5.Lcd.print(":  Found I2C 0x");M5.Lcd.print(address,HEX);break;
       }else{
          M5.Lcd.setCursor(100, i * 40);
          M5.Lcd.fillRect(100, i * 40, 280, 40, BLACK);
       }
      delay(1);
     }
  }

}
