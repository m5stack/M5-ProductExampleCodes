//Press the Atom button, test the propeller rotates in turn, and the serial monitor outputs the IMU status


#include <M5Atom.h>
#include "AtomFly.h"

CRGB led(0, 0, 0);
CRGB HSVtoRGB(uint16_t h, uint16_t s, uint16_t v);

double pitch, roll;
double Calibrationbuff[3];
double r_rand = 180 / PI;
void setup()
{
    M5.begin(true, false, true);

    Calibrationbuff[0] = 0;
    Calibrationbuff[1] = 0;
    Calibrationbuff[2] = 0;

    fly.begin();
    if( fly.initFly() != 0)
    {
        Serial.println("faild");
        led = CRGB(0,255,0);
        M5.dis.drawpix(0, led);
        while(1)
        {
            delay(100);
        }
    }

    Serial.println("OK");
    led = CRGB(255,0,0);
    M5.dis.drawpix(0, led);
    /*
    for (int i = 0; i < 360; i++)
    {
        led = HSVtoRGB(i, 100, 100);
        M5.dis.drawpix(0, led);

        if (i % 10 == 0)
        {
            fly.getAttitude(&pitch, &roll);
            Calibrationbuff[0] += pitch;
            Calibrationbuff[1] += roll;
        }
        delay(5);
    }
    */
    Calibrationbuff[0] = Calibrationbuff[0] / 36;
    Calibrationbuff[1] = Calibrationbuff[1] / 36;

    Serial.printf("Calibration:%.2f,%.2f\n", Calibrationbuff[0], Calibrationbuff[1]);

    led = HSVtoRGB(0, 0, 100);
    M5.dis.drawpix(0, led);
}

uint8_t mode = 0;
uint16_t count = 0;
void loop()
{
    uint16_t tofData = fly.readTOF();
    tofData = (tofData > 500) ? 500 : tofData;

    fly.getAttitude(&pitch, &roll);
    double arc = atan2(pitch, roll) * r_rand + 180;
    double val = sqrt(pitch * pitch + roll * roll);
    Serial.printf("%.2f,%.2f,%.2f,%.2f\n", pitch, roll, arc, val);

    val = (val * 6) > 100 ? 100 : val * 6;

    led = HSVtoRGB(arc, val, 100 - (tofData / 5));
    M5.dis.drawpix(0, led);

    switch (mode)
    {
    case 0:
        fly.WritePWM(AtomFly::kMotor_A, 0);
        fly.WritePWM(AtomFly::kMotor_B, 0);
        fly.WritePWM(AtomFly::kMotor_C, 0);
        fly.WritePWM(AtomFly::kMotor_D, 0);
        break;
    case 1:
        fly.WritePWM(AtomFly::kMotor_A, 100);
        fly.WritePWM(AtomFly::kMotor_B, 0);
        fly.WritePWM(AtomFly::kMotor_C, 0);
        fly.WritePWM(AtomFly::kMotor_D, 0);
        break;
    case 2:
        fly.WritePWM(AtomFly::kMotor_A, 0);
        fly.WritePWM(AtomFly::kMotor_B, 100);
        fly.WritePWM(AtomFly::kMotor_C, 0);
        fly.WritePWM(AtomFly::kMotor_D, 0);
        break;
    case 3:
        fly.WritePWM(AtomFly::kMotor_A, 0);
        fly.WritePWM(AtomFly::kMotor_B, 0);
        fly.WritePWM(AtomFly::kMotor_C, 100);
        fly.WritePWM(AtomFly::kMotor_D, 0);
        break;
    case 4:
        fly.WritePWM(AtomFly::kMotor_A, 0);
        fly.WritePWM(AtomFly::kMotor_B, 0);
        fly.WritePWM(AtomFly::kMotor_C, 0);
        fly.WritePWM(AtomFly::kMotor_D, 100);
        break;
    case 5:
        fly.WritePWM(AtomFly::kMotor_A, 100);
        fly.WritePWM(AtomFly::kMotor_B, 100);
        fly.WritePWM(AtomFly::kMotor_C, 100);
        fly.WritePWM(AtomFly::kMotor_D, 100);
        break;
    default:
        fly.WritePWM(AtomFly::kMotor_A, 0);
        fly.WritePWM(AtomFly::kMotor_B, 0);
        fly.WritePWM(AtomFly::kMotor_C, 0);
        fly.WritePWM(AtomFly::kMotor_D, 0);
        break;
    }
    count ++;
    if( count >= 30 )
    {
        count = 0;
        mode |= 0x80;

    }
    if (M5.Btn.wasPressed())
    {
        mode &= 0x7f;
        mode ++;
        mode = mode > 5 ? 1 : mode;
        count = 0;
    }

    M5.update();
    delay(10);
}

// R,G,B from 0-255, H from 0-360, S,V from 0-100

CRGB HSVtoRGB(uint16_t h, uint16_t s, uint16_t v)
{
    CRGB ReRGB(0, 0, 0);
    int i;
    float RGB_min, RGB_max;
    RGB_max = v * 2.55f;
    RGB_min = RGB_max * (100 - s) / 100.0f;

    i = h / 60;
    int difs = h % 60;
    float RGB_Adj = (RGB_max - RGB_min) * difs / 60.0f;

    switch (i)
    {
    case 0:

        ReRGB.r = RGB_max;
        ReRGB.g = RGB_min + RGB_Adj;
        ReRGB.b = RGB_min;
        break;
    case 1:
        ReRGB.r = RGB_max - RGB_Adj;
        ReRGB.g = RGB_max;
        ReRGB.b = RGB_min;
        break;
    case 2:
        ReRGB.r = RGB_min;
        ReRGB.g = RGB_max;
        ReRGB.b = RGB_min + RGB_Adj;
        break;
    case 3:
        ReRGB.r = RGB_min;
        ReRGB.g = RGB_max - RGB_Adj;
        ReRGB.b = RGB_max;
        break;
    case 4:
        ReRGB.r = RGB_min + RGB_Adj;
        ReRGB.g = RGB_min;
        ReRGB.b = RGB_max;
        break;
    default: // case 5:
        ReRGB.r = RGB_max;
        ReRGB.g = RGB_min;
        ReRGB.b = RGB_max - RGB_Adj;
        break;
    }

    return ReRGB;
}
