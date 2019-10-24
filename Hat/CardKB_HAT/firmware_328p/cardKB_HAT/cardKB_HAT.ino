#include <Adafruit_NeoPixel.h>
#define PIN           13
#define NUMPIXELS      1
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#include <Wire.h>

#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )


//     d0   d1   d2 d3 d4 d5 d6 d7 d8 d9   d10    d11     d12      d15    d14
//A0：  1    2   3   4  5  6  7  8  9  0
//A1:   Q    W   E   R  T  Y  U  I  O  P   esc/~  +/-     :/;      ,/.    del
//A2:shift   A   S   D  F  G  H  J  K  L   {/}    </>     //_      \/=    '/"
//A3:  fn  space Z   X  C  V  B  N  M ?/up |/dn   [/left  ]/right  enter

unsigned char KeyMap[64][3] =
{ //normal, shift, fn,
  {'1', '!', 128}, //PD:254
  {'2', '@', 129}, //253
  {'3', '#', 130}, //251
  {'4', '$', 131},//247
  {'5', '%', 132},//239
  {'6', '^', 133},//223
  {'7', '&', 134}, //191
  {'8', '*', 135},//127
  {'9', '(', 136},//PB:222
  {'0', ')', 137},//221
  {0, 0, 0},//219
  {0, 0, 0},//215
  {0, 0, 0},//207
  {0, 0, 0},//95
  {0, 0, 0},//159
  {0, 0, 0},

  {'q', 'Q', 138},
  {'w', 'W', 139},
  {'e', 'E', 140},
  {'r', 'R', 141},
  {'t', 'T', 142},
  {'y', 'Y', 143},
  {'u', 'U', 144},
  {'i', 'I', 145},
  {'o', 'O', 146},//222
  {'p', 'P', 147},//221
  {27, '~', 148},//esc//219
  {'+', '-', 149},//215
  {':', ';', 150},//207
  {',', '.', 151},//95
  {8, 8, 152},//del//159
  {0, 0, 0},

  {128, 128, 0}, //shift
  {'a', 'A', 153},
  {'s', 'S', 154},
  {'d', 'D', 155},
  {'f', 'F', 156},
  {'g', 'G', 157},
  {'h', 'H', 158},
  {'j', 'J', 159},
  {'k', 'K', 160},
  {'l', 'L', 161},
  {'{', '}', 162},
  {'<', '>', 163},
  {'/', '_', 164},
  {'\\', '=', 165},
  {'\'', '"', 166},
  {0, 0, 0},

  {129, 129, 0}, //fn
  { ' ' , ' ', 167},
  {'z', 'Z', 168},
  {'x', 'X', 169},
  {'c', 'C', 170},
  {'v', 'V', 171},
  {'b', 'B', 172},
  {'n', 'N', 173},
  {'m', 'M', 174},
  {'?', 180, 175},
  {'|', 181, 176},
  {'[', 182, 177},
  {']', 183, 178},
  {13, 13, 179}, //enter
  {0, 0, 0},
  {0, 0, 0}

};



#define shiftPressed (PINB & 0x10 ) != 0x10
#define symPressed (PINB & 0x80 ) != 0x80
#define fnPressed (PINB & 0x40 ) != 0x40
int _shift = 0, _fn = 0, _sym = 0, idle = 0;
unsigned char KEY = 0, hadPressed = 0;
int Mode = 0; //0->normal.1->shift 2->long_shift, 3->fn, 4->long_fn
void flashOn()
{
  pixels.setPixelColor(0, pixels.Color(0, 0, 3)); pixels.show();
}
void flashOff()
{
  pixels.setPixelColor(0, pixels.Color(0, 0, 0)); pixels.show();
}
void requestEvent()
{
  if (hadPressed == 1)
  {
    switch (Mode)
    {
      case 0: Wire.write(KeyMap[KEY][0]); hadPressed = 0; break;
      case 1: Wire.write(KeyMap[KEY][1]); hadPressed = 0; Mode=0;break;
      case 2: Wire.write(KeyMap[KEY][1]); hadPressed = 0; break;
      case 3: Wire.write(KeyMap[KEY][2]); hadPressed = 0; Mode=0;break;
      case 4: Wire.write(KeyMap[KEY][2]); hadPressed = 0; break;
    }
  }
  return;
}


