#ifndef TEST_H_
#define TEST_H_

#include "LORA.h"
#include "flash.h"
#include "protocol.h"

/*
 * testLORA
 * Tests communications between MSP430 and external LORA chip
 *
 * Returns 1 if all tests pass, else 0
 */
uint8_t testLORA();

/*
 * testFlash
 * Tests communications between MSP430 and internal memory
 *
 * Returns 1 if all tests pass, else 0
 */
uint8_t testFlash();

/*
 * testTransmitOneFrame
 * Sends MAX_PAYLOAD bytes of data: {1, 2, 3, 4, ..., MAX_PAYLOAD}
 */
void testTransmitOneFrame();

/*
 * testReceiveOneFrame
 * Receives MAX_PAYLOAD bytes of data: {1, 2, 3, 4, ..., MAX_PAYLOAD}
 *
 * Returns 1 if received the correct data; else 0
 */
uint8_t testReceiveOneFrame();

/*
 * testFileTransmit
 * Sends file; run from sender
 *
 * Assumes no errors or dropped packets; run testFileReceive first
 */
void testFileTransmit(uint8_t * file, uint32_t size);

/*
 * testFileReceive
 * Receives file assuming expected file size is known; run from receiver
 *
 * Assumes no errors or dropped packets; run before testFileTransmit
 */
void testFileReceive(uint8_t * file, uint32_t size);

#endif /* TEST_H_ */
