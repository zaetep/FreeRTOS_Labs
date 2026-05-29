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

// define necessary global variables here TODO
// For example, aircraft capacity, maximum name length, number of flight, etc. 
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
// *****************************************************************************


// *****************************************************************************

// *****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
// *****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

// *****************************************************************************
// Global variables
// *****************************************************************************
// define seat structure here TODO
struct Seat {
    bool assigned;
    int row;
    char col;
    char firstname[10];
    char lastname[10];
};

// *****************************************************************************
// Function declaration
// *****************************************************************************
void string_get(char * string);
void print_menu(void);
void count_empty(void);
void list_empty(void);
void show_alphabetical(void);
void assign_customer(void);
void delete_assignment(void);
void change_assignment(void);

// Declare seats as global so all functions can access
struct Seat seats[4][3];
 
 
// *****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
// *****************************************************************************
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

//void UART0IntHandler(void){
//}

//void Timer0AIntHandler(void){
//}



// *****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
// *****************************************************************************
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
int main(void){
    // Set the clocking to run at 50 MHz from the PLL.
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
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    ConfigureUART();
	UARTprintf("\n\nWelcome to the EK-TM4C123GXL FreeRTOS Demo!\n");
	
	// Initialization the airline seat set structure TODO
    struct Seat seats[4][3];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            seats[i][j].assigned = false;
            seats[i][j].row = i + 1;
            switch (j) {
                case (0):
                    seats[i][j].col = 'A';
                    break;
                case (1):
                    seats[i][j].col = 'B';
                    break;
                case (2):
                    seats[i][j].col = 'C';
                    break;
            }
        }
    }
    // Seats that are occupied on initalization
    seats[0][1].assigned = true;
    strcpy(seats[0][1].firstname, "Ada");
    strcpy(seats[0][1].lastname, "Lovelace");
    seats[1][2].assigned = true;
    strcpy(seats[1][2].firstname, "Alan");
    strcpy(seats[1][2].lastname, "Turing");
    seats[2][0].assigned = true;
    strcpy(seats[2][0].firstname, "Grace");
    strcpy(seats[2][0].lastname, "Hopper");
    seats[3][2].assigned = true;
    strcpy(seats[3][2].firstname, "Katherine");
    strcpy(seats[3][2].lastname, "Johnson");

	uint8_t ch = 0;
	while (ch != 'q' && ch != 'Q') {
	    printmenu();
	    while (1) {
	        // Check if there exist any char in the UART0 communication channel.
	        if (UARTCharsAvail(UART0_BASE)) {
	            ch = UARTCharGet(UART0_BASE);
	            break;
	        }
	    }
		// Menu selection 
        switch (ch) {
        case ('a'):
        case ('A'):
            count_empty();
            break;
        case ('b'):
        case ('B'):
            list_empty();
            break;
        case ('c'):
        case ('C'):
            show_alphabetical();
            break;
        case ('d'):
        case ('D'):
            assign_customer();
            break;
        case ('e'):
        case ('E'):
            delete_assignment();
            break;
        case ('f'):
        case ('F'):
            change_assignment();
            break;
        case ('q'):
        case ('Q'):
            break;
	    }
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
 
// Get the string via UART TODO
void string_get(char * string) {
    int idx = 0;
    char ch;
    UARTprintf("Finish the entering by ;\n");
    while (1) {
        // Wait for a character from UART
        while (!UARTCharsAvail(UART0_BASE)) {}
        ch = UARTCharGet(UART0_BASE);
        if (ch == ';' || idx >= 19) { // limit to 19 chars to avoid overflow
            string[idx] = '\0';
            UARTprintf("\n");
            break;
        }
        string[idx++] = ch;
        UARTCharPut(UART0_BASE, ch); // echo back
    }
}
// Print out the menu 
void print_menu(void) {
	UARTprintf("==== Don Airlines Seat Manager ====\n");
    UARTprintf("To choose a function, enter its letter label:\n");
    UARTprintf("a) Show number of empty seats\n");
    UARTprintf("b) Show list of empty seats\n");
    UARTprintf("c) Show alphabetical list of seats\n");
    UARTprintf("d) Assign a customer to a seat assignment\n");
    UARTprintf("e) Delete a seat assignment\n");
	UARTprintf("f) Change seat assignment\n");
    UARTprintf("q) Quit\n");
	UARTprintf("Select: > ");
}
// Count the number of empty seats TODO 
void count_empty(void) {
    int emptycount = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            if (seats[i][j].assigned == false) {
                emptycount++;
            }
        }
    }
    UARTprintf("\nEmpty seats available: %d", emptycount);
	return;
}
// List the empty seats TODO 
void list_empty(void) {
    int emptycount = 0;
    UARTprintf("\n\nEmpty seats:");
    for (int i = 0; i < 4; i++) {
        UARTprintf("\nRow %d: ", i + 1);
        for (int j = 0; j < 3; j++) {
            if (seats[i][j].assigned == false) {
                emptycount++;
                UARTprintf("%d%c ", seats[i][j].row, seats[i][j].col);
            }
        }
    }
    UARTprintf("\n\nTotal empty: %d\n", emptycount);
	return;
}
// Show the passenger info in seat's alphabetical order. TODO 
void show_alphabetical(void) {
	UARTprintf("\nAlphabetical seat list (Seat -> Last, First):");
	int assignedcount = 0;
	for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
			if (seats[i][j].assigned == true) {
				assignedcount++;
				UARTprintf("\n%d%c -> %s,%s", seats[i][j].row, seats[i][j].col, seats[i][j].lastname, seats[i][j].firstname);
            }
		}
  }
	UARTprintf("Total assigned: %d\n", assignedcount);
	return;
}
// Give passenger a seat TODO
void assign_customer(void) {
    char fname[10];
    char lname[10];
    char input[8];
    bool loop = true;
    int col;
    UARTprintf("\n\nEnter passenger first name: > ");
    string_get(fname);
    if (fname[0] == '-') {
        UARTprintf("\nOperation aborted.\n\n");
        return;
    }
    UARTprintf("\nEnter passenger last name: > ");
    string_get(lname);
    if (lname[0] == '-') {
        UARTprintf("\nOperation aborted.\n\n");
        return;
    }
    while(loop) {
        UARTprintf("\nEnter desired seat ID (e.g., 2C): > ");
        string_get(input);
        if (strlen(input) != 2 ||
            input[0] < '1' || input[0] > '4' ||
            !(input[1] == 'A' || input[1] == 'B' || input[1] == 'C')) {
            UARTprintf("\nError: Seat ID '%c%c' is invalid. Valid seats are 1A..4C.\n", input[0], input[1]);
            UARTprintf("Try a different seat? (Y/N): > ");
            string_get(input);
            if (input[0] == 'Y' || input[0] == 'y'){
                continue;
            }else {
                return;
            }
        } else {
            int row = input[0] - '0';
            switch (input[1]) {
                case 'A': col = 0; break;
                case 'B': col = 1; break;
                case 'C': col = 2; break;
            }
            if (seats[row - 1][col].assigned) {
                UARTprintf("\nError: Seat %d%c has already been assigned to %s %s.\n", row, input[1], seats[row-1][col].firstname, seats[row-1][col].lastname);
                UARTprintf("Try a different seat? (Y/N): > ");
                string_get(input);
                if (input[0] == 'Y' || input[0] == 'y'){
                    continue;
                }else {
                    return;
                }
            } else {
                strncpy(seats[row-1][col].firstname, fname, sizeof(seats[row-1][col].firstname));
                seats[row-1][col].firstname[sizeof(seats[row-1][col].firstname)-1] = '\0';
                strncpy(seats[row-1][col].lastname, lname, sizeof(seats[row-1][col].lastname));
                seats[row-1][col].lastname[sizeof(seats[row-1][col].lastname)-1] = '\0';
                seats[row-1][col].assigned = true;
                seats[row-1][col].row = row;
                seats[row-1][col].col = input[1];
                UARTprintf("\nSeat %d%c assigned to %s %s.\n", row, input[1], fname, lname);
                loop = false;
            }
        }
    }
    return;
}

