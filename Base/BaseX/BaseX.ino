/*
 * @Author: Sorzn
 * @Date: 2019-12-12 14:33:57
 * @LastEditTime: 2019-12-13 16:18:07
 * @Description: M5Stack project
 * @FilePath: /M5Stack/examples/Modules/BaseX/BaseX.ino
 */

#include <M5Stack.h>
#include "BaseX.h"

BASE_X base_x = BASE_X();
int32_t position_point = 0;
int8_t mode = NORMAL_MODE;
uint8_t max_speed = 0;
int8_t speed_point = 0;
int8_t angle = 0;
void setup()
{
    M5.begin(true, false, true, true);
    for(uint8_t i = 1; i < 5; i++)
    {
        base_x.SetMode(i, NORMAL_MODE);
    }
    M5.Lcd.setTextDatum(TC_DATUM);
    
}

void NormalMode()
{   
    M5.Lcd.drawString("BaseX Example", 160, 3, 4); 
    M5.Lcd.drawString("Normal Mode", 160, 30, 4);
    M5.Lcd.drawString("Speed:", 5, 80, 4);
    
    if(M5.BtnA.wasPressed())
    {
        M5.Lcd.fillRect(110, 150, 50, 30, BLACK);
        for (size_t i = 1; i < 5; i++)
        {
            base_x.SetMotorSpeed(i, 125);
            M5.Lcd.drawNumber(125, 150, 80, 4);
        }
    }

    if(M5.BtnC.wasPressed())
    {
        for (size_t i = 1; i < 5; i++)
        {
            base_x.SetMotorSpeed(i, -125);
            M5.Lcd.drawNumber(-125, 150, 80, 4);
        }
    }    
    
    if(M5.BtnB.wasPressed())
    {
        M5.Lcd.fillRect(110, 150, 50, 30, BLACK);
        for (size_t i = 1; i < 5; i++)
        {
            mode = POSITION_MODE;
            base_x.SetMotorSpeed(i, 0);
            base_x.SetEncoderValue(i, 0);
            base_x.SetPostionPIDMaxSpeed(i, max_speed);
            base_x.SetMode(i, mode);
        }
        M5.Lcd.clear(BLACK);

    }
}


void PositionMode()
{
    M5.Lcd.drawString("BaseX Example", 160, 3, 4); 
    M5.Lcd.drawString("Position Mode", 160, 30, 4);
    M5.Lcd.drawString("Position:", 5, 80, 4);
    M5.Lcd.drawNumber(position_point, 150, 80, 4);
    M5.Lcd.drawString("Speed:", 5, 110, 4);
    M5.Lcd.drawNumber(max_speed, 150, 110, 4);
    if(M5.BtnA.wasPressed())
    {
        position_point += 200;
        for (size_t i = 1; i < 5; i++)
        {
            base_x.SetPositionPoint(i, position_point);
        }
    }   

    if(M5.BtnB.wasPressed())
    {
        for (size_t i = 1; i < 5; i++)
        {
            mode = SPEED_MODE;
            base_x.SetMotorSpeed(i, 0);
            base_x.SetEncoderValue(i, 0);
            base_x.SetMode(i, mode);
        }
        M5.Lcd.clear(BLACK);

    } 

    if(M5.BtnC.wasPressed())
    {
        M5.Lcd.fillRect(150, 110, 50, 30, BLACK);
        max_speed += 20;
        if(max_speed > 120)
        {
            max_speed = 20;
        }

        for (size_t i = 1; i < 5; i++)
        {
            base_x.SetPostionPIDMaxSpeed(i, max_speed);
        }
    }
}

void SpeedMode()
{
    M5.Lcd.drawString("BaseX Example", 160, 3, 4); 
    M5.Lcd.drawString("Speed Mode", 160, 30, 4);
    M5.Lcd.drawString("Speed Point:", 5, 80, 4);
    M5.Lcd.drawNumber(speed_point, 260, 80, 4);
    if(M5.BtnA.wasPressed())
    {
        M5.Lcd.fillRect(240, 80, 50, 30, BLACK);
        speed_point += 2;
        for (size_t i = 1; i < 5; i++)
        {
            base_x.SetSpeedPoint(i, speed_point);
        }
    }   

    if(M5.BtnB.wasPressed())
    {
        for (size_t i = 1; i < 5; i++)
        {
            mode = 3;
            base_x.SetMotorSpeed(i, 0);
            base_x.SetEncoderValue(i, 0);
            base_x.SetMode(i, mode);
        }
        M5.Lcd.clear(BLACK);

    } 

    if(M5.BtnC.wasPressed())
    {
        M5.Lcd.fillRect(240, 80, 50, 30, BLACK);
        speed_point -= 2;   
        for (size_t i = 1; i < 5; i++)
        {
            base_x.SetSpeedPoint(i, speed_point);
        }
    }
}

void ServoMode()
{
    M5.Lcd.drawString("BaseX Example", 160, 3, 4); 
    M5.Lcd.drawString("Servo Mode", 160, 30, 4);
    M5.Lcd.drawString("Angle:", 5, 80, 4);
    M5.Lcd.drawNumber(angle, 260, 80, 4);
    if(M5.BtnA.wasPressed())
    {
        M5.Lcd.fillRect(220, 80, 80, 30, BLACK);
        angle += 10;
        for (size_t i = 1; i < 3; i++)
        {
            base_x.SetServoAngle(i, angle);
        }
    }   

    if(M5.BtnB.wasPressed())
    {
        for (size_t i = 1; i < 5; i++)
        {
            mode = NORMAL_MODE;
            base_x.SetMotorSpeed(i, 0);
            base_x.SetEncoderValue(i, 0);
            base_x.SetMode(i, mode);
        }
        M5.Lcd.clear(BLACK);

    } 

    if(M5.BtnC.wasPressed())
    {
        M5.Lcd.fillRect(220, 80, 80, 30, BLACK);
        angle -= 10;   
        for (size_t i = 1; i < 3; i++)
        {
            base_x.SetServoAngle(i, angle);
        }
    }
}




void loop()
{
    
    int32_t encoder[4];
    for (size_t i = 1; i < 5; i++)
    {
        encoder[i - 1] = base_x.GetEncoderValue(i);
    }
    
    M5.update();
    M5.Lcd.setCursor(0, 180, 4);
    M5.Lcd.printf("encoder: 1: %d,  2: %d\r\n", encoder[0], encoder[1]);
    M5.Lcd.setCursor(0, 210, 4);
    M5.Lcd.printf("encoder: 3: %d,  4: %d\r\n", encoder[2], encoder[3]);
    if(mode == NORMAL_MODE)
    {
        NormalMode();
    }
    else if(mode == POSITION_MODE)
    {
        PositionMode();
    }
    else if (mode == SPEED_MODE)
    {
        SpeedMode();
    }
    else
    {
        ServoMode();
    }
    delay(10);
}
