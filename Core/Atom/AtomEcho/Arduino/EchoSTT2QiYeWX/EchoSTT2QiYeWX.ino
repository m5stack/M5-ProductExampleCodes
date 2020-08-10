/****************************************************************

   This Example only for M5AtomEcho!

   Arduino tools Setting
   -board : Atom Echo
   -Upload Speed: 115200 / 750000 / 1500000

****************************************************************/
#include "M5Atom.h"
#include <WiFi.h>
#include <driver/i2s.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "BaiduRest.h"

#define CONFIG_I2S_BCK_PIN 19
#define CONFIG_I2S_LRCK_PIN 33
#define CONFIG_I2S_DATA_PIN 22
#define CONFIG_I2S_DATA_IN_PIN 23

#define SPEAK_I2S_NUMBER I2S_NUM_0

#define MODE_MIC 0
#define MODE_SPK 1

// Wi-Fi & m5stack token
const char *WifiSSID = "XXXXXXXXXX";
const char *WifiPWD = "XXXXXXXXXX";
const char *m5stack_token = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

// 企业微信
#define WX_CORPID      "XXXXXXXXXXXXXXXXXXXX"
#define WX_CORPSECRET  "XXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define WX_AGANTID     99999999
#define WX_USERID      "XXXXXXXXX" // 多用户用 “|” 分隔，如 "user1|user2|user3"

StaticJsonDocument<300>    WX_json_decode;
#define WX_REQUEST_TOKEN_API "https://qyapi.weixin.qq.com/cgi-bin/gettoken?corpid=%s&corpsecret=%s"
#define WX_SEND_MSG_API "https://qyapi.weixin.qq.com/cgi-bin/message/send?access_token=%s"
#define WX_TEXT_CONTENT "{\"touser\":\"%s\",\"msgtype\":\"text\",\"agentid\":%d,\"text\":{\"content\":\"%s\"},\"safe\":0,\"enable_id_trans\":0,\"enable_duplicate_check\":0,\"duplicate_check_interval\":1800}"

void setClock();
int getWXtoken(void);
int sendWXmsg(String msg);
time_t nowSecs = 0, oldSecs = 0;

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

  while ( WiFi.status() != WL_CONNECTED )
  {
    delay(500);
    Serial.print('.');
  }

  setClock();

  M5.dis.drawpix(0, CRGB(128, 0, 0));

  Serial.println("\nConnected.");

  rest.settoken(m5stack_token);

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
      // Serial.println(SpakeStr);
      Spakeflag = true;
      M5.dis.drawpix(0, CRGB(128, 0, 128));

      nowSecs = time(nullptr);
      //判断token 是否过期
      if ((nowSecs - oldSecs) > 7000)
      {
        oldSecs = nowSecs;
        getWXtoken();
      }
      sendWXmsg(SpakeStr);
    }
    else
    {
      M5.dis.drawpix(0, CRGB(0, 128, 0));
    }
  }

  if ( WiFi.status() != WL_CONNECTED )
  {
    Serial.println("Wifi reconnect");
    WiFi.reconnect();
    while ( WiFi.status() != WL_CONNECTED )
    {
      delay(100);
    }
  }
  M5.update();
  delay(100);
}


void setClock() {
  configTime(0, 0, "cn.pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

char buf[500];
String access_token_buf;

int getWXtoken(void)
{
  int ret = 0;
  {
    HTTPClient https;

    sprintf(buf, WX_REQUEST_TOKEN_API, WX_CORPID, WX_CORPSECRET);
    Serial.print("[HTTPS] begin...\n");
    if (https.begin((String)buf)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
//          Serial.println(payload);
          deserializeJson(WX_json_decode, payload);

          int errcode =  WX_json_decode["errcode"].as<int>();
          String errmsg =  WX_json_decode["errmsg"].as<String>();

          if ( errcode == 0 && strcmp((char *)&errmsg, "ok") == 0) {
            access_token_buf = WX_json_decode["access_token"].as<String>();
//            Serial.println(access_token_buf);
            ret = 1;
          }
          else {
            Serial.println("json fail!");
            ret = 0;
          }
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        ret = -1;
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
      ret = -2;
    }
  }
  return ret;
}

int sendWXmsg(String msg)
{
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    sprintf(buf, WX_SEND_MSG_API, (char *)access_token_buf.c_str());
    http.begin(buf);

    http.addHeader("Content-Type", "application/json;charset=UTF-8");

    sprintf(buf, WX_TEXT_CONTENT, WX_USERID, WX_AGANTID, (char *)msg.c_str());
    int httpResponseCode = http.POST(buf);

//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);

    // Free resources
    http.end();
  }
}
