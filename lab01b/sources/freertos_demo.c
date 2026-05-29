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
#include "buttons.h"
//LCD ST7735 related file 
#include "ST7735.h"

//UART related IO header file 
#include "utils/uartstdio.h"
//All Free RTOS related header
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//Placeholder for your applicaton header file 

//Actual LED task header 
#include "led_task.h"

// TO DELETE 
// define necessary global variables here
#define PER_ROW 3

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
// Global variables
//*****************************************************************************
const uint32_t LEDFLASHFREQ[] = {
    12,   // 240 ms
    25,   // 500 ms
    50,   // 1 s
    0     // always on
};
volatile uint32_t g_ui32TimerIntCount = 0;
volatile uint32_t g_ui32SysClock; 
//uint32_t g_ui32BlueFreq = LEDFLASHFREQ[0];
//uint32_t g_ui32GreenFreq = LEDFLASHFREQ[0];

//*****************************************************************************
// Function declaration
//*****************************************************************************
/*
void string_get(char * string);
void printmenu(void);
void countempty(void);
void listempty(void);
void showalphabetical(void);
void assigncustomer(void);
void deleteassignment(void);
*/
 
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

// CUSTOM FUNCTIONS FOR THIS LAB

void UART0IntHandler(void){
    char ch = '\0';
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    while(!UARTCharsAvail(UART0_BASE));
    ch = UARTCharGetNonBlocking(UART0_BASE);
    switch (ch) {
        case ('R'):
        case ('r'): // toggle red led
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 
                (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) ^ GPIO_PIN_1));
            break;
        case ('G'):
        case ('g'): // toggle green led
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 
                (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) ^ GPIO_PIN_3));
            break;
        case ('B'):
        case ('b'): // toggle blue led
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 
                (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) ^ GPIO_PIN_2));
            break;
//        default:
//            UARTprintf("Invalid input.");
    }
}

void Timer0AIntHandler(void){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if (g_ui32TimerIntCount >= 50) {
        g_ui32TimerIntCount = 1;
    } else {
        g_ui32TimerIntCount++;
    }

}

// function to convert floating point to char array KD
void float2bcd(float floating_num, char *bcd, uint32_t decimaldigits){
    // handle negative numbers
    if (floating_num < 0) {
        floating_num = -floating_num; // Make it positive for processing
    }

    // multiply and convert to integer to truncate unused digits
    floating_num *= 10 ^ decimaldigits;
    uint32_t int_num = (uint32_t)floating_num;

    // separate integer part and decimal part
    uint32_t intpart = int_num / (10 ^ decimaldigits);
    uint32_t decpart = int_num % (10 ^ decimaldigits);

    // turn into string
    sprintf(bcd, "%lu.%0*lu", intpart, decimaldigits, decpart);
}

// function to initialize timer 3a KD
void Timer3A_Init(void) {
    // enable timer 3 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3)) {}

    // configure timer behavior and load
    TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER3_BASE, TIMER_A, 0xFFFFFFFF);

    // enable timer
    TimerEnable(TIMER3_BASE, TIMER_A);
}

// function to initialize timer 0a KD
void Timer0A_Init(void) {
    // enable timer0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}

    // configure timer and load value
    uint32_t loadVal = (g_ui32SysClock / 50) - 1;   // since 20 ms = 1/50 s
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, loadVal);

    // configure interrupt
    IntRegister(INT_TIMER0A_TM4C123, Timer0AIntHandler);
    IntEnable(INT_TIMER0A_TM4C123);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    // enable timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}


void Case1_Time_Measurement(void) {
    //Case 1 Computation time measurement 
    // Use timer3 as either a periodic [preferred] or one-shot down-counting timer with no interrupt enabled to measure the execution time of the program. You know how to configure a general-purpose timer in lab01a; here, we explore how to read the value of the general-purpose timer
    // The TivaWare function \verb|TimerGetValue()| can be called to read the current value of the timer, and Timer 3 can be used to measure the time elapsed between certain events or execution time
    // After reading the value, you can return the difference of the timer value by UART0, so you know the execution time for a certain program segment. The time should be calculated in the unit of $\mu s$. 
    
    // You may try different operations and loops to evaluate TM4C123's computation performance. For submission purposes, you only need to measure the time elapsed for an empty loop. 
    
    while (1) {
        // enable FPU before floating point calculations occur KD
        FPUEnable();
        FPULazyStackingEnable();
        // we don't really care how fp gets rounded, leave default KD
        //Initialize your Timer
        Timer3A_Init();
        //Get the current time by clock cycle
        uint32_t start = TimerValueGet(TIMER3_BASE, TIMER_A);
        //some calculation, here an empty loop serves as a toy model. 
        for (uint32_t ui32Loop = 0; ui32Loop < 5000000; ui32Loop++) {}
        //Get the current time by clock cycle
        uint32_t end = TimerValueGet(TIMER3_BASE, TIMER_A);
        //Compute the time difference and possibly store it 
        uint32_t difference = start - end;
        //Convert the time to floating point representation 
        float elapsed_time_us = (float)difference / (SysCtlClockGet() / 1000000.0f);
        //Print out the value 
        UARTprintf("Time elapsed: ");
        char time[32];
        float2bcd(elapsed_time_us, time, 3);
        UARTprintf(time);
        UARTprintf(" us\n");
    }
    
    // All prior interactions with TM4C123 do not involve floating-point operations, though we discussed the floating-point operation and its instructions available to TM4C123
    // You can configure the FPU of TM4C123 with the following API function
    // void FPUDisable (void)
    // void FPUEnable (void)
    // void FPULazyStackingEnable(void)
    // void FPURoundingModeSet (uint32_t ui32Mode)
}

