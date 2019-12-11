#include "protocol.h"

typedef enum {Listen, Greet, Receive} HomeState; // handshake
typedef enum {Ping, WaitForGreeting, Transmit} RoamerState; // handshake
typedef enum {Send0, WaitACK0, Send1, WaitACK1} State; // data transfer

//extern int maxRetransmit; // handshake
//extern int maxRetransmit; // data transfer
extern int errorCount; // data transfer

uint8_t Home_WaitForConnection(uint8_t * data, uint8_t size){
    uint8_t receivedHello[4] = {0};
    uint16_t deviceID;
    uint8_t sendHello[3] = {0};
    uint16_t irqStatus;

    HomeState currentState = Listen;
    HomeState nextState;

    while(1){
        switch (currentState){

            // Wait for client to send "Hello" ping
            case Listen:
                //printf("\r\nlistening for a hello");
                irqStatus = LORA_WaitForReceive(0x00, receivedHello, 4, 0, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR);

                // Ping received with no errors, and password is correct
                if( (irqStatus == IRQ_RXDONE) && (receivedHello[0] == PASSWORD) ){
                    nextState = Greet; // Continue with conversation
                }
                // Ping received with errors password is incorrect
                else{
                    nextState = Listen; // Keep Listening
                }
                break;

            // Send "Hello" response to client
            case Greet:
                //printf("\r\nresponding to client");
                sendHello[0] = PASSWORD;
                sendHello[1] = receivedHello[2];
                sendHello[2] = receivedHello[3];
                LORA_TransmitAndWait(0x00, sendHello, 3, 0, IRQ_TXDONE);

                nextState = Receive; // Continue with conversation
                break;

            // Receive data
            case Receive:
                //printf("\r\nabout to start receiving data");
                deviceID = (receivedHello[2] << 8) + receivedHello[3];
                return ReceiveData(data, size, receivedHello[1], deviceID);
                // if fails, then return to main, and home will resume listening by calling Home_WaitForConnection
                // if successful, then return to main, and home will resume listening by calling Home_WaitForConnection
        }
        currentState = nextState;
    }
}

uint8_t Roamer_EstablishConnection(uint8_t * data, uint8_t size, uint8_t * startSeq){
    uint8_t receivedHello[3] = {0};
    uint8_t sendHello[4] = {0}; // PASSWORD, starting sequence number, device ID upper byte, device ID lower byte
    uint16_t irqStatus;
    uint16_t deviceID;

    uint8_t flags[1] = {0};
    Flash_ReadArray(INFOA_START, flags, 1);

    RoamerState currentState = Ping;
    RoamerState nextState;

    int retransmitCount = 0;

    while(1){
        switch(currentState){

            // Ping host with password
            case Ping:
                //printf("\r\npinging");
                sendHello[0] = PASSWORD;
                sendHello[1] = *startSeq;
                sendHello[2] = (uint8_t) ((DEVICE_ID & 0xFF00) >> 8);
                sendHello[3] = (uint8_t) (DEVICE_ID & 0x00FF);
                irqStatus = LORA_TransmitAndWait(0x00, sendHello, 4, TIMEOUT_VALUE, IRQ_TXDONE|IRQ_TIMEOUT);

                //printf("\r\npinged");
                if(irqStatus == IRQ_TIMEOUT)
                    nextState = Ping;
                else
                    nextState = WaitForGreeting;
                break;

            // Wait for client to respond with "Hello" greeting
            case WaitForGreeting:
                //printf("\r\nwaiting for greeting");
                irqStatus = LORA_WaitForReceive(0x00, receivedHello, 3, TIMEOUT_VALUE, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR|IRQ_TIMEOUT);
                deviceID = (receivedHello[1] << 8) + receivedHello[2];

                // Greeting received with no errors and before timeout
                if(irqStatus == IRQ_RXDONE){
                    if( (receivedHello[0] == PASSWORD) && (deviceID == DEVICE_ID) ) // Check password and correct device ID
                        nextState = Transmit;
                    else
                        nextState = Ping; // Retransmit "Hello" ping
                }
                // Timeout occurred
                else if (irqStatus == IRQ_TIMEOUT){
                    //printf("\r\ntimed out waiting for greeting");
                    if (retransmitCount < GIVEUP){
                        nextState = Ping;
                        retransmitCount++;
                        //if (retransmitCount > maxRetransmit){
                        //    maxRetransmit = retransmitCount;
                        //}
                    }
                    else{
                        //printf("\r\ngiveup");
                        return 1;
                    }
                }
                // Frame received with error
                else{
                    nextState = Ping; // Retransmit "Hello" ping
                }
                break;

            // transmit data
            case Transmit:
                //printf("\r\nabout to transmit data");
                return TransmitData(data, size, startSeq);
                // if fails, will return to main, and TO UPDATE: roamer will ping again by calling Roamer_EstablishConnection
                // if successful, then return to main, and TO UPDATE: roamer will ping again by calling Roamer_EstablishConnection
        }
        currentState = nextState;
    }
}

