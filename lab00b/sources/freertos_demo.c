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
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
//Placeholder for your applicaton header file 

//Actual LED task header 
#include "led_task.h"

#define TRUE 1
#define FALSE 0

#define CMDLINE_MAX_ARGS		5 // changed to 5 as per the requirements of the lab
#define arg3_default "300"
#define arg4_default "400"

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

// CommandLineProcess TODO
// return = TRUE if command line parsed successfully,
//          FALSE if command line is empty or exceeds maximum number of arguments
// @brief:  This function tokenizes an input command line string into separate arguments.
//          Leading spaces are skipped, consecutive spaces between arguments are reduced,
//          and each argument pointer is stored in g_ppcArgv. The number of extracted 
//          arguments is stored in pui8Argc. The function enforces a maximum number of 
//          arguments (CMDLINE_MAX_ARGS).
// @param [in]  *pcCmdLine: pointer to the input command line string
// @param [out] *pui8Argc: pointer to a variable where the number of parsed arguments is stored
// @param [out] *g_ppcArgv[]: array of pointers, each pointing to an extracted argument string
uint8_t CommandLineProcess(uint8_t * pcCmdLine, uint8_t * pui8Argc, uint8_t * g_ppcArgv[]) {
    uint8_t ProcessedFinished = FALSE;
    * pui8Argc = 0; // initialize input argument 0
    // skip multiple spaces in the front of pcCmdline
    while ( * pcCmdLine == ' ') {
        pcCmdLine++;
    }

// --- SECTION COMPLETED BY STUDENT --- //

    while (*pcCmdLine && *pui8Argc < CMDLINE_MAX_ARGS) {
        if (*pcCmdLine == '\0') {
            break;
        }
        // Store argument pointer
        g_ppcArgv[*pui8Argc] = pcCmdLine;
        (*pui8Argc)++;
        // Move to end of argument
        while (*pcCmdLine && *pcCmdLine != ' ') {
            pcCmdLine++;
        }
        // Null-terminate argument if not end of string
        if (*pcCmdLine) {
            *pcCmdLine = '\0';
            pcCmdLine++;
        }
    }
    // If no arguments found, return FALSE
    if (*pui8Argc == 0) {
        return FALSE;
    }
    // If too many arguments, return FALSE
    if (*pui8Argc > CMDLINE_MAX_ARGS) {
        return FALSE;
    }
    return TRUE;
}

// RemoveExtraSpace TODO
// return = none
// @brief:  This function processes the input command line string in-place and removes 
//          redundant spaces, carriage returns, and newline characters. It ensures that 
//          only a single space is kept between arguments and trims any trailing space 
//          at the end of the string.
// @param [in/out] *string: pointer to the null-terminated command line string to be modified
void RemoveExtraSpace(uint8_t * string) {
    uint8_t *src = string;
    uint8_t *dst = string;
    uint8_t prev_space = 0;

    // Remove leading spaces
    while (*src == ' ') src++;

    // Remove spaces in between
    while (*src) {
        if (*src == ' ' || *src == '\r' || *src == '\n') {
            if (!prev_space) {
                *dst++ = ' ';
                prev_space = 1;
            }
        } else {
            *dst++ = *src;
            prev_space = 0;
        }
        src++;
    }

    // Remove trailing space
    if (dst > string && *(dst - 1) == ' ') {
        dst--;
    }
    *dst = '\0';
}
 
// string_get TODO
// return = none
// @param [out] *string: pointer to the buffer where the received UART string will be stored
void string_get(char * string) {
    UARTgets(string, 51); // Reads up to 50 characters + null terminator from UART
}

// --- END SECTION COMPLETED BY STUDENT --- //

// Test case results for this lab.
struct TestCaseType {
    bool valid;
    uint8_t NumOfArg;
    char * Arg[CMDLINE_MAX_ARGS];
};
struct TestCaseType TestCase[7]={
	{TRUE,3,{"adc","0","20","",""}},
	{FALSE,0,{"","","","",""}},
	{FALSE,6,{"adc","11","222","333","444"}},
	{TRUE,5,{"adc","11","222","333","444"}},
	{TRUE,5,{"adc","11","222","333","444"}},
	{TRUE,5,{"adc","11","222","333","444"}},
	{FALSE,6,{"001","002","003","004","005"}},
};
 
 
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
uint32_t g_ui32SysClock; 
//*****************************************************************************
 
//*****************************************************************************
//
// The main application, a mostly empty c code that will be used later. 
//*****************************************************************************
int
main(void) {
    uint8_t * g_ppcArgv[CMDLINE_MAX_ARGS]; // save the pointers to input arguments
    static uint8_t * pcCmdLineArray[7] = {
        "adc 0 20",
        "",
        "adc 11 222   333 444 555",
        "adc    11    222    333 444   ",
        "      adc    11    222    333 444",
        "      adc    11    222    333 444   ",
        " 001  002   003  004  005  006"
    };
    // the above definition saves all string constant in Flash RAM
    uint8_t pcCmdLineTemp[40]; // string to save each command line in data RAM
    uint8_t ui8Argc; // number of actual argument, unsigned 8-bit 
    uint8_t idx;
    uint8_t i;
    uint8_t valid = 1;
    uint8_t error_argument;
    bool default_0_custom_1 = false;
    char custom_command[50];
    
    // Set the system clock to run at 50MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Set the direction of PF1~PF3
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x0E; //PF1, PF2, PF3 
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    // UART initialization
    ConfigureUART();
    UARTprintf("The code is running\n");

    UARTprintf("0 for default numbers, 1 for custom arguments\n");
    //The following while loop allow the user to parse the pre-defined commands or the command the user fed in. 

// --- SECTION COMPLETED BY STUDENT --- //

    if (default_0_custom_1 == 0) {
        for (i = 0; i < sizeof(pcCmdLineArray) / sizeof(pcCmdLineArray[0]); i++) {
            error_argument = 0;
            // remove extra spaces of Command Line between each argument, only keep one space
            strcpy((char * ) pcCmdLineTemp, (const char * ) pcCmdLineArray[i]);
            RemoveExtraSpace(pcCmdLineTemp);
            if (CommandLineProcess(pcCmdLineTemp, & ui8Argc, g_ppcArgv)) {

            } else { // incorrect Argc input			 
                if (TestCase[i].valid) {
                    UARTprintf("Testcase Incorrect! Invalid input %d \n Test Case %s", i, pcCmdLineArray[i]);
                    error_argument = 1;
                }
            }
            if (error_argument == 0) {
                UARTprintf("Testcase Correct! %d \n Test Case %s\n", i, pcCmdLineArray[i]);
            }
        }
    } else {
        UARTprintf("Feed in custom arguments, end with ;\n");
        UARTprintf("1st~3rd argument HAVE TO BE character, 4th~5th argument HAVE TO BE numbers\n");
        //Sample valid argument  ab d f 1 2    1421 ;
        //Sample invalid argument  1 2 1421 ab d ;
        //Sample valid argument  a b 1 0x30 0x20;
        //Sample valid argument a b red;
        UARTgets(custom_command, sizeof(custom_command));
        strcpy((char * ) pcCmdLineTemp, (const char * ) custom_command);
        RemoveExtraSpace(pcCmdLineTemp);
    }
    UARTprintf("End of the program\n");
    while (1) {}
}

// --- END SECTION COMPLETED BY STUDENT --- //