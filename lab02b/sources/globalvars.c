// ENTIRE FILE COMPLETED BY STUDENT

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

QueueHandle_t xQueue;
QueueHandle_t xUARTQueue;
SemaphoreHandle_t xUARTMutex;
