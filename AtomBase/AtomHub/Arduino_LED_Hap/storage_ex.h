/*
 * storage_ex.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: yurik
 */

#ifndef COMPONENTS_COMMON_HOMEKIT_INCLUDE_HOMEKIT_STORAGE_EX_H_
#define COMPONENTS_COMMON_HOMEKIT_INCLUDE_HOMEKIT_STORAGE_EX_H_
 #include "pairing.h"


#ifdef EX_STORAGE_CHAR_EX1

char * get_ex_storage();
int get_ex_storage_size();
int init_storage_ex(char* szdata,int size);
typedef void(*callback_function)(void);

void set_callback_storage(callback_function val);
void on_storage_change();
#endif

#ifdef EX_STORAGE_CHAR_EX
#define MAGIC_ADDR_EX 0
#define ACCESSORY_ID_ADDR_EX (0 + ACCESSORY_ID_OFFSET)
#define ACCESSORY_KEY_ADDR_EX   (0 + ACCESSORY_KEY_OFFSET)
#define PAIRINGS_ADDR_EX        (0 + PAIRINGS_OFFSET)
extern char ex_storage[PAIRINGS_OFFSET + MAX_PAIRINGS * sizeof(pairing_data_t) + 1];

int init_storage_ex(char* szdata, int size) {
	int actualsize = size > sizeof(ex_storage) ? sizeof(ex_storage) : size;
	INFO("init_storage_ex size 0x%x", actualsize);
	memcpy((void *)&ex_storage[0], szdata, actualsize);
	return actualsize;
}
char * get_ex_storage() {
	return ex_storage;
}
int get_ex_storage_size() {
	return sizeof(ex_storage);
}
typedef void(*callback_function)(void);

static callback_function callbackstorage = NULL;

void set_callback_storage(callback_function fn) {
	callbackstorage = fn;
}
void on_storage_change() {

	if (callbackstorage) {
		callbackstorage();

	}
}
#endif
#endif /* COMPONENTS_COMMON_HOMEKIT_INCLUDE_HOMEKIT_STORAGE_EX_H_ */
