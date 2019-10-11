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

    //ENABLE_RX_IR;
}


void UART_SendByte(uint8_t sendValue){
    while (UART_Busy());

    // Poll TX buffer and proceed only if it is empty
    while( !(UCTXIFG & UCA1IFG) );
    UCA1TXBUF = sendValue; // Flag automatically reset
}

int16_t UART_ReceiveByte(){
    unsigned char readValue;

    if(rx_start != rx_end){ // if not empty
        readValue = rx_buf[rx_start];  // remove a byte from rx_buf
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

uint32_t reads(uint8_t * buf, uint32_t maxSize){
    uint32_t numBytes = 0;
    int16_t temp;
    uint8_t character;

    ENABLE_RX_IR;

    while(numBytes < maxSize){ // input is not greater than maximum size
        temp = UART_ReceiveByte();
            while(temp == -1){
                temp = UART_ReceiveByte();
            }
        character = (uint8_t) temp;

        if(character == ESCAPE_CHARACTER) // check for the terminate character or sequence
            break;

        if(character == BACKSPACE_CHARACTER){
            numBytes--;
        }
        else{
            buf[numBytes] = character;
            numBytes++;
        }
    }

    DISABLE_RX_IR;

    return numBytes;
}

uint32_t readm(uint32_t maxSize, uint32_t startAddress){
    uint32_t numBytes = 0;
    uint32_t j = 2;
    uint8_t character;
    int16_t temp;
    uint32_t currentAddress = startAddress;
    uint8_t toStore[MAX_PAGE_SIZE] = {0x064};

    ENABLE_RX_IR;

    while(numBytes < maxSize){
        while(j < MAX_PAGE_SIZE){
            temp = UART_ReceiveByte();
            while(temp == -1){
                temp = UART_ReceiveByte();
            }
            character = (uint8_t) temp;

            if(character == ESCAPE_CHARACTER)
                break;

            toStore[j] = character;
            j++;
            numBytes++;
        }
        Memory_WriteToArray(currentAddress, toStore, j);

        if(character == ESCAPE_CHARACTER)
            break;

        currentAddress = currentAddress + MAX_PAGE_SIZE;
        j = 2;
    }

    DISABLE_RX_IR;

    return numBytes;
}

void printFile(uint32_t size, uint32_t startAddress){
    uint32_t i = 0;
    uint32_t j;
    uint32_t count = 0;
    uint32_t pages;
    uint8_t toPrint[MAX_PAGE_SIZE-2] = {0};
    uint32_t currentAddress = startAddress;

    //get number of pages, maxSize is in bytes
    uint16_t numWholePages = (size)/(MAX_PAGE_SIZE-2);
    uint8_t extraPage = (size % (MAX_PAGE_SIZE-2)) > 0;
    pages = numWholePages + extraPage;

    while (i < pages){

        Memory_ReadFromArray(currentAddress + 2, toPrint, MAX_PAGE_SIZE - 2);

        for (j = 0; j < MAX_PAGE_SIZE - 2; j++){
            count++;
            if (count <= size) UART_SendByte(toPrint[j]);
        }
        currentAddress = currentAddress + MAX_PAGE_SIZE;
        i++;
    }
}


// Receive and transmit vector, address at 0x0FFDC (see datasheet, pg 53)
#pragma vector = USCI_A1_VECTOR
// UART interrupt service routine
__interrupt void UARTA1_routine(void){
    switch(UCA1IV){
    case 2: // data received, highest priority
        if( ((rx_end + 1) % RX_BUF_SIZE) != rx_start ){   // if not full
            rx_buf[rx_end] = UCA1RXBUF; // add a byte to rx_buf
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