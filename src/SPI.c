#include "SPI.h"
#include <stdio.h>

/*
 * LORA chip
 */
void Configure_SPI_LORA(){
    DISABLE_USCIA0;

    // UCA0 configuration
    SET_UCA0_MASTER_MODE;
    SET_UCA0_PIN_MODE;
    SET_UCA0_CHAR_LENGTH;
    SET_UCA0_SYNC_MODE;
    SET_UCA0_CLK_PHASE;
    SET_UCA0_CLK_POLARITY;
    SET_UCA0_ENDIAN;
    SET_UCA0_LISTEN;

    // UCA0 clock source
    SET_UCA0_CLK_SRC;
    CONFIGURE_UCA0_BR0;
    CONFIGURE_UCA0_BR1;

    // MOSI, MISO, SCLK
    SET_LORA_MOSI_MODE;
    SET_LORA_MISO_MODE;
    SET_LORA_SCK_MODE;
    SET_LORA_MOSI_AS_OUTPUT;
    SET_LORA_MISO_AS_INPUT;
    SET_LORA_SCK_AS_OUTPUT;

    ENABLE_USCIA0;
}

void SPI_SendByte_LORA(unsigned char sendValue){

    while( SPI_Busy_LORA() ){}

    // Poll TX buffer and proceed only if it is empty
    while( !(UCTXIFG & UCA0IFG) ){}
    UCA0TXBUF = sendValue; // Flag automatically reset
}

unsigned char SPI_ReceiveByte_LORA(){
    unsigned char readValue;

    while( SPI_Busy_LORA() ){}

    // Poll RX buffer and proceed only if it is full
    while(!(UCRXIFG & UCA0IFG)){}
    readValue = (unsigned char)UCA0RXBUF; // Flag automatically reset

    return readValue;
}

unsigned char SPI_Busy_LORA(){
    return (UCA0STAT & BIT0);
}

/*
 * GPS chip
 */
void Configure_SPI_GPS(){
    DISABLE_USCIB0;

    // UCB0 configuration
    SET_UCB0_MASTER_MODE;
    SET_UCB0_PIN_MODE;
    SET_UCB0_CHAR_LENGTH;
    SET_UCB0_SYNC_MODE;
    SET_UCB0_CLK_PHASE;
    SET_UCB0_CLK_POLARITY;
    SET_UCB0_ENDIAN;
    SET_UCB0_LISTEN;

    // UCB0 clock source
    SET_UCB0_CLK_SRC;
    CONFIGURE_UCB0_BR0;
    CONFIGURE_UCB0_BR1;

    // MOSI, MISO, SCLK
    SET_GPS_MOSI_MODE;
    SET_GPS_MISO_MODE;
    SET_GPS_SCK_MODE;
    SET_GPS_MOSI_AS_OUTPUT;
    SET_GPS_MISO_AS_INPUT;
    SET_GPS_SCK_AS_OUTPUT;

    ENABLE_USCIB0;
}

void SPI_SendByte_GPS(unsigned char sendValue){
    while( SPI_Busy_GPS() ){}

    // Poll TX buffer and proceed only if it is empty
    while( !(UCTXIFG & UCB0IFG) ){}
    UCB0TXBUF = sendValue; // Flag automatically reset
}

unsigned char SPI_ReceiveByte_GPS(){
    char buf[1];
    unsigned char readValue;

    while( SPI_Busy_GPS() ){}

    // Poll RX buffer and proceed only if it is full
    while(!(UCRXIFG & UCB0IFG)){}
    buf[0] = (unsigned char)UCB0STAT;
    readValue = (unsigned char)UCB0RXBUF; // Flag automatically reset

    return readValue;
}

unsigned char SPI_Busy_GPS(){
    return (UCB0STAT & BIT0);
}
