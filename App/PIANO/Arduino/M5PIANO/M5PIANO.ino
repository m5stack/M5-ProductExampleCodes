#include <M5Stack.h>
#include <Wire.h>

#include <FastLED.h>
// #include <Adafruit_NeoPixel.h>
#define LED_PIN            2

#define NUM_LEDS_PER_STRIP     29
CRGB Leds[NUM_LEDS_PER_STRIP];
// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define LF        0x0A //New Line
#define CR        0x0D //Carriage  return
#define SP        0x20 //Spcae

#define Sensitivity_PWM1  0x00 //ch2,ch1
#define Sensitivity_PWM2  0x01 //ch4,ch3
#define Sensitivity_PWM3  0x02 //ch6,ch5
#define Sensitivity_PWM4  0x03 //ch21,ch7
#define Sensitivity_PWM5  0x04 //ch9,ch8
#define Sensitivity_PWM6  0x05 //ch11,ch10
#define Sensitivity_PWM7  0x06 //ch13,ch12
#define Sensitivity_PWM8  0x07 //ch15,ch14
#define Sensitivity_PWM9  0x08 //ch17,ch16
#define Sensitivity_PWM10 0x09 //ch19,ch18
#define Sensitivity_PWM11   0x0A //ch20

#define GTRL1     0x0B
#define GTRL2     0x0C

#define Cal_CTRL    0x0D

#define Port_CTRL1    0x0E
#define Port_CTRL2    0x0F
#define Port_CTRL3    0x10
#define Port_CTRL4    0x11
#define Port_CTRL5    0x12
#define Port_CTRL6    0x13

#define Cal_HOLD1   0x14
#define Cal_HOLD2   0x15
#define Cal_HOLD3   0x16

#define Err_CTRL    0x17

#define Output1     0x20
#define Output2     0x21
#define Output3     0x22

#define Ref_wr_H    0x23
#define Ref_wr_L    0x24
#define Ref_wr_CH1    0x25
#define Ref_wr_CH2    0x26
#define Ref_wr_CH3    0x27

#define Sen_RD_CTRL   0x28
#define Sensitivity_RD  0x29

#define Rd_CH1      0x30
#define Rd_CH2      0x31
#define Rd_CH3      0x32
#define Sen_H     0x33
#define Sen_L     0x34
#define Ref_H     0x35
#define Ref_L     0x36
#define Rd_CH1      0x37
#define Rd_CH2      0x38
#define Rd_CH3      0x39

#define TS20_SLAVE_GND  0x6A //0xD4<<1 //ADD Pin = GND
#define TS20_SLAVE_VDD  0x7A //0xF4<<1 //ADD Pin = VDD

void  Init_TS20(void); //Initialize TS20

#define RESET_PIN 7 //Reset pin
#define EN_PIN    6 //I2C Enable Pin
#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )



byte key[4];
// #define pRed pixels.Color(5,0,0)
// #define pBlack pixels.Color(0,0,0)

