// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define PIN            10
#define NUMPIXELS      12
#define GET_CODE() uint8_t(digitalRead(A1) << 4 | digitalRead(A0))

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int32_t count = 65536;
int32_t count_last = 65536;
int8_t count_change = 0;
uint8_t code = 0;
uint8_t code_old = 0;
uint8_t led_sta[4];
bool led_change = false;

void encoderEvent() {
  code = GET_CODE();
  if(code != code_old) {
    if(code == 0x00) {
      count_last = count;
      if(code_old == 0x10) {
        count--;
        count_change == -127 ? count_change : count_change--;
      } else {
        count_change == 127 ? count_change : count_change++;
      }
    }
    code_old = code;
  }
}

void Led(int i, int r, int g, int b) {
  pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
}

void receiveEvent(int howMany) {
  if(howMany != 4) {
    while (Wire.available() > 0) {
      Wire.read();
    }
  }

  if(Wire.available() > 0) {
    led_sta[0] = Wire.read();
    led_sta[1] = Wire.read();
    led_sta[2] = Wire.read();
    led_sta[3] = Wire.read();
    if(led_sta[0] < 12) led_change = true;
  }
}

void requestEvent() {
  Wire.write(count_change);
  count_change = 0;
  uint8_t value = digitalRead(A2);
  Wire.write(value);
}


void setup() {
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);

  Wire.begin(0x5e);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(50);
  pixels.show();

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 200; j += 4) {
      Led(i, j, j, j);
    }
  }

  delay(200);
  for (int i = 0; i < 12; i++) {
    for (int j = 200; j > 0; j -= 4) {
      Led(i, j, j, j);
    }
    Led(i, 0, 0, 0);
  }

  code = GET_CODE();
  code_old = code;
  // Led(count%12, 0, 0, 255);
}
 
void loop() {
  encoderEvent();
  if(led_change) {
    led_change = false;
    Led(led_sta[0], led_sta[1], led_sta[2], led_sta[3]);
  }
  delay(1);
}