uint8_t TransmitData(uint8_t * data, uint8_t size, uint8_t * startSeq){
    int totalPackets = 1; // delete later

    State currentState;
    State previousState;
    previousState = WaitACK1;
    State nextState;
    int count = 1;
    int retransmitCount = 0;

    if(*startSeq == 0){
        currentState = Send0;
        previousState = WaitACK1;
    }
    else{
        currentState = Send1;
        previousState = WaitACK0;
    }

    uint8_t received[3] = {0};
    uint16_t receivedID;

    // Prepare sequence values for writing into LoRa buffer
    uint8_t seq0 = 0;
    uint8_t seq1 = 1;
    //uint8_t * seq0 = &seq0Val;
    //uint8_t * seq1 = &seq1Val;

    // Transmit all packets
    while(count <= totalPackets){
        switch (currentState){
            //Send the odd numbered packets (with sequence bit 0)
            case Send0:
                //printf("\r\nin Send0");

                LORA_ImproveSensitivity();

                data[0] |= ( (seq0 << 7) & 0x80 ); // add sequence number in first bit
                LORA_WriteBuffer(0x00, data, size); // write whole packet to buffer
                //enable txdone and timeout IRQs
                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                LORA_SetTx(0x000000); // timeout disable - stay in TX mode until packet is transmitted and returns in STBY_RC mode
                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                nextState = WaitACK0;
            break;

            //data has been transmitted, need to wait for receiver to send acknowledgment
            case WaitACK0:
                //printf("\r\nin WaitACK0");
                LORA_SetRx(TIMEOUT_VALUE);
                //implicit header mode timeout bug workaround
                if(HEADER_MODE == LORA_HT_IMPLICIT){
                    LORA_ResetTimeoutCounter();
                }
                //enable rxdone, header error, crc error, timeout
                LORA_SetDioIrqParams(0x0262, 0x0000, 0x0000, 0x0000);
                //poll status register until something changes
                while( !(LORA_GetIrqStatus()) );

                //if acknowledgment is received before timeout and no errors are found, move on and send 1
                if ((LORA_GetIrqStatus()) == IRQ_RXDONE){
                    LORA_ClearIrqStatus(0x0262);
                    LORA_ReadBuffer(0x00, received, 3); //read ACK
                    receivedID = (received[1] << 8) + received[2];
                    //verify ACK and device ID
                    if ( ((received[0] & 0x80 ) == 0x00) && (receivedID == DEVICE_ID) ){
                        //printf("\r\nACK 0 with correct dev id received");
                        //if (retransmitCount > maxRetransmit){
                        //    maxRetransmit = retransmitCount;
                        //}
                        retransmitCount = 0;

                        //increment count based on a successful, acknowledged transmission
                        count++;
                        nextState = Send1;
                        break;
                    }
                    else {
                        if(receivedID != DEVICE_ID){ // if received ID incorrect, resend current frame before waiting for ACK 0
                            //printf("\r\nACK 0 with incorrect DEV ID received");
                            retransmitCount++;
                            if (retransmitCount < GIVEUP_TRANSMIT){
                                LORA_ImproveSensitivity();

                                //determine current frame based on count and re-send it
                                data[0] |= ( (seq0 << 7) & 0x80 ); // add sequence number in first bit
                                LORA_WriteBuffer(0x00, data, size);
                                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                                LORA_SetTx(0x000000);
                                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                            }
                            else return 1;
                        }

                        nextState = WaitACK0;
                        break;
                    }
                }

                //if timeout, resend current frame then wait for ACK0
                else if (LORA_GetIrqStatus() & IRQ_TIMEOUT){
                    //printf("\r\ntimed out");
                    LORA_ClearIrqStatus(0x0262);
                    if (previousState == currentState){
                        retransmitCount++;
                        //if (retransmitCount > maxRetransmit){
                        //    maxRetransmit = retransmitCount;
                        //}
                    }
                    if (retransmitCount < GIVEUP_TRANSMIT){
                        LORA_ImproveSensitivity();

                        //determine current frame based on count and re-send it
                        data[0] |= ( (seq0 << 7) & 0x80 ); // add sequence number in first bit
                        LORA_WriteBuffer(0x00, data, size);
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
                //printf("\r\nin Send1");
                LORA_ImproveSensitivity();

                //write sequence number
                data[0] |= ( (seq1 << 7) & 0x80 ); // add sequence number in first bit
                LORA_WriteBuffer(0x00, data, size);
                //enable txdone and timeout IRQs
                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                LORA_SetTx(0x000000); // timeout disable - stay in TX mode until packet is transmitted and returns in STBY_RC mode
                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone or timeout
                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                nextState = WaitACK1;
            break;

            case WaitACK1:
                //printf("\r\nin WaitACK1");
                LORA_SetRx(TIMEOUT_VALUE); //this gives a timeout duration of 1 second using formula timeout duration = timeout*15.625us
                // implement implicit header mode timeout bug workaround
                if(HEADER_MODE == LORA_HT_IMPLICIT){
                    LORA_ResetTimeoutCounter();
                }
                //enable rxdone, header error, crc error, timeout
                LORA_SetDioIrqParams(0x0262, 0x0000, 0x0000, 0x0000);
                //poll status register until something changes
                while( !(LORA_GetIrqStatus()) );

                //if acknowledgment is received before timeout and no errors are found, move on and send 0
                if ((LORA_GetIrqStatus()) == IRQ_RXDONE){
                    LORA_ClearIrqStatus(0x0262);
                    uint8_t received[1] = {0};
                    LORA_ReadBuffer(0x00, received, 3); //read ACK
                    receivedID = (received[1] << 8) + received[2];
                    //verify ACK
                    if ( ((received[0] & 0x80 ) == 0x80) && (receivedID == DEVICE_ID) ){
                        //printf("\r\nACK 1 with correct dev id received");
                        //if (retransmitCount > maxRetransmit){
                        //    maxRetransmit = retransmitCount;
                        //}
                        retransmitCount = 0;

                        count++;     //increment count based on a successful, acknowledged transmission
                        nextState = Send0;
                        break;
                    }
                    else {
                        if(receivedID != DEVICE_ID){ // if received ID incorrect, resend current frame before waiting for ACK 0
                            //printf("\r\nACK 1 with incorrect dev ID received");
                            retransmitCount++;
                            if (retransmitCount < GIVEUP_TRANSMIT){
                                LORA_ImproveSensitivity();

                                //determine current frame based on count and re-send it
                                data[0] |= ( (seq0 << 7) & 0x80 ); // add sequence number in first bit
                                LORA_WriteBuffer(0x00, data, size);
                                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                                LORA_SetTx(0x000000);
                                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
                            }
                            else return 1;
                        }

                        nextState = WaitACK1;
                        break;
                    }
                }
                //if timeout, resend current frame then wait for ACK0
                else if (LORA_GetIrqStatus() & IRQ_TIMEOUT){
                    //printf("\r\ntimed out");
                    LORA_ClearIrqStatus(0x0262);
                    if (previousState == currentState){
                        retransmitCount++;
                        //if (retransmitCount > maxRetransmit){
                        //    maxRetransmit = retransmitCount;
                        //}
                    }
                    if (retransmitCount < GIVEUP_TRANSMIT){
                        LORA_ImproveSensitivity();

                        //determine current frame based on count and re-send it
                        data[0] |= ( (seq1 << 7) & 0x80 ); // add sequence number in first bit
                        LORA_WriteBuffer(0x00, data, size);
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

    if(nextState == Send0)
        *startSeq = 0;
    else
        *startSeq = 1;

    LORA_SetStandby(STDBY_RC);


    return 0;
}

uint8_t ReceiveData(uint8_t * data, uint8_t size, uint8_t startSeq, uint16_t deviceID){
    int totalPackets = 1; //delete later

    int count = 1;
    int numErrors = 0;

    uint8_t previousSeqNum = 0x00;

    if(startSeq == 0)
        previousSeqNum = 0x80; // sequence number 1, shifted left 7 (assuming first sequence number is 0)
    else
        previousSeqNum = 0x00; // sequence number 0, shifted left 7 (assuming first sequence number is 1)

    uint8_t info[2] = {0};
    uint8_t receivedSeqNum[1] = {0}; // for sequence number
    uint16_t receivedID; // for checking source of data
    uint8_t returnACK[3] = {0};

    while (count <= totalPackets){
        // enable rxdone, header CRC, payload CRC, timeout IRQs
        LORA_SetDioIrqParams(0x0262, 0x0000, 0x0000, 0x0000);
        LORA_SetRx(GIVEUP_TIMEOUT);
        // implement implicit header mode timeout bug workaround
        if(HEADER_MODE == LORA_HT_IMPLICIT){
            LORA_ResetTimeoutCounter();
        }

        //wait for rxDone or timeout
        while( !(LORA_GetIrqStatus()) );

        //if received before timeout with no errors
        if ((LORA_GetIrqStatus()) == IRQ_RXDONE){
            //printf("\r\nreceived, no timeout, no errors");
            LORA_ClearIrqStatus(0x0262);

            //get sequence number and received device ID
            LORA_ReadBuffer(0x00, info, 2);
            *receivedSeqNum = info[0] & 0x80; // parse out sequence number
            receivedID = ((info[0] & 0x7F) << 6) + ((info[1] & 0xFC) >> 2); // parse out received device ID

            // If not a repeat and received from the correct source, save the data
            if ( (*receivedSeqNum != previousSeqNum) && (receivedID == deviceID)){
                //printf("\r\nnot a repeat");
                //read the buffer
                LORA_ReadBuffer(0x00, data, size);
                previousSeqNum = *receivedSeqNum; // parse out sequence number

                count++;
            }
            else{
                //printf("\r\nrepeat");
                numErrors++;
            }

            if(receivedID == deviceID){
                LORA_ImproveSensitivity();

                //send ACK whether repeat or not
                returnACK[0] = *receivedSeqNum;
                returnACK[1] = (uint8_t) ((deviceID & 0xFF00) >> 8);
                returnACK[2] = (uint8_t) (deviceID & 0x00FF);
                LORA_WriteBuffer(0x00, returnACK , 3);
                LORA_SetDioIrqParams(0x0201, 0x0000, 0x0000, 0x0000);
                LORA_SetTx(0x000000);
                while( !(LORA_GetIrqStatus()) ); // wait for IRQ txdone
                LORA_ClearIrqStatus(0x0201); // clear IRQ txdone/timeout flag
            }
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
