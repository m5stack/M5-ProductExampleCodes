/* 
 * L293D.h - Library for using a L293D motor controller chip
 * Created by Tyler Sweat, May 25 2015
 * Released under the MIT License
 */

#ifndef L293DD_HALF_H
#define L293DD_HALF_H

#include "Arduino.h"

class L293DDH
{
  public:
  	L293DDH(int, int);
  	void set(int);
	void stop();
  	int get();
  private:
  	int pwm_pin;
  	int dir_pin;
  	int val;
};

#endif
