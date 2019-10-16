#include "protocol.h"

typedef enum {Listen, Greet, WaitForDirections, SendToClient, ReceiveFromClient, ConfirmHostToClientTransfer} HostState; // handshake
typedef enum {Ping, WaitForGreeting, GiveDirections, SendToHost, ReceiveFromHost} ClientState; // handshake
typedef enum {Send0, WaitACK0, Send1, WaitACK1} State; // data transfer

extern int maxRetransmit; // handshake
extern int maxRetransmit; // data transfer
extern int errorCount; // data transfer

int8_t Home_WaitForConnection(uint8_t * userID, uint32_t * sizes, uint32_t * EEPROMaddresses, uint32_t * FLASHaddresses, uint8_t * numFiles){
    uint8_t i;

    uint8_t receivedCode[1] = {0};
    uint8_t receivedSize[MAX_FILE_SIZE_BYTES+MAX_USERID_BYTES] = {0};
    uint8_t receivedID[MAX_USERID_BYTES] = {0};
    uint8_t sendCode[1] = {0};
    uint8_t setInfo[3] = {0};
    uint8_t sendCodeAndNumFiles[2+MAX_FILE_SIZE_BYTES*MAX_FILES_STORED] = {0}; // code, numfiles, and 4-byte size for each file
    uint16_t irqStatus;

    HostState currentState = Listen;
    HostState nextState;

    int retransmitCount = 0;

    while(1){
        switch (currentState){

        // Wait for client to send "Hello" ping
        case Listen:
            irqStatus = LORA_WaitForReceive(0x00, receivedCode, 1, 0, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR);

            // Ping received with no errors, and password is correct
            if( (irqStatus == IRQ_RXDONE) && (*receivedCode == PASSWORD) ){
                nextState = Greet; // Continue with conversation
            }
            // Ping received with errors password is incorrect
            else{
                nextState = Listen; // Keep Listening
            }
            break;

        // Send "Hello" response to client
        case Greet:
            sendCode[0] = PASSWORD;
            LORA_TransmitAndWait(0x00, sendCode, 1, 0, IRQ_TXDONE);

            nextState = WaitForDirections; // Continue with conversation
            break;

        // Wait for client to specify transmission or reception
        case WaitForDirections:
            irqStatus = LORA_WaitForReceive(0x00, receivedCode, 1, GIVEUP_TIMEOUT, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR|IRQ_TIMEOUT);

            // Directive received with no errors and before timeout
            if(irqStatus == IRQ_RXDONE){
                if(*receivedCode == CLIENT_TO_HOST)
                    nextState = ReceiveFromClient;
                else if (*receivedCode == HOST_TO_CLIENT)
                    nextState = SendToClient;
                else
                    nextState = WaitForDirections; // Wait for retransmission of directions
            }
            else if (irqStatus == IRQ_TIMEOUT){
                return HANDSHAKE_FAILED;
            }
            // Frame received with error
            else{
                nextState = WaitForDirections; // Wait for retransmission of directions
            }
            break;

        //if the user ID matches a file on the host, move on to confirm host to client transfer
        case SendToClient:
            // Get user ID
            LORA_ReadBuffer(0x01, receivedID, MAX_USERID_BYTES+1);
            userID[0] = receivedID[0];

            Flash_ReadArray(INFOA_START, setInfo, 3);
            setInfo[2] = receivedID[1];
            Flash_WriteArray(INFOA_START, setInfo, 3);    // 1 to delete, 0 to save

            // Check if you have any files from user ID; assume user ID of 0 or 255 means that no file is in the array located at this address
            numFiles[0] = Flash_SearchForUID(userID[0], EEPROMaddresses, sizes, FLASHaddresses); // also fills EEPROMaddresses, sizes, and FLASHaddresses for use in main
            if( numFiles[0] ){
                //send a message to the client containing: the OK that the file is there, the number of files that exist under that userID (1), the size of each file
                sendCodeAndNumFiles[0] = FILES_AVIALABLE;
                sendCodeAndNumFiles[1] = numFiles[0];

                // fill size bits in sendCodeAndNumFiles
                for(i = 0; i < numFiles[0]; i++){
                    deconstructToArray(sizes[i], sendCodeAndNumFiles, i*4+2, 4);
                }

                LORA_TransmitAndWait(0x00, sendCodeAndNumFiles, 2+MAX_FILE_SIZE_BYTES*numFiles[0], 0, IRQ_TXDONE);
                nextState = ConfirmHostToClientTransfer;
            }
            else{ // no files available
                *sendCode = NO_FILES;
                LORA_TransmitAndWait(0x00, sendCode, 1, 0, IRQ_TXDONE);
                return FILE_UNAVAILABLE;
            }

        //if the client sends OK, the host can exit this function and move on to send the file
        case ConfirmHostToClientTransfer:
            irqStatus = LORA_WaitForReceive(0x00, receivedCode, 1, TIMEOUT_VALUE, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR|IRQ_TIMEOUT);

            // Greeting received with no errors and before timeout
            if(irqStatus == IRQ_RXDONE){
                if(*receivedCode == OK)
                    return HOST_SEND;
                else
                    nextState = SendToClient; // Retransmit yes/no
            }
            // Frame received with error or timeout occurred
            else{
                if (retransmitCount < GIVEUP){
                    nextState = SendToClient;
                    retransmitCount++;
                    if (retransmitCount > maxRetransmit){
                        maxRetransmit = retransmitCount;
                    }
                }
                else
                    return HOST_SEND;   // assume no problems, but that host has already closed connection; if problems, they will be caught in file transaction
            }
            return HOST_SEND;

        //send OK to tell the client to move on and transfer the file
        case ReceiveFromClient:
            // Get size as a single number
            LORA_ReadBuffer(0x01, receivedSize, MAX_FILE_SIZE_BYTES+MAX_USERID_BYTES);
            sizes[0] = reconstructTo32Bits(receivedSize, 0, 4);
            userID[0] = receivedSize[4]; // Get user ID

            // Send "OK" back to client
            receivedCode[0] = OK;
            LORA_TransmitAndWait(0x00, receivedCode, 1, 0, IRQ_TXDONE);

            return CLIENT_SEND;
        }

        currentState = nextState;
    }
}

