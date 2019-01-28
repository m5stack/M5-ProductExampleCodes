#ifndef _PM_H_
#define _PM_H_

#include <driver/adc.h>
#include <esp_pm.h>
#include <esp_sleep.h>

// ================ Power IC IP5306 ===================
#define IP5306_ADDR           117
#define IP5306_REG_SYS_CTL0   0x00
#define IP5306_REG_READ0      0x70
#define IP5306_REG_READ1      0x71
#define IP5306_REG_READ2      0x72
#define CHARGE_FULL_BIT       3
#define CHARGING_BIT          3
#define POWER_LOAD_BIT        2

//RTC_DATA_ATTR int bootCount = 0;

char * reason;
char REASON_UNDEFINED[] = "UNDEFINED";
char REASON_ALL[] = "ALL";
char REASON_EXT0[] = "RTC_IO";
char REASON_EXT1[] = "RTC_CNTL";
char REASON_TIMER[] = "TIMER";
char REASON_TOUCHPAD[] = "TOUCHPAD";
char REASON_ULP[] = "ULP";
char REASON_GPIO[] = "GPIO";
char REASON_UART[] = "UART";
char REASON_OTHERS[] = "OTHERS";


void pureDeepSleep() {
  // https://esp32.com/viewtopic.php?t=3083
  // esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  IMU.setSleepEnabled(true);
  delay(100);
  adc_power_off();
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, LOW); //1 = High, 0 = Low
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_UNDEFINED : ESP_LOGI(TAG, "In case of deep sleep, reset was not caused by exit from deep sleep"); reason = REASON_UNDEFINED; break;
    case ESP_SLEEP_WAKEUP_ALL : ESP_LOGI(TAG, "Not a wakeup cause, used to disable all wakeup sources"); reason = REASON_ALL; break;
    case ESP_SLEEP_WAKEUP_EXT0 : ESP_LOGI(TAG, "Wakeup caused by external signal using RTC_IO"); reason = REASON_EXT0; break;
    case ESP_SLEEP_WAKEUP_EXT1 : ESP_LOGI(TAG, "Wakeup caused by external signal using RTC_CNTL"); reason = REASON_EXT1; break;
    case ESP_SLEEP_WAKEUP_TIMER : ESP_LOGI(TAG, "Wakeup caused by timer"); reason = REASON_TIMER; break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : ESP_LOGI(TAG, "Wakeup caused by touchpad"); reason = REASON_TOUCHPAD; break;
    case ESP_SLEEP_WAKEUP_ULP : ESP_LOGI(TAG, "Wakeup caused by ULP program"); reason = REASON_ULP; break;
    case ESP_SLEEP_WAKEUP_GPIO : ESP_LOGI(TAG, "Wakeup caused by GPIO (light sleep only)"); reason = REASON_GPIO; break;
    case ESP_SLEEP_WAKEUP_UART : ESP_LOGI(TAG, "Wakeup caused by UART (light sleep only)"); reason = REASON_UART; break;
    default : ESP_LOGI(TAG, "Wakeup was not caused by deep sleep: %d\n", wakeup_reason); reason = REASON_OTHERS; break;
  }
}


void setPowerBoostKeepOn(bool en) {
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  else Wire.write(0x35);    // 0x37 is default reg value
  int error = Wire.endTransmission();
  Serial.print("Wire.endTransmission result: ");
  Serial.println(error);
}


bool isCharging() {
  uint8_t data;
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_READ0);
  Wire.endTransmission(false);
  Wire.requestFrom(IP5306_ADDR, 1);
  data = Wire.read();
  if (data & (1 << CHARGING_BIT)) return true;
  else return false;
}

bool isChargeFull() {
  uint8_t data;
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_READ1);
  Wire.endTransmission(false);
  Wire.requestFrom(IP5306_ADDR, 1);
  data = Wire.read();
  if (data & (1 << CHARGE_FULL_BIT)) return true;
  else return false;
}


uint8_t IP5306_readRegister(uint8_t registerAddress) {
  uint8_t data;
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(registerAddress);
  Wire.endTransmission(false);
  Wire.requestFrom(IP5306_ADDR, 1);
  data = Wire.read();
  return data;
}

//void set_freq(int freq) {
//  if (freq > 240) freq /= 1000000;
//  rtc_cpu_freq_t max_freq;
//  if (!rtc_clk_cpu_freq_from_mhz(freq, &max_freq)) {
//    Serial.println("Not a valid frequency");
//  }
//  esp_pm_config_esp32_t pm_config;
//  pm_config.max_cpu_freq = max_freq;
//  pm_config.min_cpu_freq = RTC_CPU_FREQ_XTAL;
//  pm_config.light_sleep_enable = false;
//
//  if (esp_pm_configure(&pm_config) != ESP_OK) {
//    Serial.println("Error configuring frequency");
//  }
//  rtc_clk_cpu_freq_set(max_freq);
//}

#endif // _PM_H_
