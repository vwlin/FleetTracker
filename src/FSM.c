#include "FSM.h"

//Type definitions
typedef enum {Send0, WaitACK0, Send1, WaitACK1} State;

extern int maxRetransmit;
extern int errorCount;

int TransmitFile(uint32_t startAddress, uint32_t size){
    //determine number of packets and initialize data array
    uint16_t numWholePackets = size / (MAX_BUFFER_SIZE-1);
    uint8_t extraPacket = ( size % (MAX_BUFFER_SIZE-1) ) > 0;
    uint16_t totalPackets = numWholePackets + extraPacket;
    uint16_t bytesInExtraPacket = size - (numWholePackets * (MAX_BUFFER_SIZE - 1));
    uint8_t bufData[MAX_BUFFER_SIZE-1] = {0};
    uint32_t currentAddress = startAddress;

    State currentState;
    State previousState;
    previousState = WaitACK1;
    currentState = Send0;               //always starts at send0
    State nextState;
    int count = 1;
    int retransmitCount = 0;

    uint8_t seq0Val = 0;
    uint8_t seq1Val = 1;
    uint8_t * seq0 = &seq0Val;
    uint8_t * seq1 = &seq1Val;

    //put the initial page in the buffer (bytes 1-255)
    Memory_ReadFromArray(currentAddress + 2, bufData, MAX_PAGE_SIZE - 2);

    //transmit all packets
    while(count <= totalPackets){

        switch (currentState){

            //Send the odd numbered packets (with sequence bit 0)
            case Send0:

                //add file to array bufData
                LORA_WriteBuffer(0x00, seq0, 0x01);
                LORA_WriteBuffer(0x01, bufData, MAX_BUFFER_SIZE-1);
                //enable txdone and timeout IRQs
                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                LORA_SetTx(0x000000); // timeout disable - stay in TX mode until packet is transmitted and returns in STBY_RC mode
                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                nextState = WaitACK0;

            break;

            //data has been transmitted, need to wait for receiver to send acknowledgment
            case WaitACK0:

                LORA_SetRx(TIMEOUT_VALUE);
                //enable rxdone, header error, crc error, timeout
                LORA_SetDioIrqParams(0x0262, 0x0000, 0x0000, 0x0000);
                //poll status register until something changes
                while( !(LORA_GetIrqStatus()) );

                //if acknowledgment is received before timeout and no errors are found, move on and send 1
                if ((LORA_GetIrqStatus()) == IRQ_RXDONE){
                    LORA_ClearIrqStatus(0x0262);
                    uint8_t sequenceNumber[1] = {0};
                    LORA_ReadBuffer(0x00, sequenceNumber, 1); //read ACK

                    //verify ACK
                    if (sequenceNumber[0] == (0x00)){

                        if (retransmitCount > maxRetransmit){
                            maxRetransmit = retransmitCount;
                        }
                        retransmitCount = 0;

                        //update the information in bufData
                        currentAddress = currentAddress + MAX_PAGE_SIZE;
                        if (count == totalPackets) Memory_ReadFromArray(currentAddress + 2, bufData, bytesInExtraPacket);
                        else Memory_ReadFromArray(currentAddress + 2, bufData, MAX_PAGE_SIZE - 2);

                        //increment count based on a successful, acknowledged transmission
                        count++;
                        nextState = Send1;
                        break;
                    }

                    else {
                        nextState = WaitACK0;
                        break;
                    }
                }

                //if timeout, resend current frame then wait for ACK0
                else if (LORA_GetIrqStatus() & IRQ_TIMEOUT){
                    LORA_ClearIrqStatus(0x0262);

                    if (previousState == currentState){
                        retransmitCount++;
                        if (retransmitCount > maxRetransmit){
                            maxRetransmit = retransmitCount;
                        }
                    }

                    if (retransmitCount < GIVEUP_TRANSMIT){
                        //determine current frame based on count and re-send it
                        LORA_WriteBuffer(0x00, seq0, 0x01);
                        LORA_WriteBuffer(0x01, bufData, MAX_BUFFER_SIZE-1);
                        LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                        LORA_SetTx(0x000000);
                        while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                        LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                        nextState = WaitACK0;
                    }

                    else return 1;
                }

                else {
                    nextState = WaitACK0;
                    break;
                }

            //Send the even numbered packets (with sequence bit 1)
            case Send1:

                //write sequence number
                LORA_WriteBuffer(0x00, seq1, 0x01);
                LORA_WriteBuffer(0x01, bufData, MAX_BUFFER_SIZE-1);
                //enable txdone and timeout IRQs
                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                LORA_SetTx(0x000000); // timeout disable - stay in TX mode until packet is transmitted and returns in STBY_RC mode
                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone or timeout
                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                nextState = WaitACK1;

            break;

            case WaitACK1:

                LORA_SetRx(TIMEOUT_VALUE); //this gives a timeout duration of 1 second using formula timeout duration = timeout*15.625us
                //enable rxdone, header error, crc error, timeout
                LORA_SetDioIrqParams(0x0262, 0x0000, 0x0000, 0x0000);
                //poll status register until something changes
                while( !(LORA_GetIrqStatus()) );

                //if acknowledgment is received before timeout and no errors are found, move on and send 0
                if ((LORA_GetIrqStatus()) == IRQ_RXDONE){
                    LORA_ClearIrqStatus(0x0262);
                    uint8_t sequenceNumber[1] = {0};
                    LORA_ReadBuffer(0x00, sequenceNumber, 1); //read ACK

                    //verify ACK
                    if (sequenceNumber[0] == (0x01)){

                        if (retransmitCount > maxRetransmit){
                            maxRetransmit = retransmitCount;
                        }
                        retransmitCount = 0;

                        //update the information in bufData
                        currentAddress = currentAddress + MAX_PAGE_SIZE;
                        if (count == totalPackets) Memory_ReadFromArray(currentAddress + 2, bufData, bytesInExtraPacket);
                        else Memory_ReadFromArray(currentAddress + 2, bufData, MAX_PAGE_SIZE - 2);

                        count++;     //increment count based on a successful, acknowledged transmission
                        nextState = Send0;
                        break;
                    }

                    else {
                        nextState = WaitACK1;
                        break;
                    }
                }

                //if timeout, resend current frame then wait for ACK0
                else if (LORA_GetIrqStatus() & IRQ_TIMEOUT){

                    LORA_ClearIrqStatus(0x0262);

                    if (previousState == currentState){
                        retransmitCount++;
                        if (retransmitCount > maxRetransmit){
                            maxRetransmit = retransmitCount;
                        }
                    }

                    if (retransmitCount < GIVEUP_TRANSMIT){
                        //determine current frame based on count and re-send it
                        LORA_WriteBuffer(0x00, seq1, 0x01);
                        LORA_WriteBuffer(0x01, bufData, MAX_BUFFER_SIZE-1);
                        LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                        LORA_SetTx(0x000000);
                        while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                        LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                        nextState = WaitACK1;
                        break;
                    }

                    else return 1;

                }

                else{
                    nextState = WaitACK1;
                    break;
                }
        }

        previousState = currentState;
        currentState = nextState;

    }

    LORA_SetStandby(STDBY_RC);
    return 0;
}