// Delete passenger seat assignment TODO
void delete_assignment(void) {
    char input[8];
    char ch;
    UARTprintf("\n\nEnter Seat ID to clear: > ");
    string_get(input);
    if (input[0] == '-') {
        UARTprintf("\nOperation aborted.\n\n");
        return;
    }
    // Expecting input like "1A"
    if (strlen(input) != 2 ||
        input[0] < '1' || input[0] > '4' ||
        !(input[1] == 'A' || input[1] == 'B' || input[1] == 'C')) {
        UARTprintf("Invalid seat ID format. Please enter row and column (e.g., 1A).\n");
        return;
    }
    int row = input[0] - '0';
    char col = input[1];
    UARTprintf("Confirm delete assignment for seat %d%c (Y/N): > ", row, col);
    while (!UARTCharsAvail(UART0_BASE)) {}
    ch = UARTCharGet(UART0_BASE);
    switch (ch) {
        case '-':
            UARTprintf("\nOperation aborted.\n\n");
            break;
        case 'y':
        case 'Y': {
            int col_idx = 0;
            switch (col) {
                case 'A': col_idx = 0; break;
                case 'B': col_idx = 1; break;
                case 'C': col_idx = 2; break;
            }
            if (seats[row-1][col_idx].assigned) {
                seats[row-1][col_idx].assigned = false;
                memset(seats[row-1][col_idx].firstname, 0, sizeof(seats[row-1][col_idx].firstname));
                memset(seats[row-1][col_idx].lastname, 0, sizeof(seats[row-1][col_idx].lastname));
                UARTprintf("\nSeat %d%c assignment deleted.\n\n", row, col);
            } else {
                UARTprintf("\nError: Seat %d%c is already empty.\n\n", row, col);
            }
            break;
        }
        case 'n':
        case 'N': {
            UARTprintf("\nDeletion cancelled.\n\n");
            break;
        }
        default:
            UARTprintf("\nInvalid response detected, please try again.\n\n");
            break;
    }
}


