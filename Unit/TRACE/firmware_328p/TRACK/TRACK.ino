#include "Wire.h"

// 10-bit ADC

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int LED_A = 2;
const int LED_B = 3;
const int LED_C = 4;
const int LED_D = 5;

uint16_t sensorValue_A = 0;
uint16_t sensorValue_B = 0;
uint16_t sensorValue_C = 0;
uint16_t sensorValue_D = 0;

uint16_t ThresholdValue_A = 400;
uint16_t ThresholdValue_B = 400;
uint16_t ThresholdValue_C = 400;
uint16_t ThresholdValue_D = 400;

uint8_t reg;
uint8_t level;
uint8_t enable = 1;
void receiveEvent(int howMany) {
	uint8_t number = 0;

  if ((howMany != 1) && (howMany != 3) && (howMany != 2))
  {
    while (Wire.available() > 0)
      Wire.read();
    return ;
  }

  reg = Wire.read();

  if(reg & 0x10) {
    number = reg & 0x0f;
  } else {
    while (Wire.available() > 0)
      Wire.read();
    return;
  }

  switch (number)
  {
    case 0:
       enable = Wire.read();
       if(!enable){
         digitalWrite(LED_A, LOW);
         digitalWrite(LED_B, LOW);
         digitalWrite(LED_C, LOW);
         digitalWrite(LED_D, LOW);
       }
      break;
    
    case 1:
      ThresholdValue_A = Wire.read() << 8 | Wire.read();
      break;
    case 2:
      ThresholdValue_B = Wire.read() << 8 | Wire.read();
      break;
    case 3:
      ThresholdValue_C = Wire.read() << 8 | Wire.read();
      break;
    case 4:
      ThresholdValue_D = Wire.read() << 8 | Wire.read();
      break;
    default:
      break;
  }
}

void requestEvent() {
  switch(reg)
  {
    case 0x00:
      Wire.write((uint8_t)(level));
      break;    
    case 0x01:  
      Wire.write((uint8_t)(sensorValue_A>>8)&0x03);
      Wire.write((uint8_t)(sensorValue_A&0xff));
      break;
    case 0x02:  
      Wire.write((uint8_t)(sensorValue_B>>8)&0x03);
      Wire.write((uint8_t)(sensorValue_B&0xff));
      break;
    case 0x03:  
      Wire.write((uint8_t)(sensorValue_C>>8)&0x03);
      Wire.write((uint8_t)(sensorValue_C&0xff));
      break;
    case 0x04:  
      Wire.write((uint8_t)(sensorValue_D>>8)&0x03);
      Wire.write((uint8_t)(sensorValue_D&0xff));
      break;

    case 0x05:  
      Wire.write((uint8_t)(ThresholdValue_A>>8)&0x03);
      Wire.write((uint8_t)(ThresholdValue_A&0xff));
      break;

    case 0x06:  
      Wire.write((uint8_t)(ThresholdValue_B>>8)&0x03);
      Wire.write((uint8_t)(ThresholdValue_B&0xff));
      break;

    case 0x07:  
      Wire.write((uint8_t)(ThresholdValue_C>>8)&0x03);
      Wire.write((uint8_t)(ThresholdValue_C&0xff));
      break;

    case 0x08:  
      Wire.write((uint8_t)(ThresholdValue_D>>8)&0x03);
      Wire.write((uint8_t)(ThresholdValue_D&0xff));
      break;
    default:
	  break;
  }
}

void setup() {
	Wire.begin(0x5a);
	pinMode(LED_A, OUTPUT);
    pinMode(LED_B, OUTPUT);
    pinMode(LED_C, OUTPUT);
    pinMode(LED_D, OUTPUT); 
	Wire.onReceive(receiveEvent);
	Wire.onRequest(requestEvent);
}

void loop() {
  sensorValue_A = analogRead(A0);
  sensorValue_B = analogRead(A1);
  sensorValue_C = analogRead(A2);
  sensorValue_D = analogRead(A3);
   
  if(sensorValue_A > ThresholdValue_A) {
    if(enable) digitalWrite(LED_A, LOW);
    level &= ~(1 << 0);
  } else {
    if(enable)digitalWrite(LED_A, HIGH);
    level |= 1 << 0;
  }

  if(sensorValue_B > ThresholdValue_B) {
    if(enable)digitalWrite(LED_B, LOW);
    level &= ~(1 << 1);
  } else {
    if(enable)digitalWrite(LED_B, HIGH);
    level |= 1 << 1;
  }

  if(sensorValue_C > ThresholdValue_C) {
    if(enable)digitalWrite(LED_C, LOW);
    level &= ~(1 << 2);
  } else {
    if(enable)digitalWrite(LED_C, HIGH);
    level |= 1 << 2;
  }

  if(sensorValue_D > ThresholdValue_D) {
    if(enable)digitalWrite(LED_D, LOW);
    level &= ~(1 << 3);
  } else {
    if(enable)digitalWrite(LED_D, HIGH);
    level |= 1 << 3;
  }

	delay(10);
}
