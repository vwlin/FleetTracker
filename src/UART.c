#include "UART.h"

volatile uint8_t rx_buf[RX_BUF_SIZE] = {0};
volatile uint8_t rx_start;
volatile uint8_t rx_end;
volatile uint8_t rx_overflow;

void Configure_UART(){
    rx_start = 0;
    rx_end = 0;

    DISABLE_USCIA1;

    SET_UCA1_RX_TX;
    SET_UCA1_UART_MODE;
    SET_UCA1_PARITY_NONE;
    SET_UCA1_ENDIAN;
    SET_UCA1_8BIT_DATA;
    SET_UCA1_STOPBIT_1;

    SET_UCA1_CLK;
    SET_UCA1_BR0;
    SET_UCA1_BR1;
    SET_UCA1_MODULATION;

    ENABLE_USCIA1;
}


void UART_SendByte(uint8_t sendValue){
    while (UART_Busy());

    // Poll TX buffer and proceed only if it is empty
    while( !(UCTXIFG & UCA1IFG) );
    UCA1TXBUF = sendValue; // Flag automatically reset
}

int16_t UART_ReceiveByte(){
    unsigned char readValue;

    if(rx_start != rx_end){ // If not empty
        readValue = rx_buf[rx_start];  // Remove a byte from rx_buf
        rx_start = (rx_start + 1) % RX_BUF_SIZE;
        return readValue;
    }
    else{
        return -1;
    }
}


unsigned char UART_Busy(){
    return (UCA1STAT & UCBUSY);
}

int fputc(int _c, register FILE *_fp)
{
    UART_SendByte((unsigned char) _c);
    return((unsigned char)_c);
}

int fputs(const char *_ptr, register FILE *_fp)
{
  unsigned int i, len;
  len = strlen(_ptr);
  for(i=0 ; i<len ; i++)
  {
    UART_SendByte((unsigned char) _ptr[i]);
  }
  return len;
}

uint32_t reads(uint8_t * buf, uint32_t size, uint32_t offset){
    uint32_t numBytes = 0;
    int16_t temp;
    uint8_t character;

    ENABLE_RX_IR;

    while(numBytes < (size-offset)){ // Input is not greater than maximum size
        temp = UART_ReceiveByte();
            while(temp == -1){
                temp = UART_ReceiveByte();
            }
        character = (uint8_t) temp;

        if(character == LF) // Check for line feed
            break;
        if(character == CR) // Check for carriage return
            break;
        if(character == BACKSPACE_CHARACTER){
            numBytes--;
        }
        else{
            buf[numBytes+offset] = character;
            numBytes++;
        }
    }

    DISABLE_RX_IR;

    return numBytes;
}

// Receive and transmit vector, address at 0x0FFDC (see datasheet, pg 53)
#pragma vector = USCI_A1_VECTOR
// UART interrupt service routine
__interrupt void UARTA1_routine(void){
    switch(UCA1IV){
    case 2: // Data received, highest priority
        if( ((rx_end + 1) % RX_BUF_SIZE) != rx_start ){   // If not full
            rx_buf[rx_end] = UCA1RXBUF; // Add a byte to rx_buf
            rx_end = (rx_end + 1) % RX_BUF_SIZE;
            rx_overflow = 0;
        }
        else{
            rx_overflow = 1;
            UCA1IFG &= ~UCRXIFG;
        }
        break;
    }
}
