#include <M5Stack.h>
#include "FastLED.h"
#include "arduinoFFT.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN_1    21
#define DATA_PIN_2    26
#define NUM_LEDS    1600
CRGB leds_1[NUM_LEDS];
CRGB leds_2[NUM_LEDS];

#define FRAMES_PER_SECOND  120

static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

uint8_t ring_index;
uint8_t g_last_ringindex=0, g_cur_ringindex=0;
uint8_t flag_fade = 0;

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

#define M5STACKFIRE_MICROPHONE_PIN 34
#define M5STACKFIRE_SPEAKER_PIN 25 // speaker DAC, only 8 Bit

#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION   240
#define POSITION_OFFSET_Y      20
#define SIGNAL_LENGTH 512

double oldSignal[SIGNAL_LENGTH];
double adcBuffer[SIGNAL_LENGTH];
double vImag[SIGNAL_LENGTH];

#define SAMPLINGFREQUENCY 40000
#define SAMPLING_TIME_US     ( 1000000UL/SAMPLINGFREQUENCY )
#define ANALOG_SIGNAL_INPUT        M5STACKFIRE_MICROPHONE_PIN

double AdcMeanValue = 0;

typedef void (*SimplePatternList[])();
// SimplePatternList gPatterns = { rainbow, fadetoblack, rainbow1};
SimplePatternList gPatterns = { rainbow };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void led_ring_A(uint8_t ring_index, uint8_t state){
  if(state){
    fill_rainbow( leds_1+ring_index*216, 216, gHue, 3);
  }
  else{
    fadeToBlackBy( leds_1+ring_index*216, 216, 254);
  }
}

void led_ring_B(uint8_t ring_index, uint8_t state){
  if(state){
    fill_rainbow( leds_2+ring_index*216, 216, gHue, 3);
  }
  else{
    fadeToBlackBy( leds_2+ring_index*216, 216, 254);
  }
}

void led_display(uint8_t state){
  if(state){
    led_ring_A(0, 1);
    FastLED.show();
    delay(5);
    led_ring_A(2, 1);
    FastLED.show();
    delay(20);
    led_ring_A(3, 1);
    FastLED.show();
//    delay(20);
    led_ring_A(1, 1);
    FastLED.show();
    delay(20);
    led_ring_B(0, 1);
    FastLED.show();
    delay(20);
    led_ring_B(1, 1);
    FastLED.show();
    delay(20);
    led_ring_B(2, 1);
    FastLED.show();
    delay(20);
  }
  else{
    led_ring_A(0, 0);
    FastLED.show();
    delay(40);
    led_ring_A(2, 0);
    FastLED.show();
    delay(40);
    led_ring_A(3, 0);
    FastLED.show();
//    delay(40);
    led_ring_A(1, 0);
    FastLED.show();
    delay(50);
    led_ring_B(0, 0);
    FastLED.show();
    delay(50);
    led_ring_B(1, 0);
    FastLED.show();
    delay(50);
    led_ring_B(2, 0);
    FastLED.show();
    delay(50);


    // led_ring_B(2, 0);
    // FastLED.show();
    // led_ring_B(1, 0);
    // FastLED.show();
    // led_ring_B(0, 0);
    // FastLED.show();
    // led_ring_A(3, 0);
    // FastLED.show();
    // led_ring_A(2, 0);
    // FastLED.show();
    // led_ring_A(1, 0);
    // FastLED.show();
    // led_ring_A(0, 0);
    // FastLED.show();
  }


}



void FastLEDshowESP32()
{
    if (userTaskHandle == 0) {
        userTaskHandle = xTaskGetCurrentTaskHandle();
        xTaskNotifyGive(FastLEDshowTaskHandle);
        const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );
        ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        userTaskHandle = 0;
    }
}

