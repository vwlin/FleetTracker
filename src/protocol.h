#ifndef NODE_H_
#define NODE_H_

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "LORA.h"
#include "UART.h"
#include "flash.h"
#include "help.h"

/*
 * HANDSHAKE PROCEDURE
 */
#define TRANSMIT        1
#define RECEIVE         0

#define PASSWORD        (0xFF)
#define CLIENT_TO_HOST  (0xFF)
#define HOST_TO_CLIENT  (0x00)
#define OK              (0x55)
#define FILES_AVIALABLE (0x0F)
#define NO_FILES        (0xF0)

#define CLIENT_SEND                 1
#define HOST_SEND                   0
#define HANDSHAKE_FAILED           -1
#define FILE_UNAVAILABLE           -2

/*
 * Home_WaitForConnection
 * Handshake from home node perspective: wait for a roamer to establish a connection
 *
 * Parameters:
 *  userID - length 1 buffer that will hold the ID of the roaming node sending the data
 *
 * Returns code:
 *  if client is sending file, return 1 (CLIENT_SEND);
 *  if host is sending file, return 0 (HOST_SEND);
 *  if handshake failed, return -1 (HANDSHAKE_FAILED);
 *  if no file available, return -2 (FILE_UNAVAILABLE)
 */

int8_t Home_WaitForConnection(uint8_t * userID);


/*
 * Roamer_EstablishConnection
 * Handshake from roaming node perspective: ping home node to establish a connection
 *
 * Parameters:
 *  transmit - 1 if transmitting file, 0 if receiving file
 *  size - if transmitting, size of file to be sent in bytes, length 1;
 *          if receiving, sizes of files to be received in bytes, length numFiles
 *  userID - length 1 buffer that will hold the user ID of the client sending file or user ID of requested files
 *  numFiles - if transmitting, 1; if receiving, number of files to be received
 *
 * Returns code:
 *  if client is sending file, return 1 (CLIENT_SEND);
 *  if host is sending file, return 0 (HOST_SEND);
 *  if handshake failed, return -1 (HANDSHAKE_FAILED);
 *  if no file available, return -2 (FILE_UNAVAILABLE)
 */
int8_t Roamer_EstablishConnection(uint8_t transmit, uint32_t * size, uint8_t * userID, uint8_t * numFiles);


/*
 * DATA TRANSFER PROCEDURE
 */

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
 * Sends data and waits for ACK after each frame sent, retransmits if errors occurred the first time or if ACK is lost
 *
 * Parameters:
 * data - pointer to array of 8 bit data
 * size - size of data to transmit in bytes
 *
 * Return code: 1 if there was an error, 0 if transmitted successfully
 */
int TransmitData(uint8_t * data, uint8_t size);


/* ReceiveFile
 * Receives data, sends ACK for each frame received, keeps track of sequence of frames to account for retransmissions
 * Prints data to terminal
 *
 * Return code: 1 if there was an error, 0 if it was received successfully
 */
int ReceiveData();

#endif /* NODE_H_ */
