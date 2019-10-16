#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "SPI.h"
#include "LORA.h"
#include "test.h"
#include "protocol.h"
#include "UART.h"
#include "help.h"
#include "LED.h"
#include "clock.h"

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    Configure_Clock();
    Configure_LED();

    while(1){
        TOGGLE_LED;
        _delay_cycles(1000000); // 1 second
    }
}
