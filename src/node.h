#ifndef NODE_H_
#define NODE_H_

#include <msp430.h>
#include <stdio.h>

#include "LORA.h"
#include "FSM.h"
#include "UART.h"
#include "flash.h"
#include "help.h"

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
 * Host_WaitForConnection
 * Wait for a client to establish a connection
 *
 * Parameters:
 *  userID - length 1 buffer that will hold the user ID of client sending file; ignored if host sending file
 *  sizes - if transmitting, sizes of files to be sent in bytes, length numFiles;
 *          if receiving, size of file to be received in bytes, length 1
 *  EEPROMaddresses - if transmitting, the locations in EEPROM where the files are stored, length numFiles
 *  FLASHaddresses - if transmitting, the locations in FLASH where the files' information are stored, length numFiles
 *  numFiles - if transmitting, number of files to be send; if receiving, 1
 *
 * Returns code:
 *  if client is sending file, return 1 (CLIENT_SEND);
 *  if host is sending file, return 0 (HOST_SEND);
 *  if handshake failed, return -1 (HANDSHAKE_FAILED);
 *  if no file available, return -2 (FILE_UNAVAILABLE)
 */

int8_t Host_WaitForConnection(uint8_t * userID, uint32_t * sizes, uint32_t * EEPROMaddresses, uint32_t * FLASHaddresses, uint8_t * numFiles);


/*
 * Client_EstablishConnection
 * Ping a host to establish a connection
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
int8_t Client_EstablishConnection(uint8_t transmit, uint32_t * size, uint8_t * userID, uint8_t * numFiles);

#endif /* NODE_H_ */
