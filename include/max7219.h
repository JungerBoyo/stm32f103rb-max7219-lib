#ifndef STM32F103RB_MAX7219_H
#define STM32F103RB_MAX7219_H

#include <stdint.h>

/**
 * @brief Modify test mode state, should be ORed with 0x1 for test mode.
 * Test mode turns on all LEDs. 
 */
#define M7219_CMD_TEST_MODE       0x0F00
/**
 * @brief Set bit to one on 8 least significant bits in order to enable decode
 * for respective numbers
 */
#define M7219_CMD_DECODE_MODE     0x0900
/**
 * @brief Set numbers on 8 least significant bits to change LED's intensity. Bigger number = light more intensive.
 */
#define M7219_CMD_INTENCITY       0x0A00
/**
 * @brief Should be set to max
 */
#define M7219_CMD_SCAN_LIMIT      0x0B00
/**
 * @brief Send this commad ORed with 0x1 to enter normal operation mode. Send this command to enter shutdown mode
 * (everything pulled to ground, OSC halted)
 */
#define M7219_CMD_OPERATION_MODE  0x0C00

#define M7219_ADDR_0_ROW 0x0100
#define M7219_ADDR_1_ROW 0x0200
#define M7219_ADDR_2_ROW 0x0300
#define M7219_ADDR_3_ROW 0x0400
#define M7219_ADDR_4_ROW 0x0500
#define M7219_ADDR_5_ROW 0x0600
#define M7219_ADDR_6_ROW 0x0700
#define M7219_ADDR_7_ROW 0x0800

/**
 * @brief Addresses data with M7219_ADDR_X_ROW defines. Data size MUST BE <= 8. 
 * 
 * @param data ptr to data
 * @param modules_count count of max7219 modules 
 */
void addressData(uint16_t* data);

typedef struct M7219Device M7219Device;

enum M7219Transmit {
  M7219_SPI1,
  M7219_SPI2
};

enum M7219SPIPinMaxFrequencies {
  M7219_SPI_FREQUENCY_50Mhz,
  M7219_SPI_FREQUENCY_10Mhz,
  M7219_SPI_FREQUENCY_2Mhz
};

/**
 * @brief Limited SPI config. baud_rate_exponent translates to CLK/2^(baud_rate_exponent+1).
 */
typedef struct M7219SPIConfig {
  int sck_max_frequency;
  int mosi_max_frequency;
  int ssel_max_frequency;
  uint32_t baud_rate_exponent;
} M7219SPIConfig;

/**
 * @brief Initialize M7219 device to use one of spi communication modules available on the MCU.
 * @param transfer One of enums from M7219Transmit enum definition.
 * @return M7219Device* handle to M7219 device
 */
M7219Device* M7219_init(int transfer, M7219SPIConfig config);

/**
 * @brief Sends data to max7219. Assumes that user want data to be unique for each module. 
 * 
 * @param device        pointer to device created with init
 * @param module_count  count of max7219 modules 
 * @param data          pointer to data 
 * @param size          size of data 
 */
void M7219_sendData(M7219Device* device, uint16_t module_count, const uint16_t* data, uint16_t size);

/**
 * @brief Sends data to max7219. Assumes that user want data element to be repeated for each module. 
 * 
 * @param device        pointer to device created with init
 * @param module_count  count of max7219 modules 
 * @param data          pointer to data 
 * @param size          size of data  
 */
void M7219_sendDataRepeatModule(M7219Device* device, uint16_t module_count, const uint16_t* data, uint16_t size);

/**
 * @brief Sends data to max7219. Assumes that user want data element to be repeated for each line. 
 * Data size must be at least module_count.
 * @param device        pointer to device created with init
 * @param module_count  count of max7219 modules 
 * @param data          pointer to data 
 * @param size          size of data  
 */
void M7219_sendDataRepeatLine(M7219Device* device, uint16_t module_count, const uint16_t* data);

#endif