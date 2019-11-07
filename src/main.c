#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "SPI.h"
#include "LORA.h"
#include "FSM.h"
#include "UART.h"
#include "help.h"
#include "LED.h"
#include "gps.h"
#include "clock.h"

void main(void){

    WDTCTL = WDTPW | WDTHOLD; //stop WDT

    //Configure_SPI_GPS();
    //configureGPS();
    Configure_Clock();
    Configure_UART();
    while(1){
        printf("Hello World/n");
    }
}
