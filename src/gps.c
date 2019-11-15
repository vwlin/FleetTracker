#include <driverlib/MSP430F5xx_6xx/gpio.h>
#include <msp430.h>
#include "gps.h"

void configureGPS(){
    SET_SAFEBOOT_AS_GPIO;
    SET_SAFEBOOT_AS_OUTPUT;
    DRIVE_SAFEBOOT_PIN_HIGH;

    SET_GPS_CS_AS_GPIO;
    SET_GPS_CS_AS_OUTPUT;
    SELECT_GPS_CS;
}

void ublox_configure_spi_port(){
    //create empty packet
    unsigned char packet[28];

    //initialize checksum vars to 0
    unsigned int ck_a = 0;
    unsigned int ck_b = 0;

    //initialize counter var for loop
    unsigned int i;

    packet[0] = 0xb5; //header (byte 1)
    packet[1] = 0x62; //header (byte 2)
    packet[2] = 0x06; //class
    packet[3] = 0x00; //ID

    //set packet length to be 20
    packet[4] = 0x14; //length (least significant byte)
    packet[5] = 0x00; //length (most significant byte)

    //define payload:
    packet[6] = 0x04; //Port Identifier Number (4 for SPI)
    packet[7] = 0x00; //reserved
    packet[8] = 0x00; //tx ready (Low Byte) pin configuration
    packet[9] = 0x00; //tx ready (High Byte) pin configuration
    //set CPOL = 0, CPHA = 1
    packet[10] = 0x02; //mode (bits 7 DOWNTO 0)
    packet[11] = 0x00; //mode (bits 15 DOWNTO 8)
    packet[12] = 0x00; //mode (bits 23 DOWNTO 16)
    packet[13] = 0x00; //mode (bits 31 DOWNTO 24)
    packet[14] = 0x00; //reserved
    packet[15] = 0x00; //reserved
    packet[16] = 0x00; //reserved
    packet[17] = 0x00; //reserved
    //configure for UBX input
    packet[18] = 0x01; //inProtoMask (bits 7 DOWNTO 0)
    packet[19] = 0x00; //inProtoMask (bits 15 DOWNTO 8)
    //configure for UBX output
    packet[20] = 0x01; //outProtoMask (bits 7 DOWNTO 0)
    packet[21] = 0x00; //outProtoMask (bits 15 DOWNTO 8)
    //don't set TX timeout flag
    packet[22] = 0x00; //flags (bits 7 DOWNTO 0)
    packet[23] = 0x00; //flags (bits 15 DOWNTO 8)
    packet[24] = 0x00; //reserved
    packet[25] = 0x00; //reserved

    //calculate checksum
    for(i = 2; i <= 25; i++){
        ck_a += (unsigned int)packet[i];
        ck_a += ck_b;
    }

    packet[26] = ck_a; //checksum byte: CK_A
    packet[27] = ck_b; //checksum byte: CK_B

    //send packet
    SPI_SendPacket_GPS(packet);


}
