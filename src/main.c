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
    unsigned char returnPacket[28];
    ublox_configure_spi_port(returnPacket);
    int i;
    for(i = 0; i < 28; i++){
        printf("%s\n", returnPacket);
    }
    printf("\r\n");
    while(1){
        int i = 0;
        for(i = 0; i < 1000; i++){}
    }
}
