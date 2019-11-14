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

    char buf[10];

    //define SPI Port Configuration (UBX-CFG-PRT-SPI)
    SPI_SendByte_GPS(0xB5); //SYNC CHAR 1
    SPI_SendByte_GPS(0x62); //SYNC CHAR 2
    SPI_SendByte_GPS(0x06); //Class
    SPI_SendByte_GPS(0x00); //ID

    //define payload size (in little endian)
    SPI_SendByte_GPS(0x14); //payload size LSB (20 in decimal)
    SPI_SendByte_GPS(0x00); //payload size MSB

    //payload
    SPI_SendByte_GPS(0x04); //port id (4 for SPI)
    SPI_SendByte_GPS(0x00); //reserved
    SPI_SendByte_GPS(0x); //

    while(1){
        SPI_SendByte_GPS(0xFF); //send dummy data
        int i = 0;
        for(i = 0; i < 1000; i++){}
        buf[0] = SPI_ReceiveByte_GPS();
        printf(buf);
    }
}
