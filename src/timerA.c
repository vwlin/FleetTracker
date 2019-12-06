#include "timerA.h"

uint16_t g1ms = 0;
uint16_t g1s = 0;

void Configure_TimerA(){
    // Stop and clear the timer
    TA1CTL = (MC_0 | TACLR);

    // Roll over point
    TA1CCR0 = CCR_VAL;

    // Start the timer in UP mode (counting to TA1CCR0) with SMCLK source, divided by 8
    TA1CTL |= (TASSEL_2 | ID_0 | MC_1);

    // Disable CCR0 interrupt
    DISABLE_TIMER_INTERRUPT;
}

// CCIFG0 vector, address at 0x0FFE2 (see datasheet, pg 53)
#pragma vector = TIMER1_A0_VECTOR
// Timer A interrupt service routine, runs every 15 s (at 1 MHz)
__interrupt void TimerA1_routine(void){
    g1ms++;

    if(g1ms == 1000){
        g1s++;
        g1ms = 0;
    }

    if(g1s == SLEEP_TIME){
        __bic_SR_register_on_exit (LPM1_bits); // exit LPM1
        g1s = 0;
    }
}
