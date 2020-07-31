//Low-level signal can be generated when the magnet S pole is close to the front of the sensor 
//OR the N pole is close to the back, and the internal LED indicator will light up, the screen wiil display 0.

#include <M5Stack.h>
#define HALL 36

void header(const char *string, uint16_t color)
{
    M5.Lcd.fillScreen(color);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4);     
}

void setup() {
    M5.begin(true, false, true);
    header("HALL Sensor", TFT_BLACK);
    pinMode(HALL, INPUT);
}

void loop() {
    bool status = digitalRead(HALL);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(20, 80, 2);
    M5.Lcd.printf("Hall status : %d", status);
}
