#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_timer.h>
#include <inc/hw_sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_nvic.h>
#include <inc/hw_memmap.h>
#include <inc/hw_adc.h>
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/adc.h>
#include <globalvars.h>
#include <FreeRTOS.h>
#include <event_groups.h>
// #include <task.h>

static void Timer0A_ISR(void) {
    // clear the interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    // trigger conversion
    ADCProcessorTrigger(ADC0_BASE, 3);
}

void Timer0A_Init(void) {
    // Enable Timer Peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
    // Configure the timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 50) - 1); // 20 ms
    // Configure the interrupt
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0A_ISR);
    IntRegister(INT_TIMER0A_TM4C123, Timer0A_ISR);
    IntPrioritySet(INT_TIMER0A_TM4C123, (2 << 5));  // Higher priority than ADC
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A_TM4C123);
    // Enable the timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}

// function to initialize timer 3a KD
void Timer3A_Init(void) {
    // enable timer 3 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3)) {}

    // configure timer behavior and load
    TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
    TimerPrescaleSet(TIMER3_BASE, TIMER_A, 49); // set for 1 MHz tickrate (50 MHz clockspeed)
    TimerLoadSet(TIMER3_BASE, TIMER_A, 0xFFFFFFFF);

    // enable timer
    TimerEnable(TIMER3_BASE, TIMER_A);
}