void Case2_ButtonWithLED(void) {
    // Case 2 Change blinking frequency by button 
    // User left and right buttons to change the LED changing pattern.

    // Write a program that can detect when the left and right buttons are pressed and released once. 
    // When the left button is pressed, the blue LED changes its flash frequency from always on to flashing every 240 ms, 500 ms, 1 second, always ON, ….. 
    // When the right button is pressed, the green LED changes its flash frequency in reverse order, from 1 second, to 500 ms, to 240 ms, always ON, flashes 1 second, …. 

    // You need UART interface to indicate which button is pressed, and what is the current frequency.  

    // Although you can achieve this by modifying the period through updating the loaded timer value within an appropriate interrupt service routine, you may also consider configuring the timer with a smaller base period. In each ISR execution, you first determine which button was pressed, then update the current period if necessary. After that, you compare the current count of timer interrupts to decide whether to toggle the GPIO, which can be done using a simple modulo operation.
            
    //Enable the timer to produce some short periodic interrupt, let's say 10ms
    // Initialize the buttons 
    ButtonsInit();
    // Array to store LED blink frequencies, you need to declare it as a global variable, copied below just in case you miss it. 
    // moved to global variables section KD
            
    // To achieve LED blinking intervals such as 1s, 2s, or 4s, configure the timer 
    // to interrupt every 1s. Then, based on the selected blinking frequency, 
    // count the number of interrupts before toggling the LED. 
    // For example, for a 4s interval, toggle the LED only after 4 timer interrupts.
    uint8_t debounced; // current debounced state returned by ButtonsPoll (1=pressed)
    uint8_t raw;       // raw (non-debounced) inverted read
    uint8_t delta;     // changed bits since last poll (written by ButtonsPoll)
    uint8_t left_index = 0;
    uint8_t right_index = 0;

    // configure timer to interrupt every 20 ms, as this is the lowest common factor of 240, 500, and 1000 ms. KD
    Timer0A_Init();

    {
        uint32_t lastTick = g_ui32TimerIntCount;
        uint32_t left_count = 0;   // blue LED counter (PF2)
        uint32_t right_count = 0;  // green LED counter (PF3)

        // initialize LEDs off
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0);

        while (1) {
            // Poll the buttons: ButtonsPoll(&delta, &raw) returns debounced state
            debounced = ButtonsPoll(&delta, &raw);

            // Edge detection: handle left button press (transition to pressed)
            if (BUTTON_PRESSED(LEFT_BUTTON, debounced, delta)) {
                left_index = (left_index + 1) % (sizeof(LEDFLASHFREQ) / sizeof(LEDFLASHFREQ[0]));
                if (LEDFLASHFREQ[left_index] == 0) {
                    UARTprintf("Left button pressed. Blue LED: always ON\n");
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); // force ON
                } else {
                    UARTprintf("Left button pressed. Blue LED period: %d ms\n", LEDFLASHFREQ[left_index] * 20);
                    left_count = 0; // restart counting so change takes effect
                }
            }

            // Edge detection: handle right button press (transition to pressed)
            if (BUTTON_PRESSED(RIGHT_BUTTON, debounced, delta)) {
                right_index = (right_index == 0) ? (sizeof(LEDFLASHFREQ) / sizeof(LEDFLASHFREQ[0]) - 1) : (right_index - 1);
                if (LEDFLASHFREQ[right_index] == 0) {
                    UARTprintf("Right button pressed. Green LED: always ON\n");
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); // force ON
                } else {
                    UARTprintf("Right button pressed. Green LED period: %d ms\n", LEDFLASHFREQ[right_index] * 20);
                    right_count = 0; // restart counting
                }
            }

            // Only update on timer tick; use g_ui32TimerIntCount updated by Timer0AIntHandler
            if (g_ui32TimerIntCount != lastTick) {
                lastTick = g_ui32TimerIntCount;

                // Blue LED (PF2) handling
                if (LEDFLASHFREQ[left_index] == 0) {
                    // always on - already forced above, nothing to do
                } else {
                    left_count++;
                    if (left_count >= LEDFLASHFREQ[left_index]) {
                        // toggle blue LED
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,
                            GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) ^ GPIO_PIN_2);
                        left_count = 0;
                    }
                }

                // Green LED (PF3) handling 
                if (LEDFLASHFREQ[right_index] == 0) {
                    // always on 
                } else {
                    right_count++;
                    if (right_count >= LEDFLASHFREQ[right_index]) {
                        // toggle green LED
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,
                            GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) ^ GPIO_PIN_3);
                        right_count = 0;
                    }
                }
            }
        }
    }
}

