// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Wire.h>
#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )
//KEY PORTB
//IRQ PC0
#define IRQ_1 Set_Bit(PORTB,1)
#define IRQ_0 Clr_Bit(PORTB,1)
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            8
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      4
// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int delayval = 500; // delay for half a second
int LedR0, LedG0, LedB0;
int LedR1, LedG1, LedB1;
int LedR2, LedG2, LedB2;
int LedR3, LedG3, LedB3;

bool neo_change = false;
void receiveEvent(int howMany)
{
  if (howMany != 4)
  {
    while (Wire.available() > 0)
      Wire.read();
    return;
  }
  uint8_t data = Wire.read();
  switch (data)
  {
    case 0: LedR0 = Wire.read(); LedG0 = Wire.read(); LedB0 = Wire.read(); break;
    case 1: LedR1 = Wire.read(); LedG1 = Wire.read(); LedB1 = Wire.read(); break;
    case 2: LedR2 = Wire.read(); LedG2 = Wire.read(); LedB2 = Wire.read(); break;
    case 3: LedR3 = Wire.read(); LedG3 = Wire.read(); LedB3 = Wire.read(); break;
  }
  neo_change = true;
}

int adX = 0, adY = 0;
byte ad = 0;
void requestEvent()
{
  Wire.write(uint8_t(adX&0xff));
  Wire.write(uint8_t(adX>>8));
  Wire.write(uint8_t(adY&0xff));
  Wire.write(uint8_t(adY>>8));
  Wire.write(uint8_t(digitalRead(10)));
}

void setup()
{
  DDRC = 0x00;
  PORTC = 0x00;
  DDRB = 0x001;
  PORTB = 0x00;
  pinMode(10, INPUT_PULLUP);
  //IRQ_1;
  Wire.begin(0x5e);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  pixels.begin(); // This initializes the NeoPixel library.
  for (int i = 0; i < 100; i++)
  {
    pixels.setPixelColor(0, pixels.Color(i, i, i));
    pixels.setPixelColor(1, pixels.Color(i, i, i));
    pixels.setPixelColor(2, pixels.Color(i, i, i));
    pixels.setPixelColor(3, pixels.Color(i, i, i));
    pixels.show();
    delay(1);
  }
  for (int i = 100; i > 0; i--)
  {
    pixels.setPixelColor(0, pixels.Color(i, i, i));
    pixels.setPixelColor(1, pixels.Color(i, i, i));
    pixels.setPixelColor(2, pixels.Color(i, i, i));
    pixels.setPixelColor(3, pixels.Color(i, i, i));
    pixels.show();
    delay(1);
  }
  for (int i = 0; i < 100; i++)
  {
    pixels.setPixelColor(0, pixels.Color(i, i, i));
    pixels.setPixelColor(1, pixels.Color(i, i, i));
    pixels.setPixelColor(2, pixels.Color(i, i, i));
    pixels.setPixelColor(3, pixels.Color(i, i, i));
    pixels.show();
    delay(1);
  }
  for (int i = 100; i > 0; i--)
  {
    pixels.setPixelColor(0, pixels.Color(i, i, i));
    pixels.setPixelColor(1, pixels.Color(i, i, i));
    pixels.setPixelColor(2, pixels.Color(i, i, i));
    pixels.setPixelColor(3, pixels.Color(i, i, i));
    pixels.show();
    delay(1);
  }
}

void loop()
{
  adX = analogRead(A0);
  adY = analogRead(A1);
  if(neo_change) {
    neo_change = false;
    pixels.setPixelColor(0, pixels.Color(LedR0, LedG0, LedB0));
    pixels.setPixelColor(1, pixels.Color(LedR1, LedG1, LedB1));
    pixels.setPixelColor(2, pixels.Color(LedR2, LedG2, LedB2));
    pixels.setPixelColor(3, pixels.Color(LedR3, LedG3, LedB3));
    pixels.show();
  }
  delay(20);
}
