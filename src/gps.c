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

}
