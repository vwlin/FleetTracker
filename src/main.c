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
#include "clock.h"
#include "configure.h"
#include "gps.h"
#include "timerA.h"

/* CHOOSE ONE
 *
 * ROAMING_NODE
 * HOME_NODE
 * TEST
 */

//#define ROAMING_NODE
#define HOME_NODE
//#define TEST

// TODO: move to protocol.h file somehow
uint16_t backoffSec[8] = {1, 2, 4, 8, 16, 32, 64, 128}; // Exponential back-off lookup table for MAC protocol, for a = 2 seconds

uint8_t year, month, day, hour, min, sec;
int32_t latitude, longitude;

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    Configure_Clock();
    Configure_UART();
    Configure_SPI_LORA();
    Configure_LORA();

    #ifdef ROAMING_NODE
    Configure_SPI_GPS();
    Configure_GPS();
    Configure_TimerA();
    #endif

    unsigned char configPacket[28];
    unsigned char navPacket[8];
    ublox_configure_spi_port(configPacket);
    configure_ublox_poll(navPacket, 0x01, 0x07);
    unsigned char ublox_input_buffer[100];
    unsigned char payload[92];
    int i;

    #ifdef TEST
    volatile uint8_t passLORA = testLORA();
    #endif

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
    LORA_SetRfFrequency(CENTER_FREQUENCY); // Configurable in configure.h
    LORA_SetPaConfig(0x04, 0x07, PA_CONFIG_1262);
    LORA_SetTxParams(22, SET_RAMP_200U);
    LORA_SetModulationParams(0, 0, 0, SPREADING_FACTOR, CODING_RATE, LDR_OPT_ENABLE, BANDWIDTH); // Configurable in configure.h
    LORA_SetPacketParams(0, 0, 0, 0, 0, HEADER_MODE, IQ_MODE, PREAMBLE_LENGTH, PAYLOAD_LENGTH, CRC_ENABLE); // Configurable in configure.h
    LORA_SetBufferBaseAddress(0x00, 0x00); // Use all 256 bytes for the current mode
    LORA_SetCadParams(CAD_ON_8_SYMB, 25, 6, CAD_ONLY, 0); // Perform CAD operation, then return to STBY_RC mode

    // Set LoRa Sync word MSB and LSB
    uint8_t regData[2] = {0x14, 0x24};
    LORA_WriteRegister(0x0740, regData, 2);

    // Configure hardware
    LORA_SetRegulatorMode(REGULATOR_DCDC_LDO); // Use DC-DC regulator
    SET_ANT_SW; // Set RF switch to pass through TX output
    LORA_SetDIO2AsRfSwitchCtrl(DIO2_AS_SWITCH); // Set RF switch to pass through TX output

    #ifdef TEST
    volatile uint8_t passLORA = testLORA();

    // Select on of the following tests at a time
    //LORA_SetTxContinuousWave();
    LORA_SetTxInfinitePreamble();
    //volatile uint8_t passRF = testReceiveOneFrame();  // Select either this line or testTransmitOneFrame

    while(1){
        //testTransmitOneFrame();   // Select either this line or testReceiveOneFrame
    }
    #endif

    uint8_t data[PAYLOAD_LENGTH] = {0};
    uint8_t status = 0;

    // RDT protocol related variables
    uint8_t seqNumber[1] = {0}; // Start with a sequence number of 0

    // MAC protocol related variables
    uint16_t channelStatus;
    uint8_t numAttempts;

    while(1){
        #ifdef ROAMING_NODE
            // Fill variables from payload
            for(i = 0; i<92; i++){
                payload[i] = ublox_input_buffer[i + 6];
            }
            year = (uint8_t)(payload[4]) + (uint8_t)(payload[5] << 8);
            month = (uint8_t)payload[6];
            day = (uint8_t)payload[7];
            hour = (uint8_t)payload[8] - 5; // Subtract 5 to get our local time
            min = (uint8_t)payload[9];
            sec = (uint8_t)payload[10];
            longitude = ((int32_t) payload[27]) << 24 | ((int32_t) payload[26]) << 16 |
                    ((int32_t) payload[25]) << 8 | (int32_t) (payload[24]);
            latitude = ((int32_t) payload[31]) << 24 | ((int32_t) payload[30]) << 16 |
                            ((int32_t) payload[29]) << 8 | (int32_t) (payload[28]);

            // Clear input buffer
            for(i = 0; i<100; i++){
                ublox_input_buffer[i] = 0;
            }

            SPI_SendPacket_GPS(configPacket, 28);
            while(ublox_input_buffer[0] == 0) {
                SPI_ReceivePacket_GPS(ublox_input_buffer,100);
            }
            while((ublox_input_buffer[0] != 0xb5)){
                SPI_SendPacket_GPS(navPacket,8);
                for(i = 0; i<100; i++) ublox_input_buffer[i]=0;
                while(ublox_input_buffer[0] == 0) {
                    SPI_ReceivePacket_GPS(ublox_input_buffer,100);
               }
            }

            // Fill empty locations with zeros
            for(i = 0; i < PAYLOAD_LENGTH; i++){
                data[i] = 0;
            }

            // Fill payload, leaving first bit empty for sequence number
            data[0] = (uint8_t)((DEVICE_ID & 0x1FC0) >> 6);
            data[1] = (uint8_t)((DEVICE_ID & 0x003F) << 2);
            // Optional: data[1] | first 2 bits of ADC readings
            data[4] = (uint8_t)((latitude & 0xC0000000) >> 30); // Optional: OR with end of ADC readings
            data[5] = (uint8_t)((latitude & 0x3FC00000) >> 22);
            data[6] = (uint8_t)((latitude & 0x003FC000) >> 14);
            data[7] = (uint8_t)((latitude & 0x00003FC0) >> 6);  // In python, read latitude values and shift left 6
            data[8] = (uint8_t)((longitude & 0xFF000000) >> 24);
            data[9] = (uint8_t)((longitude & 0x00FF0000) >> 16);
            data[10] = (uint8_t)((longitude & 0x0000FF00) >> 8);
            data[11] = (uint8_t)(longitude & 0x000000C0); // In python, read longitude values and shift left 6
            data[11] |= (uint8_t)((day & 0x1F) << 1);
            data[11] |= (uint8_t)((month & 0x08) >> 3);
            data[12] = (uint8_t)((month & 0x07) << 5);
            data[12] |= (uint8_t)(hour & 0x1F);
            data[13] = (uint8_t)((min & 0x3F) << 2);
            data[13] |= (uint8_t)((sec & 0x30) >> 4);
            data[14] = (uint8_t)((sec & 0x0F) << 4);

            printf("\r\nDevice %d", DEVICE_ID);
            printf("\r\nPayload: ");
            for(i = 0; i < PAYLOAD_LENGTH; i++){
                printf("%x ", data[i]);
            }

            // Check for channel activity and repeat if activity detected
            numAttempts = 0;
            channelStatus = 0x0180;
            while( ( channelStatus & 0x0100) && (numAttempts <= GIVEUP_MAC) ){ // Repeat as long as CAD has been detected
                LORA_SetDioIrqParams(0x0180, 0x0000, 0x0000, 0x0000); // Enable CadDone and CadDetected IRQs
                LORA_SetCAD(); // Begin CAD
                while( !LORA_GetIrqStatus() ); // Poll CadDone IRQ
                channelStatus = LORA_GetIrqStatus(); // Check CadDetected IRQ
                LORA_ClearIrqStatus(0x0180); // Clear CadDone and CadDetected
                numAttempts++;
                if(channelStatus & 0x0100){ // If activity detected, wait
                    delay_s(backoffSec[numAttempts]);
                }
                // Print out results
                if(channelStatus & 0x0100)
                    printf("\r\nChannel activity detected, attempt %d", numAttempts);
                else
                    printf("\r\nNo channel activity detected, attempt %d", numAttempts);
            }

            // Proceed based on results of MAC checks
            if(numAttempts > GIVEUP_MAC){ // Activity detected multiple times in a row
                printf("\r\nGave up");
            }
            else if( !(channelStatus & 0x0100) ){ // No activity detected
                printf("\r\nSending data");
                status = Roamer_EstablishConnection(data, PAYLOAD_LENGTH, seqNumber);
            }

            // Sleep for sleepTime seconds
            // Enter LPM1 with SCLK on (SMCLKOFF = 0): ACLK and SCLK on, GPIO unchanged
            ENABLE_TIMER_INTERRUPT; // Enable timer for exiting sleep mode
            __bis_SR_register(LPM1_bits + GIE); // Keeps interrupts enabled, SMCLKOFF already set to 0
            DISABLE_TIMER_INTERRUPT;
        #endif

        #ifdef HOME_NODE
            uint8_t data[PAYLOAD_LENGTH] = {0};
            uint16_t deviceID;

            status = Home_WaitForConnection(data, PAYLOAD_LENGTH);

            deviceID = ((data[0] & 0x7F) << 6) + ((data[1] & 0xFC) >> 2);

            data[0] &= ~0x80; // Zero out sequence number

            // Send data to UART
            // printf("\r\nReceived from device %d:\r\n", deviceID); // Uncomment for testing with terminal
            for(i = 0; i < PAYLOAD_LENGTH; i++){
                printf("%x ", data[i]);
            }
            printf("\r\n");
        #endif

        if(status)
            printf("\r\nData transfer failed\r\n");
        else
            printf("\r\nData transfer success!\r\n");
    }
}
