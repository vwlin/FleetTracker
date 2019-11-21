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

volatile unsigned char payload[92];
unsigned int year, month, day, hour, min, sec;

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    Configure_Clock();
    Configure_UART();
    Configure_SPI_GPS();
    configureGPS();
    unsigned char configPacket[28];
    unsigned char navPacket[8];
    //unsigned char tpPacket[8];
    ublox_configure_spi_port(configPacket);
    configure_ublox_poll(navPacket, 0x01, 0x07);
    unsigned char ublox_input_buffer[100];
    int i;


    //clear input buffer
    for(i = 0; i<100; i++){
        ublox_input_buffer[i] = 0;
    }

    while(1){

        //debugging
        for(i = 0; i<92; i++){
            payload[i] = ublox_input_buffer[i + 6];
        }
        year = (int)(payload[4]) + (int)(payload[5] << 8);
        month = (int)payload[6];
        day = (int)payload[7];
        hour = (int)payload[8] - 5; //subtract 5 to get our local time
        min = (int)payload[9];
        sec = (int)payload[10];

        SPI_SendPacket_GPS(configPacket, 28);
        while(ublox_input_buffer[0] == 0) {
            SPI_ReceivePacket_GPS(ublox_input_buffer,100);
        }
        while((ublox_input_buffer[0] != 0xb5)){
            SPI_SendPacket_GPS(navPacket,8);
            for(i = 0; i<100; i++) ublox_input_buffer[i]=0;
            while(ublox_input_buffer[0] == 0) {
                SPI_ReceivePacket_GPS(ublox_input_buffer,100);
           }
        }
        printf("hello\r\n");
    }

}
