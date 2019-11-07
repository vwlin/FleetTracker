#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "SPI.h"
#include "UART.h"
#include "LED.h"
#include "clock.h"
#include "gps.h"

/* CHOOSE ONE
 *
 * ROAMING_NODE
 * HOME_NODE
 * TEST
 */

//#define ROAMING_NODE
//#define HOME_NODE
//#define TEST

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    Configure_Clock();
    Configure_UART();
    Configure_SPI_GPS();
    Configure_SPI_LORA(); //debug
    configureGPS();

    char buf[1];


    while(1){
        int i = 0;
        SELECT_GPS_CS;
        SPI_SendByte_LORA(0xDE);
        SPI_SendByte_GPS(0xAD);
        for(i = 0; i < 1000; i++){}
        /*
        buf[0] = SPI_ReceiveByte_GPS();
        printf(buf);
        printf("\r\n");
        */

        DESELECT_GPS_CS;
        for(i = 0; i < 1000; i++){}
    }
}