void Case3_UART0Int(void) {
    //Case 3 UART0 interrupt 
    // In the discussion of ECE 36200, as well as in earlier discussions, you may already know that UART communication can be implemented using interrupts rather than polling, although we have not practiced this method here. In this part, you are required to configure the UART to use an interrupt-based approach. At a minimum, you must enable UART receive interrupts so that the processor can continue performing other tasks until it receives data from the computer. While you may also experiment with transmit interrupts, this is not required and may lead to excessive interrupt requests.

    // In this task, you are expected to implement an interrupt service routine (ISR) for UART0 that responds to the reception of the characters "R", "G", or "B" (case-insensitive). When the processor receives one of these characters, it should toggle the on-board LED once. 
    // void UARTIntRegister (uint32_t ui32Base, void (*pfnHandler)(void))r
    // void UARTIntEnable (uint32_t ui32Base, uint32_t ui32IntFlags)
    // void UARTIntClear (uint32_t ui32Base, uint32_t ui32IntFlags)
    // void UARTIntDisable (uint32_t ui32Base, uint32_t ui32IntFlags)
    // bool UARTCharsAvail (uint32_t ui32Base)
    // int32_t UARTCharGetNonBlocking (uint32_t ui32Base)
    // bool UARTCharPutNonBlocking (uint32_t ui32Base, unsigned char ucData)
            
            
    //Register UART0's interrupt  
    UARTIntRegister(UART0_BASE, UART0IntHandler);
    //Enable the UART interrupt (only receiver and receiver timeout)
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // The UART interrupt handler.
    // void UART0IntHandler(void){
        //Clear the interrupt 
        //Disable the transmission if needed.
        //Receive the characters one by one 
    // }
    while (1); // infinite loop to keep program running. KD
}

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

int main(void)
{
    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);
	g_ui32SysClock = SysCtlClockGet();
	//Below is more API-style method, and I prefer 
	// Enable the GPIO port that is used for the on-board LED.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); 
	// Check if the peripheral access is enabled.
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {} 
	//PF3-PF1: GBR
	// Enable the GPIO pins for the LED (PF1~PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);

    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();
	UARTprintf("\n\nWelcome to the EK-TM4C123GXL FreeRTOS Demo!\n");
	
    // case selection KD
    char ch = '\0';
    UARTprintf("Case 1: Computation Time Measurement\n");
    UARTprintf("Case 2: Change blinking frequency by button\n");
    UARTprintf("Case 3: UART0 Interrupt\n");
    while (1) {
        while(UARTCharsAvail(UART0_BASE)) UARTCharGet(UART0_BASE); // Clear input buffer
        while(!UARTCharsAvail(UART0_BASE)); // Wait for input
        ch = UARTCharGet(UART0_BASE);
        if (ch == '1' || ch == '2' || ch == '3') {
            break;
        }
        UARTprintf("Invalid input. Please enter a valid case number.\n");
    }
    
    // execute related function
    switch (ch) {
        case ('1'):
            Case1_Time_Measurement();
            break;
        case ('2'):
            Case2_ButtonWithLED();
            break;
        case ('3'):
            Case3_UART0Int();
            break;
        default:
            UARTprintf("There was an error processing the case nunber.\n");
            break;
    }
		
	
	UARTprintf("\n");
	UARTprintf("End of the program\n");
/*     while (1) {
      //XOR masking operation make the LED blinks 
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1) ^ GPIO_PIN_1);
      DelayWait10ms(100);
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2) ^ GPIO_PIN_2);
      DelayWait10ms(100);
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3) ^ GPIO_PIN_3);
      DelayWait10ms(100);
    } */
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