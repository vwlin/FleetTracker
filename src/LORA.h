#ifndef LORA_H_
#define LORA_H_

#include <msp430.h>
#include <stdint.h>

#include "SPI.h"

/*
 * USER CONFIGURATIONS
 */
#define HEADER_MODE     LORA_HT_IMPLICIT    // LORA_HT_EXPLICIT, LORA_HT_IMPLICIT

/*
 * Constants
 * https://github.com/Lora-net/LoRaMac-node/blob/develop/src/radio/sx126x/sx126x.h
 */
#define XTAL_FREQ                   (double)32000000
#define FREQ_DIV                    (double)33554432        // 2^25
#define FREQ_STEP                   (double)(XTAL_FREQ/FREQ_DIV)
#define SIZE_24_BIT_MAX_VAL         16777215
#define MAX_BUFFER_SIZE             255                     // in bytes
#define RESOLUTION
#define DELAY

/*
 * Masks
 */
#define LORA_STATUS_CHIP_MODE       0x70//BIT4|BIT5|BIT6
#define IRQ_TXDONE                  0x0001
#define IRQ_RXDONE                  0x0002
#define IRQ_PREAMBLE_DETECTED       0x0004
#define IRQ_SYNC_WORD_VALID         0x0008
#define IRQ_HEADER_VALID            0x0010
#define IRQ_HEADER_ERR              0x0020
#define IRQ_CRC_ERR                 0x0040
#define IRQ_CAD_DONE                0x0080
#define IRQ_CAD_DETECTED            0x0100
#define IRQ_TIMEOUT                 0x0200

/*
 * USCIA0 Slave Transmit Enable (STE)
 * GPIO : P2.3
 */
#define USCIA0_STE_BIT              BIT3
#define USCIA0_STE_PORT             P2OUT
#define USCIA0_STE_DDR              P2DIR
#define SET_ENABLE_AS_OUTPUT        USCIA0_STE_DDR |= USCIA0_STE_BIT
#define ENABLE_LORA                 USCIA0_STE_PORT &= ~USCIA0_STE_BIT // active low
#define DISABLE_LORA                USCIA0_STE_PORT |= USCIA0_STE_BIT // active low

/*
 * LORA Busy Line
 * GPIO: P3.7
 */
#define LORA_BUSY_BIT               BIT7
#define LORA_BUSY_PORT              P3IN
#define LORA_BUSY_DDR               P3DIR
#define SET_BUSY_AS_INPUT           LORA_BUSY_DDR &= ~LORA_BUSY_BIT
#define SET_BUSY_PULLUP_1           P3OUT |= BIT7
#define SET_BUSY_PULLUP_2           P3REN |= BIT7

/*
 * NRESET Pin
 * GPIO: P3.5
 */
#define LORA_NRESET_BIT             BIT5
#define LORA_NRESET_PORT            P3OUT
#define LORA_NRESET_DDR             P3DIR
#define SET_NRESET_AS_OUTPUT        LORA_NRESET_DDR |= LORA_NRESET_BIT
#define RESET_NRESET                LORA_NRESET_PORT &= ~LORA_NRESET_BIT
#define SET_NRESET                  LORA_NRESET_PORT |= LORA_NRESET_BIT

/*
 * ANT_SW Pin
 * GPIO: P1.4
 */
#define LORA_ANT_SW_BIT             BIT4
#define LORA_ANT_SW_PORT            P1OUT
#define LORA_ANT_SW_DDR             P1DIR
#define SET_ANT_SW_AS_OUTPUT        LORA_ANT_SW_DDR |= LORA_ANT_SW_BIT
#define SET_ANT_SW                  LORA_ANT_SW_PORT |= LORA_ANT_SW_BIT
#define RESET_ANT_SW                LORA_ANT_SW_PORT &= ~LORA_ANT_SW_BIT

/*
 * Opcodes for LORA chip commands
 */
