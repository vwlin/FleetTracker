#ifndef TEST_H_
#define TEST_H_

#include "LORA.h"
#include "protocol.h"

/*
 * testLORA
 * Tests communications between MSP430 and external LORA chip
 *
 * Returns 1 if all tests pass, else 0
 */
uint8_t testLORA();

/*
 * testTransmitOneFrame
 * Sends MAX_PAYLOAD bytes of data: {1, 2, 3, 4, ..., MAX_PAYLOAD}
 *
 * Run after testReceiveOneFrame
 */
void testTransmitOneFrame();

/*
 * testReceiveOneFrame
 * Receives MAX_PAYLOAD bytes of data: {1, 2, 3, 4, ..., MAX_PAYLOAD}
 *
 * Returns 1 if received the correct data; else 0
 * Run before testTransmitOneFrame
 */
uint8_t testReceiveOneFrame();

#endif /* TEST_H_ */
