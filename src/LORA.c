#include "LORA.h"

// LORA in method and file names refers to chip, not modulation scheme
// LORA in macros refers to modulation scheme (as opposed to GFSK)

void Configure_LORA(){
    //leave sleep mode by falling edge of NSS
    SET_ENABLE_AS_OUTPUT;
    DISABLE_LORA;

    SET_BUSY_AS_INPUT;
    SET_ANT_SW_AS_OUTPUT;
    SET_NRESET_AS_OUTPUT;

    SET_BUSY_PULLUP_1;
    SET_BUSY_PULLUP_2;

    RESET_ANT_SW;
    SET_NRESET;
}

uint8_t LORA_SetSleep(uint8_t sleepConfig){
    uint8_t status;

    // Command can only be set while in STDBY mode (STDBY_RC or STDBY_XOSC)
    status = LORA_GetStatus();
    if( ((status & LORA_STATUS_CHIP_MODE) != STATUS_MODE_STBYRC) && ((status & LORA_STATUS_CHIP_MODE) != STATUS_MODE_STBYXOSC) )
        return 0;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_SLEEP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(sleepConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

void LORA_SetStandby(uint8_t standbyConfig){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_STANDBY);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(standbyConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetFs(){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_FS);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_SetTx(uint32_t timeout){
    if(timeout > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint8_t timeout1 = (uint8_t)( (timeout & 0xff0000) >> 16 );
    uint8_t timeout2 = (uint8_t)( (timeout & 0x00ff00) >> 8 );
    uint8_t timeout3 = (uint8_t)( timeout & 0x0000ff);

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_TX);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout3);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

uint8_t LORA_SetRx(uint32_t timeout){
    if(timeout > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint8_t timeout1 = (uint8_t)( (timeout & 0xff0000) >> 16 );
    uint8_t timeout2 = (uint8_t)( (timeout & 0x00ff00) >> 8 );
    uint8_t timeout3 = (uint8_t)( timeout & 0x0000ff);

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_RX);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout3);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

void LORA_StopTimerOnPreamble(uint8_t stopConfig){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(STOP_TIMER_ON_PREAMBLE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(stopConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_SetRxDutyCycle(uint32_t rxPeriod, uint32_t sleepPeriod){
    if(rxPeriod > SIZE_24_BIT_MAX_VAL)
        return 0;
    if(sleepPeriod > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint8_t status;

    // Command can only be set while in STDBY_RC mode
    status = LORA_GetStatus();
    if( (status & LORA_STATUS_CHIP_MODE) != STATUS_MODE_STBYRC )
        return 0;

    uint8_t rxPeriod1 = (uint8_t)( (rxPeriod & 0xff0000) >> 16 );
    uint8_t rxPeriod2 = (uint8_t)( (rxPeriod & 0x00ff00) >> 8 );
    uint8_t rxPeriod3 = (uint8_t)( rxPeriod & 0x0000ff );
    uint8_t sleepPeriod1 = (uint8_t)( (sleepPeriod & 0xff0000) >> 16 );
    uint8_t sleepPeriod2 = (uint8_t)( (sleepPeriod & 0x00ff00) >> 8 );
    uint8_t sleepPeriod3 = (uint8_t)( sleepPeriod & 0x0000ff );

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_RX_DUTY_CYCLE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(rxPeriod1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(rxPeriod2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(rxPeriod3);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(sleepPeriod1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(sleepPeriod2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(sleepPeriod3);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

uint8_t LORA_SetCAD(){
    uint8_t packetType;

    // Command can only be used in LoRa packet type
    packetType = LORA_GetPacketType();

    if( (packetType != PACKET_TYPE_GFSK) && (packetType != PACKET_TYPE_LORA) )
        return 0;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_CAD);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

void LORA_SetTxContinuousWave(){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_TX_CONTINUOUS_WAVE);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetTxInfinitePreamble(){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_TX_INFINITE_PREAMBLE);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetRegulatorMode(uint8_t regulatorConfig){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_REGULATOR_MODE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(regulatorConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_Calibrate(uint8_t calibParam){
    uint8_t status;

    // Command can only be set while in STDBY_RC mode
    status = LORA_GetStatus();
    if( (status & LORA_STATUS_CHIP_MODE) != STATUS_MODE_STBYRC )
        return 0;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(CALIBRATE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(calibParam);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

void LORA_CalibrateImage(uint8_t freq1, uint8_t freq2){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(CALIBRATE_IMAGE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(freq1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(freq2);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetPaConfig(uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_PA_CONFIG);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(paDutyCycle);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(hpMax);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(deviceSel);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(PA_CONFIG_PALUT);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_SetRxTxFallbackMode(uint8_t modeConfig){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_FALLBACK_MODE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(modeConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

void LORA_WriteRegister(uint16_t address, uint8_t * data, uint8_t size){
    uint16_t i;
    uint8_t addr_H = (uint8_t)( (address & 0xff00) >> 8 ); // 15:8
    uint8_t addr_L = (uint8_t)( address & 0x00ff ); // 7:0

    if(size > 256)
        size = 256;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(WRITE_REGISTER);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(addr_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(addr_L);
    SPI_ReceiveByte_LORA();

    for(i = 0; i < size; i++){
        SPI_SendByte_LORA(data[i]);
        SPI_ReceiveByte_LORA();
    }

    DISABLE_LORA;
}

void LORA_ReadRegister(uint16_t address, uint8_t * buffer, uint8_t size){
    uint16_t i;
    uint8_t addr_H = (uint8_t)( (address & 0xff00) >> 8 ); // 15:8
    uint8_t addr_L = (uint8_t)( address & 0x00ff ); // 7:0

    if(size > 256)
        size = 256;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(READ_REGISTER);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(addr_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(addr_L);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    for(i = 0; i < size; i++){
        SPI_SendByte_LORA(NOP);
        buffer[i] = SPI_ReceiveByte_LORA();
    }

    DISABLE_LORA;
}

void LORA_WriteBuffer(uint8_t offset, uint8_t * data, uint8_t size){
    uint16_t i;

    if(size > 256)
        size = 256;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(WRITE_BUFFER);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(offset);
    SPI_ReceiveByte_LORA();

    for(i = 0; i < size; i++){
        SPI_SendByte_LORA(data[i]);
        SPI_ReceiveByte_LORA();
    }

    DISABLE_LORA;
}

void LORA_ReadBuffer(uint8_t offset, uint8_t * buffer, uint8_t size){
    uint16_t i;

    if(size > 256)
        size = 256;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(READ_BUFFER);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(offset);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    for(i = 0; i < size; i++){
        SPI_SendByte_LORA(NOP);
        buffer[i] = SPI_ReceiveByte_LORA();
    }

    DISABLE_LORA;
}

void LORA_SetDioIrqParams(uint16_t IRQMask, uint16_t DIO1Mask, uint16_t DIO2Mask,
                          uint16_t DIO3Mask){
    uint8_t IRQMask_H = (uint8_t)( (IRQMask & 0xff00) >> 8 );
    uint8_t IRQMask_L = (uint8_t)( IRQMask & 0x00ff );
    uint8_t DIO1Mask_H = (uint8_t)( (DIO1Mask & 0xff00) >> 8 );
    uint8_t DIO1Mask_L = (uint8_t)( DIO1Mask & 0x00ff );
    uint8_t DIO2Mask_H = (uint8_t)( (DIO2Mask & 0xff00) >> 8 );
    uint8_t DIO2Mask_L = (uint8_t)( DIO2Mask & 0x00ff );
    uint8_t DIO3Mask_H = (uint8_t)( (DIO3Mask & 0xff00) >> 8 );
    uint8_t DIO3Mask_L = (uint8_t)( DIO3Mask & 0x00ff );
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_DIO_IRQ_PARAMS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(IRQMask_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(IRQMask_L);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(DIO1Mask_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(DIO1Mask_L);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(DIO2Mask_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(DIO2Mask_L);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(DIO3Mask_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(DIO3Mask_L);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint16_t LORA_GetIrqStatus(){
    uint8_t status_H;
    uint8_t status_L;
    uint16_t status;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_IRQ_STATUS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status_H = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status_L = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    status = (status_H << 8) + status_L;

    return status;
}

void LORA_ClearIrqStatus(uint16_t clearMask){
    uint8_t clearMask_H = (uint8_t)( (clearMask & 0xff00) >> 8 );
    uint8_t clearMask_L = (uint8_t)( clearMask & 0x00ff );

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(CLEAR_IRQ_STATUS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(clearMask_H);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(clearMask_L);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetDIO2AsRfSwitchCtrl(uint8_t modeConfig){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_DIO2_MODE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(modeConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_SetDIO3AsTCXOCtrl(uint8_t tcxoVoltage, uint32_t timeout){
    if(timeout > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint8_t timeout1 = (uint8_t)( (timeout & 0xff0000) >> 16 );
    uint8_t timeout2 = (uint8_t)( (timeout & 0x00ff00) >> 8 );
    uint8_t timeout3 = (uint8_t)( timeout & 0x0000ff );

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_DIO3_MODE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(tcxoVoltage);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(timeout3);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

//https://github.com/Lora-net/LoRaMac-node/blob/develop/src/radio/sx126x/sx126x.c
void LORA_SetRfFrequency(uint32_t frequency){
    uint32_t freqScaled = (uint32_t)( (double)frequency/(double)FREQ_STEP );

    uint8_t frequency1 = (uint8_t)( (freqScaled & 0xff000000) >> 24 );
    uint8_t frequency2 = (uint8_t)( (freqScaled & 0x00ff0000) >> 16 );
    uint8_t frequency3 = (uint8_t)( (freqScaled & 0x0000ff00) >> 8 );
    uint8_t frequency4 = (uint8_t)( freqScaled & 0x000000ff );

    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_RF_FREQUENCY);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(frequency1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(frequency2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(frequency3);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(frequency4);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetPacketType(uint8_t packetConfig){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_PACKET_TYPE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(packetConfig);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_GetPacketType(){
    uint8_t type;

    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_PACKET_TYPE);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    type = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return type;
}

void LORA_SetTxParams(int8_t power, uint8_t rampTime){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_TX_PARAMS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(power);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(rampTime);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_SetModulationParams(uint32_t GFSK_br, uint8_t GFSK_ps, uint32_t GFSK_fdev,
                             uint8_t LORA_sf, uint8_t LORA_cr, uint8_t LORA_opt,
                             uint8_t BW){
    if(GFSK_br > SIZE_24_BIT_MAX_VAL)
        return 0;
    if(GFSK_fdev > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint8_t p1;
    uint8_t p2;
    uint8_t p3;
    uint8_t p4;
    uint8_t p5 = 0;
    uint8_t p6 = 0;
    uint8_t p7 = 0;
    uint8_t p8 = 0;

    uint8_t type = LORA_GetPacketType();

    if(type == PACKET_TYPE_GFSK){
        p1 = (uint8_t)( (GFSK_br & 0xff0000) >> 16 );
        p2 = (uint8_t)( (GFSK_br & 0x00ff00) >> 8 );
        p3 = (uint8_t)( GFSK_br & 0x0000ff );
        p4 = GFSK_ps;
        p5 = BW;
        p6 = (uint8_t)( (GFSK_fdev & 0xff0000) >> 16 );
        p7 = (uint8_t)( (GFSK_fdev & 0x00ff00) >> 8 );
        p8 = (uint8_t)( GFSK_fdev & 0x0000ff );
    }
    else if(type == PACKET_TYPE_LORA){
        p1 = LORA_sf;
        p2 = BW;
        p3 = LORA_cr;
        p4 = LORA_opt;
    }
    else{
        return 0;
    }

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_MODULATION_PARAMS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p3);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p4);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p5);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p6);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p7);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p8);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

int LORA_SetPacketParams(uint8_t GFSK_pdl, uint8_t GFSK_swLen, uint8_t GFSK_ac,
                         uint8_t GFSK_pt, uint8_t GFSK_w, uint8_t LORA_ht,
                         uint8_t LORA_iq, uint16_t prLen, uint8_t plLen,
                         uint8_t crcType){
    uint8_t p1;
    uint8_t p2;
    uint8_t p3;
    uint8_t p4;
    uint8_t p5;
    uint8_t p6;
    uint8_t p7 = 0;
    uint8_t p8 = 0;
    uint8_t p9 = 0;

    uint8_t type = LORA_GetPacketType();

    if(type == PACKET_TYPE_GFSK){
        p1 = (uint8_t)( (prLen & 0xff00) >> 8 );
        p2 = (uint8_t)( prLen & 0x00ff );
        p3 = GFSK_pdl;
        p4 = GFSK_swLen;
        p5 = GFSK_ac;
        p6 = GFSK_pt;
        p7 = plLen;
        p8 = crcType;
        p9 = GFSK_w;
    }
    else if(type == PACKET_TYPE_LORA){
        p1 = (uint8_t)( (prLen & 0xff00) >> 8 );
        p2 = (uint8_t)( prLen & 0x00ff );
        p3 = LORA_ht;
        p4 = plLen;
        p5 = crcType;
        p6 = LORA_iq;
    }
    else{
        return 0;
    }

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_PACKET_PARAMS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p3);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p4);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p5);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p6);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p7);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p8);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(p9);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

//????????????????????
uint8_t LORA_SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin,
                       uint8_t cadExitMode, uint32_t cadTimeout){
    if(cadTimeout > SIZE_24_BIT_MAX_VAL)
        return 0;

    uint8_t cadTimeout1 = (uint8_t)( (cadTimeout & 0xff0000) >> 16 );
    uint8_t cadTimeout2 = (uint8_t)( (cadTimeout & 0x00ff00) >> 8 );
    uint8_t cadTimeout3 = (uint8_t)( cadTimeout & 0x0000ff );

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_CAD_PARAMS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadSymbolNum);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadDetPeak);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadDetMin);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadExitMode);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadTimeout1);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadTimeout2);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(cadTimeout3);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

void LORA_SetBufferBaseAddress(uint8_t txBase, uint8_t rxBase){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_BUFFER_BASE_ADDR);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(txBase);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(rxBase);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

void LORA_SetLoRaSymbNumTimeout(uint8_t symbNum){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(SET_LORA_SYMB_NUM_TIMEOUT);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(symbNum);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_GetStatus(){
    uint8_t status;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_STATUS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return status;
}

uint8_t LORA_GetRxBufferStatus(uint8_t * status, uint8_t size){
    if(size < 2)
        return 0;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_RX_BUFFER_STATUS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status[0] = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status[1] = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

uint8_t LORA_GetPacketStatus(uint8_t * status, uint8_t size){
    if(size < 3)
        return 0;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_PACKET_STATUS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status[0] = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status[1] = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    status[2] = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return 1;
}

uint8_t LORA_GetRssiInst(){
    uint8_t rssiInst;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_RSSI_INST);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    rssiInst = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    return rssiInst;
}

uint8_t LORA_GetStats(uint16_t * stats, uint8_t size){
    if(size < 3)
        return 0;

    uint8_t pktReceived_H;
    uint8_t pktReceived_L;
    uint8_t crcErr_H;
    uint8_t crcErr_L;
    uint8_t lenHeaderErr_H;
    uint8_t lenHeaderErr_L;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_STATS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    pktReceived_H = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    pktReceived_L = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    crcErr_H = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    crcErr_L = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    lenHeaderErr_H = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    lenHeaderErr_L = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    stats[0] = (pktReceived_H << 8) + pktReceived_L;
    stats[1] = (crcErr_H << 8) + crcErr_L;
    stats[2] = (lenHeaderErr_H << 8) + lenHeaderErr_L;

    return 1;
}

void LORA_ResetStats(){
    int i;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(RESET_STATS);
    SPI_ReceiveByte_LORA();

    for(i = 0; i < 6; i++){
        SPI_SendByte_LORA(NOP);
        SPI_ReceiveByte_LORA();
    }

    DISABLE_LORA;
}

uint16_t LORA_GetDeviceErrors(){
    uint16_t errors;
    uint8_t errors_H;
    uint8_t errors_L;

    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(GET_DEVICE_ERRORS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    errors_H = SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    errors_L = SPI_ReceiveByte_LORA();

    DISABLE_LORA;

    errors = (errors_H << 8) + errors_L;

    return errors;
}

void LORA_ClearDeviceErrors(){
    // Poll Busy line
    while( LORA_Busy() );

    ENABLE_LORA;

    SPI_SendByte_LORA(CLEAR_DEVICE_ERRORS);
    SPI_ReceiveByte_LORA();

    SPI_SendByte_LORA(NOP);
    SPI_ReceiveByte_LORA();

    DISABLE_LORA;
}

uint8_t LORA_Busy(){
    return (LORA_BUSY_PORT & LORA_BUSY_BIT);
}

void LORA_Reset(){
    RESET_NRESET;

    // 16 MHz clock -> 62.5 ns per cycle
    // Wait 100 us, or 1600 62.5 ns cycles
    _delay_cycles(1600);

    SET_NRESET;
}

uint16_t LORA_TransmitAndWait(uint8_t offset, uint8_t * data, uint8_t size, uint32_t timeout, uint16_t IRQMask){
    uint16_t irqContents;

    LORA_WriteBuffer(offset, data, size);
    LORA_SetDioIrqParams(IRQMask, 0x0000, 0x0000, 0x0000);
    LORA_SetTx(timeout);

    irqContents = LORA_GetIrqStatus();
    while(!irqContents){
        irqContents = LORA_GetIrqStatus();
    }
    LORA_ClearIrqStatus(IRQMask);

    return irqContents;
}

uint16_t LORA_WaitForReceive(uint8_t offset, uint8_t * data, uint8_t size, uint32_t timeout, uint16_t IRQMask){
    uint16_t irqContents;

    LORA_SetDioIrqParams(IRQMask, 0x0000, 0x0000, 0x0000);
    LORA_SetRx(timeout);
    if(timeout > 0){ // implicit header timeout bug workaround
        LORA_ResetTimeoutCounter();
    }

    irqContents = LORA_GetIrqStatus();
    while(!irqContents){
        irqContents = LORA_GetIrqStatus();
    }
    LORA_ClearIrqStatus(IRQMask);

    LORA_ReadBuffer(offset, data, size);

    return irqContents;
}

void LORA_ResetTimeoutCounter(){
    uint8_t regData[1] = {0x00};

    LORA_WriteRegister(0x0920, regData, 1);
    LORA_ReadRegister(0x0944, regData, 1);
    regData[0] |= 0x02;
    LORA_WriteRegister(0x0944, regData, 1);
}
