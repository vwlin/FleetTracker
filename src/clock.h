#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

#include "driverlib\MSP430F5xx_6xx\driverlib.h"
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   1000    // 1 MHz
#define UCS_MCLK_FLLREF_RATIO   (1000000UL/32768)

/*
 * Configure_Clock
 * Changes the MCLK frequency to UCS_DESIRED_FREQUENCY_IN_KHZ
 */
void Configure_Clock (void);

/*
 * NMI_ISR
 * Interrupt handler for NMI
 */
void NMI_ISR(void);

#endif /* CLOCK_H_ */
