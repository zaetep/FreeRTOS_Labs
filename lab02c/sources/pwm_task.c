#include <stdint.h>
#include <stdbool.h>

#include <inc/hw_memmap.h>
#include <inc/hw_pwm.h>
#include <inc/hw_gpio.h>
#include <inc/hw_sysctl.h>

#include <driverlib/pin_map.h>
#include <driverlib/pwm.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>

#include <FreeRTOS.h>
#include <event_groups.h>

#include <priorities.h>
#include <globalvars.h>

#define PWMTASKSTACKSIZE 128

uint32_t period;

static void PWMInit(void) {
    // enable PWM peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));
    // enable GPIO peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
    // configure pins
    GPIOPinConfigure(GPIO_PD0_M0PWM6);
    GPIOPinConfigure(GPIO_PD1_M0PWM7);
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // configure PWM module
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64); // divide sys clock by 64
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3,
                PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    period = (SysCtlClockGet() / SysCtlPWMClockGet()) / (uint32_t)1000;
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, period);
    // initial duty cycle of 50%
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, period / 2);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, period / 2);
    // enable PWM
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_3);
}

static void vPWMTask(void *pvParameters) {
    EventBits_t uxBits;
    uint32_t dutyCyclePD0 = 50, dutyCyclePD1 = 50;

    for ( ;; ) {
        uxBits = xEventGroupWaitBits(
            xPWMEvent,
            ADC_GT_4090 | ADC_LT_10,
            pdTRUE,      // Clear bits on exit
            pdFALSE,     // Wait for any bit
            portMAX_DELAY
        );

        if (uxBits & (ADC_GT_4090 | ADC_LT_10)) { // PD0 increases, PD1 decreases
            if (dutyCyclePD0 < 99) { // increase by 1% if less than 99%
                dutyCyclePD0++;
            }
            if (dutyCyclePD1 > 1) {  // decrease by 1% if greater than 1%
                dutyCyclePD1--;
            }
            // set new duty cycles
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6,
                             (dutyCyclePD0 * period) / 100);

            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,
                             (dutyCyclePD1 * period) / 100);
        }

    }
}

uint32_t
PWMTaskInit(void)
{
	PWMInit();
    //
    // Create the LED task.
    //
    if(xTaskCreate(vPWMTask, (const portCHAR *)"PWM", PWMTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_PWM_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}
