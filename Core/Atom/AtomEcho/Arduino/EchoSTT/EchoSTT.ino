/****************************************************************
 * 
 * This Example only for M5AtomEcho!
 * 
 * Arduino tools Setting 
 * -board : M5StickC
 * -Upload Speed: 115200 / 750000 / 1500000
 * 
****************************************************************/
#include "M5Atom.h"
#include <WiFi.h>
#include <driver/i2s.h>
#include <HTTPClient.h>
#include "BaiduRest.h"

const char *WifiSSID = "SSID";
const char *WifiPWD = "PASSWORD";

#define CONFIG_I2S_BCK_PIN 19
#define CONFIG_I2S_LRCK_PIN 33
#define CONFIG_I2S_DATA_PIN 22
#define CONFIG_I2S_DATA_IN_PIN 23

#define SPEAK_I2S_NUMBER I2S_NUM_0

#define MODE_MIC 0
#define MODE_SPK 1

bool InitI2SSpeakOrMic(int mode)
{
    esp_err_t err = ESP_OK;

    i2s_driver_uninstall(SPEAK_I2S_NUMBER);
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
    };
    if (mode == MODE_MIC)
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM);
    }
    else
    {
        i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
        i2s_config.use_apll = false;
        i2s_config.tx_desc_auto_clear = true;
    }

    Serial.println("Init i2s_driver_install");

    err += i2s_driver_install(SPEAK_I2S_NUMBER, &i2s_config, 0, NULL);
    i2s_pin_config_t tx_pin_config;

    tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;
    tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;
    tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN;

    Serial.println("Init i2s_set_pin");
    err += i2s_set_pin(SPEAK_I2S_NUMBER, &tx_pin_config);
    Serial.println("Init i2s_set_clk");
    err += i2s_set_clk(SPEAK_I2S_NUMBER, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

    return true;
}

BaiduRest rest;
uint8_t microphonedata0[1024 * 80];
size_t byte_read = 0;
int16_t *buffptr;
uint32_t data_offset = 0;

void setup()
{
    M5.begin(true, false, true);
    M5.dis.clear();

    Serial.println("Init Spaker");
    InitI2SSpeakOrMic(MODE_SPK);
    delay(100);

    Serial.println("Connecting Wifi");
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(WifiSSID, WifiPWD);

    M5.dis.drawpix(0, CRGB(0, 128, 0));
    size_t bytes_written;

    while( WiFi.status() != WL_CONNECTED )
    {
        delay(500);
        Serial.print('.');
    }

    M5.dis.drawpix(0, CRGB(128, 0, 0));

    Serial.println("Connected.");

    rest.settoken("YOUR-TOKEN");//<------------------------------------REPLACE YOUR_TOKEN-------------------

}

#define DATA_SIZE 1024

String SpakeStr;
bool Spakeflag = false;


void loop()
{

    if (M5.Btn.isPressed())
    {
        data_offset = 0;
        Spakeflag = false;
        InitI2SSpeakOrMic(MODE_MIC);
        M5.dis.drawpix(0, CRGB(128, 128, 0));
        while (1)
        {
            i2s_read(SPEAK_I2S_NUMBER, (char *)(microphonedata0 + data_offset), DATA_SIZE, &byte_read, (100 / portTICK_RATE_MS));
            data_offset += 1024;
            M5.update();
            if (M5.Btn.isReleased() || data_offset >= 81919)
                break;
            //delay(60);
        }
        Serial.println("end");

        if (rest.Pcm2String(microphonedata0, data_offset, DEV_PID_MANDARIN, &SpakeStr) != -1)
        {
            Serial.println(SpakeStr);
            Spakeflag = true;
            M5.dis.drawpix(0, CRGB(128, 0, 128));
        }
        else
        {
            M5.dis.drawpix(0, CRGB(0, 128, 0));
        }
    }

    if( WiFi.status() != WL_CONNECTED )
    {
        Serial.println("Wifi reconnect");
        WiFi.reconnect();
        while( WiFi.status() != WL_CONNECTED )
        {
            delay(100);
        }
    }
    M5.update();
    delay(100);
}
