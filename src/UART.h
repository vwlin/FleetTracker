#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
//#include "protocol.h"

/*
 * uses UCA1
 */

#define ESCAPE_CHARACTER            0x04                                  // End of Transmission (EOT)
#define BACKSPACE_CHARACTER         0x08
#define MAX_PAGE_SIZE               256
#define RX_BUF_SIZE                 16

#define SET_UCA1_RX_TX              P4SEL |= BIT4+BIT5;

#define SET_UCA1_UART_MODE          UCA1CTL0 &= ~BIT0 & ~BIT1 & ~BIT2
#define SET_UCA1_PARITY_NONE        UCA1CTL0 &= ~BIT6 & ~BIT7
#define SET_UCA1_PARITY_ODD         UCA1CTL0 |= BIT7
#define SET_UCA1_PARITY_EVEN        UCA1CTL0 |= BIT6 + BIT7
#define SET_UCA1_ENDIAN             UCA1CTL0 &= ~BIT5                      // USB uses LSB first
#define SET_UCA1_8BIT_DATA          UCA1CTL0 &= ~BIT4                      // for Unicode or extended ASCII
#define SET_UCA1_7BIT_DATA          UCA1CTL0 |= BIT4
#define SET_UCA1_STOPBIT_1          UCA1CTL0 &= ~BIT3

#define SET_UCA1_CLK                UCA1CTL1 |= UCSSEL_2                   // SMCLK source
#define ENABLE_USCIA1               UCA1CTL1 &= ~UCSWRST                   // Clear UCSWRST bit
#define DISABLE_USCIA1              UCA1CTL1 |= UCSWRST                    // Set UCSWRST bit

#define UCBRx                       1666                                   // for 9600 with a clock speed of 16MHz
#define UCBRFx                      0
#define UCBRSx                      6

#define SET_UCA1_BR0                UCA1BR0 = UCBRx & 0x00FF
#define SET_UCA1_BR1                UCA1BR1 = (UCBRx & 0xFF00) >> 8
#define SET_UCA1_MODULATION         UCA1MCTL |= UCBRFx << 4 + UCBRSx << 1

#define ENABLE_RX_IR                UCA1IE |= UCRXIE
#define ENABLE_TX_IR                UCA1IE |= UCTXIE
#define DISABLE_RX_IR               UCA1IE &= ~UCRXIE
#define DISABLE_TX_IR               UCA1IE &= ~UCTXIE

/*
 * Configure_UART
 * Sets up the appropriate parameters for UART communication
 */
void Configure_UART();

/*
 * UART_SendByte
 * Sends a byte over UART
 *
 * Parameters:
 *  sendValue - the byte to send
 */
void UART_SendByte(uint8_t sendValue);

/*
 * UART_ReceiveByte
 * Receives a byte over UART
 *
 * Returns: the byte received
 */
int16_t UART_ReceiveByte();

/*
 * UART_Busy
 * Checks the busy line, which must be false (0) to send or receive a byte
 */
unsigned char UART_Busy();

/*
 * fputc
 * Character writing function, used by printf
 */
int fputc(int _c, register FILE *_fp);

/*
 * fputs
 * String writing function, used by printf
 */
int fputs(const char *_ptr, register FILE *_fp);

/*
 * reads
 * Reads a string from the terminal into an array
 *
 * Parameters:
 *  buf - array into which string is read
 *  maxSize - maximum possible size of the string
 *
 * Returns the size of the string read in bytes
 */
uint32_t reads(uint8_t * buf, uint32_t maxSize);

#endif
