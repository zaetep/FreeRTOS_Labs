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
#include <ctype.h>
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
// TO DELETE 
struct Seat {
    bool assigned;
};

//*****************************************************************************
// Function declaration
//*****************************************************************************
void string_get(char * string);
 
 
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

//void UART0IntHandler(void){
//}

//void Timer0AIntHandler(void){
//}



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

// --- SECTION COMPLETED BY STUDENT --- //

// @brief Find the total amount of points from 10 soceer matches.
// @param [in] points[][] - an array of string with 10 match points, such as {"13:2","0:0","2:25",....}
// @param [out] int8_t *winner
//  1:  A (the 1st team) is the winner   -1: B (the 2nd team) is the winner; 0: tie; 2: invalud match points
// @return: total points of the winner team

uint8_t matchPoints(char * points[], int8_t * pwinner, uint8_t size_of_points) {
  uint8_t matchPoints0 = 0, matchPoints1 = 0; // total points of the winner team
  uint8_t invalid_entries = 0; // counter for invalid entries
  bool second_score = false;
  bool invalid_detected = false;
  int team_score0 = 0;
  int team_score1 = 0;
  char tempscore[10] = "";

  for (int i = 0; i < size_of_points; i++) { // loop through each string
    char *current_char = (char *)points[i];
    second_score = false;
    team_score0 = 0;
    team_score1 = 0;
    invalid_detected = false;
    int len = 0;
    if (*current_char == '\0') { // if nothing in string, it's invalid
      invalid_entries++;
      invalid_detected = true;
    } else {
      while (*current_char != '\0') { // else continue until end of string
        if (isdigit(*current_char)) {
          len = strlen(tempscore);
          tempscore[len] = *current_char;
          tempscore[len + 1] = '\0';
        } else if (*current_char == ':') { // if we find colon it delimits the two scores
          if (strlen(tempscore) != 0) { // check if score was detected
            if (!(second_score)) { // if this isn't the second score, store the score in team_score0
              second_score = true;
              team_score0 = atoi(tempscore);
              tempscore[0] = '\0';
            } else { // if we get here, there were two colons, therefore invalid
              invalid_entries++;
              invalid_detected = true;
              break;
            }
          } else { // no first score was detected, default to 0
            team_score0 = 0;
          }
        } else if (*current_char == ' ') { // skip spaces
          // do nothing
        } else { // wrong type of character detected, therefore invalid
          invalid_entries++;
          invalid_detected = true;
          break;
        }
        current_char++; // go to next character
      }
      if (second_score) { // if there was a second score detected, convert now
        if (strlen(tempscore) != 0) { // make sure there was a second score
          team_score1 = atoi(tempscore);
          tempscore[0] = '\0';
        } else { // no second score was detected, default to 0
          team_score1 = 0;
        }
      }
    }
    if (invalid_detected) {
      // dont add to scores if invalid
    } else if (team_score0 > team_score1) { // team 0 wins
      matchPoints0 += 3;
    } else if (team_score0 < team_score1) { // team 1 wins
      matchPoints1 += 3;
    } else { // tie case
      matchPoints0++;
      matchPoints1++;
    }
  }

  // print results
  UARTprintf("TEAM0_POINTS = %d\n", matchPoints0);
  UARTprintf("TEAM1_POINTS = %d\n", matchPoints1);
  UARTprintf("INVALID_ENTRIES = %d\n", invalid_entries);
  UARTprintf("WINNER = ");

  if (matchPoints0 > matchPoints1) {
	  * pwinner = 1;
    UARTprintf("0\n");
    return matchPoints0;
  } else if (matchPoints0 < matchPoints1) {
	  * pwinner = -1;
    UARTprintf("1\n");
    return matchPoints1;
  } else {
	  * pwinner = 0;
    UARTprintf("TIE\n");
    return matchPoints0;
  }
}

// --- END SECTION COMPLETED BY STUDENT --- //

// An structure to define the data structure to store the game result.
struct TestCaseType {
	 int8_t winner; // = 1, team 1 winner, =-1 team 2 winner; =0 tie	
	 uint8_t totalpoints;  // total points of winning team 
}; 
// An structure array to store the match result. 
// Here we only consider maximum 9 test cases for both stages (use pre-set values, and custom random values) for simplicity.
// You need to store the results in this structure, and possibly return it to UART channel if needed. 

// ORIGINAL TESTCASE STRUCT ARRAY
// struct TestCaseType TestCase[9]= {
//	 {-1,16},	 {1,16},	 {0,14},	 {2,16},
//	 {2,16},	 {2,16},	 {2,16},	 {2,16},	{0,0},
// };

