#ifndef __HX711__H__
#define __HX711__H__

#include <Arduino.h>

#define HX711_SCK 32

#define HX711_DT 33

extern void Init_Hx711();
extern unsigned long HX711_Read(void);
extern unsigned long Get_Weight();
extern void Get_Gross();

#endif
