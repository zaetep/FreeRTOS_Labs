#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <utils/uartstdio.h>
#include <driverlib/uart.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/adc.h>
#include <driverlib/pin_map.h>
#include <inc/hw_memmap.h>
#include <inc/hw_uart.h>
#include <inc/hw_gpio.h>
#include <inc/hw_sysctl.h>
#include <inc/hw_adc.h>
#include <ST7735.h>
#include <lcd_tasks.h>
#include <adc_tasks.h>
#include <timer_tasks.h>
#define CMD_BUFFER_SIZE 64

// global variables
static char commandBuffer[CMD_BUFFER_SIZE];
static uint8_t bufferIndex = 0;
static bool commandReady = false;

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

// Check if command ready
bool UART_CommandAvailable(void) {
    // Check if a new character has arrived
    if (UARTCharsAvail(UART0_BASE)) {
        char c = UARTCharGet(UART0_BASE);

        if (c == '\r' || c == '\n') {
            // End of command
            commandBuffer[bufferIndex] = '\0'; // Null-terminate
            bufferIndex = 0;
            commandReady = true;
        } else if (bufferIndex < CMD_BUFFER_SIZE - 1) {
            // Store character
            commandBuffer[bufferIndex++] = c;
        }
    }
    return commandReady;
}

// read commnand into destination string
void UART_ReadCommand(char *dest) {
    if (commandReady) {
        strcpy(dest, commandBuffer);
        commandReady = false;
    } else {
        dest[0] = '\0';
    }
}

// parse and execute the command
void CMD_Run(char *cmd) {
    if (strncmp(cmd, "ADC", 3) == 0) {
        int channel;
        if (sscanf(cmd, "ADC %d", &channel) == 1 && channel >= 0 && channel <= 2) {
            // Change the ADC channel
            Timer_SetADCChannel((uint8_t)channel);
            
            UARTprintf("ADC channel changed to: %d\n", channel);
        } else {
            UARTprintf("Invalid ADC command! Use: ADC <0|1|2>\n");
        }
    }
    else if (strncmp(cmd, "print", 5) == 0) {
        int device, line;
        char message[40];
        if (sscanf(cmd, "print %d %d %[^\n]", &device, &line, message) == 3) {
            if ((device >= 0 && device <= 1) && (line >= 0 && line <= 7)) {
                ST7735_Message(device, line, message, strlen(message));
            } else {
                UARTprintf("Device must be 0-1, Line must be 0-7\n");
            }
        } else {
            UARTprintf("Usage: print <device 0-1> <line 0-7> <message>\n");
        }
    }
    else if (strcmp(cmd, "show pic") == 0) {
        LCD_ShowImage();
    }
    else if (strcmp(cmd, "clear") == 0) {
        ST7735_FillScreen(ST7735_BLACK);
    }
    else if (strcmp(cmd, "help") == 0) {
        UARTprintf("Valid commands:\n");
        UARTprintf("  ADC <0|1|2>        - Change ADC sampling channel\n");
        UARTprintf("  print <device> <line> <message>  - Print text on LCD\n");
        UARTprintf("     device: 0=top, 1=bottom; line: 0-7\n");
        UARTprintf("     Example: print 0 2 Hello\n");
        UARTprintf("  show pic           - Display mascot image\n");
        UARTprintf("  clear              - Clear LCD but keep ADC values\n");
        UARTprintf("  help               - Show this menu\n");
    }
    else {
        UARTprintf("Incorrect command!\n");
    }
}