#define NOP                         (0x00)
#define SET_SLEEP                   (0x84)
#define SET_STANDBY                 (0x80)
#define SET_FS                      (0XC1)
#define SET_TX                      (0x83)
#define SET_RX                      (0x82)
#define STOP_TIMER_ON_PREAMBLE      (0x9F)
#define SET_RX_DUTY_CYCLE           (0x94)
#define SET_CAD                     (0xC5)
#define SET_TX_CONTINUOUS_WAVE      (0xD1)
#define SET_TX_INFINITE_PREAMBLE    (0xD2)
#define SET_REGULATOR_MODE          (0x96)
#define CALIBRATE                   (0x89)
#define CALIBRATE_IMAGE             (0x98)
#define SET_PA_CONFIG               (0x95)
#define SET_FALLBACK_MODE           (0x93)
#define WRITE_REGISTER              (0x0D)
#define READ_REGISTER               (0x1D)
#define WRITE_BUFFER                (0x0E)
#define READ_BUFFER                 (0x1E)
#define SET_DIO_IRQ_PARAMS          (0x08)
#define GET_IRQ_STATUS              (0x12)
#define CLEAR_IRQ_STATUS            (0x02)
#define SET_DIO2_MODE               (0x9D)
#define SET_DIO3_MODE               (0x97)
#define SET_RF_FREQUENCY            (0x86)
#define SET_PACKET_TYPE             (0x8A)
#define GET_PACKET_TYPE             (0x11)
#define SET_TX_PARAMS               (0x8E)
#define SET_MODULATION_PARAMS       (0x8B)
#define SET_PACKET_PARAMS           (0x8C)
#define SET_CAD_PARAMS              (0x88)
#define SET_BUFFER_BASE_ADDR        (0x8F)
#define SET_LORA_SYMB_NUM_TIMEOUT   (0xA0)
#define GET_STATUS                  (0xC0)
#define GET_RX_BUFFER_STATUS        (0x13)
#define GET_PACKET_STATUS           (0x14)
#define GET_RSSI_INST               (0x15)
#define GET_STATS                   (0x10)
#define RESET_STATS                 (0x00)
#define GET_DEVICE_ERRORS           (0x17)
#define CLEAR_DEVICE_ERRORS         (0x07)

/*
 * Parameters for LORA chip commands
 */

// LORA_SetStandby
#define STDBY_RC                    0       // run on RC13M, 13MHz
#define STDBY_XOSC                  1       // run on XTAL, 32MHz

// LORA_StopTimerOnPreamble
#define STOP_ON_SW_HEADER           (0x00)  // timer is stopped upon Sync Word or Header detection
#define STOP_ON_PREAMBLE            (0x01)  // timer is stopped upon preamble detection

// LORA_SetRegulatorMode
#define REGULATOR_LDO               0       // use only LDO for all modes
#define REGULATOR_DCDC_LDO          1       // use DC_DC+LDO for STBY_XOSC, FS, RX, TX modes

// LORA_SetPaConfig
#define PA_CONFIG_1262              0       // select SX1262
#define PA_CONFIG_1261              1       // select SX1261
#define PA_CONFIG_PALUT             (0x01)  // reserved and always 0x01

// LORA_SetRxTxFallbackMode
#define STATUS_MODE_STBYRC          (0x20)  // standby RC mode
#define STATUS_MODE_STBYXOSC        (0x30)  // standby XOSC mode
#define STATUS_MODE_FS              (0x40)  // FS mode

// LORA_SetDIO2AsRfSwitchCtrl
#define DIO2_AS_IRQ                 0       // DIO2 is free to be used as an IRQ
#define DIO2_AS_SWITCH              1       // DIO2 is to be used to control an RF switch (1 in TX mode, else 0)

// LORA_SetDIO3AsTCXOCtrl
#define SUPPLY_16                   (0x00)  // DIO3 outputs 1.6 V to supply the TCXO
#define SUPPLY_17                   (0x01)  // DIO3 outputs 1.7 V to supply the TCXO
#define SUPPLY_18                   (0x02)  // DIO3 outputs 1.8 V to supply the TCXO
#define SUPPLY_22                   (0x03)  // DIO3 outputs 2.2 V to supply the TCXO
#define SUPPLY_24                   (0x04)  // DIO3 outputs 2.4 V to supply the TCXO
#define SUPPLY_27                   (0x05)  // DIO3 outputs 2.7 V to supply the TCXO
#define SUPPLY_30                   (0x06)  // DIO3 outputs 3.0 V to supply the TCXO
#define SUPPLY_33                   (0x07)  // DIO3 outputs 3.3 V to supply the TCXO

