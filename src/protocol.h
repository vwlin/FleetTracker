#ifndef NODE_H_
#define NODE_H_

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "LORA.h"
#include "UART.h"
#include "help.h"
#include "configure.h"

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

/*
 * Home_WaitForConnection
 * Handshake from home node perspective: wait for a roamer to establish a connection
 *
 * Parameters:
 *  data - pointer to array of 8 bit data to be received
 *  size - size of data to receive in bytes
 *
 * Returns code:
 * 1 if failed
 * else 0
 */

uint8_t Home_WaitForConnection(uint8_t * data, uint8_t size);


/*
 * Roamer_EstablishConnection
 * Handshake from roaming node perspective: ping home node to establish a connection
 *
 * Parameters:
 *  data - pointer to array of 8 bit data to be transmitted
 *  size - size of data to transmit in bytes
 *
 * Returns code:
 * 1 if failed
 * else 0
 */
uint8_t Roamer_EstablishConnection(uint8_t * data, uint8_t size, uint8_t * startSeq);


/*
 * DATA TRANSFER PROCEDURE
 */
#define TIMEOUT_IN_SECONDS          .750
#define TIMEOUT_VALUE               TIMEOUT_IN_SECONDS/.000015625
#define GIVEUP                      4
#define GIVEUP_TRANSMIT             4
#define GIVEUP_TIMEOUT              4 * TIMEOUT_VALUE
#define GIVEUP_RECEIVE              6
#define GIVEUP_MAC                  5                       // number of times to sense for channel activity before giving up

#define MAX_FILE_SIZE_BYTES         4                       // maximum bytes needed to represent the size of the file
#define MAX_USERID_BYTES            1                       // maximum bytes needed to represent the user ID
#define MAX_NUMFILES_SIZE_BYTES     1                       // maximum bytes needed to represent the number of files Host->Client

#define MAX_SIZE_CHAR_LEN           5                       // maximum number of digits in base 10 (as a string)
#define MAX_USERID_CHAR_LEN         3                       // maximum number of digits in base 10 (as a string)

/* TransmitFile
 * Sends data and waits for ACK after each frame sent, retransmits if errors occurred the first time or if ACK is lost
 *
 * Parameters:
 *  data - pointer to array of 8 bit data to be transmitted
 *  size - size of data to transmit in bytes
 *  startSeq - pointer to array (size 1) with starting sequence number for data transmission, 0 or 1
 *
 * Return code: 1 if there was an error, 0 if transmitted successfully
 */
uint8_t TransmitData(uint8_t * data, uint8_t size, uint8_t * startSeq);


/* ReceiveFile
 * Receives data, sends ACK for each frame received, keeps track of sequence of frames to account for retransmissions
 * Prints data to terminal
 *
 * Parameters:
 *  data - pointer to array of 8 bit data to be received
 *  size - size of data to receive in bytes
 *  startSeq - starting sequence number for data transmission, 0 or 1
 *  deviceID - ID of device sending data
 *
 * Return code: 1 if there was an error, 0 if it was received successfully
 */
uint8_t ReceiveData(uint8_t * data, uint8_t size, uint8_t startSeq, uint16_t deviceID);

#endif /* NODE_H_ */