int ReceiveFile(uint32_t startAddress, uint32_t size){
    uint16_t numWholePackets = size / (MAX_BUFFER_SIZE-1);
    uint8_t extraPacket = ( size % (MAX_BUFFER_SIZE-1) ) > 0;
    uint16_t totalPackets = numWholePackets + extraPacket;
    uint16_t bytesInExtraPacket = size - (numWholePackets * (MAX_BUFFER_SIZE - 1));

    uint32_t currentAddress = startAddress;
    uint8_t sequenceNumber[1] = {0}; // for sequence number
    uint8_t bufData[MAX_BUFFER_SIZE-1] = {0}; //for received packet to go in memory page

    int count = 1;
    int numErrors = 0;
    uint8_t previousSeqNum = 0x01; //assuming first sequence number is always 0

    while (count <= totalPackets){

        // enable rxdone, header CRC, payload CRC, timeout IRQs
        LORA_SetDioIrqParams(0x0262, 0x0000, 0x0000, 0x0000);
        LORA_SetRx(GIVEUP_TIMEOUT);

        //wait for rxDone or timeout
        while( !(LORA_GetIrqStatus()) );

        //if received before timeout with no errors
        if ((LORA_GetIrqStatus()) == IRQ_RXDONE){
            LORA_ClearIrqStatus(0x0262);

            //get sequence number and check if it is the expected value
            LORA_ReadBuffer(0x00, sequenceNumber, 1);

            // If not a repeat, put the packet in the file
            if (sequenceNumber[0] == !(previousSeqNum)){
                //read the buffer (don't read sequence number)
                LORA_ReadBuffer(0x01, bufData, MAX_BUFFER_SIZE-1);
                previousSeqNum = sequenceNumber[0];

                if (count == totalPackets) Memory_WriteToArray(currentAddress + 2, bufData, bytesInExtraPacket);
                Memory_WriteToArray(currentAddress + 2, bufData, MAX_PAGE_SIZE -2);

                currentAddress = currentAddress + MAX_PAGE_SIZE;
                count++;
            }
            else{
                numErrors++;
            }

            //send ACK whether repeat or not
            LORA_WriteBuffer(0x00, sequenceNumber , 0x01);
            LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
            LORA_SetTx(0x000000);
            while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
            LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
        }

        if ( (LORA_GetIrqStatus() & IRQ_CRC_ERR ) || (LORA_GetIrqStatus() & IRQ_HEADER_ERR ) ){
            numErrors++;
        }

        if (LORA_GetIrqStatus() & IRQ_TIMEOUT){
            return 1;
        }

        if (numErrors >= GIVEUP_RECEIVE){
            return 1;
        }
    }
    LORA_SetStandby(STDBY_RC);
    return 0;
}