// LORA_SetPacketType
#define PACKET_TYPE_GFSK            (0x00)  // GFSK packet type
#define PACKET_TYPE_LORA            (0x01)  // LORA mode

// LORA_SetTxParams
#define SET_RAMP_10U                (0x00)  // 10 us ramp time
#define SET_RAMP_20U                (0x01)  // 20 us ramp time
#define SET_RAMP_40U                (0x02)  // 40 us ramp time
#define SET_RAMP_80U                (0x03)  // 80 us ramp time
#define SET_RAMP_200U               (0x04)  // 200 us ramp time
#define SET_RAMP_800U               (0x05)  // 800 us ramp time
#define SET_RAMP_1700U              (0x06)  // 1700 us ramp time
#define SET_RAMP_3400U              (0x07)  // 3400 us ramp time

// LORA_SetModulationParams
#define GFSK_PS_NONE                (0x00)  // no filter applied
#define GFSK_PS_03                  (0x08)  // gaussian BT 0.3
#define GFSK_PS_05                  (0x09)  // gaussian BT 0.5
#define GFSK_PS_07                  (0x0A)  // gaussian BT 0.7
#define GFSK_PS_10                  (0x0B)  // gaussian BT 1
#define GFSK_BW_4800                (0X1F)  // 4.8 kHz DSB
#define GFSK_BW_5800                (0x17)  // 5.8 kHz DSB
#define GFSK_BW_7300                (0x0F)  // 7.3 kHz DSB
#define GFSK_BW_9700                (0x1E)  // 9.7 kHz DSB
#define GFSK_BW_11700               (0x16)  // 11.7 kHz DSB
#define GFSK_BW_14600               (0x0E)  // 14.6 kHz DSB
#define GFSK_BW_19500               (0x1D)  // 19.5 kHz DSB
#define GFSK_BW_23400               (0x15)  // 23.4 kHz DSB
#define GFSK_BW_29300               (0x0D)  // 29.3 kHz DSB
#define GFSK_BW_39000               (0x1C)  // 39 kHz DSB
#define GFSK_BW_46900               (0x14)  // 46.9 kHz DSB
#define GFSK_BW_58600               (0x0C)  // 58.6 kHz DSB
#define GFSK_BW_78200               (0x1B)  // 78.2 kHz DSB
#define GFSK_BW_93800               (0x13)  // 93.8 kHz DSB
#define GFSK_BW_117300              (0x0B)  // 117.3 kHz DSB
#define GFSK_BW_156200              (0x1A)  // 156.2 kHz DSB
#define GFSK_BW_187200              (0x12)  // 187.2 kHz DSB
#define GFSK_BW_234300              (0x0A)  // 232.3 kHz DSB //???? typo in DS?
#define GFSK_BW_312000              (0x19)  // 312 kHz DSB
#define GFSK_BW_373600              (0x11)  // 373.6 kHz DSB
#define GFSK_BW_46700               (0x09)  // 467 kHz DSB
#define LORA_SF5                    (0x05)
#define LORA_SF6                    (0x06)
#define LORA_SF7                    (0x07)
#define LORA_SF8                    (0x08)
#define LORA_SF9                    (0x09)
#define LORA_SF10                   (0x0A)
#define LORA_SF11                   (0x0B)
#define LORA_SF12                   (0x0C)
#define LORA_BW_7                   (0x00)  // 7.81 kHz real
#define LORA_BW_10                  (0x08)  // 10.42 kHz real
#define LORA_BW_15                  (0x01)  // 15.63 kHz real
#define LORA_BW_20                  (0x09)  // 20.83 kHz real
#define LORA_BW_31                  (0x02)  // 31.25 kHz real
#define LORA_BW_41                  (0x0A)  // 41.67 kHz real
#define LORA_BW_62                  (0x03)  // 62.50 kHz real
#define LORA_BW_125                 (0x04)  // 125 kHz real
#define LORA_BW_250                 (0x05)  // 250 kHz real
#define LORA_BW_500                 (0x06)  // 500 kHz real
#define LORA_CR_4_5                 (0x01)
#define LORA_CR_4_6                 (0x02)
#define LORA_CR_4_7                 (0x03)
#define LORA_CR_4_8                 (0x04)
#define LORA_OPT_ON                 (0x00)  // low data rate optimize on
#define LORA_OPT_OFF                (0x01)  // low data rate optimize off

