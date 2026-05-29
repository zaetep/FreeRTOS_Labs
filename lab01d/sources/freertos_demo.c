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
#include "queue.h"
#include "semphr.h"
//Placeholder for your applicaton header file 

//Actual LED task header 
#include "led_task.h"

//
// USER DEFINED INCLUDES KD
//
#include <lcd_tasks.h>
#include <uart_tasks.h>
#include <timer_tasks.h>
#include <adc_tasks.h>
#include <gpio_tasks.h>

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
static bool newCMD = false; 
uint32_t g_ui32SysClock; 




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





int main(void) {
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
	UARTprintf("\n\nWelcome to the EK-TM4C123GXL FreeRTOS Demo!\n");
	
	//Expected project structure 
	//main.c file: freeRTOS_demo.c

// --- SECTION COMPLETED BY STUDENT --- //
	
	// Initialize the LCD first (needs to be ready before updates)
    LCD_Init();
    // Initalize the LEDs
    LED_Init();
	//Initialize your ADC
    ADC0_Init();
	//Initialize your timer for ADC conversion measurement
    Timer3A_Init();
	//Initialize your timer for periodic ADC sampling with Timer interrupt 
    Timer0A_Init();

	UARTprintf("\n");
	UARTprintf("End of the program\n");
    
    // Initial LCD display update
    DelayWait10ms(10); // Wait 100ms for ADC buffer to start filling
    LCD_UpdateADCReadings();
    
    uint32_t lcdUpdateCounter = 0;
    while (1) {
		if(UART_CommandAvailable()) {	
            char cmd[64];
            UART_ReadCommand(cmd);
		    // The actual function that runs the input command, prints out the latest ADC values, or responds to the text print request.
		    CMD_Run(cmd);
		    newCMD = false;			 
		}
        LED_Blink();
        
        // Update LCD display periodically (every ~1000000 iterations for ~1 sec updates)
        lcdUpdateCounter++;
        if (lcdUpdateCounter >= 1000000) {
            LCD_UpdateADCReadings();
            lcdUpdateCounter = 0;
        }
    }  
}

// --- END SECTION COMPLETED BY STUDENT --- //

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