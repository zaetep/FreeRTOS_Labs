#ifndef __QUEUES_H__
#define __QUEUES_H__

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#define MAX_STRING_LEN 64

extern QueueHandle_t xQueue;
extern QueueHandle_t xUARTQueue;
extern SemaphoreHandle_t xUARTMutex;

#endif
