//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
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
// This Keil project has been reformatted to standardize the startup structure, reducing clutter on your PC from multiple project formats.
// The project includes the FreeRTOS component, although it will not be utilized in the initial stages.
// After compiling the project and downloading it to the board, the system will automatically reset and begin execution.
// To disable this automatic reset and run behavior, navigate to: Project menu -> Options for Target 'freertos_demo' -> Debug -> Settings -> uncheck "Reset and Run."
// Standard C headers
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//TM4C123 related header, you can find all these files under 
//\ece485_tiva_823_freertos_demo_with_LCD\TivaWare
// The following header files include the register address and configuration bit-wise setting information.
#include "inc/hw_adc.h"
//#include "inc/hw_aes.h"
#include "inc/hw_can.h"
//#include "inc/hw_ccm.h"
#include "inc/hw_comp.h"
//#include "inc/hw_des.h"
#include "inc/hw_eeprom.h"
//#include "inc/hw_emac.h"
//#include "inc/hw_epi.h"
//#include "inc/hw_fan.h"
//#include "inc/hw_flash.h"
#include "inc/hw_gpio.h"
#include "inc/hw_hibernate.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
//#include "inc/hw_lcd.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
//#include "inc/hw_onewire.h"
#include "inc/hw_pwm.h"
//#include "inc/hw_qei.h"
//#include "inc/hw_shamd5.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
//#include "inc/hw_sysexc.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
//#include "inc/hw_udma.h"
//#include "inc/hw_usb.h"
#include "inc/hw_watchdog.h"


//Driver library file 
#include "driverlib/adc.h"
//#include "driverlib/aes.h"
#include "driverlib/can.h"
#include "driverlib/comp.h"
//#include "driverlib/cpu.h"
//#include "driverlib/crc.h"
#include "driverlib/debug.h"
//#include "driverlib/des.h"
#include "driverlib/eeprom.h"
//#include "driverlib/emac.h"
//#include "driverlib/epi.h"
//#include "driverlib/flash.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
//#include "driverlib/hibernate.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
//#include "driverlib/lcd.h"
//#include "driverlib/mpu.h"
//#include "driverlib/onewire.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
//#include "driverlib/qei.h"
#include "driverlib/rom.h"
//#include "driverlib/rom_map.h"
//#include "driverlib/rtos_bindings.h"
//#include "driverlib/shamd5.h"
#include "driverlib/ssi.h"
#include "driverlib/sw_crc.h"
#include "driverlib/sysctl.h"
//#include "driverlib/sysexc.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
//#include "driverlib/udma.h"
//#include "driverlib/usb.h"
#include "driverlib/watchdog.h"

//LCD ST7735 related file 
#include "ST7735.h"

//UART related IO header file 
#include "utils/uartstdio.h"
//All Free RTOS related header
#include "FreeRTOS.h"
#include "event_groups.h"
#include "list.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
//Placeholder for your applicaton header file 

//Actual LED task header 
#include "led_task.h"
void DelayWait10ms(uint32_t n);


//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//!
//! This application demonstrates the use of FreeRTOS on Launchpad.
//!
//! The application blinks the user-selected LED at a user-selected frequency.
//! To select the LED press the left button and to select the frequency press
//! the right button.  The UART outputs the application status at 115,200 baud,
//! 8-n-1 mode.
//!
//! This application utilizes FreeRTOS to perform the tasks in a concurrent
//! fashion.  The following tasks are created:
//!
//! - An LED task, which blinks the user-selected on-board LED at a
//!   user-selected rate (changed via the buttons).
//!
//! - A Switch task, which monitors the buttons pressed and passes the
//!   information to LED task.
//!
//! In addition to the tasks, this application also uses the following FreeRTOS
//! resources:
//!
//! - A Queue to enable information transfer between tasks.
//!
//! - A Semaphore to guard the resource, UART, from access by multiple tasks at
//!   the same time.
//!
//! - A non-blocking FreeRTOS Delay to put the tasks in blocked state when they
//!   have nothing to do.
//!
//! For additional details on FreeRTOS, refer to the FreeRTOS web page at:
//! http://www.freertos.org/
//
//*****************************************************************************


