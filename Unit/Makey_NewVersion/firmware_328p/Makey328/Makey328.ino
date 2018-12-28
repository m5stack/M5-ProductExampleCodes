#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#include <Wire.h>
#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN           A2
#define NUMPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int LedR = 0, LedG = 0, LedB = 0, LedW = 0;

int Key1, Key2, Key3;
int key[20], key_delay[20];
uint32_t color[18] =
{
  pixels.Color(10, 0, 0),
  pixels.Color(10, 5, 0),
  pixels.Color(10, 10, 0),
  pixels.Color(0, 10, 0),
  pixels.Color(0, 10, 10),
  pixels.Color(0, 0, 10),
  pixels.Color(10, 0, 10),

  pixels.Color(20, 0, 0),
  pixels.Color(20, 10, 0),
  pixels.Color(20, 20, 0),
  pixels.Color(0, 20, 0),
  pixels.Color(0, 20, 20),
  pixels.Color(0, 0, 20),
  pixels.Color(20, 0, 20),

  pixels.Color(10, 0, 0),
  pixels.Color(10, 5, 0),
  pixels.Color(10, 10, 0),
  pixels.Color(0, 10, 0),
};
void RGB(int c)
{
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setPixelColor(0, color[c]);
  pixels.show();
  //DDRB = 0x00;
  //PORTB = 0x00;
}
void setup()
{
  DDRC = 0x00;
  PORTC = 0x00;
  DDRB = 0x000;
  PORTB = 0x00;
  DDRD = 0x00;
  PORTD = 0x00;
  pinMode(A3,OUTPUT);
  Wire.begin(0x1);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  for (int i = 0; i < 16; i++)
  {
    RGB(i);
    delay(50);
  }
  DDRB = 0x000;
  PORTB = 0x00;
  for (int i = 0; i < 16; i++)
  {
    key[i] = 0;
    key_delay[i] = 0;
  }


}

void receiveEvent(int howMany)
{
  if (howMany != 4)
  {
    while (Wire.available() > 0)
      Wire.read();
    return;
  }

  LedR = Wire.read();
  LedG = Wire.read();
  LedB = Wire.read();
  LedW = Wire.read();

}

