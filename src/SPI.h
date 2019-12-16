#ifndef SPI_H_
#define SPI_H_

#include <msp430.h>

/*
 * LORA SPI
 * UCA0
 */

#define SET_LORA_MOSI_MODE              P3SEL |= BIT3
#define SET_LORA_MISO_MODE              P3SEL |= BIT4
#define SET_LORA_SCK_MODE               P2SEL |= BIT7
#define SET_LORA_MOSI_AS_OUTPUT         P3DIR |= BIT3
#define SET_LORA_MISO_AS_INPUT          P3DIR &= ~BIT4
#define SET_LORA_SCK_AS_OUTPUT          P2DIR |= BIT7
#define SET_LORA_MISO_PULLDOWN_1        P3OUT &= ~BIT4
#define SET_LORA_MISO_PULLDOWN_2        P3REN |= BIT4

#define ENABLE_USCIA0                   UCA0CTL1 &= ~BIT0       // Clear UCSWRST bit
#define DISABLE_USCIA0                  UCA0CTL1 |= BIT0        // Set UCSWRST bit

#define SET_UCA0_CLK_PHASE              UCA0CTL0 |= BIT7        // CPHA = 0 (capture on leading edge, change on trailing edge)
#define SET_UCA0_CLK_POLARITY           UCA0CTL0 &= ~BIT6       // CPOL = 0 (inactive at 0)
#define SET_UCA0_ENDIAN                 UCA0CTL0 |= BIT5        // MSB first
#define SET_UCA0_MASTER_MODE            UCA0CTL0 |= UCMST       // Master mode
#define SET_UCA0_PIN_MODE               UCA0CTL0 |= UCMODE_1    // 4-pin SPI, STE active high
#define SET_UCA0_CHAR_LENGTH            UCA0CTL0 &= ~BIT4       // 8-bit data
#define SET_UCA0_SYNC_MODE              UCA0CTL0 |= UCSYNC      // Synchronous
#define SET_UCA0_LISTEN                 UCA0STAT &= ~BIT7       // Listen disabled

#define SET_UCA0_CLK_SRC                UCA0CTL1 |= UCSSEL_2    // SMCLK source (1 MHz)
#define CONFIGURE_UCA0_BR0              UCA0BR0 = 0x000         // Divide by 1
#define CONFIGURE_UCA0_BR1              UCA0BR1 = 0x000         // Divide by 1

/*
 * Configure_SPI_LORA
 * Configure the SPI module used for the LORA chip
 */
void Configure_SPI_LORA();

/*
 * SPI_SendByte_LORA
 * Sends one byte of data through the appropriate SPI line
 */
void SPI_SendByte_LORA(unsigned char sendValue);

/*
 * SPI_ReceiveByte_LORA
 * Receives one byte of data from the appropriate SPI line
 */
unsigned char SPI_ReceiveByte_LORA();

/*
 * SPI_Busy_LORA
 * Checks the BUSY bit in the UCA0 status register
 */
unsigned char SPI_Busy_LORA();


/*
 * GPS SPI
 * UCB0
 */
#define SET_GPS_MOSI_MODE            P3SEL |= BIT0
#define SET_GPS_MISO_MODE            P3SEL |= BIT1
#define SET_GPS_SCK_MODE             P3SEL |= BIT2
#define SET_GPS_MOSI_AS_OUTPUT       P3DIR |= BIT0
#define SET_GPS_MISO_AS_INPUT        P3DIR &= ~BIT1
#define SET_GPS_SCK_AS_OUTPUT        P3DIR |= BIT2
#define SET_GPS_MISO_PULLDOWN_1      P3OUT &= ~BIT1
#define SET_GPS_MISO_PULLDOWN_2      P3REN |= BIT1

#define ENABLE_USCIB0                   UCB0CTL1 &= ~BIT0       // Clear UCSWRST bit
#define DISABLE_USCIB0                  UCB0CTL1 |= BIT0        // Set UCSWRST bit

#define SET_UCB0_CLK_PHASE              UCB0CTL0 |= BIT7        // CPHA = 1 (capture on falling edge, change on leading edge)
#define SET_UCB0_CLK_POLARITY           UCB0CTL0 &= ~BIT6       // CPOL = 0 (inactive at 0)
#define SET_UCB0_ENDIAN                 UCB0CTL0 |= BIT5        // MSB first
#define SET_UCB0_MASTER_MODE            UCB0CTL0 |= UCMST       // Master mode
#define SET_UCB0_PIN_MODE               UCB0CTL0 |= UCMODE_1    // 4-pin SPI, STE active high
#define SET_UCB0_CHAR_LENGTH            UCB0CTL0 &= ~BIT4       // 8-bit data
#define SET_UCB0_SYNC_MODE              UCB0CTL0 |= UCSYNC      // Synchronous
#define SET_UCB0_LISTEN                 UCB0STAT &= ~BIT7       // Listen disabled

#define SET_UCB0_CLK_SRC                UCB0CTL1 |= UCSSEL_2    // SMCLK source
#define CONFIGURE_UCB0_BR0              UCB0BR0 = 0x000         // Divide by 1
#define CONFIGURE_UCB0_BR1              UCB0BR1 = 0x000         // Divide by 1

/*
 * Configure_SPI_GPS
 * Configure the SPI module used for the GPS chip
 */
void Configure_SPI_GPS();

/*
 * SPI_SendByte_GPS
 * Sends one byte of data through the appropriate SPI line
 */
void SPI_SendByte_GPS(unsigned char sendValue);

/*
 * SPI_ReceiveByte_GPS
 * Receives one byte of data from the appropriate SPI line
 */
unsigned char SPI_ReceiveByte_GPS();

/*
 * SPI_Busy_GPS
 * Checks the BUSY bit in the UCB0 status register
 */
unsigned char SPI_Busy_GPS();

/*
 * SPI_SendPacket_GPS
 * Sends a packet of a given length over
 * @param packet: array of characters
 * @param size: number of bytes in packet
 */
void SPI_SendPacket_GPS(unsigned char* packet, int size);

/*
 * SPI_SendPacket_GPS
 * Sends a packet of a given length over
 * @param packet: array of characters
 * @param size: number of bytes in packet
 */
void SPI_ReceivePacket_GPS(unsigned char* packet, int size);

#endif /* SPI_H_ */