// LORA_SetPacketParams
#define GFSK_PDL_OFF                (0x00)  // preamble detector length off
#define GFSK_PDL_8                  (0x04)  // preamble detector length 8 bits
#define GFSK_PDL_16                 (0x05)  // preamble detector length 16 bits
#define GFSK_PDL_24                 (0x06)  // preamble detector length 24 bits
#define GFSK_PDL_32                 (0x07)  // preamble detector length 32 bits
#define GFSK_AC_DISABLED            (0x00)  // address filtering disabled
#define GFSK_AC_NODE                (0x01)  // address filtering activated on node address
#define GFSK_AC_NODE_BROADCAST      (0x02)  // address filtering activated on node and broadcast addresses
#define GFSK_PT_LEN_KNOWN           (0x00)  // packet length known on both sides, not added to packet
#define GFSK_PT_LEN_VARIABLE        (0x01)  // packet on variable size, first byte of payload is packet size
#define GFSK_CRC_OFF                (0x01)  // no CRC
#define GFSK_CRC_1_BYTE             (0x00)  // CRC computed on 1 byte
#define GFSK_CRC_2_BYTE             (0x02)  // CRC computed on 2 bytes
#define GFSK_CRC_1_BYTE_INV         (0x04)  // CRC computed on 1 byte and inverted
#define GFSK_CRC_2_BYTE_INV         (0x06)  // CRC computed on 2 bytes and inverted
#define GFSK_W_OFF                  (0x00)  // no encoding
#define GFSK_W_ENABLE               (0x01)  // whitening enable
#define LORA_HT_EXPLICIT            (0x00)  // variable length packet (explicit header)
#define LORA_HT_IMPLICIT            (0x01)  // fixed length packet (implicit header)
#define LORA_CRC_OFF                (0x00)
#define LORA_CRC_ON                 (0x01)
#define LORA_IQ_STANDARD            (0x00)  // standard IQ setup
#define LORA_IQ_INVERTED            (0x01)  // inverted IQ setup

// LORA_SetCadParams
#define CAD_ON_1_SYMB               (0x00)  // 1 symbol used for CAD
#define CAD_ON_2_SYMB               (0x01)  // 2 symbols used for CAD
#define CAD_ON_4_SYMB               (0x02)  // 4 symbols used for CAD
#define CAD_ON_8_SYMB               (0x03)  // 8 symbols used for CAD
#define CAD_ON_16_SYMB              (0x04)  // 16 symbols used for CAD
#define CAD_ONLY                    (0x00)  // exit mode is STBY_RC mode
#define CAD_RX                      (0x01)  // exit mode is RX mode until a packet is detected or timeout is reached

/*
 * Configure_LORA();
 * Configure Chip Select, ANT_SW output and Busy input
 */
void Configure_LORA();


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * OPERATIONAL MODES FUNCTIONS
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * LORA_SetSleep
 * Set the device in SLEEP mode with the lowest current consumption possible
 *
 * Parameters:
 *  sleepConfig - see SX126X datasheet page 66 for configuration codes
 *
 * Returns 1 if successful, else 0
 */
uint8_t LORA_SetSleep(uint8_t sleepConfig);

/*
 * LORA_SetStandby
 * Set device in a configuration mode which is at an intermediate level of consumption
 *
 * Parameters:
 *  standbyConfig - STDBY_RC or STDBY_XOSC
 */
void LORA_SetStandby(uint8_t standbyConfig);

/*
 * LORA_SetFs
 * Set the device in the frequency synthesis mode where the PLL is locked to the
 * carrier frequency
 */
void LORA_SetFs();

/*
 * LORA_SetTx
 * Set the device in transmit mode
 *
 * Parameters:
 *  timeout - 24 bits, defines the period by which the last bit of the packet must be sent;
 *          function fails if value > 16777215
 *
 * Returns 1 if successful; else 0
 *
 * see SX126X datasheet page 68 for timeout calculation and meaning
 */
