#include "node.h"

typedef enum {Listen, Greet, WaitForDirections, SendToClient, ReceiveFromClient, ConfirmHostToClientTransfer} HostState;
typedef enum {Ping, WaitForGreeting, GiveDirections, SendToHost, ReceiveFromHost} ClientState;

extern int maxRetransmit;

int8_t Host_WaitForConnection(uint8_t * userID, uint32_t * sizes, uint32_t * EEPROMaddresses, uint32_t * FLASHaddresses, uint8_t * numFiles){
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

int8_t Client_EstablishConnection(uint8_t transmit, uint32_t * size, uint8_t * userID, uint8_t * numFiles){
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
