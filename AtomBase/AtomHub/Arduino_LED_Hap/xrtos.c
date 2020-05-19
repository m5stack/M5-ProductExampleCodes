#include "port_x.h"
#if defined(ARDUINO8266_SERVER)
#include "c_types.h"
#include "eagle_soc.h"
#include "osapi.h"
#include "xrtos.h"
#include "ets_sys.h"
#include "os_type.h"
#include "queue.h"

#define Q_NUM 3
#define MAX_ETS_TASK 3

typedef struct  {
	callbacktask_with_arg_t fn;
	void* context;
	unsigned int state;
}loc_task_t;
loc_task_t loc_tasks[MAX_ETS_TASK] = {0};
static unsigned int  t_num=0;
os_event_t local_event[Q_NUM];
//simple queue
// STAILQ.

QueueHandle_t xQueueCreate(unsigned int size, unsigned int elem_size) {
	if (elem_size != sizeof(unsigned int))
		return NULL;  // not supported
	//STAILQ_HEAD(stailqhead, q_data_s)
	q_head_t* head=malloc(sizeof(q_head_t));
	STAILQ_INIT(head);
	return head;
}


int xQueueSendToBack(QueueHandle_t xQueue, const void * pvItemToQueue, unsigned int xTicksToWait) {
	q_data_t *datap = malloc(sizeof(q_data_t));
	datap->value = (int)pvItemToQueue;
	
	STAILQ_INSERT_TAIL(xQueue, datap, entries);
}
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, unsigned int xTicksToWait) {
	if (STAILQ_EMPTY(xQueue))
		return FALSE;
	q_data_t* datap = STAILQ_FIRST(xQueue);
	memcpy(pvBuffer, datap->value, sizeof(unsigned int));
	
	STAILQ_REMOVE_HEAD(xQueue, entries);
	free(datap);
	return TRUE;
}
void vQueueDelete(QueueHandle_t q) {
	while (!STAILQ_EMPTY(q)) {
		q_data_t* datap = STAILQ_FIRST(q);
		STAILQ_REMOVE_HEAD(q, entries);
		free(datap);
	}

	free(q);
}
static void local_task(os_event_t* ev) {
	for (unsigned int i = 0; i < t_num; i++)
		if (loc_tasks[i].state==1 && loc_tasks[i].fn) {
			loc_tasks[i].state = 2;
			loc_tasks[i].fn(loc_tasks[i].context);
		}
	//esp_yield();
	//esp_schedule();
	//ets_post(0x2, 0, 0);
}
void* xTaskCreate(callbacktask_with_arg_t fn, const char* szname, unsigned int stacksize, void* context, unsigned int priority, void* handle) {
	//os_timer_disarm(&modeTimer);
	//os_timer_setfn(&modeTimer, (os_timer_func_t *)lpd6803_loop, NULL);
	//os_timer_arm(&modeTimer, 50, 1);
	if (t_num >= MAX_ETS_TASK)
		return NULL;
	loc_tasks[t_num].fn = fn;
	loc_tasks[t_num].context = context;
	loc_tasks[t_num].state = 1;
//if(!t_num)
//	fn(context);
//	return NULL;
	ets_task(&local_task, 0x2, local_event, Q_NUM);
t_num++;
ets_post(0x2, 0, 0);
	return &loc_tasks[t_num];
	
};
void vTaskDelay(unsigned int delay) {
	//none 
}
void vTaskDelete(void* handle) {
	//none 
}

#endif