#include <M5Stack.h>

void setup() {
  M5.begin();
  //disable the speak noise
  dacWrite(25, 0);

  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(2);

  M5.Lcd.setCursor(80, 80);
  M5.Lcd.print("GPIO34_x:");
  M5.Lcd.setCursor(80, 100);
  M5.Lcd.print("GPIO35_y:");
  M5.Lcd.setCursor(80, 120);
  M5.Lcd.print("GPIO2_z:");

  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(2, INPUT_PULLUP);
}

uint16_t x_value = 0;
uint16_t y_value = 0;
uint16_t z_value = 0;
void loop() {
  // put your main code here, to run repeatedly:
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setCursor(185, 80);
  M5.Lcd.printf("%d\n", x_value);
  M5.Lcd.setCursor(185, 100);
  M5.Lcd.printf("%d\n", y_value);
  M5.Lcd.setCursor(185, 120);
  M5.Lcd.printf("%d\n", z_value);
  
  x_value = analogRead(34);
  y_value = analogRead(35);
  z_value = digitalRead(2);
  
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(185, 80);
  M5.Lcd.printf("%d\n", x_value);
  M5.Lcd.setCursor(185, 100);
  M5.Lcd.printf("%d\n", y_value);
  M5.Lcd.setCursor(185, 120);
  M5.Lcd.printf("%d\n", z_value);
  delay(10);
}
