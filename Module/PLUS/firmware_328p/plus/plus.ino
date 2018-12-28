// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
//15/11/10/14  7/3/2/6
#include <Wire.h>
#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )
#define GetCoder (PINC&0b00000011)
#define CoderA 0b000
#define CoderB 0b10
#define CoderC 0b11
#define CoderD 0b01
int CoderCount = 0, OldCoder = 0;
unsigned char CODER;
void encoderEvent()
{
  switch (OldCoder)
  {
    case CoderA: OldCoder = GetCoder;
      switch (OldCoder)
      {
        case CoderA: break;
        case CoderB: if (CoderCount > -127)CoderCount--;  break;
        case CoderC: break;
        case CoderD: if (CoderCount < 127)CoderCount++;break;
      } break;
    case CoderB: OldCoder = GetCoder;
      switch (OldCoder)
      {
        case CoderA: if (CoderCount < 127)CoderCount++;  break;
        case CoderB: break;
        case CoderC: if (CoderCount > -127)CoderCount--;break;
        case CoderD: break;
      } break;
    case CoderC: OldCoder = GetCoder;
      switch (OldCoder)
      {
        case CoderA: break;
        case CoderB: if (CoderCount < 127)CoderCount++;  break;
        case CoderC: break;
        case CoderD:if (CoderCount > -127)CoderCount--; break;
      } break;
    case CoderD: OldCoder = GetCoder;
      switch (OldCoder)
      {
        case CoderA: if (CoderCount > -127)CoderCount--; break;
        case CoderB: break;
        case CoderC:  if (CoderCount < 127)CoderCount++;break;
        case CoderD: break;
      } break;
  }


}
void setup()
{
  DDRB=0x00;
  PORTB=0xff;
  DDRC = 0x00;
  PORTC = 0x07;
  Wire.begin(0x62);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}
void receiveEvent(int howMany)
{
}
void requestEvent()
{
  Wire.write(CoderCount);
  if((PINB&0b00000100)==0)
  Wire.write(1);
  else
  Wire.write(255);
  CoderCount = 0;
}
void loop()
{
  encoderEvent();
}