// Change passenger seat assignment TODO
void change_assignment(void) {
    char input[8];
    char fname[10], lname[10];
    int old_row, new_row, old_col_idx, new_col_idx;
    char old_col, new_col;
    UARTprintf("\nEnter current seat ID to change (e.g., 2C): > ");
    string_get(input);
    if (input[0] == '-') {
        UARTprintf("\nOperation aborted.\n\n");
        return;
    }
    if (strlen(input) != 2 ||
        input[0] < '1' || input[0] > '4' ||
        !(input[1] == 'A' || input[1] == 'B' || input[1] == 'C')) {
        UARTprintf("\nInvalid seat ID format.\n");
        return;
    }
    old_row = input[0] - '0';
    old_col = input[1];
    switch (old_col) {
        case 'A': old_col_idx = 0; break;
        case 'B': old_col_idx = 1; break;
        case 'C': old_col_idx = 2; break;
        default: UARTprintf("\nInvalid column.\n"); return;
    }
    if (!seats[old_row-1][old_col_idx].assigned) {
        UARTprintf("\nError: Seat %d%c is not assigned.\n", old_row, old_col);
        return;
    }
    strncpy(fname, seats[old_row-1][old_col_idx].firstname, sizeof(fname)-1);
    fname[sizeof(fname)-1] = '\0';
    strncpy(lname, seats[old_row-1][old_col_idx].lastname, sizeof(lname)-1);
    lname[sizeof(lname)-1] = '\0';

    UARTprintf("\nEnter new seat ID (e.g., 3A): > ");
    string_get(input);
    if (input[0] == '-') {
        UARTprintf("\nOperation aborted.\n\n");
        return;
    }
    if (strlen(input) != 2 ||
        input[0] < '1' || input[0] > '4' ||
        !(input[1] == 'A' || input[1] == 'B' || input[1] == 'C')) {
        UARTprintf("\nInvalid seat ID format.\n");
        return;
    }
    new_row = input[0] - '0';
    new_col = input[1];
    switch (new_col) {
        case 'A': new_col_idx = 0; break;
        case 'B': new_col_idx = 1; break;
        case 'C': new_col_idx = 2; break;
        default: UARTprintf("\nInvalid column.\n"); return;
    }
    if (seats[new_row-1][new_col_idx].assigned) {
        UARTprintf("\nError: Seat %d%c is already assigned to %s %s.\n", new_row, new_col,
            seats[new_row-1][new_col_idx].firstname, seats[new_row-1][new_col_idx].lastname);
        return;
    }
    // Move assignment
    seats[new_row-1][new_col_idx].assigned = true;
    strncpy(seats[new_row-1][new_col_idx].firstname, fname, sizeof(seats[new_row-1][new_col_idx].firstname)-1);
    seats[new_row-1][new_col_idx].firstname[sizeof(seats[new_row-1][new_col_idx].firstname)-1] = '\0';
    strncpy(seats[new_row-1][new_col_idx].lastname, lname, sizeof(seats[new_row-1][new_col_idx].lastname)-1);
    seats[new_row-1][new_col_idx].lastname[sizeof(seats[new_row-1][new_col_idx].lastname)-1] = '\0';
    seats[new_row-1][new_col_idx].row = new_row;
    seats[new_row-1][new_col_idx].col = new_col;

    // Clear old seat
    seats[old_row-1][old_col_idx].assigned = false;
    memset(seats[old_row-1][old_col_idx].firstname, 0, sizeof(seats[old_row-1][old_col_idx].firstname));
    memset(seats[old_row-1][old_col_idx].lastname, 0, sizeof(seats[old_row-1][old_col_idx].lastname));

    UARTprintf("\nSeat assignment changed from %d%c to %d%c for %s %s.\n",
        old_row, old_col, new_row, new_col, fname, lname);
    return;
}