uint8_t LORA_SetTx(uint32_t timeout);

/*
 * LORA_SetRx
 * Set the device in receiver mode
 *
 * Parameters:
 *  timeout - 24 bits, defines the period by which the preamble+Sync Word (GFSK) or Header (LoRa)
 *          must be detected; function fails if value > 16777215
 *
 * Returns 1 if successful; else 0
 *
 * see SX126X datasheet page 69 for timeout meaning
 */
uint8_t LORA_SetRx(uint32_t timeout);

/*
 * LORA_StopTimerOnPreamble
 * Select if timer is stopped upon preamble detection of Sync Word / header detection
 *
 * Parameters:
 *  stopConfig: STOP_ON_SW_HEADER to stop on Sync Word or Header detection; STOP_ON_PREAMBLE to
 *          stop on preamble detection
 */
void LORA_StopTimerOnPreamble(uint8_t stopConfig);

/*
 * LORA_SetRxDutyCycle
 * Set the chip in sniff mode so that it regularly looks for new packets (listen mode)
 * When called in STDBY_RC mode, the context is saved and the enters the sniffing loop
 *
 * Parameters:
 *  rxPeriod - 24-bit period of time that the chip enters RX mode and listens for a packet
 *  sleepPeriod - 24-bit period of time that the chip enters SLEEP mode
 *          function fails if either value > 16777215
 *
 * Returns 1 if successful, else 0
 */
uint8_t LORA_SetRxDutyCycle(uint32_t rxPeriod, uint32_t sleepPeriod);

/*
 * LORA_SetCAD
 * Device searches for presence of LoRa preamble signal, then returns to STDBY_RC mode
 *
 * Returns 1 if successful, else 0
 */
uint8_t LORA_SetCAD();

/* TEST COMMAND
 * LORA_SetTxContinuousWave
 * Generate a continuous wave (RF tone) at selected frequency and output power
 */
void LORA_SetTxContinuousWave();

/* TEST COMMAND
 * LORA_SetTxInfinitePreamble
 * Generate an infinite sequence of alternating zeros and ones in FSK modulation
 */
void LORA_SetTxInfinitePreamble();

/* CHECK HARDWARE IMPLEMENTATION BEFORE USING
 * LORA_SetRegulatorMode
 * Specify if DC-DC or LDO (default) is used for power regulation
 *
 * Parameters:
 *  regulatorConfig - REGULATOR_LDO to use only LDO for all modes; REGULATOR_DCDC_LDO to use DC_DC+LDO for STBY_XOSC, FS, RX, TX modes
 */
void LORA_SetRegulatorMode(uint8_t regulatorConfig);

/*
 * LORA_Calibrate
 * Launch a calibration of one or several blocks at any time starting in STDBY_RC mode
 *
 * Parameters:
 *  calibParam - see SX126X datasheet page 74 for calibration codes
 *
 * Returns 1 if successful, else 0
 */
uint8_t LORA_Calibrate(uint8_t calibParam);

/*
 * LORA_CalibrateImage
 * Allows the user to calibrate the image rejection of the device for the device operating frequency band
 *
 * Parameters:
 *  freq1 - ??
 *  freq2 - ??
 */
void LORA_CalibrateImage(uint8_t freq1, uint8_t freq2);

/*
 * LORA_SetPaConfig
 * Select the power amplifier to be used by the device and its configuration
 *
 * Parameters:
 *  paDutyCycle - controls duty cycle (conduction angle) of power amplifier
 *  hpMax - selects the size of the power amplifier in the SX1262
 *  deviceSel - select device using PA_CONFIG_1261 or PA_CONFIG_1262
 *
 * See SX126X datasheet pf 75 for cautions about settings
 */
void LORA_SetPaConfig(uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel);

/*
 * LORA_SetRxTxFallbackMode
 * Define which mode the chip goes into after a successful transmission or packet reception
 *
 * Parameters:
 *  modeConfig - select FS, standby XOSC, or standby RC mode using STATUS_MODE_FS, STATUS_MODE_STBYXOSC, or STATUS_MODE_STBYRC
 *
 * Returns 1 if successful, else 0
 */
