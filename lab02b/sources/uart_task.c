// ENTIRE FILE COMPLETED BY STUDENT

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <inc/hw_uart.h>
#include <inc/hw_sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <globalvars.h>
#include <queue.h>
#include <task.h>
#include <priorities.h>
#include <utils/uartstdio.h>

#define UARTTASKSTACKSIZE 256

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
static void ConfigureUART(void) {
    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Use the system clock as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_SYSTEM);
    // Set the baud rate based on the clock source.
    UARTStdioConfig(0, 115200, SysCtlClockGet());
}

static void vUARTGatekeeperTask(void *pvParameters) {
    char outputString[MAX_STRING_LEN];
    for ( ;; ) {
        if (xQueueReceive(xUARTQueue, outputString, pdMS_TO_TICKS(100)) == pdPASS) {
            UARTprintf("%s", outputString);
        }
    }
}

uint32_t UARTTaskInit(void) {
    ConfigureUART();
    //
    // Create the task
    //
    if(xTaskCreate(vUARTGatekeeperTask, (const portCHAR *)"UARTGatekeep", UARTTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_UART_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
