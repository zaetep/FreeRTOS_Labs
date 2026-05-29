//*****************************************************************************
//
// rgbled.h - Prototypes for the evaluation board RGB LED driver.
//
// PF1 - RED, PF2 - GREEN PF3 - BLUE
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#ifndef __RGBLED_H__
#define __RGBLED_H__

//*****************************************************************************
//
// Defines for the hardware 
//
// The switches are on the following ports/pins:
//
// PF1 - RED    (632 nanometer)
// PF2 - GREEN  (518 nanometer)
// PF3 - BLUE   (465 nanometer)

//
// The RGB LED is tied up to 5V but since the lowest Vf is 1.75 we can still
// use a General Purpose Timer in pulse out mode.
//
//*****************************************************************************

//
//  PORTF GPIO PIN NUMBER  PF1-Red LED, PF2 - Blue LED, & PF3 - Green LED
// 
#define PF1   (*((volatile uint32_t *)(GPIO_PORTF_BASE |(1<<(1+2)))))  // Red LED
#define PF2   (*((volatile uint32_t *)(GPIO_PORTF_BASE |(1<<(2+2)))))  // blue LED
#define PF3   (*((volatile uint32_t *)(GPIO_PORTF_BASE |(1<<(3+2)))))  // Green LED
	
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Functions exported from rgbled.c
//
//*****************************************************************************
extern void PortFInit(void); // Port F initialization PF1, PF2, PF3 output 

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************

#endif // __RGBLED_H__
