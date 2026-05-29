#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_adc.h>
#include <inc/hw_gpio.h>
#include <inc/hw_nvic.h>
#include <inc/hw_ints.h>
#include <inc/hw_sysctl.h>
#include <inc/hw_memmap.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <globalvars.h>
#include <priorities.h>
#include <string.h>
#include <stdio.h>
#include <event_groups.h>

#define ADCTASKSTACKSIZE 128

static void ADC0Seq3_Handler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t ui32ADCValue = 0;
    // Clear the ADC interrupt
    ADCIntClear(ADC0_BASE, 3);
    // Wait for conversion to complete
//  while(!ADCIntStatus(ADC0_BASE, 3, false));
    // Read the result (sequence 3 returns a single sample)
    ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADCValue);
    
    g_ui32ADCSum += ui32ADCValue;
    g_ui32ADCSampleNum++;

    if (ui32ADCValue > 4090) {
        xEventGroupSetBitsFromISR(xPWMEvent, ADC_GT_4090, &xHigherPriorityTaskWoken);
    }
    if (ui32ADCValue < 10)   {
        xEventGroupSetBitsFromISR(xPWMEvent, ADC_LT_10, &xHigherPriorityTaskWoken);
    }
    
    if (g_ui32ADCSampleNum == 100) {
        xSemaphoreGiveFromISR(xADCAvgReady, &xHigherPriorityTaskWoken);
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void ADC0Init(void) {
    // Enable ADC0 Peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    // Enable corresponding GPIO module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    // Configure Pins for ADC
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
    // Configure ADC
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); // Sequence 3 since only 1 channel
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);
    ADCIntRegister(ADC0_BASE, 3, ADC0Seq3_Handler);
    IntRegister(INT_ADC0SS3_TM4C123, ADC0Seq3_Handler);
    IntPrioritySet(INT_ADC0SS3_TM4C123, (5 << 5));  // Must be >= configMAX_SYSCALL_INTERRUPT_PRIORITY
    ADCIntEnable(ADC0_BASE, 3);
    IntEnable(INT_ADC0SS3_TM4C123);
    // Clear interrupt
    ADCIntClear(ADC0_BASE, 3);
    // Enable ADC
    ADCSequenceEnable(ADC0_BASE, 3);   
}

static void vADCTask(void *pvParameters) {
    char formattedString[MAX_STRING_LEN];
    float ADCAverage;
    uint32_t ledCmd;
    for ( ;; ) {
        if (xSemaphoreTake(xADCAvgReady, pdMS_TO_TICKS(5000)) == pdTRUE) {
            ledCmd = 2;
            xQueueSend(xLEDQueue, &ledCmd, portMAX_DELAY); // turn on blue LED
            ADCAverage = (float)g_ui32ADCSum / 100;
            g_ui32ADCSum = 0;
            g_ui32ADCSampleNum = 0;
            snprintf(formattedString, sizeof(formattedString), "Avg ADC Value: %6.2f\n", ADCAverage);
            xQueueSend(xUARTQueue, formattedString, portMAX_DELAY);       
            vTaskDelay(pdMS_TO_TICKS(500));
            ledCmd = 2;
            xQueueSend(xLEDQueue, &ledCmd, portMAX_DELAY); // turn off blue LED
        }
    }
}

uint32_t
ADCTaskInit(void)
{
    ADC0Init();
    if(xTaskCreate(vADCTask, (const portCHAR *)"ADC", ADCTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_ADC_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