void setup()
{
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  DDRB = 0x00;
  PORTB = 0xff;
  DDRD = 0x00;
  PORTD = 0xff;

  pixels.begin();

  for (int i = 0; i < 30; i++)
  {
    pixels.setPixelColor(0, pixels.Color(i, 0, 0)); pixels.show();
    delay(6);
  }
  for (int i = 30; i > 0; i--)
  {
    pixels.setPixelColor(0, pixels.Color(i, 0, 0)); pixels.show();
    delay(6);
  }
  for (int i = 0; i < 30; i++)
  {
    pixels.setPixelColor(0, pixels.Color(0, i, 0)); pixels.show();
    delay(6);
  }
  for (int i = 30; i > 0; i--)
  {
    pixels.setPixelColor(0, pixels.Color(0, i, 0)); pixels.show();
    delay(6);
  }
  for (int i = 0; i < 30; i++)
  {
    pixels.setPixelColor(0, pixels.Color(0, 0, i)); pixels.show();
    delay(6);
  }
  for (int i = 30; i > 0; i--)
  {
    pixels.setPixelColor(0, pixels.Color(0, 0, i)); pixels.show();
    delay(6);
  }
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();
  Wire.begin(0x5f);
  Wire.onRequest(requestEvent);
}
void loop()
{
  //if (hadPressed == 0)
  //{
  //KEY = GetInput();
  switch (Mode) //Mode0:normal; 1:shift_one;2:shift_long;3:fn_one;4_fn_long
  {
    case 0:
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      if (hadPressed == 0)
      {
        KEY = GetInput();
        switch (KEY)
        {
          case 255:  break; //no input
          case 0x20://shift
            while (GetInput() != 255)delay(1); //
            delay(200);
            if (GetInput() == 0x20) //longShift
            {
              Mode = 2;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 1;
              return;
            } break;
          case 0x30://fn
            while (GetInput() != 255)delay(1); //
            delay(200);
            if (GetInput() == 0x30) //longfn
            {
              Mode = 4;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 3;
              return;
            } break;
          default: hadPressed = 1; break;
        }
      } break;
    case 1: //oneShift
      if ((idle / 6) % 2 == 1)
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      else
        pixels.setPixelColor(0, pixels.Color(5, 0, 0));
      if (hadPressed == 0)
      {
        KEY = GetInput();
        switch (KEY)
        {
          case 255:  break; //no input
          case 0x20://shift
            while (GetInput() != 255)delay(1);
            delay(200);
            if (GetInput() == 0x20) //double click
            {
              Mode = 2;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 0;
              return;
            }
          case 0x30://fn
            while (GetInput() != 255)delay(1); //
            delay(200);
            if (GetInput() == 0x30) //longfn
            {
              Mode = 4;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 3;
              return;
            } break;
          default: hadPressed = 1; break;
        }
      }
      break;
    case 2://longShift
      pixels.setPixelColor(0, pixels.Color(5, 0, 0));
      if (hadPressed == 0)
      {
        KEY = GetInput();
        switch (KEY)
        {
          case 255:  break; //no input
          case 0x20://shift
            while (GetInput() != 255)delay(1);
            delay(200);
            if (GetInput() == 0x20) //double click
            {
              Mode = 0;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 0;
              return;
            }
          case 0x30://fn
            while (GetInput() != 255)delay(1); //
            delay(200);
            if (GetInput() == 0x30) //longfn
            {
              Mode = 4;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 3;
              return;
            } break;
          default: hadPressed = 1; break;
        }
      }
      break;
    case 3: //oneFn
      if ((idle / 6) % 2 == 1)
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      else
        pixels.setPixelColor(0, pixels.Color(0, 5, 0));
      if (hadPressed == 0)
      {
        KEY = GetInput();
        switch (KEY)
        {
          case 255:  break; //no input
          case 0x20://shift
            while (GetInput() != 255)delay(1);
            delay(200);
            if (GetInput() == 0x20) //double click
            {
              Mode = 2;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 1;
              return;
            }
          case 0x30://fn
            while (GetInput() != 255)delay(1); //
            delay(200);
            if (GetInput() == 0x30) //longfn
            {
              Mode = 4;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 0;
              return;
            } break;
          default: hadPressed = 1; break;
        }
      }
      break;
    case 4: //longFn
      pixels.setPixelColor(0, pixels.Color(0, 5, 0));
      if (hadPressed == 0)
      {
        KEY = GetInput();
        switch (KEY)
        {
          case 255:  break; //no input
          case 0x20://shift
            while (GetInput() != 255)delay(1);
            delay(200);
            if (GetInput() == 0x20) //double click
            {
              Mode = 2;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 1;
              return;
            }
          case 0x30://fn
            while (GetInput() != 255)delay(1); //
            delay(200);
            if (GetInput() == 0x30) //longfn
            {
              Mode = 0;
              while (GetInput() != 255)delay(1);
              return;
            }
            else
            {
              Mode = 0;
              return;
            } break;
          default: hadPressed = 1; break;
        }
      }
      break;








  }

  pixels.show();
  idle++;
  delay(10);
}
unsigned char GetInput()
{
  digitalWrite(A3, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, HIGH);
  delay(2);

  switch (PIND)
  {
    case 254: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x00; break;
    case 253: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x01; break;
    case 251: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x02; break;
    case 247: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x03; break;
    case 239: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x04; break;
    case 223: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x05; break;
    case 191: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x06; break;
    case 127: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x07; break;
  }
  switch (PINB)
  {
    case 222: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x08; break;
    case 221: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x09; break;
    case 219: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x0a; break;
    case 215: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x0b; break;
    case 207: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x0c; break;
    case  95: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x0d; break;
    case 159: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x0e; break;
  }

  digitalWrite(A3, HIGH);
  digitalWrite(A2,  LOW);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, HIGH);
  delay(2);

  switch (PIND)
  {
    case 254: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x10; break;
    case 253: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x11; break;
    case 251: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x12; break;
    case 247: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x13; break;
    case 239: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x14; break;
    case 223: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x15; break;
    case 191: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x16; break;
    case 127: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x17; break;
  }
  switch (PINB)
  {
    case 222: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x18; break;
    case 221: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x19; break;
    case 219: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x1a; break; //esc
    case 215: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x1b; break;
    case 207: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x1c; break;
    case  95: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x1d; break;
    case 159: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x1e; break; //del
  }

  digitalWrite(A3, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A0, HIGH);
  delay(2);

  switch (PIND)
  {
    case 254: while (PIND != 0xff); flashOff();   hadPressed = 1; return  0x20; break; //shift
    case 253: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x21; break;
    case 251: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x22; break;
    case 247: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x23; break;
    case 239: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x24; break;
    case 223: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x25; break;
    case 191: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x26; break;
    case 127: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x27; break;
  }
  switch (PINB)
  {
    case 222: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x28; break;
    case 221: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x29; break;
    case 219: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x2a; break;
    case 215: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x2b; break;
    case 207: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x2c; break;
    case  95: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x2d; break;
    case 159: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x2e; break;
  }

  digitalWrite(A3, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, LOW);
  delay(2);

  switch (PIND)
  {
    case 254: while (PIND != 0xff); flashOff();   hadPressed = 1; return  0x30; break; //fn
    case 253: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x31; break;
    case 251: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x32; break;
    case 247: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x33; break;
    case 239: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x34; break;
    case 223: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x35; break;
    case 191: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x36; break;
    case 127: while (PIND != 0xff)flashOn(); flashOff();   hadPressed = 1; return  0x37; break;
  }
  switch (PINB)
  {
    case 222: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x38; break;
    case 221: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x39; break;
    case 219: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x3a; break;
    case 215: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x3b; break;
    case 207: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x3c; break;
    case  95: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x3d; break;
    case 159: while (PINB != 223) flashOn(); flashOff();   hadPressed = 1; return  0x3e; break; //enter
  }
  hadPressed = 0;
  return 255;
}
