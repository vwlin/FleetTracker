#include <driverlib/MSP430F5xx_6xx/gpio.h>
#include <msp430.h>
#include <stdio.h>

#define SAFEBOOT_DIRECTION           P6DIR
#define SAFEBOOT_PIN                 BIT4
#define SAFEBOOT_SELECT              P6SEL
#define SAFEBOOT_OUTPUT              P6OUT

#define SET_SAFEBOOT_AS_GPIO         SAFEBOOT_SELECT &= ~SAFEBOOT_PIN
#define SET_SAFEBOOT_AS_OUTPUT       SAFEBOOT_DIRECTION |= SAFEBOOT_PIN
#define DRIVE_SAFEBOOT_PIN_HIGH      SAFEBOOT_OUTPUT |= SAFEBOOT_PIN
#define DRIVE_SAFEBOOT_PIN_LOW       SAFEBOOT_OUTPUT &= SAFEBOOT_PIN
#define TOGGLE_SAFEBOOT_PIN          SAFEBOOT_OUTPUT ^= SAFEBOOT_PIN

void configureGPS();
