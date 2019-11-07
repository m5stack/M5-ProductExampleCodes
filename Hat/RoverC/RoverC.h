#ifndef _ROVERC_H_
#define _ROVERC_H_

#include <M5StickC.h>

#define ROVER_ADDRESS	0X38
void RoverC_Init(void);	//sda  0     scl  26

void Move_forward(uint8_t Speed);

void Move_back(uint8_t Speed);

void Move_turnleft(uint8_t Speed);

void Move_turnright(uint8_t Speed);

void Move_left(uint8_t Speed);

void Move_right(uint8_t Speed);

void Move_stop(uint8_t Speed);

void Send_iic(uint8_t Register, uint8_t Speed);

#endif
