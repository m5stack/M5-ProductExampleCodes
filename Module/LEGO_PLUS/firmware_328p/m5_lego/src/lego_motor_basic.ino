/*
 * This file is part of the M5GO Balance Bot project
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 M5Stack (https://github.com/M5stack)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Arduino.h>
#include <Encoder.h>
#include <L293DDH_half.h>
#include <Wire.h>

// ==================== System =====================
#define FIRWMARE_VER        01
#define SLAVE_DEFAULT_ADDR  0x58
#define I2C_SET_ADDR        0x63
#define FIRWMARE_VER_ADDR   0x64

#define MAGIC_VALUE         0x86
#define MAGIC_ADDR          0x00
#define EEROM_I2C_ADDR      0x01

enum MODE { DIRECT, SPEED, ANGLE };

// ==================== I2C Slave =====================
uint8_t i2c_slave_address = 0;
uint8_t i2c_read_address = 0;
uint8_t i2c_registers[32] = {0};

#define SLAVE_ADDR          0x56
#define I2C_ADDR_OFFSET     0

#define MOTOR_CTRL_ADDR     (I2C_ADDR_OFFSET + 0)
#define MOTOR_CTRL_LEN      2
#define NUMS_OF_MOTOR       4
#define MOTOR_TOTAL_LEN     (MOTOR_CTRL_LEN * NUMS_OF_MOTOR)

#define ENCODER_READ_ADDR   (MOTOR_CTRL_ADDR + MOTOR_TOTAL_LEN)
#define ENCODER_READ_LEN    4
#define NUMS_OF_ENCODER     4
#define ENCODER_TOTAL_LEN   (ENCODER_READ_LEN * NUMS_OF_ENCODER)

// ====================== Motor ========================
#define M1_PWM_PIN 3
#define M1_DIR_PIN 4
#define M2_PWM_PIN 5
#define M2_DIR_PIN 11
#define M3_PWM_PIN 6
#define M3_DIR_PIN 8
#define M4_PWM_PIN 9
#define M4_DIR_PIN 10

L293DDH motor1(M1_PWM_PIN, M1_DIR_PIN);
L293DDH motor2(M2_PWM_PIN, M2_DIR_PIN);
L293DDH motor3(M3_PWM_PIN, M3_DIR_PIN);
L293DDH motor4(M4_PWM_PIN, M4_DIR_PIN);
L293DDH *motor[4] = {&motor1, &motor2, &motor3, &motor4};
int16_t* motor_val = (int16_t*)(&(i2c_registers[MOTOR_CTRL_ADDR]));
bool motor_write_flg = false;

// ===================== Encoder =======================
#define M1_ENC_A_PIN A0
#define M1_ENC_B_PIN A1
#define M2_ENC_A_PIN A2
#define M2_ENC_B_PIN A3
#define M3_ENC_A_PIN 2
#define M3_ENC_B_PIN 7
#define M4_ENC_A_PIN 12
#define M4_ENC_B_PIN 13

Encoder encoder1(M1_ENC_A_PIN, M1_ENC_B_PIN);
Encoder encoder2(M2_ENC_A_PIN, M2_ENC_B_PIN);
Encoder encoder3(M3_ENC_A_PIN, M3_ENC_B_PIN);
Encoder encoder4(M4_ENC_A_PIN, M4_ENC_B_PIN);
Encoder *encoder[4] = {&encoder1, &encoder2, &encoder3, &encoder4};
int32_t* encoder_val = (int32_t*)(&(i2c_registers[ENCODER_READ_ADDR]));

// =================== I2c write event ==================
void receiveEvent(int howMany) {
	uint8_t write_addr = Wire.read();
	if (howMany == 1) {
		i2c_read_address = write_addr;

	} else if ((write_addr >= MOTOR_CTRL_ADDR) &&
				(write_addr < ENCODER_READ_ADDR)) {
		for (int i = 0; i < (howMany - 1); i++) {
			((uint8_t *)motor_val)[write_addr - MOTOR_CTRL_ADDR + i] = Wire.read();
		}
		motor_write_flg = true;
	}
}

// =================== I2c read event ====================
void requestEvent() {
	if (i2c_read_address < MOTOR_CTRL_ADDR + MOTOR_TOTAL_LEN) {
		Wire.write(i2c_registers[i2c_read_address]);
		Wire.write(i2c_registers[i2c_read_address + 1]);

	} else if (i2c_read_address < ENCODER_READ_ADDR + ENCODER_TOTAL_LEN) {
		for (int i = i2c_read_address; i < (i2c_read_address + 4); i++) {
			Wire.write(i2c_registers[i]);
		}

	} else if (i2c_read_address == I2C_SET_ADDR) {
		Wire.write(i2c_slave_address);

	} else if (i2c_read_address == FIRWMARE_VER_ADDR) {
		Wire.write(FIRWMARE_VER);
	}

	i2c_read_address = 0xff;
}

void setup() {
	// put your setup code here, to run once:
	delay(200);
	// motor.stop();

	Wire.begin(SLAVE_ADDR);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);
}

void loop() {

	encoder_val[0] = encoder[0]->read();
	encoder_val[1] = encoder[1]->read();
	encoder_val[2] = encoder[2]->read();
	encoder_val[3] = encoder[3]->read();

	if (motor_write_flg) {
		motor_write_flg = false;
		for (int i = 0; i < NUMS_OF_MOTOR; i++) {
			motor[i]->set(-motor_val[i]);
		}
	}
}
