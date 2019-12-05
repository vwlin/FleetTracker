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
#include "gps.h"

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
uint16_t backoffSec[8] = {1, 2, 4, 8, 16, 32, 64, 128}; // exponential back-off lookup table for MAC protocol, for a = 2 seconds

volatile uint8_t year, month, day, hour, min, sec;
volatile int32_t latitude, longitude;

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    Configure_Clock();
    Configure_UART();
    Configure_SPI_LORA();
    Configure_LORA();

    #ifdef ROAMING_NODE
    Configure_SPI_GPS();
    configureGPS();
    #endif

    unsigned char configPacket[28];
    unsigned char navPacket[8];
    ublox_configure_spi_port(configPacket);
    configure_ublox_poll(navPacket, 0x01, 0x07);
    unsigned char ublox_input_buffer[100];
    unsigned char payload[92];
    unsigned char finalPayload[78];
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
    LORA_SetRfFrequency(CENTER_FREQUENCY); // configurable in configure.h
    LORA_SetPaConfig(0x04, 0x07, PA_CONFIG_1262);
    LORA_SetTxParams(22, SET_RAMP_200U);
    LORA_SetModulationParams(0, 0, 0, SPREADING_FACTOR, CODING_RATE, LDR_OPT_ENABLE, BANDWIDTH); // configurable in configure.h
    LORA_SetPacketParams(0, 0, 0, 0, 0, HEADER_MODE, IQ_MODE, PREAMBLE_LENGTH, PAYLOAD_LENGTH, CRC_ENABLE); // configurable in configure.h // todo figure out why TOTAL_PAYLOAD+1, +1 necessary?
    LORA_SetBufferBaseAddress(0x00, 0x00); // Use all 256 bytes for the current mode
    LORA_SetCadParams(CAD_ON_8_SYMB, 25, 6, CAD_ONLY, 0); // Perform CAD operation, then return to STBY_RC mode

    // Set LoRa Sync word MSB and LSB ?? needed? TODO: check if needed
    uint8_t regData[2] = {0x14, 0x24};
    LORA_WriteRegister(0x0740, regData, 2);

    // Configure hardware
    LORA_SetRegulatorMode(REGULATOR_DCDC_LDO); // Use DC-DC regulator
    SET_ANT_SW; // Set RF switch to pass through TX output
    LORA_SetDIO2AsRfSwitchCtrl(DIO2_AS_SWITCH); // Set RF switch to pass through TX output

    #ifdef TEST
    //volatile uint8_t passLORA = testLORA();
    //LORA_SetTxContinuousWave();
    LORA_SetTxInfinitePreamble();
    //volatile uint8_t passRF = testReceiveOneFrame();

    while(1){
        //testTransmitOneFrame();
    }
    #endif

    uint8_t data[PAYLOAD_LENGTH] = {0};
    uint8_t status = 0;

    // to delete - for testing only
    uint8_t strbuf[3];

    // RDT protocol related variables
    uint8_t seqNumber[1] = {0}; // start with a sequence number of 0

    // MAC protocol related variables
    uint16_t channelStatus;
    uint8_t numAttempts;

    // duty cycling related variables
    uint8_t sleepTime; // seconds

    while(1){
        //printf("\r\nentering while loop");
        #ifdef ROAMING_NODE
            // fill variables from payload
            for(i = 0; i<92; i++){
                payload[i] = ublox_input_buffer[i + 6];
            }
            year = (uint8_t)(payload[4]) + (uint8_t)(payload[5] << 8);
            month = (uint8_t)payload[6];
            day = (uint8_t)payload[7];
            hour = (uint8_t)payload[8] - 5; //subtract 5 to get our local time
            min = (uint8_t)payload[9];
            sec = (uint8_t)payload[10];
            longitude = ((int32_t) payload[27]) << 24 | ((int32_t) payload[26]) << 16 |
                    ((int32_t) payload[25]) << 8 | (int32_t) (payload[24]);
            latitude = ((int32_t) payload[31]) << 24 | ((int32_t) payload[30]) << 16 |
                            ((int32_t) payload[29]) << 8 | (int32_t) (payload[28]);


            //clear input buffer
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

            // fill empty locations with zeros
            for(i = 0; i < PAYLOAD_LENGTH; i++){
                data[i] = 0;
            }
            /*
            printf("\r\nEnter up to %d characters you want to send, then press enter:\r\n", PAYLOAD_LENGTH-2);
            reads(data, PAYLOAD_LENGTH, 2);
            printf("\r\n");
            */

            /*
            year = (uint8_t) 0xFF;
            month = (uint8_t) 0x55;
            day = (uint8_t) 0xAA;
            hour = (uint8_t) 0xFF;
            min = (uint8_t) 0x55;
            sec = (uint8_t) 0xAA;
            longitude = (int32_t) 0xFFFFFFFF;
            latitude = (int32_t) 0x55555555;
            */

            // fill payload, leaving first bit empty for sequence number
            data[0] = (uint8_t)((DEVICE_ID & 0x1FC0) >> 6);
            data[1] = (uint8_t)((DEVICE_ID & 0x003F) << 2);
            //TODO: data[1] | first 2 bits of ADC readings
            data[4] = (uint8_t)((latitude & 0xC0000000) >> 30); // or with end of ADC readings
            data[5] = (uint8_t)((latitude & 0x3FC00000) >> 22);
            data[6] = (uint8_t)((latitude & 0x003FC000) >> 14);
            data[7] = (uint8_t)((latitude & 0x00003FC0) >> 6);  // in python, read latitude values and shift left 6
            data[8] = (uint8_t)((longitude & 0xFF000000) >> 24);
            data[9] = (uint8_t)((longitude & 0x00FF0000) >> 16);
            data[10] = (uint8_t)((longitude & 0x0000FF00) >> 8);
            data[11] = (uint8_t)(longitude & 0x000000C0); // in python, read longitude values and shift left 6
            data[11] |= (uint8_t)((day & 0x1F) << 1);
            data[11] |= (uint8_t)((month & 0x08) >> 3);
            data[12] = (uint8_t)((month & 0x07) << 5);
            data[12] |= (uint8_t)(hour & 0x1F);
            data[13] = (uint8_t)((min & 0x3F) << 2);
            data[13] |= (uint8_t)((sec & 0x30) >> 4);
            data[14] = (uint8_t)((sec & 0x0F) << 4);

            printf("\r\n");
            for(i = 0; i < PAYLOAD_LENGTH; i++){
                //if(i >= 2)
                printf("%x", data[i]);
                //sprintf(strbuf, "%d", data[i]);
            }

            // check for channel activity and repeat if activity detected
            numAttempts = 0;
            channelStatus = 0x0180;
            while( ( channelStatus & 0x0100) && (numAttempts <= GIVEUP_MAC) ){ // repeat as long as CAD has been detected
                LORA_SetDioIrqParams(0x0180, 0x0000, 0x0000, 0x0000); // enable CadDone and CadDetected IRQs
                LORA_SetCAD(); // begin CAD
                while( !LORA_GetIrqStatus() ); // poll CadDone IRQ
                channelStatus = LORA_GetIrqStatus(); // check CadDetected IRQ
                LORA_ClearIrqStatus(0x0180); // clear CadDone and CadDetected
                numAttempts++;
                if(channelStatus & 0x0100){ // if activity detected, wait
                    delay_s(backoffSec[numAttempts]);
                }
                // for testing - print out results
                if(channelStatus & 0x0100)
                    printf("\r\nchannel activity detected, attempt %d", numAttempts);
                else
                    printf("\r\nno channel activity detected, attempt %d", numAttempts);
            }
            printf("\r\nexited MAC loop"); // for testing

            // proceed based on results of MAC checks
            if(numAttempts > GIVEUP_MAC){
                sleepTime = 15; // seconds - LATER: calculate dynamically?
                printf("\r\ngave up, calculating sleepTime"); // for testing
            }
            if( !(channelStatus & 0x0100) ){ // no activity detected
                printf("\r\nabout to call Roamer_EstablishConnection");
                status = Roamer_EstablishConnection(data, PAYLOAD_LENGTH, seqNumber);
                // TODO: within transmit data function:
                    // implement roaming node sub-block flow diagram
                    // need to keep track of numSuccess as a pointer and pass it to the function
                    // need to keep track of numFailures as a pointer and pass it to the function
                    // make the decisions based on what was passed into the function?
                    // based on results, update the pointer from within the function
                    // update status accordingly (add a third status result - success, failure and retransmit, failure and giveup
                sleepTime = 15; // seconds
            }

            // TODO
            // sleep for sleepTime seconds
            // enter LPM1 with SCLK on (SMCLKOFF = 0): ACLK and SCLK on, GPIO unchanged
            //ENABLE_TIMER_INTERRUPT; // enable timer for exiting sleep mode
            //__bis_SR_register(LPM1_bits + GIE); // keeps interrupts enabled, SMCLKOFF already set to 0
            //DISABLE_TIMER_INTERRUPT;
        #endif

        #ifdef HOME_NODE
            uint8_t data[PAYLOAD_LENGTH] = {0};
            uint16_t deviceID;

            //printf("\r\nabout to call Home_WaitForConnection");
            status = Home_WaitForConnection(data, PAYLOAD_LENGTH);

            deviceID = ((data[0] & 0x7F) << 6) + ((data[1] & 0xFC) >> 2);

            // to delete - for testing only
            data[0] &= ~0x80;

            // print data to terminal TODO: send to a pc instead
            //printf("\r\nReceived from device %d:\r\n", deviceID);
            for(i = 0; i < PAYLOAD_LENGTH; i++){
                //if(i >= 2)
                printf("%x ", data[i]);
                //data[i] = 0;
            }
            printf("\r\n");
        #endif

        if(status)
            printf("\r\nData transfer Failed\r\n");
    }
}
