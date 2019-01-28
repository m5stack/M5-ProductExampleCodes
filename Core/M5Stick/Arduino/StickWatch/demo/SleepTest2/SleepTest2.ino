#include<Wire.h>

#define LedPin 19
#define BuzzerPin 26
#define BtnPin 35

// ================ Power IC IP5306 ===================
#define IP5306_ADDR           117
#define IP5306_REG_SYS_CTL0   0x00
#define IP5306_REG_READ1      0x71
#define CHARGE_FULL_BIT       3

void setPowerBoostKeepOn(bool en) {
  Wire.beginTransmission(IP5306_ADDR);
  Wire.write(IP5306_REG_SYS_CTL0);
  if (en) Wire.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  else Wire.write(0x35);    // 0x37 is default reg value
  int error = Wire.endTransmission();
  Serial.print("Wire.endTransmission: ");
  Serial.println(error);
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
  Wire.begin(21, 22, 100000);
  setPowerBoostKeepOn(false);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();
  Serial.print("isChargeFull? ");
  bool isFull = isChargeFull();
  Serial.println(isFull);

  if (isFull) {
    // beep
    for (int i = 0; i < 200; i++) {
      digitalWrite(BuzzerPin, HIGH);
      delayMicroseconds(2);
      digitalWrite(BuzzerPin, LOW);
      delayMicroseconds(1998);
    }
  }
  
  // led flash
  for (int i = 0; i < 20; i++) {
    digitalWrite(LedPin, 1 - digitalRead(LedPin));
    delay(500);
  }

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, LOW); //1 = High, 0 = Low
  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
  //This is not going to be called
}
