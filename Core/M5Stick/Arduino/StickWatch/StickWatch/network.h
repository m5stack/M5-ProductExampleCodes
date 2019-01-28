#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include "ArduinoJson.h"

#include "utils.h"

WiFiMulti wifiMulti;

bool isTimeOK = false;

uint64_t queryTimeByHttp() {
  uint64_t c_time = 0;
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  http.begin("http://worldclockapi.com/api/json/utc/now"); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.printf("length: %d\n", payload.length());
      DynamicJsonDocument doc;
      deserializeJson(doc, payload);
      JsonObject obj = doc.as<JsonObject>();

      String currentFileTime = obj[String("currentFileTime")];
      Serial.println( currentFileTime);
      c_time = obj["currentFileTime"].as<unsigned long long>();
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  return c_time;
}
void syncTimeFromWifi() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("RTC already have time, will not connect to time server");
    isTimeOK = true;
    printLocalTime();
  } else {
    ESP_LOGE(TAG, "Failed to obtain time");
    Serial.printf("Connecting to %s ", ssid);
    boolean wifiConnected = true;
    unsigned long wifiStartTime = millis();
    showConnectingWifi();
    while (wifiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      if (millis() - wifiStartTime >= 10 * 1000) {
        wifiConnected = false;
        break;
      }
    }
    Serial.println("");
    if (wifiConnected) {
      showWifiConnected();
      Serial.println("Yes! WIFI Connected");
      uint64_t c_time = queryTimeByHttp();
      struct timeval tv = {};
      tv.tv_sec = c_time / HUNDRED_NANO_SECONDS - 11644473600 + gmtOffset_sec;
      tv.tv_usec = 0;
      settimeofday(&tv, NULL);
      printLocalTime();
      // Serial.println( int64String(c_time));
      struct tm timeinfo;
      //init and get the time
      if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed config time by server");
      } else {
        Serial.println("Successfully config time by server:");
        isTimeOK = true;
        printLocalTime();
      }
    } else {
      Serial.println("No, WIFI not connected");
    }
    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
}


#endif // _NETWORK_H_
