#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/gpio.h>
#include <buttons.h>
#include <queues.h>
#include <priorities.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <utils/uartstdio.h>

#define SWITCHTASKSTACKSIZE 128

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
            UARTprintf("Left Button Pressed");
            xQueueSend(xQueue, &command, portMAX_DELAY);
            left_sent = true;
        } else if (!left) {
            left_sent = false;
        }

        if (right && !right_sent) {
            command = 2;
            UARTprintf("Right Button Pressed");
            xQueueSend(xQueue, &command, portMAX_DELAY);
            right_sent = true;
        } else if (!right) {
            right_sent = false;
        }
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
