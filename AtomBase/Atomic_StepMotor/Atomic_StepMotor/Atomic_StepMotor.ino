//Press button the stepmotor will turn 5000 steps clockwise then turn 5000 steps anti-clockwise


#include <M5Atom.h>
#include "StepperDriver.h"

int motor_steps = 200;    
int step_divisition = 32;
int en_pin = 22;
int dir_pin = 23;
int step_pin = 19;

int step = 0;
int speed = 0;


StepperDriver ss(motor_steps, step_divisition, en_pin, dir_pin, step_pin);  

void setup()
{
  M5.begin(true, false, true);
  ss.setSpeed(0);
  ss.powerEnable(true);
  delay(1600);
}

void loop()
{
   if(M5.Btn.wasPressed()) {
      ss.setSpeed(300);
      ss.step(5000);
      ss.step(-5000);
   }
   M5.update();
}