void requestEvent()
{
  Wire.write(Key1);
  Wire.write(Key2);
  Wire.write(Key3);
}
void loop()
{
  if ((PIND & 0x01) == 0x00)  {
    key[0] = 1;
    key_delay[0] = 20;
    RGB(0);
    tone(A3, NOTE_C4, 20);
  }
  else   if (key_delay[0] > 0)      key_delay[0]--;    else     key[0] = 0;

  if ((PIND & 0x02) == 0x00) {
    key[1] = 1;
    key_delay[1] = 20;
    RGB(1);
    tone(A3, NOTE_D4, 20);
  } else   if (key_delay[1] > 0)      key_delay[1]--;    else     key[1] = 0;

  if ((PIND & 0x04) == 0x00) {
    key[2] = 1;
    key_delay[2] = 20;
    RGB(2);
    tone(A3, NOTE_E4, 20);
  } else   if (key_delay[2] > 0)      key_delay[2]--;    else     key[2] = 0;

  if ((PIND & 0x08) == 0x00) {
    key[3] = 1;
    key_delay[3] = 20;
    RGB(3);

    tone(A3, NOTE_F4, 20);
  } else   if (key_delay[3] > 0)      key_delay[3]--;    else     key[3] = 0;

  if ((PIND & 0x10) == 0x00) {
    key[4] = 1;
    key_delay[4] = 20;
    RGB(4);
    tone(A3, NOTE_G4, 20);
  } else   if (key_delay[4] > 0)      key_delay[4]--;    else     key[4] = 0;

  if ((PIND & 0x20) == 0x00) {
    key[5] = 1;
    key_delay[5] = 20;
    RGB(5);

    tone(A3, NOTE_A4, 20);
  } else   if (key_delay[5] > 0)      key_delay[5]--;    else     key[5] = 0;

  if ((PIND & 0x40) == 0x00) {
    key[6] = 1;
    key_delay[6] = 20;
    RGB(6);
    tone(A3, NOTE_B4, 20);
  } else   if (key_delay[6] > 0)      key_delay[6]--;    else     key[6] = 0;

  if ((PIND & 0x80) == 0x00) {
    key[7] = 1;
    key_delay[7] = 20;
    RGB(7);

    tone(A3, NOTE_C5, 20);
  } else   if (key_delay[7] > 0)      key_delay[7]--;    else     key[7] = 0;

  /////////////
  if ((PINB & 0x01) == 0x00) {
    key[8] = 1;
    key_delay[8] = 20;
    RGB(8);

    tone(A3, NOTE_D5, 20);
  } else   if (key_delay[8] > 0)      key_delay[8]--;    else     key[8] = 0;

  if ((PINB & 0x02) == 0x00) {
    key[9] = 1;
    key_delay[9] = 20;
    RGB(9);
    tone(A3, NOTE_E5, 20);
  } else   if (key_delay[9] > 0)      key_delay[9]--;    else     key[9] = 0;

  if ((PINB & 0x04) == 0x00) {
    key[10] = 1;
    key_delay[10] = 20;
    RGB(10);
    tone(A3, NOTE_F5, 20);
  } else   if (key_delay[10] > 0)      key_delay[10]--;    else     key[10] = 0;

  if ((PINB & 0x08) == 0x00) {
    key[11] = 1;
    key_delay[11] = 20;
    RGB(11);
    tone(A3, NOTE_G5, 20);
  } else   if (key_delay[11] > 0)      key_delay[11]--;    else     key[11] = 0;

  if ((PINB & 0x10) == 0x00) {
    key[12] = 1;
    key_delay[12] = 20;
    RGB(12);

    tone(A3, NOTE_A5, 20);
  } else   if (key_delay[12] > 0)      key_delay[12]--;    else     key[12] = 0;


  if ((PINB & 0x20) == 0x00) {
    key[13] = 1;
    key_delay[13] = 20;
    RGB(13);
    tone(A3, NOTE_B5, 20);
  } else   if (key_delay[13] > 0)      key_delay[13]--;    else     key[13] = 0;

  ///////////////////
  if ((PINC & 0x01) == 0x00) {
    key[14] = 1;
    key_delay[14] = 20;
    RGB(14);
    tone(A3, NOTE_C6, 20);
  } else   if (key_delay[14] > 0)      key_delay[14]--;    else     key[14] = 0;

  if ((PINC & 0x02) == 0x00) {
    key[15] = 1;
    key_delay[15] = 20;
    RGB(15);
    tone(A3, NOTE_D6, 20);
  } else   if (key_delay[15] > 0)      key_delay[15]--;    else     key[15] = 0;


  // 低音1 低音2 低音3 低音4 低音5 低音6 低音7 中音1
  Key1 = key[0]+key[1]*2+key[2]*4+key[3]*8+key[4]*16+key[5]*32+key[6]*64+key[7]*128;
  // 中音2 中音3 中音4 中音5 中音6 中音7 高音1 高音2
  Key2 = key[8]+key[9]*2+key[10]*4+key[11]*8+key[12]*16+key[13]*32+key[14]*64+key[15]*128;

0x0001 1
0x0002 2
0x0004 4
0x0008 8
0x0010 10
0x0020 14
0x0040 28
0x0080 50

0x0100
0x0200
0x0400
0x0800
0x1000
0x2000
0x4000
0x8000


 // Key3 = key[16];
  delay(4);

  int keyCount = Key1 + Key2 ;
  if (keyCount == 0)
  {
    pixels.begin(); // This initializes the NeoPixel library.
    pixels.setPixelColor(0, 0);
    pixels.show();
  }
}
