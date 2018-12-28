// Visual Micro is in vMicro>General>Tutorial Mode
/*
    Name:       Servo328.ino
    Created:		2018/10/9 9:34:42
    Author:     BIN\salahei
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
#include <Servo.h>
#include <Wire.h>

#define Server_Addr 0x53
Servo ch[12];
uint8_t pin[12] = { 0, 1, 2, 3, 4, 5, 6, 7, A0, A1, A2, A3 };
bool flag[12] = { 0 };
uint8_t commandStatus = 0;

// The setup() function runs once each time the micro-controller starts
void receiveEvent(int rev_number) {
	if (rev_number != 3 && rev_number != 2) {
		while (Wire.available() > 0)
			Wire.read();
		return;
	}

	while (Wire.available() > 0)
	{
		uint8_t data = Wire.read();
		uint8_t rev_event = data & 0xF0;
		uint8_t number = data & 0x0F;
		if (number > 11) { break; }

    if(flag[number] == 0) {
      flag[number] = 1;
      ch[number].attach(pin[number]);
    }

		if (rev_event == 0x00) {
			if (rev_number != 3) { break; }
			uint8_t time_low = Wire.read();
			uint8_t time_higt = Wire.read();
			ch[number].writeMicroseconds((uint16_t)time_higt*256 + time_low);
		} else if (rev_event == 0x10) {
			if (rev_number != 2) { break; }
			uint8_t angle = Wire.read();
			ch[number].write(angle);
		}
	}
	while (Wire.available() > 0) {
			Wire.read();
	}
}


void setup() {
  for(uint8_t i = 0; i < 12; i++) {
    digitalWrite(pin[i], HIGH);
  }

	Wire.begin(Server_Addr);
	Wire.onReceive(receiveEvent);
}

// Add the main program code into the continuous loop() function
void loop() {

}
