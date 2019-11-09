#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "SPI.h"
#include "LORA.h"
#include "test.h"
#include "protocol.h"
#include "UART.h"
#include "help.h"
#include "LED.h"
#include "clock.h"
#include "configure.h"

/* CHOOSE ONE
 *
 * ROAMING_NODE
 * HOME_NODE
 * TEST
 */

//#define ROAMING_NODE
#define HOME_NODE
//#define TEST

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    Configure_Clock();
    //Configure_LED();
    Configure_UART();
    Configure_SPI_LORA();
    Configure_LORA();

    // Reset LoRa chip
    LORA_Reset();

    #ifndef TEST
    // Check for errors in the LoRa chip
    uint16_t errors = LORA_GetDeviceErrors();
    if(errors != 0){
        printf("\r\nErrors found. Restart the device.");
        while(1);
    }
    #endif

    // Configure communications
    LORA_SetStandby(STDBY_RC);
    LORA_SetPacketType(PACKET_TYPE_LORA);
    LORA_SetRfFrequency(CENTER_FREQUENCY); // configurable in configure.h
    LORA_SetPaConfig(0x04, 0x07, PA_CONFIG_1262);
    LORA_SetTxParams(22, SET_RAMP_200U);
    LORA_SetModulationParams(0, 0, 0, SPREADING_FACTOR, CODING_RATE, LDR_OPT_ENABLE, BANDWIDTH); // configurable in configure.h
    LORA_SetPacketParams(0, 0, 0, 0, 0, HEADER_MODE, IQ_MODE, PREAMBLE_LENGTH, PAYLOAD_LENGTH, CRC_ENABLE); // configurable in configure.h
    LORA_SetBufferBaseAddress(0x00, 0x00); // Use all 256 bytes for the current mode

    // Set LoRa Sync word MSB and LSB ?? needed? TODO: check if needed
    uint8_t regData[2] = {0x14, 0x24};
    LORA_WriteRegister(0x0740, regData, 2);

    // Configure hardware
    LORA_SetRegulatorMode(REGULATOR_DCDC_LDO); // Use DC-DC regulator
    SET_ANT_SW; // Set RF switch to pass through TX output
    LORA_SetDIO2AsRfSwitchCtrl(DIO2_AS_SWITCH); // Set RF switch to pass through TX output

    //LORA_SetTxContinuousWave();
    //LORA_SetTxInfinitePreamble();

    #ifdef TEST
    volatile uint8_t passLORA = testLORA();
    //volatile uint8_t passRF = testReceiveOneFrame();

    while(1){
        testTransmitOneFrame();
    }
    #endif

    uint8_t i;
    uint8_t readIn[MAX_PAYLOAD+1] = {0};
    uint8_t data[MAX_PAYLOAD] = {0};
    uint8_t status = 0;

    while(1){
        //printf("\r\nentering while loop");
        #ifdef ROAMING_NODE
            printf("\r\nEnter up to %d characters you want to send, then press enter:\r\n", MAX_PAYLOAD);
            reads(readIn, MAX_PAYLOAD+1);
            printf("\r\n");

            for(i = 0; i < MAX_PAYLOAD; i++){
                data[i] = readIn[i];
                readIn[i] = 0;
            }

            //printf("\r\nfilling data");
            /*
            for(i = 0; i < MAX_PAYLOAD; i++){
                data[i] = i;
            }
            */

            //printf("\r\nabout to call Roamer_EstablishConnection");
            status = Roamer_EstablishConnection(data, MAX_PAYLOAD);
        #endif

        #ifdef HOME_NODE
            //printf("\r\nabout to call Home_WaitForConnection");
            status = Home_WaitForConnection();
        #endif

        //printf("\r\n%d", status);
        if(status)
            printf("\r\nData transfer Failed\r\n");
    }
}
