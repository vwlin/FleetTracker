#ifndef HELP_H_
#define HELP_H_

#include <stdint.h>

#include "clock.h"

/*
 * reconstructTo32Bits
 * Reconstruct an array of bytes into a single 32-bit value, assuming MSB is stored in lowest index
 *
 * Parameters:
 *  array - the array of bytes to reconstruct the value from
 *  startIndex - index in array to start reconstructing from
 *  length - number of indices to reconstruct
 *
 * Returns the reconstructed value
 */
uint32_t reconstructTo32Bits(uint8_t * array, uint8_t startIndex, uint8_t length);

/*
 * deconstructToArray
 * Deconstruct 32-bit value into a length 3 or 4 array
 *
 * Parameters:
 *  value - the value to be deconstructed
 *  array - the array to fill with the deconstructed value
 *  startIndex - index in array to start deconstructing into
 *  length - number of indices to deconstruct into
 */
void deconstructToArray(uint32_t value, uint8_t * array, uint8_t startIndex, uint8_t length);

/*
 * delay_s
 * Wait by delay time, given in units of seconds
 *
 * Parameters:
 *  delay - amount to delay by, in units of seconds
 */
void delay_s(uint8_t delay);

#endif /* HELP_H_ */
