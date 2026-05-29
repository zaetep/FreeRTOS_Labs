#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <event_groups.h>

QueueHandle_t xLEDQueue;
QueueHandle_t xUARTQueue;
SemaphoreHandle_t xADCAvgReady;
EventGroupHandle_t xPWMEvent;
volatile uint32_t g_ui32ADCSum = 0;
volatile uint32_t g_ui32ADCSampleNum = 0;