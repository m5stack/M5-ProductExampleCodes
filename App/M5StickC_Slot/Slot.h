//
// Slot.h
//
#pragma once

#include <M5StickC.h>

#define SLOT_COUNT 3

enum SlotState { SLOT_INIT, SLOT_START, SLOT_STOP, SLOT_DRIFT };

class Slot {
public:
	Slot() {}
	void init(int unit, int index);
	void draw();
	void flush(uint16_t bgColor = TFT_RED);
	void start(int acc = 12, int maxVel = 720);
	void stop(int acc = -20, int minVel = 50);
	int getSymbol() { return (index == -1) ? -1 : symbolIndices[index]; }
	bool update();

	static void initShadow(int shadowHeight = 16);
	static void setReel(const int *symbolIndices, int reelLength);

private:
	int index;
	int posX;
	int height;
	float degree;
	int vel; // deg/s
	int maxVel;
	int minVel;
	int acc; // deg/s^2
	SlotState state;
	unsigned long tick;

	static const int *symbolIndices;
	static int reelLength;
	static int reelHeight;
	static int shadowHeight;
	static uint8_t *shadowBrigtness;
	static uint16_t *buffer;

	static uint16_t darker(uint16_t rgb, uint8_t brightness);
	static uint8_t calcBrightness(float step);
	static float sigmoid(float t);
};