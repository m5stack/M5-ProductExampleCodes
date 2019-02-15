#include <M5Stack.h>
#include <TinyGPS++.h>
#define M5STACKFIRE_SPEAKER_PIN 25 // speaker DAC, only 8 Bit
#include "Array.h"

static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;

unsigned long rel_x,rel_y;
unsigned int rec_w,rec_h;
unsigned long min_x,max_x,min_y,max_y=0;


const byte size = 10;
byte Index =0;
float rawVel[size] = {0,0,0,0,0,0,0,0,0,0};
float rawLat[size] = {0,0,0,0,0,0,0,0,0,0};
float rawLng[size] = {0,0,0,0,0,0,0,0,0,0};

// The serial connection to the GPS device
HardwareSerial ss(2);


// For stats that happen every 5 seconds
unsigned long last = 0UL;



  String filename = "/LOGGER00.CSV";
  String filenameXY = "/LOGGER00XY.CSV";
  String dataToWrite = "";
  String dataToWriteXY = "";

  
 File logfile;
 File logfileXY;


void coords_to_pixel(double lat,double lng,int &x,int &y)
{
  int delta = 10000;
  rel_x = lat* 1000000;
  rel_y = lng* 1000000;
  if (min_x == 0)
    {
      min_x = rel_x-delta;
    }
  if (max_x == 0)
    {
      max_x = rel_x+delta;
    }

  if (min_y == 0)
    {
      min_y = rel_y-delta;
    }
  if (max_y == 0)
    {
      max_y = rel_y+delta;
    }

/*  if ((min_x>(rel_x-delta)))
    min_x = (rel_x-delta);

  if ((max_x<(rel_x+delta)))
    max_x = (rel_x+delta);
  
  if ((min_y>(rel_y-delta)))
    min_y = (rel_y-delta);

  if ((max_y<(rel_y+delta)))
    max_y = (rel_y+delta);
*/
  rec_w = (max_x - min_x);
  rec_h = (max_y - min_y);

x = rel_x - min_x;
y = rel_y - min_y;

}


void saveData()
{
if (dataToWrite != "")
{  
if(SD.exists(filename)){ // check the card is still there
// now append new data file
logfile = SD.open(filename, FILE_APPEND );
if (logfile){
logfile.println(dataToWrite);
logfile.close(); // close the file
dataToWrite = "";

}
}
else{
  logfile = SD.open(filename, FILE_WRITE );
if (logfile){
logfile.println("DATE,TIME,AGE,LAT,LATM,LNG,LNGM,HDOP,COURSE,SPEED,SPEEDM,ALTITUDE,SAT");
logfile.println(dataToWrite);
logfile.close(); // close the file
dataToWrite = "";

}
Serial.println("Error writing to file !");
}
}
}



void saveDataXY()
{
if (dataToWriteXY != "")
{  
if(SD.exists(filenameXY)){ // check the card is still there
// now append new data file
logfileXY = SD.open(filenameXY, FILE_APPEND );
if (logfileXY){
logfileXY.println(dataToWriteXY);
logfileXY.close(); // close the file
dataToWriteXY = "";

}
}
else{
  logfileXY = SD.open(filenameXY, FILE_WRITE );
if (logfileXY){
logfileXY.println("DATE,TIME,X,Y,SPEED");
logfileXY.println(dataToWriteXY);
logfileXY.close(); // close the file
dataToWriteXY = "";

}
Serial.println("Error writing to file !");
}
}
}


void connectToGPS()
{
  Serial.begin(115200);  

  M5.Lcd.clear();
  
  ss.begin(GPSBaud);
  delay(1000);
  M5.Lcd.println(F("Set Baud to 57600..."));
  ss.println("$PCAS01,4*18");//57600
  delay(1000);
  ss.flush(); 
  delay(1000);
  ss.end();
  delay(1000);
  ss.begin(57600);
  delay(1000);
 // ss.println("$PCAS01,3*1F");//38400
  //ss.updateBaudRate(57600);
  //delay(5);
  //ss.end();
  //delay(2000);
  //ss.begin(57600);
  
  delay(250);
  M5.Lcd.println(F("Set Frequency to 10Hz..."));
  ss.println("$PCAS02,100*1E");

  delay(250);
  M5.Lcd.println(F("Set Costellation to GPS,BDS,GLONASS..."));
  ss.println("$PCAS04,7*1E");

  
  delay(250);
  M5.Lcd.println(F("Set Receiver to Automotive..."));
  ss.println("$PCAS11,3*1E");

  delay(250);
  M5.Lcd.println(F("Set Reboot to HOT..."));
  ss.println("$PCAS10,0*1C");


  M5.Lcd.println(F("Inizialization terminated."));
}


void setup()
{
 
  M5.begin();
//  dacWrite(M5STACKFIRE_SPEAKER_PIN, 0); // make sure that the speaker is quite
//  Wire.begin();    
 connectToGPS();  
  M5.Lcd.setTextColor(GREEN, BLACK);
  //M5.powerOFF();

}

