/*
  Designed by eggfly(https://github.com/eggfly)
  Fork from https://github.com/eggfly/StickWatch
*/

#include "sensor.h"
#include "temp_sensor.h"
#include "io.h"
#include "pm.h"
#include "screen.h"
#include "config.h"
#include "network.h"
#include "http_requests.h"
#include "utils.h"

void setup() {
  setPinModes();
  // simply to sleep when power chip reboot
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    led(true, 200);
    pureDeepSleep();
  }

  ESP_LOGD(TAG, "0");
  Serial.begin(115200);
  Wire.begin(21, 22, 100000);
  setPowerBoostKeepOn(true);

  // set_freq(240);
  ESP_LOGD(TAG, "1");

  u8g2.begin();
  // lightness
  u8g2.setContrast(32);

  setPinModes(); // pin modes must be set after u8g2
  ESP_LOGD(TAG, "2");
  // u8g2.fillDisplay();
  // u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setFont(u8g2_font_6x10_mf);
  // u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  // u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  // u8g2.setFontDirection(0);
  ESP_LOGD(TAG, "3");
  showSplashScreen();
  // buzzer();
  flashLed(1000);
  setupMPU9250();
  wifiMulti.addAP(ssid, password);
  wifiMulti.addAP("eggfly", "12345678");
  syncTimeFromWifi();
  print_wakeup_reason();
}


unsigned long powerOffButtonTime = 0;
unsigned long keepWakeUpTime = 0;
unsigned long lastUpdateChargingTime = 0;
bool isLastChargeFull = false;
bool isLastCharging = false;
int temp_farenheit = 0;

unsigned long fps_prev_time;

void loop() {
  unsigned long curr_time = millis();
  Serial.print("time delta: ");
  Serial.println(curr_time - fps_prev_time);
  fps_prev_time = curr_time;
  if (curr_time - keepWakeUpTime > 60 * 1000) {
    // increasePrefCounter();
    deepSleep();
  }
  if (digitalRead(BtnPin) == 1) {
    powerOffButtonTime = 0;
    // u8x8.setInverseFont(1);
    readMPU9250();
    u8g2.clearBuffer();
    drawScreenBackground();
    // drawHelloStickWatch();
    if (millis() - lastUpdateChargingTime > 5000) {
      lastUpdateChargingTime = millis();
      isLastChargeFull = isChargeFull();
      isLastCharging = isCharging();
      temp_farenheit = read_temp();
    }
    // TODO
    drawBatteryStatusWithFont(isLastCharging, isLastChargeFull);
    // u8g2.setFont(u8g2_font_6x10_tf);
    // u8g2_font_4x6_tf, u8g2_font_5x7_tf, u8g2_font_5x8_tf
    if (isTimeOK) {
      drawTime();
    }
    drawCursor();
    drawChipTemprature(temp_farenheit);
    u8g2.sendBuffer();
  } else {
    unsigned long currTime = millis();
    keepWakeUpTime = currTime;
    if (powerOffButtonTime == 0) {
      powerOffButtonTime = currTime;
    }
    if (powerOffButtonTime != 0 && currTime - powerOffButtonTime > 1000) {
      Serial.println("long pressed, delay and going to sleep now");
      deepSleep();
    }
  }
}

void deepSleep() {
  drawSleepText();
  delay(500);
  screenOffAnimation();
  pureDeepSleep();
}
