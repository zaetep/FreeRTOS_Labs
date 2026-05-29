// ENTIRE FILE COMPLETED BY STUDENT

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_adc.h>
#include <inc/hw_sysctl.h>
#include <inc/hw_gpio.h>
#include <driverlib/adc.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

// Enable ADC and configure it to use channels 0-2 KD
// Using sequencer 3 for single-channel sampling
// Sequencer 3 will be called three times to sample all channels
void ADC0_Init(void){
    // The ADC0 peripheral must be enabled for use.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    // Enable the corresponding pins 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    // Configure the corresponding pins to ADC 
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
    // Configure the ADC sequence 3 (single sample)
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    // ADC sequence step configuration - sequencer 3 has only 1 step
    // We'll change the channel dynamically in the interrupt handler
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    // Enable ADCSequence
    ADCSequenceEnable(ADC0_BASE, 3);
    // Clear the ADC interrupt before sample 
    ADCIntClear(ADC0_BASE, 3);
}

// Set the ADC channel for sequencer 3
void ADC0_SetChannel(uint32_t channel) {
    // Disable the sequencer before reconfiguring
    ADCSequenceDisable(ADC0_BASE, 3);
    
    // Reconfigure the sequencer step with the new channel
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 + channel | ADC_CTL_IE | ADC_CTL_END);
    
    // Re-enable the sequencer
    ADCSequenceEnable(ADC0_BASE, 3);
    
    // Clear any pending interrupts
    ADCIntClear(ADC0_BASE, 3);
}