#include "max7219.h"

void delay(int32_t delay) {
  for (int32_t i=0; i<delay; ++i) {
    asm volatile("NOP");
  }
}

int main(void) {
  M7219SPIConfig config;
  config.baud_rate_exponent = 0;
  config.mosi_max_frequency = M7219_SPI_FREQUENCY_50Mhz;
  config.ssel_max_frequency = M7219_SPI_FREQUENCY_50Mhz;
  config.sck_max_frequency = M7219_SPI_FREQUENCY_50Mhz;

  //M7219Device* device = M7219_init(M7219_SPI1, config);
  M7219Device* device = M7219_init(M7219_SPI2, config);

  const uint16_t init_data[] = {
    M7219_CMD_TEST_MODE,
    M7219_CMD_DECODE_MODE,
    M7219_CMD_INTENCITY,
    M7219_CMD_SCAN_LIMIT        | 0x7,
    M7219_CMD_OPERATION_MODE    | 0x1
  };

  const uint16_t modules_count = 4;
  M7219_sendDataRepeatModule(device, modules_count, init_data, 5);

  
  uint16_t data[] = { 
    0b10010001,
    0b01001010,
    0b00110100,
    0b11111111,
    0b11111111,
    0b00101100,
    0b01010010,
    0b10001001,
    
    0b00001000,
    0b01010010,
    0b00101100,
    0b11110111,
    0b11101111,
    0b00110100,
    0b01001010,
    0b00010000
  };   
  addressData(data);
  addressData(&data[8]);
  

  uint16_t data_anim[] = { 
    0b11111111, 0b11111111, 0b11111111, 0b11111111, 
    0b11111110, 0b11110111, 0b11101111, 0b01111111,
    0b11111100, 0b01110111, 0b11101110, 0b00111111,
    0b11111000, 0b01110011, 0b11001110, 0b00011111,
    0b11110000, 0b00110011, 0b11001100, 0b00001111,
    0b11100000, 0b00110001, 0b10001100, 0b00000111,
    0b11000000, 0b00010001, 0b10001000, 0b00000011,
    0b10000000, 0b00010000, 0b00001000, 0b00000001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000,
  };

  uint16_t i = 0;
  while(1) {
    M7219_sendDataRepeatLine(device, modules_count, &data_anim[i]);

    delay(1000000);
    i += modules_count;
    if (i == 36) { 
      for (uint16_t j=0; j<3; ++j) {
        M7219_sendDataRepeatModule(device, modules_count, data, 8);
        delay(3000000);
        M7219_sendDataRepeatModule(device, modules_count, &data[8], 8);
        delay(3000000);
      }
      i = 0;
    }
  }

  return 0;
}