void loop()
{
/*  uint8_t data;
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_READ1);
  Wire.endTransmission(false);
  Wire.requestFrom(IP5306_ADDR, 1);
  data = Wire.read();
  M5.Lcd.setCursor(250, 150);
    M5.Lcd.print(String(data));
*/
  M5.update();  
  if (M5.BtnA.pressedFor(700))
    M5.powerOFF();
  // Dispatch incoming characters
  while (ss.available() > 0)
  {
    int ch = ss.read();
    Serial.write(ch);
    gps.encode(ch);
  }
  
  if (gps.location.isValid())
  {
    M5.Lcd.setCursor(0, 70);
    M5.Lcd.print(F("LOCATION  Fix Age: "));
    M5.Lcd.println(gps.location.age());

    M5.Lcd.setCursor(0, 150);
    M5.Lcd.print(F("LAT: "));
    M5.Lcd.print(gps.location.lat(), 6);
    M5.Lcd.print(F(" - LNG: "));
    M5.Lcd.println(gps.location.lng(), 6);
       
    //static char str[50];
    //sprintf(str, "%04d-%02d-%02d %02d:%02d.%03d", String(gps.date.year()),String(gps.date.month()),String(gps.date.day()),String(gps.time.hour()),String(gps.time.minute()),String(gps.time.second()),String(gps.time.centisecond()));
    //dataToWrite = String(str) + "," + String(gps.location.lat(),6) + "," + String(gps.location.lng(),6) + "," + String(gps.hdop.hdop()) + "," + String(gps.course.deg()) + "," + String(gps.speed.kmph()) +","+String(gps.altitude.meters());
    Index++;
    if (Index>=size)
      Index = 0;

    rawVel[Index] =  gps.speed.kmph();
    rawLat[Index] =  gps.location.lat();
    rawLng[Index] =  gps.location.lng();
    
    Array<float> velocity = Array<float>(rawVel,size);
    Array<float> latitude = Array<float>(rawLat,size);
    Array<float> longitude = Array<float>(rawLng,size);
     
    dataToWrite = String(gps.date.value())+","+String(gps.time.value()) + ","+String(gps.location.age())+"," + String(gps.location.lat(),6) +"," + String(latitude.getAverage(),6) + "," + String(gps.location.lng(),6) + "," + String(longitude.getAverage(),6) + "," + String(gps.hdop.hdop()) + "," + String(gps.course.deg()) + "," +String(gps.speed.kmph())+"," + String(velocity.getAverage()) +","+String(gps.altitude.meters())+","+String(gps.satellites.value());
    M5.Lcd.setCursor(0, 190);
    M5.Lcd.println(dataToWrite);
    saveData();

    int XX = 0;
    int YY = 0;
    coords_to_pixel(gps.location.lat(),gps.location.lng(),XX,YY);
    dataToWriteXY = String(gps.date.value())+","+String(gps.time.value()) + "," + String(XX) + "," + String(YY) + "," + String(gps.speed.kmph());
   // M5.Lcd.setCursor(0, 210);
   // M5.Lcd.println(dataToWriteXY);
    saveDataXY();
    
    
  }

 if (gps.date.isUpdated())
  {
    M5.Lcd.setCursor(0, 80);
    //M5.Lcd.print(F("DATE       Fix Age="));
    //M5.Lcd.print(gps.date.age());
//    M5.Lcd.print(F("ms Raw="));
//    M5.Lcd.print(gps.date.value());
    M5.Lcd.print(F("DATE: "));
    M5.Lcd.print(gps.date.year());
    M5.Lcd.print(F("/"));
    M5.Lcd.print(gps.date.month());
    M5.Lcd.print(F("/"));
    M5.Lcd.println(gps.date.day());
    filename = "/"+String(gps.date.value())+".CSV";
    filenameXY = "/"+String(gps.date.value())+"XY.CSV";
  }

 if (gps.time.isUpdated())
  {
    M5.Lcd.setCursor(0, 90);
    M5.Lcd.print(F("TIME: "));
//    M5.Lcd.print(gps.time.age());
//    M5.Lcd.print(F("ms Raw="));
//    M5.Lcd.print(gps.time.value());
//    M5.Lcd.print(F(" Hour="));
    M5.Lcd.print(gps.time.hour());
    M5.Lcd.print(F(":"));
    M5.Lcd.print(gps.time.minute());
    M5.Lcd.print(F(":"));
    M5.Lcd.print(gps.time.second());
    M5.Lcd.print(F("."));
    M5.Lcd.println(gps.time.centisecond());
    //static char str[31];
    //sprintf(str, "%04d-%02d-%02d %02d:%02d.%03d", String(gps.date.year()),String(gps.date.month()),String(gps.date.day()),String(gps.time.hour()),String(gps.time.minute()),String(gps.time.second()),String(gps.time.centisecond()));
//    dataToWrite = String(gps.date.value())+","+String(gps.time.value()) + "," + String(gps.location.lat(),6) + "," + String(gps.location.lng(),6) + "," + String(gps.hdop.hdop()) + "," + String(gps.course.deg()) + "," + String(gps.speed.kmph()) +","+String(gps.altitude.meters());
//    M5.Lcd.setCursor(0, 190);
//    M5.Lcd.println(dataToWrite);
  }

 
 if (millis() - last > 5000)
  {
     M5.Lcd.setCursor(0, 160);
//    M5.Lcd.print(F("DIAGS Chars: "));
//    M5.Lcd.print(gps.charsProcessed());
//    Serial.println(gps.charsProcessed());

    M5.Lcd.print(F("Sentences-with-Fix: "));
    M5.Lcd.print(gps.sentencesWithFix());
    M5.Lcd.print(F(" Failed-checksum: "));
    M5.Lcd.print(gps.failedChecksum());
    M5.Lcd.print(F(" Passed-checksum: "));
    M5.Lcd.println(gps.passedChecksum());

    if (gps.charsProcessed() < 10)
    {
      connectToGPS();
      M5.Lcd.println(F("WARNING: No GPS data.  Check wiring."));
    }
    last = millis();
   
  }
}
