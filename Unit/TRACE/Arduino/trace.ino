#include <M5Stack.h>

// #define VALUE_SPLIT

uint8_t value;
int sensorValue[4] = {0};

void setup() {
    m5.begin();
    Serial.begin(115200);
    Wire.begin();
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(40, 0);
    M5.Lcd.println("TRACE example");
    M5.Lcd.setTextColor(WHITE);
}

void loop(){
    SensorStatus();
    delay(100);
}

void SensorStatus(void){
    Wire.beginTransmission(0x5a);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(0x5a,1);
    while(Wire.available()){
        value = Wire.read();
    }
    Serial.print(" value = ");Serial.println(value, HEX);
#ifdef VALUE_SPLIT
    sensorValue[0] = (value&0x08)>>3;
    sensorValue[1] = (value&0x04)>>2;
    sensorValue[2] = (value&0x02)>>1;
    sensorValue[3] = (value&0x01)>>0;
    Serial.println("After splitting... ");
    Serial.print("SensorValue[0] = ");Serial.print(sensorValue[0]);
    Serial.print("  SensorValue[1] = ");Serial.print(sensorValue[1]);
    Serial.print("  SensorValue[2] = ");Serial.print(sensorValue[2]);
    Serial.print("  SensorValue[3] = ");Serial.print(sensorValue[3]);
    Serial.println();
#endif
}