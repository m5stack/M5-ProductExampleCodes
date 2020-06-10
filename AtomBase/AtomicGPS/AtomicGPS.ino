/*This is an example used SerialBT,you can can view gps data by connecting 
 * to Bluetooth assistant on your mobilephone or Serial Monitor
 * the GPS log will be written to SD card
 * 
 */


#include "M5Atom.h"
#include "GPSAnalyse.h"
#include <SPI.h>
#include "FS.h"
#include <SD.h>
#include <BluetoothSerial.h>
 
BluetoothSerial SerialBT;
GPSAnalyse GPS;

uint64_t chipid;
char chipname[256];

const char filename[] = "/GPSdata.txt";
File txtFile;

float Lat;
float Lon;
String Utc;

bool writeLog(String filename) {          //Write GPSdata to SDcard
  txtFile = SD.open(filename, FILE_APPEND);
  if(txtFile){
    txtFile.print(Lat);
    txtFile.print(", ");
    txtFile.print(Lon);
    txtFile.print(", ");
    txtFile.println(Utc);
    txtFile.close();
  }else{
    return false;
  }
  return true;
}

void setup() {

    M5.begin(true,false,true); 
    chipid = ESP.getEfuseMac();
    sprintf( chipname, "SerialBT_%04X", (uint16_t)(chipid >> 32));
    Serial.printf("Bluetooth: %s\n", chipname);   
    SPI.begin(23,33,19,-1);
    if(!SD.begin(-1, SPI, 40000000)){
      Serial.println("initialization failed!");
    } 
    sdcard_type_t Type = SD.cardType();

	  Serial.printf("SDCard Type = %d \r\n",Type);
	  Serial.printf("SDCard Size = %d \r\n" , (int)(SD.cardSize()/1024/1024));

    M5.dis.fillpix(0x00004f);
    
    Serial1.begin(9600,SERIAL_8N1,22,-1);
    SerialBT.begin(chipname);
    GPS.setTaskName("GPS");
    GPS.setTaskPriority(2);
    GPS.setSerialPtr(Serial1);
    GPS.start();
}

void loop() {
    GPS.upDate();
    Lat = GPS.s_GNRMC.Latitude;
    Lon = GPS.s_GNRMC.Longitude;
    Utc = GPS.s_GNRMC.Utc;
    SerialBT.printf("Latitude= %.5f \r\n",Lat);
    SerialBT.printf("Longitude= %.5f \r\n",Lon);
    SerialBT.printf("DATA= %s \r\n",Utc);
    Serial.printf("Latitude= %.5f \r\n",Lat);
    Serial.printf("Longitude= %.5f \r\n",Lon);
    Serial.printf("DATA= %s \r\n",Utc);
    writeLog(filename);
    delay(1000);
}
