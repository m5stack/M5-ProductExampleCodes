#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

#include "axp192.h"
#include "Lcd_Driver.h"
#include "LCD_Config.h"
#include "IMU.h"
#include "rtc.h"
#include "esp32_rmt.h"
#include "DHT12.h"

AXP192 axp192;
Rtc  rtc;
ESP32_RMT rem;
DHT12 dht12;  

int LED_RI = 9;
int LED_BUILTIN = 10;
int BUTTON_HOME = 37;
int BUTTON_PIN = 39;



int16_t accX = 0;
int16_t accY = 0;
int16_t accZ = 0;

int16_t gyroX = 0;
int16_t gyroY = 0;
int16_t gyroZ = 0;


void wifi_test() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("scan start");
    //M5.Lcd.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    //M5.Lcd.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
        //M5.Lcd.println("no networks found");
    } else {
        Serial.print(n);
        //M5.Lcd.print(n);
        Serial.println(" networks found");
        //M5.Lcd.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            //M5.Lcd.print(i + 1);
            Serial.print(": ");
            //M5.Lcd.print(": ");
            Serial.print(WiFi.SSID(i));
           // M5.Lcd.print(WiFi.SSID(i));
            Serial.print(" (");
            //M5.Lcd.print(" (");
            Serial.print(WiFi.RSSI(i));
            //M5.Lcd.print(WiFi.RSSI(i));
            Serial.print(")");
            //M5.Lcd.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            //M5.Lcd.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(5);
        }
    }
    Serial.println("");
    //M5.Lcd.println("");
}

void grove_test() {
  Lcd_Asc(0,120,"T:");
  //dht12.readTemperature();
  Serial.println(dht12.readTemperature());
  Lcd_PrintValueF(20, 120, dht12.readTemperature(),6);
  Lcd_Asc(60,120,"C");
  //Serial.println(dht12.readHumidity());

}

void sh200i_test() {

  //sh200i_init();
  unsigned char tempdata[1];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X, 1, tempdata);
  accX = tempdata[0];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+1, 1, tempdata);
  accX |= tempdata[0] << 8;


  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+2, 1, tempdata);
  accY = tempdata[0];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+3, 1, tempdata);
  accY |= tempdata[0] << 8;
    
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+4, 1, tempdata);
  accZ = tempdata[0];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+5, 1, tempdata);
  accZ |= tempdata[0] << 8;

  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+6, 1, tempdata);
  gyroX = tempdata[0];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+7, 1, tempdata);
  gyroX |= tempdata[0] << 8;

  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+8, 1, tempdata);
  gyroY = tempdata[0];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+9, 1, tempdata);
  gyroY |= tempdata[0] << 8;

  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+10, 1, tempdata);
  gyroZ = tempdata[0];
  I2C_Read_NBytes(SH200I_ADDRESS, SH200I_OUTPUT_ACC_X+11, 1, tempdata);
  gyroZ |= tempdata[0] << 8;

#if 0
  Serial.printf("aX = %ld,aY = %ld,aZ = %ld,gX = %ld,gY = %ld,gZ = %ld\r\n", accX, accY, accZ,gyroX, gyroY, gyroZ);
#else
  Serial.printf("aX = %ld,aY = %ld,aZ = %ld,gX = %ld,gY = %ld,gZ = %ld\r\n", accX, accY, accZ,gyroX, gyroY, gyroZ);
  //Lcd_Asc(0,60,"sh200i ok!");
  Lcd_ClearRect(0,20,80,100,WHITE);
  Lcd_Asc(0,20,"a:");
  //Lcd_Asc(50,20,"Rt");
  Lcd_PrintValueI(40, 20, accX);
  Lcd_PrintValueI(0, 40, accY);
  Lcd_PrintValueI(40, 40, accZ);
  //Lcd_Asc(0,40,"Rt");
  //Lcd_Asc(60,40,"Rt");
  Lcd_Asc(0,60,"g:");
  Lcd_PrintValueI(40, 60, gyroX);
  Lcd_PrintValueI(0, 80, gyroY);
  Lcd_PrintValueI(40, 80, gyroZ);
  //Lcd_Asc(50,60,"Rt");
  //Lcd_Asc(0,80,"Rt");
  //Lcd_Asc(60,80,"Rt");
#endif
  //delay(1000);
}

void ir_test() {
  //!IR
  //rem.begin(LED_RI,1);
  rem.necSend(0x3000, 0xfd02);
  //Lcd_Asc(0,100,"ir ok!");
}

uint8_t Rt_second;
uint8_t Rt_old_second;
void rtc_test() {
    rtc.GetBm8563Time();
    Rt_second = rtc.Second;
    #if 0
    Serial.printf("time = %02d : %02d : %02d\n",rtc.Hour, rtc.Minute, rtc.Second);
    delay(1000);
    #else
    if(Rt_old_second != Rt_second){
     Lcd_Asc(0,100,"Rt");
     Lcd_Asc(17,100,(char *)rtc.DateString);
    }

    Rt_old_second = Rt_second;
    #endif
    //rtc.GetBm8563Time();
    //second_test_2 = rtc.Second;
    //Serial.printf("time = %02d : %02d : %02d\n",rtc.Hour, rtc.Minute, rtc.Second);
    //if(second_test_1 != second_test_2)
      //Lcd_Asc(0,80,"rtc ok!");
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  //Wire1.begin();
  Serial.begin(115200);

  //power
  axp192.Init();
   
  //lcd
  Lcd_Init();
  Lcd_Clear(WHITE);
  delay(100);
  Lcd_Clear(BLUE);
  delay(100);
  Lcd_Clear(RED);
  delay(100);
  Lcd_Clear(GREEN);
  delay(100);
  Lcd_Clear(YELLOW);
  delay(100);

  wifi_test();
  Lcd_Clear(WHITE);

  Lcd_Asc(10,0,"M5stick");

  //!sh200i
  sh200i_init();
  //Lcd_Asc(0,20,"my name is");

  //!IR
  rem.begin(LED_RI,1);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(BUTTON_HOME, INPUT);
  pinMode(BUTTON_PIN, INPUT);
}
bool led = true;
long loopTime, startTime = 0;
int led_count = 2000;
void loop() {
  // put your main code here, to run repeatedly:
  loopTime = millis();
  //startTime = loopTime;
#if 1
  if(digitalRead(BUTTON_HOME) == LOW){
    led_count++;
    
    //if(led_count >= 15);
    //led_count = 0;
    //while(digitalRead(BUTTON_HOME) == LOW);
  }

  if(digitalRead(BUTTON_PIN) == LOW){
    //Lcd_Asc(0,140,"Kpin ok!");
    led = !led;
    rem.necSend(0x3000, 0xfd02);
    while(digitalRead(BUTTON_PIN) == LOW);
  }
  digitalWrite(LED_BUILTIN, led);
#endif
  //!rtc
  if(startTime < (loopTime - 1000)){
  startTime = loopTime;
  rtc_test();
  sh200i_test();
  grove_test();
  }

  axp192.ScreenBreath(led_count / 200);
    //delay(1000);
}
