#include <MSP430F5xx_6xx/gpio.h>
#include <msp430.h>
#include "gps.h"

void configureGPS(){
    SET_SAFEBOOT_AS_GPIO;
    SET_SAFEBOOT_AS_OUTPUT;
    volatile unsigned int i;
    while(1){
        DRIVE_SAFEBOOT_PIN_HIGH;
        for(i=10000; i>0; i--);     // delay
        DRIVE_SAFEBOOT_PIN_LOW;
    }
}
