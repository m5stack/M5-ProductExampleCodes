
#include "RoverC.h"



void RoverC_Init(void)    //sda  0     scl  26
{
    Wire.begin(0,26,100);
}

void Send_iic(uint8_t Register, uint8_t Speed)
{
  Wire.beginTransmission(ROVER_ADDRESS);
  Wire.write(Register);
  Wire.write(Speed);
  Wire.endTransmission();
}

void Move_forward(uint8_t Speed)
{
  Send_iic(0x00, 127 );
  Send_iic(0x01, 127 );
  Send_iic(0x02, 127 );
  Send_iic(0x03, 127 );
}

void Move_back(uint8_t Speed)
{
  Send_iic(0x00, -127 );
  Send_iic(0x01, -127 );
  Send_iic(0x02, -127 );
  Send_iic(0x03, -127 );
}

void Move_turnleft(uint8_t Speed)
{
  Send_iic(0x00, 127 );
  Send_iic(0x01, -127 );
  Send_iic(0x02, 127 );
  Send_iic(0x03, -127 );
}

void Move_turnright(uint8_t Speed)
{
  Send_iic(0x00, -127 );
  Send_iic(0x01, 127 );
  Send_iic(0x02, -127 );
  Send_iic(0x03, 127 );
}

void Move_left(uint8_t Speed)
{
  Send_iic(0x00, -127 );
  Send_iic(0x01, 127 );
  Send_iic(0x02, 127 );
  Send_iic(0x03, -127 );
}

void Move_right(uint8_t Speed)
{
  Send_iic(0x00, 127 );
  Send_iic(0x01, -127 );
  Send_iic(0x02, -127 );
  Send_iic(0x03, 127 );
}
void Move_stop(uint8_t Speed)
{
  Send_iic(0x00, 0 );
  Send_iic(0x01, 0 );
  Send_iic(0x02, 0 );
  Send_iic(0x03, 0 );
}