//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
} 

#define ON true
#define OFF false

#define APP_SYSTICKS_PER_SEC    (1000/20) // 50 system ticks each second = 20 ms

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void) {
    /*// Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // Enable UART0
    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Use the internal 16MHz oscillator as the UART clock source.
    // UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Initialize the UART for console I/O.
    // UARTStdioConfig(0, 115200, 16000000);
	*/
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

// SysTick Handler 
void SysTickIntHandler(void) {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 
        (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) ^ GPIO_PIN_2));

}
// Timer Handler KD
void TimerIntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    uint8_t val = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, (val ^ GPIO_PIN_2));
}

//Case 1 SysTick way 
void Case1_SysTick(void) {
    //The following prompts include all three cases for this lab:
            
    // Write a program to blink the blue LED every 500ms using the SysTick timer.  
    // Be mindful of the maximum count range of the 24-bit SysTick timer and the clock frequency driving the SysTick counter.
    // void SysTickEnable(void);
    // void SysTickIntRegister(void (*pfnHandler)(void));
    // void SysTickIntEnable(void);
    // void SysTickPeriodSet(uint32_t ui32Period);
    // Set the System tick interrupt handler.
    SysTickIntRegister(SysTickIntHandler);
    // Configure the SysTick timer period
    SysTickPeriodSet(SysCtlClockGet());
    //Enable the Systick.
    SysTickEnable();
    //Enable the Systick interrupt 
    SysTickIntEnable();
    //Enable all interrupts
    IntMasterEnable();
}

//Case 2 General Purpose Timer way 
void Case2_Timer(void) {
    //Instead of using the system timer, you will use a general-purpose timer to blink the Blue LED. Configure Timer0 to operate in periodic interrupt mode with a 500ms period. When Timer0 reaches the predefined count value, the corresponding GPIO pin should be toggled to change the LED state.
    // void IntRegister(uint32_t ui32Interrupt, void (*pfnHandler)(void));
    // void IntEnable(uint32_t ui32Interrupt);
    // bool IntMasterEnable(void);
    // void SysCtlPeripheralEnable (uint32_t ui32Peripheral);
    // void TimerConfigure (uint32_t ui32Base, uint32_t ui32Config);
    // void TimerLoadSet (uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Value);
    // void TimerIntEnable (uint32_t ui32Base, uint32_t ui32IntFlags);
    // void TimerIntClear (uint32_t ui32Base, uint32_t ui32IntFlags);
            
    //Register Timer0's interrupt  
    //Find the macro defined in the inc/hw_ints.h header file.
    IntRegister(INT_TIMER0A_TM4C123, TimerIntHandler);
    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}
    // Configure the two 32-bit periodic timers.
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Configure the Timer value for a interrupt
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 2) - 1);
    // Setup the interrupts for the timer timeouts.
    // NVIC enable TIMER0A 
    IntEnable(INT_TIMER0A_TM4C123);
    // TIMER0 A enable interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Enable the timers.
    TimerEnable(TIMER0_BASE, TIMER_A);
    //Enable all interrupts
    IntMasterEnable();
}

    
//Case 3 PWM module way
void Case3_PWM(int32_t g_ui32SysClock) {
    //In this task, you will use the PWM module to blink the Blue LED.   Configure the PWM to operate with a 500ms period. Once the PWM counter reaches the predefined value, it should toggle the corresponding GPIO pin to change the LED state.
    // void PWMGenConfigure (uint32_t ui32Base, uint32_t ui32Gen, uint32_t ui32Config);
    // void PWMGenPeriodSet (uint32_t ui32Base, uint32_t ui32Gen, uint32_t ui32Period)
    // void PWMPulseWidthSet (uint32_t ui32Base, uint32_t ui32PWMOut, uint32_t ui32Width)
    // void PWMOutputState (uint32_t ui32Base, uint32_t ui32PWMOutBits, bool bEnable);
    // void PWMGenEnable (uint32_t ui32Base, uint32_t ui32Gen);
    // Set the PWM clock to the system clock.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    uint32_t pwmPeriod = g_ui32SysClock / 64;
    uint32_t pwmPulse = pwmPeriod / 2;
    // The PWM peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM1)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    // Configure the GPIO pin muxing to select PWM16 functions for these pins.
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    // Configure the PWM function for this pin.
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    // For this example PWM1 is used with PortF Pin2. 
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the PWM period.
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, pwmPeriod);
    //Set it to the duty cycle you want, let's say 50%
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, pwmPulse); 
    // Enable the PWM output signal (PF2).
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
    // Enables the PWM generator block.
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);
}

