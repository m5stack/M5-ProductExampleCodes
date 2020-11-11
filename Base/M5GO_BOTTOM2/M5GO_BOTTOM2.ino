/*
    Description: TEST M5GO BOTTOM2(Only for Core2),IMU,MIC,LED.
    Please install library before compiling:  
    FastLED: https://github.com/FastLED/FastLED
*/

#include <M5Core2.h>
#include <FastLED.h>

#include <driver/i2s.h>
#include "fft.h"

#define PIN_CLK  0
#define PIN_DATA 34

#define MODE_MIC 0

TFT_eSprite DisFFTbuff =  TFT_eSprite(&M5.Lcd);
TFT_eSprite DisIMUbuff =  TFT_eSprite(&M5.Lcd);

static QueueHandle_t fftvalueQueue = nullptr;
// static QueueHandle_t i2sstateQueue = nullptr;

#define NUM_LEDS 10

#define DATA_PIN 25

CRGB leds[NUM_LEDS];

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

float temp = 0.0F;



bool InitI2SSpakerOrMic(int mode)
{


    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 128,
    };
    if (mode == MODE_MIC)
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    }

    //Serial.println("Init i2s_driver_install");

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_pin_config_t pin_config;
    pin_config.bck_io_num   = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num    = PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num  = PIN_DATA;

    //Serial.println("Init i2s_set_pin");
    i2s_set_pin(I2S_NUM_0, &pin_config);
    //Serial.println("Init i2s_set_clk");
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

    return true;
}

static void i2sMicroFFTtask(void *arg)
{
    uint8_t FFTDataBuff[128];
    uint8_t FFTValueBuff[24];
    uint8_t* microRawData = (uint8_t*)calloc(2048,sizeof(uint8_t));
    size_t bytesread;
    int16_t* buffptr;
    double data = 0;
    float adc_data;
    uint16_t ydata;
    uint32_t subData;

    uint8_t state = MODE_MIC;
    while(1)
    {
        if( state == MODE_MIC )
        {
            fft_config_t *real_fft_plan = fft_init(1024, FFT_REAL, FFT_FORWARD, NULL, NULL);
            i2s_read(I2S_NUM_0, (char *)microRawData, 2048, &bytesread, (100 / portTICK_RATE_MS));
            buffptr = ( int16_t*)microRawData;

            for ( int count_n = 0; count_n < real_fft_plan->size; count_n++)
            {
                adc_data = (float)map(buffptr[count_n], INT16_MIN, INT16_MAX, -2000, 2000);
                real_fft_plan->input[count_n] = adc_data;
            }
            fft_execute(real_fft_plan);

            for ( int count_n = 1; count_n < real_fft_plan->size / 4; count_n++)
            {
                data = sqrt(real_fft_plan->output[2 * count_n] * real_fft_plan->output[2 * count_n] + real_fft_plan->output[2 * count_n + 1] * real_fft_plan->output[2 * count_n + 1]);
                if ((count_n - 1) < 128)
                {
                    data = ( data > 2000 ) ? 2000 : data;
                    ydata = map(data, 0, 2000, 0, 255);
                    FFTDataBuff[128 - count_n] = ydata;
                }
            }

            for( int count = 0; count < 24; count++ )
            {
                subData = 0;
                for( int count_i = 0; count_i < 5; count_i++ )
                {
                    subData += FFTDataBuff[count * 5 + count_i ];
                }
                subData /= 5;
                FFTValueBuff[count] = map(subData,0,255,0,8);
            }
            xQueueSend( fftvalueQueue, (void * )&FFTValueBuff, 0 );
            fft_destroy(real_fft_plan);
        }
        else
        {
            delay(10);
        }
    }
}