int8_t Roamer_EstablishConnection(uint8_t transmit, uint32_t * size, uint8_t * userID, uint8_t * numFiles){
    int i;
    uint8_t receivedCode[1] = {0};
    uint8_t sendCode[1] = {0};
    uint8_t sendCodeAndSize[2+MAX_FILE_SIZE_BYTES*MAX_FILES_STORED] = {0};
    uint8_t sendCodeAndUserID[3] = {0};
    uint16_t irqStatus;

    uint8_t flags[1] = {0};
    Flash_ReadArray(INFOA_START, flags, 1);

    ClientState currentState = Ping;
    ClientState nextState;

    int retransmitCount = 0;

    while(1){
        switch(currentState){

        // Ping host with password
        case Ping:
            sendCode[0] = PASSWORD;
            LORA_TransmitAndWait(0x00, sendCode, 1, 0, IRQ_TXDONE);

            nextState = WaitForGreeting;

            break;

        // Wait for client to respond with "Hello" greeting
        case WaitForGreeting:
            irqStatus = LORA_WaitForReceive(0x00, receivedCode, 1, TIMEOUT_VALUE, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR|IRQ_TIMEOUT);

            // Greeting received with no errors and before timeout
            if(irqStatus == IRQ_RXDONE){
                if(*receivedCode == PASSWORD)
                    nextState = GiveDirections;
                else
                    nextState = Ping; // Retransmit "Hello" ping
            }
            // Timeout occurred
            else if (irqStatus == IRQ_TIMEOUT){
                if (retransmitCount < GIVEUP){
                    nextState = Ping;
                    retransmitCount++;
                    if (retransmitCount > maxRetransmit){
                        maxRetransmit = retransmitCount;
                    }
                }
                else
                    return HANDSHAKE_FAILED;
            }
            // Frame received with error
            else{
                nextState = Ping; // Retransmit "Hello" ping
            }
            break;

        // Send client a "transmit" or "receive" directive
        case GiveDirections:

            //if the client is transmitting, send the size of the file for the host to receive in 4 bytes along with the sender's user ID
            if(transmit){
                sendCodeAndSize[0] = CLIENT_TO_HOST;
                deconstructToArray(size[0], sendCodeAndSize, 1, 4);
                sendCodeAndSize[5] = userID[0];
                LORA_TransmitAndWait(0x00, sendCodeAndSize, 6, 0, IRQ_TXDONE);
                nextState = SendToHost;
            }

            //if the client is receiving from the host, send the userID to receive from and whether or not to delete the file after reception
            else{
                sendCodeAndUserID[0] = HOST_TO_CLIENT;
                sendCodeAndUserID[1] = userID[0];
                sendCodeAndUserID[2] = ( (flags[0] & DELETE_FLAG) == DELETE_FLAG );
                LORA_TransmitAndWait(0x00, sendCodeAndUserID, 3, 0, IRQ_TXDONE);
                nextState = ReceiveFromHost;
            }
            break;

        //if the OK is received, the client can exit this state and move on to transmitting
        case SendToHost:
            irqStatus = LORA_WaitForReceive(0x00, receivedCode, 1, TIMEOUT_VALUE, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR|IRQ_TIMEOUT);

            // Greeting received with no errors and before timeout
            if(irqStatus == IRQ_RXDONE){
                if(*receivedCode == OK)
                    return CLIENT_SEND;
                else
                    nextState = GiveDirections; // Retransmit directive
            }
            // Frame received with error or timeout occurred
            else{
                if (retransmitCount < GIVEUP){
                    nextState = GiveDirections;
                    retransmitCount++;
                    if (retransmitCount > maxRetransmit){
                        maxRetransmit = retransmitCount;
                    }
                }
                else
                    return CLIENT_SEND;   // assume no problems, but that host has already closed connection; if problems, they will be caught in file transaction
            }
            break;

        //if the file with requested userID exists, client can move on receiving from the host
        case ReceiveFromHost:
            irqStatus = LORA_WaitForReceive(0x00, sendCodeAndSize, 2+MAX_FILE_SIZE_BYTES*MAX_FILES_STORED, TIMEOUT_VALUE, IRQ_RXDONE|IRQ_HEADER_ERR|IRQ_CRC_ERR|IRQ_TIMEOUT);

            // Confirmation received with no errors and before timeout
            if(irqStatus == IRQ_RXDONE){
                if(sendCodeAndSize[0] == NO_FILES)
                    return FILE_UNAVAILABLE;
                else if(sendCodeAndSize[0] == FILES_AVIALABLE){
                    //store the information
                    numFiles[0] = sendCodeAndSize[1];   // will change - this is total number of files; doesn't differentiate between user IDs
                    for(i = 0; i < numFiles[0]; i++){
                        size[i] = reconstructTo32Bits(sendCodeAndSize, 2+MAX_FILE_SIZE_BYTES*i, 4);
                    }

                    receivedCode[0] = OK;
                    LORA_TransmitAndWait(0x00, receivedCode, 1, 0, IRQ_TXDONE);

                    return HOST_SEND;
                }
                else
                    nextState = GiveDirections; // Retransmit directive
            }
            // Frame received with error or timeout occurred
            else{
                if (retransmitCount < GIVEUP){
                    nextState = GiveDirections;
                    retransmitCount++;
                    if (retransmitCount > maxRetransmit){
                        maxRetransmit = retransmitCount;
                     }
                }
                else
                    return HANDSHAKE_FAILED;
                }
        }

        currentState = nextState;
    }

}

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
    //Memory_ReadFromArray(currentAddress + 2, bufData, MAX_PAGE_SIZE - 2);

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
                        //if (count == totalPackets) Memory_ReadFromArray(currentAddress + 2, bufData, bytesInExtraPacket);
                        //else Memory_ReadFromArray(currentAddress + 2, bufData, MAX_PAGE_SIZE - 2);

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
                        //if (count == totalPackets) Memory_ReadFromArray(currentAddress + 2, bufData, bytesInExtraPacket);
                        //else Memory_ReadFromArray(currentAddress + 2, bufData, MAX_PAGE_SIZE - 2);

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

                //if (count == totalPackets) Memory_WriteToArray(currentAddress + 2, bufData, bytesInExtraPacket);
                //Memory_WriteToArray(currentAddress + 2, bufData, MAX_PAGE_SIZE -2);

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
