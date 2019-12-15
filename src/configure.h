#ifndef CONFIGURE_H_
#define CONFIGURE_H_

/*
 * USER CONFIGURATIONS FOR DEVICE
 */
#define DEVICE_ID           35                   // Device ID, range: 0-8191

/*
 * USER CONFIGURATIONS FOR LORA
 */
#define CENTER_FREQUENCY    915000000           // Center frequency, units: Hz
#define SPREADING_FACTOR    LORA_SF12           // Spreading factor, options: LORA_SFx for x=5-12
#define CODING_RATE         LORA_CR_4_5         // Coding rate, options:LORA_CR_4_x for x=5-8
#define LDR_OPT_ENABLE      LORA_OPT_OFF        // Low data rate optimization, options: LORA_OPT_ON, LORA_OPT_OFF
#define BANDWIDTH           LORA_BW_500         // Bandwidth, options: LORA_BW_x for x=7, 10, 15, 20, 31, 41, 62, 125, 250, 500
#define HEADER_MODE         LORA_HT_IMPLICIT    // Header mode, options: LORA_HT_EXPLICIT, LORA_HT_IMPLICIT
#define IQ_MODE             LORA_IQ_STANDARD    // IQ mode, options: LORA_IQ_STANDARD,LORA_IQ_INVERTED
#define PREAMBLE_LENGTH     12                  // Preamble length, range: 10-65535
#define PAYLOAD_LENGTH      15                  // Payload length, range: 0-254
#define CRC_ENABLE          LORA_CRC_ON         // CRC enable, options: LORA_CRC_ON, LORA_CRC_OFF

#endif /* CONFIGURE_H_ */
