#include "gps.h"
#include <msp430.h>

void configureGPS(){
    SET_SAFEBOOT_AS_OUTPUT;
    DRIVE_SAFEBOOT_PIN_HIGH;
    GPIO_setAsOutputPin(SAFEBOOT_PORT, SAFEBOOT_PIN)
}
