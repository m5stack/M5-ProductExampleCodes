#pragma once
#include "port_x.h"

#if defined(ARDUINO8266_SERVER)

#include "Arduino.h"
#include "queue.h"

#define gpio_enable(p,m) pinMode(p,m)
#define gpio_set_direction(p,m) pinMode(p,m)
#define gpio_set_level(p,l) digitalWrite(p,l)

typedef void(*callbacktask_with_arg_t)(void*);
void* xTaskCreate(callbacktask_with_arg_t fn, const char* szname, unsigned int stacksize, void* context, unsigned int priority, void* handle);
void vTaskDelay(unsigned int delay);
void vTaskDelete(void* thandle);


typedef struct q_data_s q_data_t;
struct q_data_s {
	unsigned int value;
	STAILQ_ENTRY(q_data_s) entries;
};
typedef STAILQ_HEAD(stailqhead, q_data_s) q_head_t;
typedef q_head_t* QueueHandle_t;
QueueHandle_t xQueueCreate(unsigned int size, unsigned int elem_size);

int xQueueSendToBack(QueueHandle_t xQueue, const void * pvItemToQueue, unsigned int xTicksToWait);
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, unsigned int xTicksToWait);

void vQueueDelete(QueueHandle_t q);
#endif
