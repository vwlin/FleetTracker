#include <driverlib/MSP430F5xx_6xx/gpio.h>
#include <msp430.h>
#include "gps.h"

void configureGPS(){
    SET_SAFEBOOT_AS_GPIO;
    SET_SAFEBOOT_AS_OUTPUT;
    DRIVE_SAFEBOOT_PIN_HIGH;
}
