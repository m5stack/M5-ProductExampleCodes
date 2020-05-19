#pragma once

#if defined ARDUINO
#if defined ESP32
//#include <task.h>
//#include <queue.h>
#elif defined ESP8266
#define portTICK_PERIOD_MS 10
#if( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

#else
#endif

#endif