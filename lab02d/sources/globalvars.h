// ENTIRE FILE COMPLETED BY STUDENT

#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <event_groups.h>

#define MAX_STRING_LEN 128
#define ADC_GT_4090    (1 << 0)
#define ADC_LT_10      (1 << 1)

extern QueueHandle_t xLEDQueue;
extern QueueHandle_t xUARTQueue;
extern SemaphoreHandle_t xADCAvgReady;
extern EventGroupHandle_t xPWMEvent;
extern uint32_t g_ui32ADCSum;
extern uint32_t g_ui32ADCSampleNum;

#endif
