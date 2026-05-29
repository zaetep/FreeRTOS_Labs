// ENTIRE FILE COMPLETED BY STUDENT

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_adc.h>
#include <inc/hw_timer.h>
#include <inc/hw_sysctl.h>
#include <inc/hw_nvic.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <inc/hw_types.h>
#include <driverlib/adc.h>
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/fpu.h>
#include <adc_tasks.h>

volatile uint16_t g_ui16ADCSample = 0;

// Circular buffer for ADC samples (16 samples per channel)
#define ADC_BUFFER_SIZE 16
static uint32_t adcBuffer[3][ADC_BUFFER_SIZE];  // 3 channels, 16 samples each
static uint8_t adcBufferIndex = 0;
static volatile uint32_t adcConversionTime = 0;  // Time in Timer3 ticks
static uint8_t currentADCChannel = 0;  // Track current ADC channel

// Handle ADC conversion on Timer0A Interrupt
void Timer0AIntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    uint32_t ui32ADCValue = 0;
    uint32_t startTime, endTime;
    
    // Start timing
    startTime = TimerValueGet(TIMER3_BASE, TIMER_A);
    
    ADCProcessorTrigger(ADC0_BASE, 3);
    // Wait for conversion to complete
    while(!ADCIntStatus(ADC0_BASE, 3, false)) { }
    // Read the result (sequence 3 returns a single sample)
    ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADCValue);
    // Clear the ADC interrupt
    ADCIntClear(ADC0_BASE, 3);
    
    // End timing
    endTime = TimerValueGet(TIMER3_BASE, TIMER_A);
    // Timer counts down, so calculate elapsed time
    adcConversionTime = startTime - endTime;
    
    // Store sample in circular buffer for current channel
    adcBuffer[currentADCChannel][adcBufferIndex] = ui32ADCValue;
    
    // Update buffer index (circular)
    adcBufferIndex = (adcBufferIndex + 1) % ADC_BUFFER_SIZE;
    
    g_ui16ADCSample = (uint16_t)ui32ADCValue;
}

// Set the current ADC channel being sampled
void Timer_SetADCChannel(uint8_t channel) {
    if (channel <= 2) {
        currentADCChannel = channel;
        ADC0_SetChannel(channel);
    }
}

// Initialize Timer 0A for periodic ADC sampling KD
void Timer0A_Init(void) {
    // enable timer peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    // configure timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 1000000) - 1); // 1 MHz

    // configure interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntRegister(INT_TIMER0A_TM4C123, Timer0AIntHandler);
    IntEnable(INT_TIMER0A_TM4C123);
    
    // enable timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}

// Initalize Timer3A for time measurement KD
void Timer3A_Init(void) {
    // enable FPU for time calculations later (only if MCU has an FPU).
    #if defined(__ARM_FP) && (__ARM_FP != 0)
    FPUEnable();
    FPULazyStackingEnable();
    #endif

    // enable timer 3 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER3)) {}

    // configure timer behavior and load
    TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER3_BASE, TIMER_A, 0xFFFFFFFF); // 32-bit timer for better precision

    // enable timer
    TimerEnable(TIMER3_BASE, TIMER_A);
}

// Get average voltage for a specific channel from last 16 samples
float ADC_GetChannelAvgVoltage(uint8_t channel) {
    if (channel > 2) return 0.0f;
    
    uint32_t sum = 0;
    for (uint8_t i = 0; i < ADC_BUFFER_SIZE; i++) {
        sum += adcBuffer[channel][i];
    }
    
    float avgRaw = (float)sum / ADC_BUFFER_SIZE;
    // Convert 12-bit ADC value (0-4095) to voltage (0-3.3V)
    float voltage = (avgRaw * 3.3f) / 4095.0f;
    
    return voltage;
}

// Get the most recent ADC conversion time in microseconds
uint32_t ADC_GetConversionTime(void) {
    // Convert timer ticks to microseconds
    // Timer3 runs at system clock (50 MHz)
    // 1 tick = 1 / 50,000,000 sec = 0.02 microseconds
    return (adcConversionTime * 1000000) / SysCtlClockGet();
}
