#include "flash.h"

uint8_t Flash_EraseSegment(uint32_t startAddress){
    if( (startAddress != INFOA_START) && (startAddress != INFOB_START) && (startAddress != INFOC_START) && (startAddress != INFOD_START) )
        return 0;

    uint16_t status;
    uint8_t *flash_ptr = (uint8_t *)startAddress;

    WDT_A_hold(WDT_A_BASE);

    // Unlock segment A if necessary
    if(startAddress == INFOA_START){
        FlashCtl_unlockInfoA();
    }

    do {
        FlashCtl_eraseSegment(flash_ptr);
        status = FlashCtl_performEraseCheck(flash_ptr, FLASH_SEGMENT_SIZE);
    } while (status == 0x00);

    // Unlock segment A if necessary
    if(startAddress == INFOA_START){
        FlashCtl_lockInfoA();
    }

    return 1;
}

uint8_t Flash_ReadArray(uint32_t startAddress, uint8_t * readData, uint32_t size){
    if(startAddress > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint32_t i;
    uint8_t *flash_ptr = (uint8_t *)startAddress;

    // Poll busy bit in FCTL3 register
    while(HWREG8(FLASH_BASE + OFS_FCTL3) & BUSY);

    for(i = 0; i < size; i++){
        readData[i] = *flash_ptr++;
    }

    return 1;
}

uint8_t Flash_WriteArray(uint32_t startAddress, uint8_t * writeData, uint32_t size){
    if( (startAddress != INFOA_START) && (startAddress != INFOB_START) && (startAddress != INFOC_START) && (startAddress != INFOD_START) )
        return 0;

    uint8_t *flash_ptr = (uint8_t *)startAddress;

    // Erase segment of flash
    Flash_EraseSegment(startAddress);

    // Unlock segment A if necessary
    if(startAddress == INFOA_START){
        FlashCtl_unlockInfoA();
    }

    // Write to segment of flash
    FlashCtl_write8(writeData, flash_ptr, size);

    // Unlock segment A if necessary
    if(startAddress == INFOA_START){
        FlashCtl_lockInfoA();
    }

    return 1;
}

uint16_t Flash_SearchForUID(uint8_t UID, uint32_t * EEPROMaddresses, uint32_t * sizes, uint32_t * FLASHaddresses){
    uint16_t count = 0;
    uint8_t readInfo[8] = {0};

    Flash_ReadArray(INFOB_START, readInfo, 8);
    if( readInfo[0] == UID ){
        EEPROMaddresses[count] = reconstructTo32Bits(readInfo, 1, 3);
        sizes[count] = reconstructTo32Bits(readInfo, 4, 4);
        FLASHaddresses[count] = INFOB_START;
        count++;
    }

    Flash_ReadArray(INFOC_START, readInfo, 8);
    if( readInfo[0] == UID ){
        EEPROMaddresses[count] = reconstructTo32Bits(readInfo, 1, 3);
        sizes[count] = reconstructTo32Bits(readInfo, 4, 4);
        FLASHaddresses[count] = INFOC_START;
        count++;
    }

    Flash_ReadArray(INFOD_START, readInfo, 8);
    if( readInfo[0] == UID ){
        EEPROMaddresses[count] = reconstructTo32Bits(readInfo, 1, 3);
        sizes[count] = reconstructTo32Bits(readInfo, 4, 4);
        FLASHaddresses[count] = INFOD_START;
        count++;
    }

    return count;
}
