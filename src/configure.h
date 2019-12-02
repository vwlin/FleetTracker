#ifndef CONFIGURE_H_
#define CONFIGURE_H_

/*
 * USER CONFIGURATIONS FOR DEVICE
 */
#define DEVICE_ID           2                   // device ID, range: 0-8191

/*
 * USER CONFIGURATIONS FOR LORA
 */
#define CENTER_FREQUENCY    915000000           // center frequency, units: Hz
#define SPREADING_FACTOR    LORA_SF12           // spreading factor, options: LORA_SFx for x=5-12
#define CODING_RATE         LORA_CR_4_5         // coding rate, options:LORA_CR_4_x for x=5-8
#define LDR_OPT_ENABLE      LORA_OPT_OFF        // low data rate optimization, options: LORA_OPT_ON, LORA_OPT_OFF
#define BANDWIDTH           LORA_BW_500         // bandwidth, options: LORA_BW_x for x=7, 10, 15, 20, 31, 41, 62, 125, 250, 500
#define HEADER_MODE         LORA_HT_IMPLICIT    // header mode, options: LORA_HT_EXPLICIT, LORA_HT_IMPLICIT
#define IQ_MODE             LORA_IQ_STANDARD    // IQ mode, options: LORA_IQ_STANDARD,LORA_IQ_INVERTED
#define PREAMBLE_LENGTH     12                  // preamble length, range: 10-65535
#define PAYLOAD_LENGTH      50                  // payload length, range: 0-254
#define CRC_ENABLE          LORA_CRC_ON         // CRC enable, options: LORA_CRC_ON, LORA_CRC_OFF

#endif /* CONFIGURE_H_ */
