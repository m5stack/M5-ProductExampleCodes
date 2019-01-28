
#define LedPin 19

#define BuzzerPin 26

#define BtnPin 35

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void setup() {
  pinMode(LedPin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(BtnPin, INPUT);
  Serial.begin(115200);

  for (int i = 0; i < 200; i++) {
    digitalWrite(BuzzerPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(BuzzerPin, LOW);
    delayMicroseconds(1998);
  }
  
  for (int i = 0; i < 20; i++) {
    digitalWrite(LedPin, 1 - digitalRead(LedPin));
    delay(500);
  }
  //Increment boot number and print it every reboot

  //Print the wakeup reason for ESP32
  // print_wakeup_reason();

  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1); //1 = High, 0 = Low

  //If you were to use ext1, you would use it like
  //esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
  //This is not going to be called
}
