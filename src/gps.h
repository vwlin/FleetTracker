#include <driverlib/MSP430F5xx_6xx/gpio.h>
#include <msp430.h>
#include <stdio.h>
#include "SPI.h"

#define SAFEBOOT_DIRECTION           P6DIR
#define SAFEBOOT_PIN                 BIT4
#define SAFEBOOT_SELECT              P6SEL
#define SAFEBOOT_OUTPUT              P6OUT

#define GPS_CS_DIRECTION             P2DIR
#define GPS_CS_PIN                   BIT2
#define GPS_CS_SELECT                P2SEL
#define GPS_CS_OUTPUT                P2OUT

#define SET_SAFEBOOT_AS_GPIO         SAFEBOOT_SELECT &= ~SAFEBOOT_PIN
#define SET_SAFEBOOT_AS_OUTPUT       SAFEBOOT_DIRECTION |= SAFEBOOT_PIN
#define DRIVE_SAFEBOOT_PIN_HIGH      SAFEBOOT_OUTPUT |= SAFEBOOT_PIN
#define DRIVE_SAFEBOOT_PIN_LOW       SAFEBOOT_OUTPUT &= SAFEBOOT_PIN
#define TOGGLE_SAFEBOOT_PIN          SAFEBOOT_OUTPUT ^= SAFEBOOT_PIN

#define SET_GPS_CS_AS_GPIO           GPS_CS_SELECT &= ~GPS_CS_PIN
#define SET_GPS_CS_AS_OUTPUT         GPS_CS_DIRECTION |= GPS_CS_PIN
#define SELECT_GPS_CS                GPS_CS_OUTPUT &= ~GPS_CS_PIN //drive LOW
#define DESELECT_GPS_CS              GPS_CS_OUTPUT |= GPS_CS_PIN //drive HIGH

void configureGPS();

/*
 * Configures SPI port of ublox module
 */
void ublox_configure_spi_port(unsigned char* returnPacket);
void configure_ublox_nav_pvt(uint8_t* ublox_nav);
void configure_ublox_poll(uint8_t* ublox_poll, uint8_t id1, uint8_t id2);
