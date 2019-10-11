#ifndef FLASH_H_
#define FLASH_H_

#include <msp430.h>

#include "driverlib\MSP430F5xx_6xx\driverlib.h"
#include "help.h"

/*
 * Flash memory stores information about files, but not the files themselves
 *
 * From the CLIENT perspective:
 * Information Section A stores flags and information
 *  byte 1: 1-bit flags TX, RX, D, U
 *      TX: BIT7, there is a file to be transmitted (1) or there is no file to be transmitted (0)
 *      RX: BIT6, there are files to be requested/received (1) or there is no file to be requested/received (0)
 *      D: BIT5, if receiving a file, indicates that host should delete file after it is sent to client (1) or save file (0)
 *      U: BIT4, if receiving a file, indicates that the file requested was unavailable (1) or it was available (0)
 *  byte 2: if receiving a file, user ID to request/receive files from
 *  byte 3: if receiving a file, number of files under user ID that were available but could not be transmitted successfully from host to client
 *  byte 4: if receiving a file, number of files under user ID that were available and a were transmitted successfully from host to client
 * Information Sections B, C, and D store information about the files
 *  A file to be transmitted is always stored in INFO B
 *  Files received are stored in INFO B, C, and D in the order that they are received
 *  A client can never be in transmit and receive mode at the same time, so files to be transmitted won't overwrite files received, and vice versa
 * Each file is described by 8 bytes of information stored in Info Section B, C, or D:
 *  byte 1: user ID (between 1 and 254, inclusive)
 *  bytes 2-4: 24-bit address of file in EEPROM
 *  bytes 5-8: 32-bit size of file in bytes
 * Invalid files (files that have already been sent or received files that have been read) have a user ID of 0 or 255
 *
 * From the HOST perspective:
 * Information Section A stores flags and information
 *  byte 1: pointer to next spot in flash where an incoming file's information should be stored
 *      will point to address of INFO B, INFO C, or INFO D, then be incremented after the incoming file is stored
 *  byte 2: number of active files stored in host
 *  byte 3: if transmitting a file, indicates that host should delete file after it is sent to client (1) or save file (0)
 * Information Sections B, C, and D store information about the files stored in the host
 *  A pointer in INFO A points to the next address where an incoming file's information will be stored - this is always an empty spot
 *  or the oldest active file (which would be overwritten by the newest file)
 * Each file is described by 8 bytes of information stored in Info Section B, C, or D:
 *  byte 1: user ID (between 1 and 254, inclusive)
 *  bytes 2-4: 24-bit address or file in EEPROM
 *  bytes 5-8: 32-bit size of file in bytes
 * Invalid files (inactive files, which have been sent and deleted) have a user ID or 0 or 255
 */

#define INFOA_START                 (0x1980)
#define INFOB_START                 (0x1900)
#define INFOC_START                 (0x1880)
#define INFOD_START                 (0x1800)

#define RX_FLAG                     (0x80)
#define TX_FLAG                     (0x40)
#define DELETE_FLAG                 (0x20)
#define FILE_UNAVAILABLE_FLAG       (0x10)

#define FLASH_SEGMENT_SIZE          128         // bytes
#define SIZE_24_BIT_MAX_VAL         16777215

/*
 * Flash_EraseSegment
 * Erases full segment of flash (A, B, C, or D)
 *
 * Parameters:
 *  startAddress - 24-bit address of segment to erase; function fails if address is not INFOA_START, INFOB_START, INFOC_START, or INFOD_START
 *
 * Returns 1 if successful, else 0
 */
uint8_t Flash_EraseSegment(uint32_t startAddress);

/*
 * Flash_ReadArray
 * Read bytes of data from EEPROM and store into readData array
 *
 * Parameters:
 *  startAddress - 24-bit address to start reading from; function fails if address is greater than 16777215
 *  readData - buffer to fill with the read data
 *  size - amount of data in bytes to read
 *
 * Returns 1 if successful, else 0
 */
uint8_t Flash_ReadArray(uint32_t startAddress, uint8_t * readData, uint32_t size);

/*
 * Flash_WriteArray
 * Write bytes of data to EEPROM from the writeData array
 *
 * Parameters:
 *  startAddress - //24-bit address to start writing to; function fails if address is greater than 16777215;
 *          address is INFOA_START, INFOB_START, INFOC_START, or INFOD_START; function fails if address is not one of these values
 *  readData - buffer to fill with the read data
 *  size - amount of data in bytes to read
 *
 * Returns 1 if successful, else 0
 */
uint8_t Flash_WriteArray(uint32_t startAddress, uint8_t * writeData, uint32_t size);

/*
 * Flash_SearchForUID
 * Search file information stored in flash for files with a specific UID
 *
 * Parameters:
 *  UID - user ID to search for
 *  EEPROMaddresses - EEPROM addresses for each file with the specific UID; this array must be of length greater than or equal to MAX_FILES_STORED
 *  sizes - size in bytes of each file with the specific UID; this array must be of length greater than or equal to MAX_FILES_STORED
 *  FLASHaddresses - Flash addresses for each file with the specific UID; this array must be of length greater than or equal to MAX_FILES_STORED
 *
 * Returns the number of files found
 */
uint16_t Flash_SearchForUID(uint8_t UID, uint32_t * EEPROMaddresses, uint32_t * sizes, uint32_t * FLASHaddresses);

#endif /* FLASH_H_ */