void FastLEDshowTask(void *pvParameters)
{
    for(;;) {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(flag_fade == 1){
          led_display(1);
        }
        else{
          led_display(0);
        }
        FastLED.show();
        EVERY_N_MILLISECONDS( 20 ) { gHue++; }
        // xTaskNotifyGive(userTaskHandle);
    }
}

void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(115200);
  M5.begin();

  dacWrite(M5STACKFIRE_SPEAKER_PIN, 0); // make sure that the speaker is quite
  M5.Lcd.begin();
  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.fillRect(10, 1, 150, 160, BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(GREEN);  //M5.Lcd.setTextSize(3);
  M5.Lcd.setTextSize(1);

  M5.Lcd.println("analog input MIC");
  M5.Lcd.print("max. frequency: "); M5.Lcd.print(SAMPLINGFREQUENCY / 2); M5.Lcd.println(" Hz");
  M5.Lcd.setTextSize(2);

  FastLED.addLeds<WS2811,DATA_PIN_1,GRB>(leds_1, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811,DATA_PIN_2,GRB>(leds_2, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(60);

  int core = xPortGetCoreID();
  Serial.print("Main code running on core ");
  Serial.println(core);

  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, NULL, 1);
}

void showSignal()
{
  int n;

  int oldx;
  int oldy;
  int oldSig;
  int x, y;

  for (n = 0; n < SIGNAL_LENGTH/2; n++)
  {
    x = n;
    y = map(adcBuffer[n], 0, 512, VERTICAL_RESOLUTION, POSITION_OFFSET_Y);

    if (n > 0)
    {
      M5.Lcd.drawLine(oldx , oldSig, x, oldSignal[n], BLACK );
      if (n < SIGNAL_LENGTH - 1) // don't draw last element because it would generate artifacts
      {
        M5.Lcd.drawLine(oldx,    oldy, x,            y, GREEN );
      }
    }
    oldx = x;
    oldy = y;
    oldSig = oldSignal[n];
    oldSignal[n] = y;
  }
}

void loop()
{
  int n;
  uint32_t nextTime = 0;

  for (n = 1; n < SIGNAL_LENGTH; n++)
  {
    double v = analogRead( ANALOG_SIGNAL_INPUT );
    AdcMeanValue += (v - AdcMeanValue) * 0.001;
    adcBuffer[n] = v - AdcMeanValue;
    while (micros() < nextTime);
    nextTime = micros() + SAMPLING_TIME_US;
  }

  FFT.Windowing(adcBuffer, SIGNAL_LENGTH, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  FFT.Compute(adcBuffer, vImag, SIGNAL_LENGTH, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(adcBuffer, vImag, SIGNAL_LENGTH); /* Compute magnitudes */
  int x = FFT.MajorPeak(adcBuffer, SIGNAL_LENGTH, SAMPLINGFREQUENCY);

  int maxAmplitudeDB = 0;
  for (n = 1; n < SIGNAL_LENGTH; n++)
  {
    int a = log10(adcBuffer[n]) * 20 - 54.186; // convert amplitude to dB scale, dB relative to log10(512samples)*20=54.186dB
    if (a > maxAmplitudeDB) maxAmplitudeDB = a;
    adcBuffer[n] = (a + 30) * 5; // scale for TFT display
    vImag[n] = 0; // clear imaginary part
  }

  showSignal();

  M5.Lcd.fillRect(200, 0, 119, 40, BLUE);
  M5.Lcd.setCursor(210, 1);
  M5.Lcd.print(x); M5.Lcd.print(" Hz");
  M5.Lcd.setCursor(210, 21);
  M5.Lcd.print(maxAmplitudeDB); M5.Lcd.print(" dB");
  M5.Lcd.fillRect(20,41,50,100,
  M5.Lcd.setCursor(20,41);
M5.Lcd.print(adcBuffer[64]);
  ring_index = maxAmplitudeDB;
  if(ring_index > 27){
    flag_fade = 1;
  }
  else{
    flag_fade = 0;
  }

  Serial.println(ring_index);

  delay(50);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds_1, 20, gHue, 7);
}