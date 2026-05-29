// ENTIRE FILE COMPLETED BY STUDENT

#ifndef TIMER_TASKS_H
#define TIMER_TASKS_H

#include <stdint.h>

extern uint16_t g_ui16ADCSample;

void Timer0A_Init(void);
void Timer3A_Init(void);
void Timer0AIntHandler(void);
void Timer_SetADCChannel(uint8_t channel);
float ADC_GetChannelAvgVoltage(uint8_t channel);
uint32_t ADC_GetConversionTime(void);

#endif