#define INT_SysTick 15 
uint32_t g_ui32SysClock; 
//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{


    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
	g_ui32SysClock = SysCtlClockGet();

    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();
	UARTprintf("\n\nCurrent Clock Frequency %d\n",g_ui32SysClock);
	UARTprintf("\n\nWelcome to the EK-TM4C123GXL FreeRTOS Demo!\n");
    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //
	// GPIO configuration 
	// Below is ROM-based setting, which is OK to use, but I do not prefer 
	// ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// ROM_SysCtlDelay(1);
	// ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	// ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
	// ROM_SysCtlDelay(1000000); // Delay for a while.
	// ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	
	//Below is more API-style method, and I prefer 
	// Enable the GPIO port that is used for the on-board LED.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// Check if the peripheral access is enabled.
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
	//PF3-PF1: GBR
	// Enable the GPIO pins for the LED (PF1~PF3).  Set the direction as output, and
	// enable the GPIO pin for digital function.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
	// You may also bother yourself in doing the above step in the following way:
	// HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	// HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x0E; 			 //PF1, PF2, PF3 
	// HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	
	
	ST7735_InitR(INITR_REDTAB);
	DelayWait10ms(100);

	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetTextColor(ST7735_YELLOW);

	ST7735_Message(1, 1, "5line  1", 8);
	ST7735_Message(1, 2, "6line  2", 8);
	ST7735_Message(1, 3, "7line  3", 8);
	ST7735_Message(1, 4, "8line  4", 8);
	ST7735_Message(1, 5, "line  5", 8);
	
    // This RTOS task creation does it all for us! KD
    // Comment out the task initalization and scheduler initialization to test other functionality.

	// Below are the FreeRTOS part partial start up process 	
	
    
    // Create the LED task.
    //
    if(LEDTaskInit() != 0)
    {
        while(1)
        {
        }
    }
    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();
    
	
    // All of this (below) is incredibly pointless given the LED task.
    // We don't need to touch SysTick, Timers, or PWM to blink the LED.
    // Also, we already did this in ECE362. Why do it again?
    // We should be learning about how freeRTOS handles the task for us.
    // Or, even, better, how to code the task!
    // Instead, we're just redoing the bare metal programming.
    // Why run an OS if we're going to do this?
    // /rant - Kaden Downes 9/24/2025

    // logic to choose and run a case KD
    char ch = ' ';
    UARTprintf("\n\nCase 1: SysTick\nCase 2: General Purpose Timer\nCase 3: PWM\n");
    while (1) {
        while(UARTCharsAvail(UART0_BASE)) UARTCharGet(UART0_BASE); // Clear input buffer
        while(!UARTCharsAvail(UART0_BASE)); // Wait for input
        ch = UARTCharGet(UART0_BASE);
        if (ch == '1' || ch == '2' || ch == '3') {
            break;
        }
        UARTprintf("Invalid input. Please enter a valid case number.\n");
    }

    switch (ch) {
        case ('1'):
            Case1_SysTick();
            break;
        case ('2'):
            Case2_Timer();
            break;
        case ('3'):
            Case3_PWM(g_ui32SysClock);
            break;
        default:
            UARTprintf("An error occurred processing the case number.");
    }
    while (1) { 
    }
}

// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n) {
  uint32_t volatile time;
  while (n) {
    time = 727240 * 2 / 91; // 10msec
    while (time) {
      time--;
    }
    n--;
  }
}
