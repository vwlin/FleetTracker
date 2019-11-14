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
    configureGPS();

    unsigned char buf[1];

    while(1){
        buf[0] = sizeof(buf) + 23;
        printf("\r\n");
        SPI_SendByte_GPS(0xFF); //send dummy data
        int i = 0;
        for(i = 0; i < 1000; i++){}
        //buf[0] = SPI_ReceiveByte_GPS();
        printf(buf);
    }
}
