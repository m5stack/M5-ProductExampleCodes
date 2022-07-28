/*
    note: need add library Adafruit_BMP280 from library manage
    Github: https://github.com/adafruit/Adafruit_BMP280_Library
*/

#include <M5StickCPlus.h>
#include "SHT3X.h"
#include <Wire.h>
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "bmm150.h"
#include "bmm150_defs.h"
SHT3X sht3x; 
BMM150 bmm = BMM150();
bmm150_mag_data value_offset_min;
bmm150_mag_data value_offset_max;
Adafruit_BMP280 bme;

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;

void calibrate(uint32_t timeout)
{
  uint32_t timeStart = 0;

  bmm.read_mag_data();  
  value_offset_min.x = bmm.raw_mag_data.raw_datax;
  value_offset_max.x = bmm.raw_mag_data.raw_datax;
  value_offset_min.y = bmm.raw_mag_data.raw_datay;
  value_offset_max.y = bmm.raw_mag_data.raw_datay;
  value_offset_min.z = bmm.raw_mag_data.raw_dataz;
  value_offset_max.z = bmm.raw_mag_data.raw_dataz;
  delay(100);

  timeStart = millis();
  
  while((millis() - timeStart) < timeout*1000)
  {
    bmm.read_mag_data();
    
    /* Update x-Axis max/min value */
    if(value_offset_min.x > bmm.raw_mag_data.raw_datax)
    {
      value_offset_min.x = bmm.raw_mag_data.raw_datax;
      // Serial.print("Update value_x_min: ");
      // Serial.println(value_offset_min.x);

    } 
    else if(value_offset_max.x < bmm.raw_mag_data.raw_datax)
    {
      value_offset_max.x = bmm.raw_mag_data.raw_datax;
      // Serial.print("update value_x_max: ");
      // Serial.println(value_offset_max.x);
    }

    /* Update y-Axis max/min value */
    if(value_offset_min.y > bmm.raw_mag_data.raw_datay)
    {
      value_offset_min.y = bmm.raw_mag_data.raw_datay;
      // Serial.print("Update value_y_min: ");
      // Serial.println(value_offset_min.y);

    } 
    else if(value_offset_max.y < bmm.raw_mag_data.raw_datay)
    {
      value_offset_max.y = bmm.raw_mag_data.raw_datay;
      // Serial.print("update value_y_max: ");
      // Serial.println(value_offset_max.y);
    }

    /* Update z-Axis max/min value */
    if(value_offset_min.z > bmm.raw_mag_data.raw_dataz)
    {
      value_offset_min.z = bmm.raw_mag_data.raw_dataz;
      // Serial.print("Update value_z_min: ");
      // Serial.println(value_offset_min.z);

    } 
    else if(value_offset_max.z < bmm.raw_mag_data.raw_dataz)
    {
      value_offset_max.z = bmm.raw_mag_data.raw_dataz;
      // Serial.print("update value_z_max: ");
      // Serial.println(value_offset_max.z);
    }
    
    Serial.print(".");
    delay(1);

  }


  Serial.print("Update value_x_min: ");
  Serial.println(value_offset_min.x);
  Serial.print("update value_x_max: ");
  Serial.println(value_offset_max.x);

  Serial.print("Update value_y_min: ");
  Serial.println(value_offset_min.y);
  Serial.print("update value_y_max: ");
  Serial.println(value_offset_max.y);
  
  Serial.print("Update value_z_min: ");
  Serial.println(value_offset_min.z);
  Serial.print("update value_z_max: ");
  Serial.println(value_offset_max.z);

}
void setup() {
  // put your setup code here, to run once:
  M5.begin();
  Wire.begin(0,26);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.println("ENV TEST");
  pinMode(M5_BUTTON_HOME, INPUT);

  if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
    Serial.println("Chip ID can not read!");
    while(1);
  } else {
    Serial.println("Initialize done!");
  }
  if (!bme.begin(0x76)){  
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      while (1);
  }
  calibrate(10);
  Serial.print("\n\rCalibrate done..");
}
uint8_t setup_flag = 1;
void loop() {
  // put your main code here, to run repeatedly:
  if(sht3x.get()==0){
    tmp = sht3x.cTemp;
    hum = sht3x.humidity;
  }

  
  M5.Lcd.setCursor(0, 20, 2);
  M5.Lcd.printf("Temp: %2.1f Humi: %2.0f", tmp, hum);


  float x,y,z;
  bmm.read_mag_data();

  x = (2.0 * bmm.raw_mag_data.raw_datax - value_offset_max.x - value_offset_min.x)/(value_offset_max.x - value_offset_min.x);
  y = (2.0 * bmm.raw_mag_data.raw_datay - value_offset_max.y - value_offset_min.y)/(value_offset_max.y - value_offset_min.y);
  z = (2.0 * bmm.raw_mag_data.raw_dataz - value_offset_max.z - value_offset_min.z)/(value_offset_max.z - value_offset_min.z);
  
  float xyHeading = atan2(x, y);
  float zxHeading = atan2(z, x);
  float heading = xyHeading;

  if(heading < 0)
    heading += 2*PI;
  if(heading > 2*PI)
    heading -= 2*PI;
  float headingDegrees = heading * 180/M_PI; 
  float xyHeadingDegrees = xyHeading * 180 / M_PI;
  float zxHeadingDegrees = zxHeading * 180 / M_PI;

  Serial.print("Heading: ");
  Serial.println(headingDegrees);
  Serial.print("xyHeadingDegrees: ");
  Serial.println(xyHeadingDegrees);
  Serial.print("zxHeadingDegrees: ");
  Serial.println(zxHeadingDegrees);
  M5.Lcd.setCursor(0, 40, 2);
  M5.Lcd.printf("headingDegrees: %2.1f", headingDegrees);
  
  float pressure = bme.readPressure();
  M5.Lcd.setCursor(0, 60, 2);
  M5.Lcd.printf("pressure: %2.1f", pressure);
  delay(100);

  if(!setup_flag){
     setup_flag = 1;

     if(bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
    Serial.println("Chip ID can not read!");
    while(1);
  } else {
    Serial.println("Initialize done!");
  }
  if (!bme.begin(0x76)){  
      Serial.println("Could not find a valid BMP280 sensor, check wiring!");
      while (1);
  }
  calibrate(10);
  Serial.print("\n\rCalibrate done..");
 }


 if(digitalRead(M5_BUTTON_HOME) == LOW){
  setup_flag = 0;
  while(digitalRead(M5_BUTTON_HOME) == LOW);
 }

  
}