uint8_t LORA_SetRxTxFallbackMode(uint8_t modeConfig);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * REGISTERS AND BUFFER ACCESS
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * LORA_WriteRegister
 * Write 16 bits starting from the location at the 16-bit long address
 *
 * Parameters:
 *  address - 16 bit starting address for writing data block
 *  data - 16 bits of data to be written to the register
 *  size - size of data array
 */
void LORA_WriteRegister(uint16_t address, uint8_t * data, uint8_t size);

/*
 * LORA_ReadRegister
 * Read 16 bits starting from the location at the 16-bit long address
 *
 * Parameters:
 *  address - 16 bit starting address for reading data block
 *  buffer - array to be filled with read data
 *  size - size of data array
 */
void LORA_ReadRegister(uint16_t address, uint8_t * buffer, uint8_t size);

/*
 * LORA_WriteBuffer
 * Store data 16-bit payload to be transmitted
 *
 * Parameters:
 *  offset - defines the address pointer of the first data to be written
 *  data - 16-bit data payload to be transmitted
 *  size - size of data array
 */
void LORA_WriteBuffer(uint8_t offset, uint8_t * data, uint8_t size);

/*
 * LORA_ReadBuffer
 * Read 16-bit payload
 *
 * Parameters:
 *  offset - read payload starting from this offset
 *  buffer - array to be filled with read data
 *  size - size of data array
 */
void LORA_ReadBuffer(uint8_t offset, uint8_t * buffer, uint8_t size);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * DIO AND IRQ CONTROL FUNCTIONS
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * LORA_SetDioIrqParams
 * Use to set the IRQ flag
 *
 * See SX126X datasheet page 78 for parameter information
 */
void LORA_SetDioIrqParams(uint16_t IRQMask, uint16_t DIO1Mask, uint16_t DIO2Mask,
                          uint16_t DIO3Mask);

/*
 * LORA_GetIrqStatus
 * Returns the value of the IRQ register
 */
uint16_t LORA_GetIrqStatus();

/*
 * LORA_ClearIrqStatus
 * Clears an IRQ flag in the IRQ register
 *
 * Parameters:
 *  clearMask - set to 1 the bit in clearMask corresponding to the same position as
 *      the IRQ flag to be cleared
 */
void LORA_ClearIrqStatus(uint16_t clearMask);

/*
 * LORA_SetDIO2AsRfSwitchCtrl
 * Configure DIO2 so that it can be used as an external RF switch
 *
 * Parameters:
 *  modeConfig - DIO2_AS_IRQ to set as IRQ, DIO2_AS_SWITCH to set as RF switch
 */
void LORA_SetDIO2AsRfSwitchCtrl(uint8_t modeConfig);

/*
 * LORA_SetDIO3AsTCXOCtrl
 * Configure the chip for an external TCXO reference voltage controlled by DIO3
 *
 * Parameters:
 *  tcxoVoltage - voltage that DIO3 will output to supply the TCXO
 *  timeout - 24-bit time within which the 32 MHz must appear and stabilize;
 *          function fails if value > 16777215
 *
 * Returns 1 if successful; else 0
 */
uint8_t LORA_SetDIO3AsTCXOCtrl(uint8_t tcxoVoltage, uint32_t timeout);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * RF MODULATION AND PACKET-RELATED FUNCTIONS
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * LORA_SetRfFrequency
 * Set frequency of the RF frequency mode
 *
 * Parameters:
 *  frequency - desired frequency, in units of Hz
 */
void LORA_SetRfFrequency(uint32_t frequency);

/*
 * LORA_SetPacketType
 * Sets radio in LoRa or FSK  mode
 *
 * Parameters:
 *  packetConfig - select LoRa or FSK mode using PACKET_TYPE_LORA or PACKET_TYPE_GFSK
 */
void LORA_SetPacketType(uint8_t packetConfig);

/*
 * LORA_GetPacketType()
 * Returns the current operating packet type of the radio
 *
 * See SX126X datasheet page 82 for type codes
 */
uint8_t LORA_GetPacketType();

/*
 * LORA_SetTxParams
 * Set the TX output power and TX ramping time
 */
