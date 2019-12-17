#include <driverlib/MSP430F5xx_6xx/gpio.h>
#include <msp430.h>
#include "gps.h"

void Configure_GPS(){
    SET_SAFEBOOT_AS_GPIO;
    SET_SAFEBOOT_AS_OUTPUT;
    DRIVE_SAFEBOOT_PIN_HIGH;

    SET_GPS_CS_AS_GPIO;
    SET_GPS_CS_AS_OUTPUT;
    DESELECT_GPS_CS;
}

void ublox_configure_spi_port(unsigned char * cfgpacket){
    //initialize checksum vars to 0
    unsigned int ck_a = 0;
    unsigned int ck_b = 0;

    //initialize counter var for loop
    unsigned int i;

    cfgpacket[0] = 0xb5; //header (byte 1)
    cfgpacket[1] = 0x62; //header (byte 2)
    cfgpacket[2] = 0x06; //class
    cfgpacket[3] = 0x00; //ID

    //set packet length to be 20
    cfgpacket[4] = 0x14; //length (least significant byte)
    cfgpacket[5] = 0x00; //length (most significant byte)

    //define payload:
    cfgpacket[6] = 0x04; //Port Identifier Number (4 for SPI)
    cfgpacket[7] = 0x00; //reserved
    cfgpacket[8] = 0x00; //tx ready (Low Byte) pin configuration
    cfgpacket[9] = 0x00; //tx ready (High Byte) pin configuration
    //set CPOL = 0, CPHA = 1
    cfgpacket[10] = 0x02; //mode (bits 7 DOWNTO 0)
    cfgpacket[11] = 0x00; //mode (bits 15 DOWNTO 8)
    cfgpacket[12] = 0x00; //mode (bits 23 DOWNTO 16)
    cfgpacket[13] = 0x00; //mode (bits 31 DOWNTO 24)
    cfgpacket[14] = 0x00; //reserved
    cfgpacket[15] = 0x00; //reserved
    cfgpacket[16] = 0x00; //reserved
    cfgpacket[17] = 0x00; //reserved
    //configure for UBX input
    cfgpacket[18] = 0x01; //inProtoMask (bits 7 DOWNTO 0)
    cfgpacket[19] = 0x00; //inProtoMask (bits 15 DOWNTO 8)
    //configure for UBX output
    cfgpacket[20] = 0x01; //outProtoMask (bits 7 DOWNTO 0)
    cfgpacket[21] = 0x00; //outProtoMask (bits 15 DOWNTO 8)
    //don't set TX timeout flag
    cfgpacket[22] = 0x00; //flags (bits 7 DOWNTO 0)
    cfgpacket[23] = 0x00; //flags (bits 15 DOWNTO 8)
    cfgpacket[24] = 0x00; //reserved
    cfgpacket[25] = 0x00; //reserved

    //calculate checksum
    for(i = 2; i <= 25; i++){
        ck_a += (unsigned int)cfgpacket[i];
        ck_a += ck_b;
    }

    cfgpacket[26] = ck_a; //checksum byte: CK_A
    cfgpacket[27] = ck_b; //checksum byte: CK_B
}

void configure_ublox_poll(uint8_t* ublox_poll, uint8_t id1, uint8_t id2){
    uint8_t ck_a=0, ck_b=0;
    uint8_t i;

    ublox_poll[0] = 0xb5;//header
    ublox_poll[1] = 0x62;
    ublox_poll[2] = id1;//id
    ublox_poll[3] = id2;
    ublox_poll[4] = 0x00;//payload size
    ublox_poll[5] = 0x00;

    for(i=2;i<=5;i++) {
        ck_a += ublox_poll[i];
        ck_b += ck_a;
    }

    ublox_poll[6] = ck_a;//check_sum
    ublox_poll[7] = ck_b;
}

void configure_ublox_nav_pvt(uint8_t* ublox_nav){
    uint8_t ck_a=0, ck_b=0;
    uint8_t i;

    ublox_nav[0] = 0xb5;//header
    ublox_nav[1] = 0x62;
    ublox_nav[2] = 0x06;//id
    ublox_nav[3] = 0x08;

    for(i=2;i<=3;i++) {
        ck_a += ublox_nav[i];
        ck_b += ck_a;
    }

    ublox_nav[4] = ck_a;//check_sum
    ublox_nav[5] = ck_b;
}
