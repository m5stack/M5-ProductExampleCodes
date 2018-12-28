/* 
 * L293DDH.cpp - Library for using a L293DDH motor controller chip
 * Created by Tyler Sweat, May 25 2015
 * Released under the MIT License
 */

#include "Arduino.h"
#include "L293DDH_half.h"

L293DDH::L293DDH(int _pwm_pin, int _dir_pin)
{
    pwm_pin = _pwm_pin;
    dir_pin = _dir_pin;
    pinMode(pwm_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    // Set initially to 0
    set(0);
}

void L293DDH::set(int value)
{
    val = constrain(value, -255, 255);
    if(value == 0)
    {
        digitalWrite(dir_pin, 0);
	    analogWrite(pwm_pin, 0);
    }
    else if(value > 0 && value <= 255)
    {
        digitalWrite(dir_pin, 0);
	    analogWrite(pwm_pin, value);
    }
    else if(value < 0 && value >= -255)
    {
        digitalWrite(dir_pin, 1);
	    analogWrite(pwm_pin, 255 + value);
    }
}

void L293DDH::stop()
{
    set(0);
}

int L293DDH::get()
{
    return val;
}
