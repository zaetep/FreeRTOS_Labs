// ENTIRE FILE COMPLETED BY STUDENT

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <inc/hw_sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>

void LED_Init(void) {
    // Enable the GPIO port that is used for the on-board LED.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); 
    // Check if the peripheral access is enabled.
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {} 
    //PF3-PF1: GBR
	// Enable the GPIO pins for the LED (PF1~PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
}

void LED_Blink(void) {
    // Toggle PF2
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 
                 ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) & GPIO_PIN_2);
    // Wait 500ms
    SysCtlDelay(SysCtlClockGet() / 6);  // 500ms delay (ClockGet/3 = 1 sec)
}