void LORA_SetTxParams(int8_t power, uint8_t rampTime);

/*
 * LORA_SetModulationParams
 * Configure the modulation parameters of the radio
 *
 * Parameters:
 *  GFSK_br - bit rate; function fails if value > 16777215
 *  GFSK_ps - pulse shape
 *  GFSK_fdev - frequency deviation; function fails if value > 16777215
 *  LORA_sf - spreading factor
 *  LORA_cr - coding rate
 *  LORA_opt - low data rate optimize enable
 *  BW - bandwidth
 *
 * If mode is GFSK, all GFSK_* parameters are used and LORA_* parameters are ignored
 * If mode is LORA, all LORA_* parameters are used and GFSK_* parameters are ignored
 * BW is used regardless of mode
 *
 * Returns 1 if successful, else 0
 *
 * See SX126X datasheet page 85 for parameter guides
 */
uint8_t LORA_SetModulationParams(uint32_t GFSK_br, uint8_t GFSK_ps, uint32_t GFSK_fdev,
                             uint8_t LORA_sf, uint8_t LORA_cr, uint8_t LORA_opt,
                             uint8_t BW);

/*
 * LORA_SetPacketParams
 * Set the parameters of the packet handling block
 *
 * Parameters:
 *  GFSK_pdl - preamble detector length
 *  GFSK_swLen - sync word length
 *  GFSK_ac - addr comp ??
 *  GFSK_pt - packet type
 *  GFSK_w - whitening
 *  LORA_ht - header type
 *  LORA_iq - invert IQ
 *  prLen - preamble length
 *  plLen - payload length
 *  crcType - CRC type
 *
 * If mode is GFSK, all GFSK_* parameters are used and LORA_* parameters are ignored
 * If mode is LORA, all LORA_* parameters are used and GFSK_* parameters are ignored
 * prLen, plLen, crcType are used regardless of mode
 *
 * Returns 1 if successful, else 0
 *
 * See SX126X datasheet page 87 for parameter guides
 */
int LORA_SetPacketParams(uint8_t GFSK_pdl, uint8_t GFSK_swLen, uint8_t GFSK_ac,
                         uint8_t GFSK_pt, uint8_t GFSK_w, uint8_t LORA_ht,
                         uint8_t LORA_iq, uint16_t prLen, uint8_t plLen,
                         uint8_t crcType);

/*
 * LORA_SetCadParams
 * Defines the number of symbols on which CAD operates
 *
 * Parameters:
 *  cadSymbolNum - number of symbols used
 *  cadDetPeak - defines sensitivity of LoRa modem when trying to correlate to actual LoRa preamble symbols
 *  cadDetMin - defines sensitivity of LoRa modem when trying to correlate to actual LoRa preamble symbols
 *  cadExitMode - defines the action to be done after a CAD operation (optional)
 *  cadTimeout - the time the device will stay in RX following a successful CAD;
 *          function fails if value > 16777215
 *
 * Returns 1 if successful; else 0
 *
 * See SX126X datasheet page 92 for recommended cadDetPeak and cadDetMin settings based on spreading factor
 */
uint8_t LORA_SetCadParams(uint8_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin,
                       uint8_t cadExitMode, uint32_t cadTimeout);

/*
 * LORA_SetBufferBaseAddress
 * Sets the base addresses in the data buffer in all modes of operations
 *
 * Parameters:
 *  txBase - base address for TX, 0x00 to 0xFF
 *  rxBase - base address for RX, 0x00 to 0xFF
 */
void LORA_SetBufferBaseAddress(uint8_t txBase, uint8_t rxBase);

/*
 * LORA_SetLoRaSymbNumTimeout
 * Sets the number of symbols used by the modem to validate a successful reception
 *
 * Parameters:
 *  symbNum - when 0, validate reception as soon as LoRa Symbol has been detected; else when SymbNum symbols have been detected
 */
void LORA_SetLoRaSymbNumTimeout(uint8_t symbNum);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * COMMUNICATION STATUS INFORMATION
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * LORA_GetStatus
 * Get the chip status
 *
 * See SX126X datasheet page 94 for status codes
 */
uint8_t LORA_GetStatus();

