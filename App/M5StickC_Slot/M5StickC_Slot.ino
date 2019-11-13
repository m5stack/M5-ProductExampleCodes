#include <M5StickC.h>
#include "Slot.h"

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

#define LOOP_WAIT 30
#define FLUSH_DELAY 100
#define FLUSH_COUNT 3

Slot slots[SLOT_COUNT];
const int symbolIndices[] = { 2, 4, 5, 0, 3, 4, 1, 5, 3 };

enum SlotsState { SLOTS_INIT, SLOTS_START, SLOTS_STOP = SLOT_COUNT + 1, SLOTS_FLUSH };
int state = SLOTS_INIT;



//
// setup()
//
void setup() {
 	M5.begin();
	M5.Lcd.setRotation(3);
	M5.Lcd.fillScreen(TFT_BLACK);
	M5.Lcd.setSwapBytes(true);

	M5.Axp.ScreenBreath(12);


	Slot::initShadow();
	Slot::setReel(symbolIndices, _countof(symbolIndices));
	for (int i = 0; i < SLOT_COUNT; i++) {
		slots[i].init(i, i * SLOT_COUNT);
		slots[i].draw();
	}
}

//
// loop()
//
void loop() {
	unsigned long tick = millis();
	static unsigned long flushTick;
	static int flushCount;
  M5.update();
	if (M5.BtnA.wasPressed()) {
		if (state == SLOTS_INIT) {
			for (int i = 0; i < SLOT_COUNT; i++) {
				slots[i].start();
			}
			state++;
		} else if (state < SLOTS_STOP) {
			slots[state - 1].stop();
			state++;
		}
	}
	if (state == SLOTS_STOP) {
		int symbol = -1;
		bool stopAll = true;
		for (int i = 0; i < SLOT_COUNT; i++) {
			int n = slots[i].getSymbol();
			if (n == -1) {
				stopAll = false;
			} else {
				symbol = (n == symbol || symbol == -1) ? n : -2;
			}
		}
		if (stopAll) {
			if (symbol >= 0) {
				flushTick = tick;
				flushCount = 0;
				state = SLOTS_FLUSH;
			} else {
				state = SLOTS_INIT;
			}
		}
	}
	if (state == SLOTS_FLUSH) {
		if (tick >= flushTick + FLUSH_DELAY) {
			flushTick = tick;
			for (int i = 0; i < SLOT_COUNT; i++) {
				slots[i].flush((flushCount & 1) ? TFT_WHITE : TFT_BLUE);
			}
			if (++flushCount >= FLUSH_COUNT * 2) {
				state = SLOTS_INIT;
			}
		}
	}

	for (int i = 0; i < SLOT_COUNT; i++) {
		if (slots[i].update()) {
			slots[i].draw();
		}
	}

	int ms = millis() - tick;
	if (ms < LOOP_WAIT) {
		delay(LOOP_WAIT - ms);
	}
}
