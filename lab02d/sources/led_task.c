//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "rgbled.h"
#include "buttons.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <globalvars.h>
#define LEFTRECEIVE "LEFT RECV\r\n" // when left button message is received in LED task
#define RIGHTRECEIVE "RIGT RECV\r\n" // when right button message is received in LED task

//*****************************************************************************
//
// The stack size for the LED toggle task.
//
//*****************************************************************************
#define LEDTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************


// task to toggle LEDs based on commands in the queue.
static void
vLEDTask(void *pvParameters)
{
    uint32_t receivedCommand;
    for ( ;; ) {
        if (xQueueReceive(xLEDQueue, &receivedCommand, portMAX_DELAY) == pdPASS)
        {
            switch (receivedCommand) {
                case (1): // LEFT  button pressed, toggle RED  LED
//                  if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
//                      UARTprintf(LEFTRECEIVE);
//                      xSemaphoreGive(xUARTMutex);
//                  }
//                  xQueueSendToBack(xUARTQueue, (void *)LEFTRECEIVE, portMAX_DELAY);
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,
                        GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) ^ GPIO_PIN_1);
                    break;
                case (2): // RIGHT button pressed, toggle BLUE LED
//                  if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
//                      UARTprintf(RIGHTRECEIVE);
//                      xSemaphoreGive(xUARTMutex);
//                  }
//                  xQueueSendToBack(xUARTQueue, (void *)RIGHTRECEIVE, portMAX_DELAY);
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,
                        GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) ^ GPIO_PIN_2);
                    break;
            }
        }
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
uint32_t
LEDTaskInit(void)
{

	PortFInit(); 
	
    PF1 = 0;
	PF2 = 0;
	PF3 = 0;  // red,blue, green LEDs off 
	
    //
    // Create the LED task.
    //
    if(xTaskCreate(vLEDTask, (const portCHAR *)"LED", LEDTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LED_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
