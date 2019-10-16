#ifndef LED_H_
#define LED_H_

#include <msp430.h>

/*
 * LED
 * GPIO: P1.0
 */
#define LED_BIT                     BIT0
#define LED_PORT                    P1OUT
#define LED_DDR                     P1DIR
#define SET_LED_AS_OUTPUT           LED_DDR |= LED_BIT
#define TURN_ON_LED                 LED_PORT |= LED_BIT
#define TURN_OFF_LED                LED_PORT &= ~LED_BIT
#define TOGGLE_LED                  LED_PORT ^= LED_BIT

typedef enum {off, slow, fast, on} LEDState;

void Configure_LED();

#endif /* LED_H_ */
