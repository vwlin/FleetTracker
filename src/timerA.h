#ifndef TIMERA_H_
#define TIMERA_H_

#include <msp430.h>
#include <stdint.h>

#define CCR_VAL                     14999999        // Interrupt period = (CCR_VAL + 1)/(f_CLK) = 15 s

#define DISABLE_TIMER_INTERRUPT     TA1CCTL0 &= ~CCIE
#define ENABLE_TIMER_INTERRUPT      TA1CCTL0 |= CCIE

void Configure_TimerA();

#endif /* TIMERA_H_ */