/*
 * LORA_GetRxBufferStatus
 * Return the array of (ele 1) length of the last received packet (PayloadLengthRx) and (ele 2) the address of the
 * first byte received (RxStartBufferPointer)
 *
 * Parameters:
 *  status - buffer of size 2 that will hold the status information
 *  size - size of buffer; function fails if size is less than 2
 *
 * Returns 1 if successful; else 0
 */
uint8_t LORA_GetRxBufferStatus(uint8_t * status, uint8_t size);

/*
 * LORA_GetPacketStatus
 * If FSK, returns RxStatus, RssiSync, RssiAvg; if LoRa, returns RssiPkt, SnrPkt, SignalRssiPkt
 * returned as buffer of size three (elements in above order)
 *
 * Parameters:
 *  status - buffer of size 3 that will hold the status information
 *  size - size of buffer; function fails if size is less than 3
 *
 * Returns 1 if successful; else 0
 *
 * See SX126X datasheet page 95 for codes
 */
uint8_t LORA_GetPacketStatus(uint8_t * status, uint8_t size);

/*
 * LORA_GetRssiInst
 * Returns the instantaneous RSSI value during reception of the packet
 * Can be used for all protocols
 *
 * See SX126X datasheet page 06 for RSSiInst to dBm conversion
 */
uint8_t LORA_GetRssiInst();

/*
 * LORA_GetStats
 * Returns the number of informations received on a few last packets. If FSK, returns NbPktReceived,
 * NbPktCrcError, NbPktLengthError; if LoRa, returns NbPktReceived, NbPktCrcError, NbPktHeaderErr
 * Can be used for all protocols
 *
 * Parameters:
 *  stats - buffer of size 3 that will hold the stats
 *  size - size of buffer; function fails if size is less than 3
 *
 * Returns 1 if successful; else 0
 */
uint8_t LORA_GetStats(uint16_t * stats, uint8_t size);

/*
 * LORA_ResetStats
 * Resets the value read by the command GetStats
 */
void LORA_ResetStats();

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * MISCELLANEOUS
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * LORA_GetDeviceErrors
 * Returns flags for the possible errors that could occur during different chip operation
 * See SX126X datasheet page 97 for flag translation
 */
uint16_t LORA_GetDeviceErrors();

/*
 * LORA_ClearDeviceErrors
 * Clears all the errors recorded in the device. The errors cannot be cleared independently
 */
void LORA_ClearDeviceErrors();

/*
 * LORA_Busy
 * Returns if the chip is currently busy
 */
uint8_t LORA_Busy();

/*
 * LORA_Reset
 * Toggles NReset pin to reset the LORA chip
 */
void LORA_Reset(); //TODO: fix so that nreset is low for the actual required time (instead of estimating)

/*
 * LORA_TransmitAndWait
 * Transmit data and set transmission parameters; wait until transmission times out or transmission completes
 *
 * Parameters:
 *  offset - location in buffer to write data to
 *  data - data payload to be transmitted
 *  size - size of data payload
 *  timeout - timeout, no timeout active if 0x00
 *  IRQMask - mask to enable interrupts in IRQ status register
 *
 * Returns IRQ status register contents
 */
uint16_t LORA_TransmitAndWait(uint8_t offset, uint8_t * data, uint8_t size, uint32_t timeout, uint16_t IRQMask);

/*
 * LORA_WaitForReceive
 * Set receive parameters and wait for data; wait until receipt times out or frame received
 *
 * Parameters:
 *  offset - location of buffer to read data from
 *  data - array to fill with received data
 *  size - size of data expected
 *  timeout - timeout, no timeout active if 0x00
 *  IRQMask - mask to enable interrupts in IRQ status register
 *
 * Returns IRQ status register contents
 */
uint16_t LORA_WaitForReceive(uint8_t offset, uint8_t * data, uint8_t size, uint32_t timeout, uint16_t IRQMask);

/*
 * LORA_ResetTimeoutCounter
 * Stops the timeout counter and clears the potential timeout event; Call after every RX with timeout active sequence
 *
 * Suggested solution to known implicit header mode timeout behavior bug
 * See SX1262 datasheet page 103 for further explanation
 */
void LORA_ResetTimeoutCounter();

#endif /* LORA_H_ */
