#include "help.h"

uint32_t reconstructTo32Bits(uint8_t * array, uint8_t startIndex, uint8_t length){
    uint8_t i;
    uint32_t value = 0;

    volatile uint32_t test;

    for(i = 0; i < length; i++){
        test = (8*(length - i - 1));
        value += (uint32_t)array[i+startIndex] << (8*(length - i - 1));
    }

    return value;
}

void deconstructToArray(uint32_t value, uint8_t * array, uint8_t startIndex, uint8_t length){
    uint8_t i;
    uint32_t masks[4] = {0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};

    for(i = 0; i < length; i++){
        array[i+startIndex] = (uint8_t)( (value & masks[length - i - 1]) >> (8*(length - i - 1)) );
    }
}

void delay_s(uint8_t delay){
    uint8_t i;

    for(i = 0; i < delay; i++){
        _delay_cycles( (uint32_t)(UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ)*(uint32_t)(1000));
    }
}
