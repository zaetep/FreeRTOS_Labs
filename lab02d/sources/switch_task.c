#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/gpio.h>
#include <buttons.h>
#include <globalvars.h>
#include <priorities.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <utils/uartstdio.h>

#define SWITCHTASKSTACKSIZE 128
#define LEFTSEND "LEFT SEND\r\n" // when left button is pressed/released in switch task
#define RIGHTSEND "RIGT SEND\r\n" // when right button is pressed/released in switch task

static void vSwitchTask(void *pvParameters) {
    uint32_t command;
    uint8_t buttonsDebounced;
    uint8_t buttonsDelta;
    uint8_t buttonsRaw;
    bool left_sent = false;
    bool right_sent = false;

    for ( ;; ) {
        buttonsDebounced = ButtonsPoll(&buttonsDelta, &buttonsRaw);

        bool left = BUTTON_PRESSED(LEFT_BUTTON, buttonsDebounced, buttonsDelta);
        bool right = BUTTON_PRESSED(RIGHT_BUTTON, buttonsDebounced, buttonsDelta);

        if (left && !left_sent) {
            command = 1;
            xQueueSend(xLEDQueue, &command, pdMS_TO_TICKS(100));
//          if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
//              UARTprintf(LEFTSEND);
//              xSemaphoreGive(xUARTMutex);
//          }
//          xQueueSendToBack(xUARTQueue, (void *)LEFTSEND, pdMS_TO_TICKS(100));
            left_sent = true;
        } else if (!left) {
            left_sent = false;
        }

        if (right && !right_sent) {
            command = 2;
            xQueueSend(xLEDQueue, &command, pdMS_TO_TICKS(100));
//          if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
//              UARTprintf(RIGHTSEND);
//              xSemaphoreGive(xUARTMutex);
//          }
//          xQueueSendToBack(xUARTQueue, (void *)RIGHTSEND, pdMS_TO_TICKS(100));
            right_sent = true;
        } else if (!right) {
            right_sent = false;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

uint32_t SwitchTaskInit(void) {
    ButtonsInit();
    //
    // Create the Input Task
    //
    if(xTaskCreate(vSwitchTask, (const portCHAR *)"SWITCH", SWITCHTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_SWITCH_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
