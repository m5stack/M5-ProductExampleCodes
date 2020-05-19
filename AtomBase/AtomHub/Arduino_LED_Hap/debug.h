#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdlib.h>
#include <stdio.h>
#include "port_x.h"
#ifdef ARDUINO8266_SERVER_CPP
#ifdef __cplusplus
extern "C" {
#endif
#endif

	typedef unsigned char byte;
	//#define HOMEKIT_DEBUG
#ifdef HOMEKIT_DEBUG
#if defined(ARDUINO) && defined(ESP8266)
#define DEBUG(message, ...) printf_P(PSTR(">>> %s: " message "\n"), __func__, ##__VA_ARGS__)
#else
#define DEBUG(message, ...) printf(">>> %s: " message "\n", __func__, ##__VA_ARGS__)
#endif
#else

#define DEBUG(message, ...)

#endif
#if defined(ARDUINO) && defined(ESP8266)
#define INFO(message, ...) printf_P(PSTR(">>> HomeKit: " message "\n"), ##__VA_ARGS__)
#define ERROR(message, ...) printf_P(PSTR("!!! HomeKit: " message "\n"), ##__VA_ARGS__)
#else
#define INFO(message, ...) printf(">>> HomeKit: " message "\n", ##__VA_ARGS__)
#define ERROR(message, ...) printf("!!! HomeKit: " message "\n", ##__VA_ARGS__)
#endif
#ifdef ESP_IDF

//#define DEBUG_HEAP() DEBUG("Free heap: %d", esp_get_free_heap_size());

#elif defined ARDUINO && defined ESP32
	//#define DEBUG_HEAP() 
#define DEBUG_HEAP()  DEBUG("Free heap: %d", esp_get_free_heap_size());
#define INFO_HEAP()  INFO("Free heap: %d", esp_get_free_heap_size());
#elif defined(ARDUINO) && defined(ESP8266)
#define DEBUG_HEAP()  DEBUG("Free heap: %d", system_get_free_heap_size());
#define INFO_HEAP()  INFO("Free heap: %d", system_get_free_heap_size());
#else

//#define DEBUG_HEAP() DEBUG("Free heap: %d", xPortGetFreeHeapSize());

#endif

	char *binary_to_string(const byte *data, size_t size);
	void print_binary(const char *prompt, const byte *data, size_t size);


#ifdef ARDUINO8266_SERVER_CPP
#ifdef __cplusplus
} //extern C
#endif
#endif
#endif // __DEBUG_H__