// updated struct array that ignores invalid inputs
struct TestCaseType TestCase[9]= {
	 {-1,16},	 {1,16},	 {0,14},	 {1,16},
	 {1,16},	 {1,16},	 {0,16},	 {1,16},	{0,21},
};

// Array to store the game result.
// You can test your functionality first here using this test cases
// However, you are expected to record the game results from the UART input as the completed lab. 
char *(matchTest[10][10]) ={	{"2:300", "0:0", "15:12","2:13","1:5","55:7","7:2","17:2","2:17","17:18"},  // team 2 winner 16 points
								{"2:3", "0:0", "15:12","2:13","1:5","55:7","7:2","17:2","2:17","18:17"},   // team 1 winner, 16 points
								{"2:3", "0:0", "15:12","2:13","1:5","55:7","7:2","17:2","2:17","17:17"},   // team 1 & 2 tie, 14 points
								{"2:3", "0:0", "15:12","2:13","1:5","55ab:7","7:2","17:2","2:17","17:18"}, // letter match point input, 
								{"2:300", "0:0", "15:12","2:13","1:5","55:7","7:2  ","17:2","2:17","17:18"},  // space in one team match point
								{"2:3", "0:0", "15:12","2:13","1:5","55:7","7:  ","17:2","2:17","17:18"},  // only space in one team match point
								{"2:", "0:0", "15:12","2:13","1:5","55:7","7:2","17:","2:17","17:18"},   // NULL in one team point
								{"2:3", "0:0", "15:12","2:13","1:5","55:7","7:2","","2:17","17:18"},      // NULL in one match points
								{"2:5", "4:0", "1:12","23:1","5:1","55:7","7:2","17:2","2:7","27:18"}      // Place holder for the custom input 
						};
char matchBuffer[100];
char * matchTest_self[10];  
uint8_t match_size[]={10,10,10,10,10,10,10,10,10}; 

int
main(void)
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

// --- SECTION COMPLETED BY STUDENT --- //

  char input_char = 0;
  UARTprintf("Enter 't' to compute a predefined case or 'c' to enter a custom input.\n");
  while (1) {
    while(UARTCharsAvail(UART0_BASE)) UARTCharGet(UART0_BASE); // Clear input buffer
    while(!UARTCharsAvail(UART0_BASE)); // Wait for input
    input_char = UARTCharGet(UART0_BASE);
    if (input_char == 't' || input_char == 'c') {
      break;
    }
    UARTprintf("Invalid input. Please enter 't' or 'c'.\n");
  }

  if (input_char == 't') {
    UARTprintf("Enter a number between 1 and 9.\n");
    while (1) {
      while(UARTCharsAvail(UART0_BASE)) UARTCharGet(UART0_BASE); // Clear input buffer
      while(!UARTCharsAvail(UART0_BASE)); // Wait for input
      input_char = UARTCharGet(UART0_BASE);
      if (input_char >= '1' && input_char <= '9') {
        break;
      }
      UARTprintf("Invalid input. Please try again.\n");
    }
    char temp_str[2] = {input_char, '\0'};
    int test_case = atoi(temp_str); // convert to integer
    int8_t winner;
    uint8_t winner_points = matchPoints((uint8_t **)matchTest[test_case-1], &winner, match_size[test_case-1]);
    if (TestCase[test_case-1].winner == winner && TestCase[test_case-1].totalpoints == winner_points) {
      UARTprintf("Test Case Correct!\n");
    } else {
      UARTprintf("Test Case Incorrect.\n");
    }
  } else {
    	UARTprintf("Fed in custom score for checking\n");
	    uint8_t ui8Argc; // number of actual argument, unsigned 8-bit 

      UARTprintf("Enter the scores in format of 0:0 and seperate by space, end with semicolon;\n");
      //Sample input 3:1 2:2  4:0  1:2  5:3  0:1  3:2  2:0  1:1  4:2; 
      //Team 0 score 20
      int idx = 0;
      char ch = 0;
      while (1) {
        while (!UARTCharsAvail(UART0_BASE)); // Wait for input
        ch = UARTCharGet(UART0_BASE);

        if (ch == ';' || idx >= sizeof(matchBuffer) - 1) {
          break;
        }
        if (ch != '\n' && ch != '\r') { // Filter out newline and carriage return characters
          matchBuffer[idx++] = ch;
        }
      }
      matchBuffer[idx] = '\0';
      uint8_t arg_count = 0;
      char *token = strtok(matchBuffer, " ");
      while (token != NULL && arg_count < 10) {
        matchTest_self[arg_count++] = token;
        token = strtok(NULL, " ");
      }
      int8_t winner;
      uint8_t winner_points = matchPoints(matchTest_self, &winner, arg_count);
      UARTprintf("Custom Input Processed\n");
      
  }

// --- END SECTION COMPLETED BY STUDENT --- //


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
