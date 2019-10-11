#ifndef FSM_H_
#define FSM_H_

#include <msp430.h>
#include <stdint.h>

#include "LORA.h"
#include "UART.h"

#define TIMEOUT_IN_SECONDS          1
#define TIMEOUT_VALUE               TIMEOUT_IN_SECONDS/.000015625
#define GIVEUP                      4
#define GIVEUP_TRANSMIT             4
#define GIVEUP_TIMEOUT              4 * TIMEOUT_VALUE
#define GIVEUP_RECEIVE              6

#define MAX_FILES_STORED            3            // maximum number of files Host can store - can never be greater than 63 (bounded by LORA buffer size, assuming 32-bit size)
#define MAX_FILE_SIZE               42180        // maximum size of the file in bytes, for 3 files
#define MAX_USERID                  255          // maximum user ID possible
#define START_ADDRESS               0x00         //start address of the EEPROM

#define MAX_FILE_SIZE_BYTES         4            // maximum bytes needed to represent the size of the file
#define MAX_USERID_BYTES            1            // maximum bytes needed to represent the user ID
#define MAX_NUMFILES_SIZE_BYTES     1            // maximum bytes needed to represent the number of files Host->Client

#define MAX_SIZE_CHAR_LEN           5            // maximum number of digits in base 10 (as a string)
#define MAX_USERID_CHAR_LEN         3            // maximum number of digits in base 10 (as a string)


/* TransmitFile
 * Sends a file and waits for ACK after each frame sent, retransmits if errors occurred the first time or if ACK is lost
 *
 * Parameters:
 * startAddress - address in EEPROM to start storing file
 * size - size of file to transmit
 *
 * Return code: 1 if there was an error, 0 if transmitted successfully
 */
int TransmitFile(uint32_t startAddress, uint32_t size);


/* ReceiveFile
 * Receives a file, sends ACK for each frame received, keeps track of sequence of frames to account for retransmissions
 *
 * Parameters:
 * startAddress - address in EEPROM to start reading file
 * size - size of file to receive
 *
 * Return code: 1 if there was an error, 0 if it was received successfully
 */
int ReceiveFile(uint32_t startAddress, uint32_t size);

#endif
