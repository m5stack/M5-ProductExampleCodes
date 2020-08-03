#include "Arduino.h"
#include "StepperDriver.h"


StepperDriver::StepperDriver(int number_of_steps, int step_division, int en_pin, int dir_pin, int step_pin)
{
  this->number_of_steps = number_of_steps;
  this->step_division = step_division;
  this->step_interval = 10000;
  this->last_step_time = 0;
  this->target_step_time1 = 0;
  this->target_step_time2 = 0;
  
  // Arduino pins for the motor control connection:
  this->en_pin = en_pin;
  this->dir_pin = dir_pin;
  this->step_pin = step_pin;

  // setup the pins on the microcontroller:
  pinMode(en_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
}

/*
  Sets the speed in revs per minute
*/
void StepperDriver::setSpeed(float rpm)
{
  step_interval = 60000000L / (number_of_steps * rpm * step_division);
}



void StepperDriver::powerEnable(bool ena)
{
  if (ena) {
    digitalWrite(en_pin, LOW);
  } else {
    digitalWrite(en_pin, HIGH);
  }
}


/*
  Moves the motor steps_to_move steps.  If the number is negative, 
  the motor moves in the reverse direction.
 */
void StepperDriver::step(long steps_to_move)
{
  steps_to_move *= step_division;
  setDirection(steps_to_move);
  last_step_time = micros();

  for (long i = abs(steps_to_move); i > 0; i--) {
    move();
  }
}



void StepperDriver::step(long steps_to_move, long steps_acc, long steps_dec)
{
  steps_to_move *= step_division;
  steps_acc *= step_division;
  steps_dec *= step_division;
  setDirection(steps_to_move);
  last_step_time = micros();

  if (steps_acc > 0) {
    for (long i = 1; i <= steps_acc; i++) {
      dynamicMove( i , steps_acc );
    }
  }

  for (long i = (abs(steps_to_move) - abs(steps_acc) - abs(steps_dec)); i > 0; i--) {
    move();
  }

  if (steps_dec > 0) {
    for (long i = (steps_dec - 1); i >= 0; i--) {
      dynamicMove( i , steps_dec );
    }
  }
}



void StepperDriver::setDirection(long steps_to_move)
{
  if (steps_to_move < 0) {
    digitalWrite(dir_pin, HIGH);
  }
  else {
    digitalWrite(dir_pin, LOW);
  }
}



void StepperDriver::move()
{
  digitalWrite(step_pin, HIGH);
  moveInterval(step_interval);
}



void StepperDriver::dynamicMove(int s1, int s2)
{
  digitalWrite(step_pin, HIGH);
  double r1 = (double)s1 / (double)s2;
  double r2 = 0.1 + 0.2*r1 + 2.2*r1*r1 - 1.5*r1*r1*r1;
  moveInterval( (unsigned long)(step_interval / r2) );
}



void StepperDriver::moveInterval(unsigned long target_delay)
{
  target_step_time1 = last_step_time + (target_delay / 2);
  target_step_time2 = last_step_time + target_delay;

  if (target_step_time1 >= last_step_time) {
    while (micros() < target_step_time1) {}
  }
  else {
    while ((long)(micros()) < (long)target_step_time1) {}
  }

  digitalWrite(step_pin, LOW);

  if (target_step_time2 >= last_step_time) {
    while (micros() < target_step_time2) {}
  }
  else {
    while ((long)(micros()) < (long)target_step_time2) {}
  }
  last_step_time = micros();
}
