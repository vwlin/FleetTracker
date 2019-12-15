#include "test.h"

uint8_t testLORA(){
    uint8_t i;

    uint8_t status;
    uint8_t pass = 1;

    uint16_t writeAddr;
    uint8_t rxBufferBaseAddr;
    uint8_t txBufferBaseAddr;

    LORA_Reset();

    // Test setting standby modes and getting status
    LORA_SetStandby(STDBY_RC);
    status = LORA_GetStatus();
    pass &= ( (status & LORA_STATUS_CHIP_MODE) == STATUS_MODE_STBYRC );

    // Test writing and reading to registers
    uint8_t regWriteData[2] = {0x34, 0x9F};
    writeAddr = 0x06C0;
    LORA_WriteRegister(writeAddr, regWriteData, 2);
    uint8_t regReadData[2] = {0};
    LORA_ReadRegister(writeAddr, regReadData, 2);
    for(i = 0; i < 2; i++){
        pass &= (regReadData[i] == regWriteData[i]);
    }

    // Test setting buffer base addresses
    rxBufferBaseAddr = 0x02;
    txBufferBaseAddr = 0x50;
    LORA_SetBufferBaseAddress(txBufferBaseAddr, rxBufferBaseAddr);
    uint8_t bufStatus[2] = {0};
    LORA_GetRxBufferStatus(bufStatus, 2);
    pass &= (bufStatus[1] == rxBufferBaseAddr);

    // Test writing and reading to buffers
    uint8_t bufWriteData[2] = {0x55, 0xAA};
    LORA_WriteBuffer(txBufferBaseAddr, bufWriteData, 2);
    uint8_t bufReadData[2] = {0};
    LORA_ReadBuffer(txBufferBaseAddr, bufReadData, 2);
    for(i = 0; i < 2; i++){
        pass &= (bufReadData[i] == bufWriteData[i] );
    }

    // Test GetRxBufferStatus
    uint8_t RxBufferStatus[2] = {0};
    LORA_GetRxBufferStatus(RxBufferStatus, 2);
    pass &= (rxBufferBaseAddr == RxBufferStatus[1]);

    // Test checking for values above SIZE_24_BIT_MAX_VAL
    uint8_t returned = LORA_SetTx(SIZE_24_BIT_MAX_VAL + 1);
    pass &= (returned == 0);
    status = LORA_GetStatus();
    pass &= ( (status & LORA_STATUS_CHIP_MODE) == STATUS_MODE_STBYRC );

    LORA_Reset();

    return pass;
}

void testTransmitOneFrame(){
    uint16_t i;
    LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000); // Enable txdone and timeout IRQs

    uint8_t bufData[PAYLOAD_LENGTH] = {0};
    for(i = 0; i < PAYLOAD_LENGTH; i++){
        bufData[i] = i;
    }
    LORA_WriteBuffer(0x00, bufData, PAYLOAD_LENGTH);

    LORA_SetTx(0x000000); // Timeout disable - stay in TX mode until packet is transmitted and returns in STBY_RC mode
    while( !(LORA_GetIrqStatus()) ); // Wait for IRQ txdone or timeout
    LORA_ClearIrqStatus(0x0201); // Clear IRQ txdone/timeout flag
}

uint8_t testReceiveOneFrame(){
    uint16_t i;
    uint8_t pass = 1;

    uint8_t correctData[PAYLOAD_LENGTH] = {0};
    for(i = 0; i < PAYLOAD_LENGTH; i++){
        correctData[i] = i;
    }

    LORA_SetDioIrqParams(0x0202, 0x0000, 0x0000, 0x0000); // Enable rxdone and timeout IRQs

    LORA_SetRx(0x000000);
    while( !(LORA_GetIrqStatus()) );
    LORA_ClearIrqStatus(0x0202);

    uint8_t received[PAYLOAD_LENGTH] = {0};
    LORA_ReadBuffer(0x00, received, PAYLOAD_LENGTH);

    for(i = 0; i < PAYLOAD_LENGTH; i++){
        pass &= (received[i] == correctData[i]);
    }

    return pass;
}
