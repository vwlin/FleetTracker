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

/* CHOOSE ONE
 *
 * ROAMING_NODE
 * HOME_NODE
 */

#define ROAMING_NODE
//#define HOME_NODE

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    Configure_Clock();
    //Configure_LED();
    Configure_UART();
    Configure_SPI_LORA();
    Configure_LORA();

    // Check for errors in the LoRa chip
    uint16_t errors = LORA_GetDeviceErrors();
    if(errors != 0){
        printf("\r\nErrors found. Restart the device.");
        while(1);
    }

    // Reset LoRa chip
    LORA_Reset();

    // Configure communications
    LORA_SetStandby(STDBY_RC);
    LORA_SetPacketType(PACKET_TYPE_LORA);
    LORA_SetRfFrequency(915000000);
    LORA_SetPaConfig(0x04, 0x07, PA_CONFIG_1262);
    LORA_SetTxParams(22, SET_RAMP_200U);
    LORA_SetModulationParams(0, 0, 0, LORA_SF6, LORA_CR_4_5, LORA_OPT_OFF, LORA_BW_500);
    LORA_SetPacketParams(0, 0, 0, 0, 0, LORA_HT_EXPLICIT, LORA_IQ_STANDARD, 12, 15, LORA_CRC_ON);   // TODO MAKE VARIABLE OR MACRO INSTEAD OF 15 FOR PAYLOAD LEN
    LORA_SetBufferBaseAddress(0x00, 0x00); // Use all 256 bytes for the current mode

    // Set LoRa Sync word MSB and LSB ?? needed? TODO: check if needed
    uint8_t regData[2] = {0x14, 0x24};
    LORA_WriteRegister(0x0740, regData, 2);

    // Configure hardware
    LORA_SetRegulatorMode(REGULATOR_DCDC_LDO); // Use DC-DC regulator
    SET_ANT_SW; // Set RF switch to pass through TX output
    LORA_SetDIO2AsRfSwitchCtrl(DIO2_AS_SWITCH); // Set RF switch to pass through TX output


    /*
    while(1){
        TOGGLE_LED;
        _delay_cycles(1000000); // 1 second
    }
    */

    uint8_t i;
    uint8_t readIn[MAX_PAYLOAD+1] = {0};
    uint8_t data[MAX_PAYLOAD] = {0};
    uint8_t status;

    while(1){
        //printf("\r\nentering while loop");
        #ifdef ROAMING_NODE
            printf("\r\nEnter up to %d characters you want to send, then press enter:\r\n", MAX_PAYLOAD);
            reads(readIn, MAX_PAYLOAD+1);
            printf("\r\n");

            for(i = 0; i < MAX_PAYLOAD; i++){
                data[i] = readIn[i];
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
    }
}