void setup() {

  M5.begin();
  Wire.begin(26, 5);       // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output (Spped)
  delay(100); //wait for 100[msec]
  Init_TS20(); //Initialize TS20
  delay(100); //wait for 100[msec]
  key[0] = 0;
  key[1] = 0;
  key[2] = 0;
  key[3] = 0;
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(Leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(10);
  // pixels.begin();


  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(1, 10);
  M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("M5PIANO");

  for (int i = 0; i < 29; i++)
  {
    Leds[i] = CRGB::Red;
    // pixels.setPixelColor(i, pRed);
    delay(20);
    FastLED.show();
  }
  delay(1000);
  for (int i = 0; i < 29; i++)
  {
    Leds[i] = CRGB::Black;
    // FastLED.setPixelColor(i, pBlack);
    delay(10);
    FastLED.show();
  }
}


#define NOTE_D0 -1
#define NOTE_D1 524
#define NOTE_D2 588
#define NOTE_D3 660
#define NOTE_D4 698
#define NOTE_D5 784
#define NOTE_D6 880
#define NOTE_D7 988

#define NOTE_DL1 261
#define NOTE_DL2 293
#define NOTE_DL3 329
#define NOTE_DL4 349
#define NOTE_DL5 392
#define NOTE_DL6 440
#define NOTE_DL7 494

#define NOTE_DH1 1048
#define NOTE_DH2 1176
#define NOTE_DH3 1320
#define NOTE_DH4 1396
#define NOTE_DH5 1568
#define NOTE_DH6 1760
#define NOTE_DH7 1976

void loop() {

  int data0, data1, data2, data3, data4, data5;
  delay(5);

  Wire.beginTransmission(TS20_SLAVE_GND); // sned ic slave address
  Wire.write(byte(Output1)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TS20_SLAVE_GND, 3); // key data read (3 byte)
  data0 = Wire.read();
  data1 = Wire.read();
  data2 = Wire.read();
  Wire.beginTransmission(TS20_SLAVE_VDD); // sned ic slave address
  Wire.write(byte(Output1)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TS20_SLAVE_VDD, 3); // key data read (3 byte)
  data3 = Wire.read();
  data4 = Wire.read();
  data5 = Wire.read();


  M5.Lcd.setCursor(1, 50);
  M5.Lcd.printf("%d,%d,%d,%d,%d,%d   ", data0, data1, data2, data3, data4, data5);

  for (int i = 0; i < 29; i++)
  {
    Leds[i] = CRGB::Black;
    // pixels.setPixelColor(i, pBlack);
  }

  if ((data0 & 0x10) == 0x10) {
    Set_Bit(key[0], 0);
    Leds[0] = CRGB::Red;
    // pixels.setPixelColor(0, pRed);
    M5.Speaker.tone(NOTE_DL1, 20);
  } else {
    Leds[0] = CRGB::Black;
    // pixels.setPixelColor(0, pBlack);
    Clr_Bit(key[0], 0);
  }
  if ((data0 & 0x20) == 0x20) {
    Set_Bit(key[0], 1);
    Leds[1] = CRGB::Red;
    // pixels.setPixelColor(1, pRed);
    M5.Speaker.tone(NOTE_DL1, 20);
  } else {
    Leds[1] = CRGB::Black;
    // pixels.setPixelColor(1, pBlack);
    Clr_Bit(key[0], 1);
  }
  if ((data0 & 0x40) == 0x40) {
    Set_Bit(key[0], 2);
    Leds[2] = CRGB::Red;
    // pixels.setPixelColor(2, pRed);
    M5.Speaker.tone(NOTE_DL2, 20);
  } else {
    Leds[2] = CRGB::Black;
    // pixels.setPixelColor(2, pBlack);
    Clr_Bit(key[0], 2);
  }
  if ((data1 & 0x01) == 0x01) {
    Set_Bit(key[0], 3);
    Leds[3] = CRGB::Red;
    // pixels.setPixelColor(3, pRed);
    M5.Speaker.tone(NOTE_DL2, 20);
  } else {
    Leds[3] = CRGB::Black;
    // pixels.setPixelColor(3, pBlack);
    Clr_Bit(key[0], 3);
  }
  if ((data1 & 0x02) == 0x02) {
    Set_Bit(key[0], 4);
    Leds[4] = CRGB::Red;
    // pixels.setPixelColor(4, pRed);
    M5.Speaker.tone(NOTE_DL3, 20);
  } else {
    Leds[4] = CRGB::Black;
    // pixels.setPixelColor(4, pBlack);
    Clr_Bit(key[0], 4);
  }

  if ((data2 & 0x04) == 0x04) {
    Set_Bit(key[0], 5);
    Leds[6] = CRGB::Red;
    // pixels.setPixelColor(6, pRed);
    M5.Speaker.tone(NOTE_DL4, 20);
  } else {
    Leds[6] = CRGB::Black;
    // pixels.setPixelColor(6, pBlack);
    Clr_Bit(key[0], 5);
  }
  if ((data2 & 0x08) == 0x08) {
    Set_Bit(key[0], 6);
    Leds[7] = CRGB::Red;
    // pixels.setPixelColor(7, pRed);
    M5.Speaker.tone(NOTE_DL4, 20);
  } else {
    Leds[7] = CRGB::Black;
    // pixels.setPixelColor(7, pBlack);
    Clr_Bit(key[0], 6);
  }
  if ((data2 & 0x10) == 0x10) {
    Set_Bit(key[0], 7);
    Leds[8] = CRGB::Red;
    // pixels.setPixelColor(8, pRed);
    M5.Speaker.tone(NOTE_DL5, 20);
  } else {
    Leds[8] = CRGB::Black;
    // pixels.setPixelColor(8, pBlack);
    Clr_Bit(key[0], 7);
  }

  if ((data0 & 0x01) == 0x01) {
    Set_Bit(key[1], 0);
    Leds[9] = CRGB::Red;
    // pixels.setPixelColor(9, pRed);
    M5.Speaker.tone(NOTE_DL5, 20);
  } else {
    Leds[9] = CRGB::Black;
    // pixels.setPixelColor(9, pBlack);
    Clr_Bit(key[1], 0);
  }
  if ((data0 & 0x02) == 0x02) {
    Set_Bit(key[1], 1);
    Leds[10] = CRGB::Red;
    // pixels.setPixelColor(10, pRed);
    M5.Speaker.tone(NOTE_DL6, 20);
  } else {
    Leds[10] = CRGB::Black;
    // pixels.setPixelColor(10, pBlack);
    Clr_Bit(key[1], 1);
  }
  if ((data0 & 0x04) == 0x04) {
    Set_Bit(key[1], 2);
    Leds[11] = CRGB::Red;
    // pixels.setPixelColor(11, pRed);
    M5.Speaker.tone(NOTE_DL6, 20);
  } else {
    Leds[11] = CRGB::Black;
    // pixels.setPixelColor(11, pBlack);
    Clr_Bit(key[1], 2);
  }
  if ((data0 & 0x08) == 0x08) {
    Set_Bit(key[1], 3);
    Leds[12] = CRGB::Red;
    // pixels.setPixelColor(12, pRed);
    M5.Speaker.tone(NOTE_DL7, 20);
  } else {
    Leds[12] = CRGB::Black;
    // pixels.setPixelColor(12, pBlack);
    Clr_Bit(key[1], 3);
  }
  if ((data3 & 0x10) == 0x10) {
    Set_Bit(key[1], 4);
    Leds[14] = CRGB::Red;
    // pixels.setPixelColor(14, pRed);
    M5.Speaker.tone(NOTE_D1, 20);
  } else {
    Leds[14] = CRGB::Black;
    // pixels.setPixelColor(14, pBlack);
    Clr_Bit(key[1], 4);
  }
  if ((data3 & 0x20) == 0x20) {
    Set_Bit(key[1], 5);
    Leds[15] = CRGB::Red;
    // pixels.setPixelColor(15, pRed);
    M5.Speaker.tone(NOTE_D1, 20);
  } else {
    Leds[15] = CRGB::Black;
    // pixels.setPixelColor(15, pBlack);
    Clr_Bit(key[1], 5);
  }
  if ((data3 & 0x40) == 0x40) {
    Set_Bit(key[1], 6);
    Leds[16] = CRGB::Red;
    // pixels.setPixelColor(16, pRed);
    M5.Speaker.tone(NOTE_D2, 20);
  } else {
    Leds[16] = CRGB::Black;
    // pixels.setPixelColor(16, pBlack);
    Clr_Bit(key[1], 6);
  }
  if ((data4 & 0x01) == 0x01) {
    Set_Bit(key[1], 7);
    Leds[17] = CRGB::Red;
    // pixels.setPixelColor(17, pRed);
    M5.Speaker.tone(NOTE_D2, 20);
  } else {
    Leds[17] = CRGB::Black;
    // pixels.setPixelColor(17, pBlack);
    Clr_Bit(key[1], 7);
  }

  if ((data4 & 0x02) == 0x02) {
    Set_Bit(key[2], 0);
    Leds[18] = CRGB::Red;
    // pixels.setPixelColor(18, pRed);
    M5.Speaker.tone(NOTE_D3, 20);
  } else {
    Leds[18] = CRGB::Black;
    // pixels.setPixelColor(18, pBlack);
    Clr_Bit(key[2], 0);
  }
  if ((data5 & 0x02) == 0x02) {
    Set_Bit(key[2], 1);
    Leds[20] = CRGB::Red;
    // pixels.setPixelColor(20, pRed);
    M5.Speaker.tone(NOTE_D4, 20);
  } else {
    Leds[20] = CRGB::Black;
    // pixels.setPixelColor(20, pBlack);
    Clr_Bit(key[2], 1);
  }
  if ((data5 & 0x04) == 0x04) {
    Set_Bit(key[2], 2);
    Leds[21] = CRGB::Red;
    // pixels.setPixelColor(21, pRed);
    M5.Speaker.tone(NOTE_D4, 20);
  } else {
    Leds[21] = CRGB::Black;
    // pixels.setPixelColor(21, pBlack);
    Clr_Bit(key[2], 2);
  }
  if ((data5 & 0x08) == 0x08) {
    Set_Bit(key[2], 3);
    Leds[22] = CRGB::Red;
    // pixels.setPixelColor(22, pRed);
    M5.Speaker.tone(NOTE_D5, 20);
  } else {
    Leds[22] = CRGB::Black;
    // pixels.setPixelColor(22, pBlack);
    Clr_Bit(key[2], 3);
  }
  if ((data5 & 0x10) == 0x10) {
    Set_Bit(key[2], 4);
    Leds[23] = CRGB::Red;
    // pixels.setPixelColor(23, pRed);
    M5.Speaker.tone(NOTE_D5, 20);
  } else {
    Leds[23] = CRGB::Black;
    // pixels.setPixelColor(23, pBlack);
    Clr_Bit(key[2], 4);
  }
  if ((data3 & 0x01) == 0x01) {
    Set_Bit(key[2], 5);
    Leds[24] = CRGB::Red;
    // pixels.setPixelColor(24, pRed);
    M5.Speaker.tone(NOTE_D6, 20);
  } else {
    Leds[24] = CRGB::Black;
    // pixels.setPixelColor(24, pBlack);
    Clr_Bit(key[2], 5);
  }
  if ((data3 & 0x02) == 0x02) {
    Set_Bit(key[2], 6);
    Leds[25] = CRGB::Red;
    // pixels.setPixelColor(25, pRed);
    M5.Speaker.tone(NOTE_D6, 20);
  } else {
    Leds[25] = CRGB::Black;
    // pixels.setPixelColor(25, pBlack);
    Clr_Bit(key[2], 6);
  }
  if ((data3 & 0x04) == 0x04) {
    Set_Bit(key[2], 7);
    Leds[26] = CRGB::Red;
    // pixels.setPixelColor(26, pRed);
    M5.Speaker.tone(NOTE_D7, 20);
  } else {
    Leds[26] = CRGB::Black;
    // pixels.setPixelColor(26, pBlack);
    Clr_Bit(key[2], 7);
  }

  if ((data3 & 0x08) == 0x08) {
    Set_Bit(key[3], 0);
    Leds[27] = CRGB::Red;
    // pixels.setPixelColor(28, pRed);
    M5.Speaker.tone(NOTE_DH1, 20);
  } else {
    Leds[27] = CRGB::Black;
    // pixels.setPixelColor(28, pBlack);
    Clr_Bit(key[3], 0);
  }

  delay(20);
  FastLED.show();

  M5.update();

}

void  Init_TS20(void)
{
  //------------------ Software Reset Enable (Set)----------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(GTRL2)); // sends register address
  Wire.write(byte(0x1A)); // sends register data
  Wire.endTransmission(); // stop transmitting

  //----------------- Port Control ------------------------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL1)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL2)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL3)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL4)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL5)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL6)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  //--------------- Sensitivty control,(threshold level)-----------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM1)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM2)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM3)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM4)); // sends register address
  Wire.write(byte(0xF5)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM5)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM6)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM7)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM8)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); /// send ic slave address
  Wire.write(byte(Sensitivity_PWM9)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM10)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // sned ic slave address
  Wire.write(byte(Sensitivity_PWM11)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(GTRL1)); // sends register address
  Wire.write(byte(0x4B)); // sends register data
  Wire.endTransmission(); // stop transmitting
  //------------------ Calibration On ---------------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_HOLD1)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_HOLD2)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_HOLD3)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Err_CTRL)); // sends register address
  Wire.write(byte(0x0D)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_CTRL)); // sends register address
  Wire.write(byte(0xFA)); // sends register data
  Wire.endTransmission(); // stop transmitting

  //------------------ Software Reset Disable (Clear) -----------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(GTRL2)); // sends register address
  Wire.write(byte(0x12)); // sends register data
  Wire.endTransmission(); // stop transmitting

}