void microPhoneInit()
{
    fftvalueQueue = xQueueCreate(5, 24 * sizeof(uint8_t));
    if( fftvalueQueue == 0 )
    {
        return;
    }
    InitI2SSpakerOrMic(MODE_MIC);
    xTaskCreatePinnedToCore(i2sMicroFFTtask, "microPhoneTask", 4096, NULL, 3, NULL, 0);

    DisFFTbuff.createSprite(320,100);
}


void MicroPhoneFFT()
{
    uint8_t FFTValueBuff[24];
    xQueueReceive( fftvalueQueue, (void * )&FFTValueBuff, portMAX_DELAY );
    DisFFTbuff.fillRect(0,0,320,54,DisFFTbuff.color565(0x00,0x00,0x00));
    uint32_t colorY = DisFFTbuff.color565(0xff,0x9c,0x00);
    uint32_t colorG = DisFFTbuff.color565(0x66,0xff,0x00);
    uint32_t colorRect;
    for( int x = 0; x < 24; x++ )
    {
        for( int y = 0; y < 9; y++ )
        {
            if( y < FFTValueBuff[23-x] )
            {
                colorRect = colorY;
            }
            else if( y == FFTValueBuff[23-x] )
            {
                colorRect = colorG;
            }
            else
            {
                continue;
            }
            DisFFTbuff.fillRect(x*12,54-y*6 - 5,5,5,colorRect);
        }
    }
    DisFFTbuff.pushSprite(20, 130);
}


void imuDisplay() {
    M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
    M5.IMU.getAccelData(&accX,&accY,&accZ);
    M5.IMU.getAhrsData(&pitch,&roll,&yaw);
    M5.IMU.getTempData(&temp);
    DisIMUbuff.fillRect(0,0,320,100,BLACK);
    char data[60];
    snprintf(data , sizeof(data), "%6.2f  %6.2f  %6.2f", gyroX, gyroY, gyroZ);
    DisIMUbuff.drawString(data,0, 0);

    snprintf(data , sizeof(data), "%6.2f  %6.2f  %6.2f", gyroX, gyroY, gyroZ);
    DisIMUbuff.drawString(data,0, 15);

    snprintf(data , sizeof(data), "%5.2f   %5.2f   %5.2f", pitch, roll, yaw);
    DisIMUbuff.drawString(data,0, 30);

    snprintf(data , sizeof(data), "Temperature : %.2f", temp);
    DisIMUbuff.drawString(data,0, 45);
    DisIMUbuff.pushSprite(0, 0);
}

void LEDtask(void *arg){
    while (1){
        if(roll>20){
            for(int i = 0; i<5 ; i++){
                leds[i] = CRGB::Green;
                leds[9-i] = CRGB::Green;
                FastLED.show();
                delay(80);
            }
            for(int i = 0; i<5 ; i++){
                leds[i] = CRGB::Black;
                leds[9-i] = CRGB::Black;
                FastLED.show();
                delay(80);
            }
            
        }else if(roll<-20){
            for(int i = 4; i>=0 ; i--){
                leds[i] = CRGB::Green;
                leds[9-i] = CRGB::Green;
                FastLED.show();
                delay(80);
            }
            for(int i = 4; i>=0 ; i--){
                leds[i] = CRGB::Black;
                leds[9-i] = CRGB::Black;
                FastLED.show();
                delay(80);
            }
        }else{
            fill_solid(leds, 10, CRGB::Red);
            FastLED.show();
            delay(1000);
            fill_solid(leds, 10, CRGB::Blue);
            FastLED.show();
            delay(1000);
        }
        delay(200);
    }
}


// the setup routine runs once when M5Stack starts up
void setup(){

  // Initialize the M5Stack object
  M5.begin();
  M5.IMU.Init();
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setTextSize(2);
  microPhoneInit();
  DisIMUbuff.createSprite(320,100);
  xTaskCreatePinnedToCore(LEDtask, "LEDTask", 4096, NULL, 2, NULL, 0);
}

void loop() {
    MicroPhoneFFT();
    imuDisplay();
}
