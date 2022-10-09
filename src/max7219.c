#include "max7219.h"
#include "cmsis/stm32f1xx.h"

#define NULL 0x0

struct M7219Device {
  uint32_t bit_set;
  uint32_t bit_reset;
  volatile uint32_t* bsrr_register;
  volatile uint32_t* data_register;
  volatile uint32_t* status_register;
};

static M7219Device devices_[2];

static void setMaxFreq(volatile uint32_t* SPI_CR_register, uint32_t bit0, uint32_t bit1, int frequency); 

void addressData(uint16_t* data) {
  data[0] |= M7219_ADDR_0_ROW;
  data[1] |= M7219_ADDR_1_ROW;
  data[2] |= M7219_ADDR_2_ROW;
  data[3] |= M7219_ADDR_3_ROW;
  data[4] |= M7219_ADDR_4_ROW;
  data[5] |= M7219_ADDR_5_ROW;
  data[6] |= M7219_ADDR_6_ROW;
  data[7] |= M7219_ADDR_7_ROW;
}

M7219Device* M7219_init(int transfer, M7219SPIConfig config) {
  switch (transfer) {
  case M7219_SPI1:        
    /// Enable clock for GPIOA and SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN|RCC_APB2ENR_IOPAEN;
    /// Setup GPIOA pins for SPI1
    GPIOA->CRL &= ~(GPIO_CRL_CNF5_0|GPIO_CRL_CNF7_0);
    GPIOA->CRL |=  (GPIO_CRL_CNF5_1|GPIO_CRL_CNF7_1);              // pa5(SCK) and pa7(MOSI) set to AF push-pull
    GPIOA->CRL &= ~(GPIO_CRL_CNF4);                                // pa4(NSS) gpio output 
    GPIOA->CRL &= ~(GPIO_CRL_MODE4|GPIO_CRL_MODE5|GPIO_CRL_MODE7);
    setMaxFreq(&GPIOA->CRL, GPIO_CRL_MODE4_0, GPIO_CRL_MODE4_1, config.ssel_max_frequency);
    setMaxFreq(&GPIOA->CRL, GPIO_CRL_MODE5_0, GPIO_CRL_MODE5_1, config.sck_max_frequency);
    setMaxFreq(&GPIOA->CRL, GPIO_CRL_MODE7_0, GPIO_CRL_MODE7_1, config.mosi_max_frequency);
    /// Setup SPI1
    SPI1->CR1 &= ~SPI_CR1_SPE;                  // disable spi
    SPI1->CR1 |= config.baud_rate_exponent << SPI_CR1_BR_Pos; // spi clk = apb2clk / 256
    SPI1->CR1 &= ~(SPI_CR1_CPOL|SPI_CR1_CPHA);  // idle = low, shift data on rising edge
    SPI1->CR1 |= SPI_CR1_DFF;                   // 8 bits / (16 bits)
    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);           // MSB first
    SPI1->CR1 |= (SPI_CR1_SSM|SPI_CR1_SSI);     // nss will be used as gpio
    SPI1->CR2 &= ~SPI_CR2_SSOE;                 // NSS pin as gpio output
    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE|SPI_CR1_RXONLY); // full duplex
    SPI1->CR1 |= (SPI_CR1_MSTR|SPI_CR1_SPE); // enable spi in master mode

    devices_[0].bit_set = GPIO_BSRR_BS4;
    devices_[0].bit_reset = GPIO_BSRR_BR4;
    devices_[0].bsrr_register = &GPIOA->BSRR;
    devices_[0].data_register = &SPI1->DR;
    devices_[0].status_register = &SPI1->SR;

    return &devices_[0];
  case M7219_SPI2:     
    /// Enable clock for GPIOA and SPI1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    /// Setup GPIOA pins for SPI1
    GPIOB->CRH &= ~(GPIO_CRH_CNF13_0|GPIO_CRH_CNF15_0);
    GPIOB->CRH |=  (GPIO_CRH_CNF13_1|GPIO_CRH_CNF15_1);            // pa5(SCK) and pa7(MOSI) set to AF push-pull
    GPIOB->CRH &= ~(GPIO_CRH_CNF12);                                // pa4(NSS) gpio output 
    GPIOB->CRH &= ~(GPIO_CRH_MODE12|GPIO_CRH_MODE13|GPIO_CRH_MODE15);
    setMaxFreq(&GPIOB->CRH, GPIO_CRH_MODE12_0, GPIO_CRH_MODE12_1, config.ssel_max_frequency);
    setMaxFreq(&GPIOB->CRH, GPIO_CRH_MODE13_0, GPIO_CRH_MODE13_1, config.sck_max_frequency);
    setMaxFreq(&GPIOB->CRH, GPIO_CRH_MODE15_0, GPIO_CRH_MODE15_1, config.mosi_max_frequency);
    /// Setup SPI1
    SPI2->CR1 &= ~SPI_CR1_SPE;                  // disable spi
    SPI2->CR1 |= config.baud_rate_exponent << SPI_CR1_BR_Pos; // spi clk = apb2clk / 256
    SPI2->CR1 &= ~(SPI_CR1_CPOL|SPI_CR1_CPHA);  // idle = low, shift data on rising edge
    SPI2->CR1 |= SPI_CR1_DFF;                   // 8 bits / (16 bits)
    SPI2->CR1 &= ~(SPI_CR1_LSBFIRST);           // MSB first
    SPI2->CR1 |= (SPI_CR1_SSM|SPI_CR1_SSI);     // nss will be used as gpio
    SPI2->CR2 &= ~SPI_CR2_SSOE;                 // NSS pin as gpio output
    SPI2->CR1 &= ~(SPI_CR1_BIDIMODE|SPI_CR1_RXONLY); // full duplex
    SPI2->CR1 |= (SPI_CR1_MSTR|SPI_CR1_SPE); // enable spi in master mode

    devices_[1].bit_set = GPIO_BSRR_BS12;
    devices_[1].bit_reset = GPIO_BSRR_BR12;
    devices_[1].bsrr_register = &GPIOB->BSRR;
    devices_[1].data_register = &SPI2->DR;
    devices_[1].status_register = &SPI2->SR;

    return &devices_[1];
  
  default: return NULL;
  }
}

void M7219_sendDataModuleInterleaved(M7219Device* device, uint16_t module_count, const uint16_t* data, uint16_t size) {
  for (uint16_t m=0; m<size/module_count; ++m) {
    (*device->bsrr_register) = device->bit_reset;

    for (uint16_t i=0; i<module_count; ++i) {
      while(((*device->status_register) & SPI_SR_TXE) == 0);
      (*device->data_register) = data[i + m * module_count];
    }
    
    while(((*device->status_register) & SPI_SR_BSY) != 0);
    (*device->bsrr_register) = device->bit_set;
  }
}

void M7219_sendDataLinesInterleaved(M7219Device* device, uint16_t module_count, const uint16_t* data, uint16_t size) {
  for (uint16_t m=0; m<8; ++m) {
    (*device->bsrr_register) = device->bit_reset;

    for (uint16_t i=0; i<module_count; ++i) {
      while(((*device->status_register) & SPI_SR_TXE) == 0);
      (*device->data_register) = data[i * 8 + m];
    }
    
    while(((*device->status_register) & SPI_SR_BSY) != 0);
    (*device->bsrr_register) = device->bit_set;
  }

}

void M7219_sendDataRepeatModule(M7219Device* device, uint16_t module_count, const uint16_t* data, uint16_t size) {
  for (uint16_t m=0; m<size; ++m) {
    (*device->bsrr_register) = device->bit_reset;

    for (uint16_t i=0; i<module_count; ++i) {
      while(((*device->status_register) & SPI_SR_TXE) == 0);
      (*device->data_register) = data[m];
    }
    
    while(((*device->status_register) & SPI_SR_BSY) != 0);
    (*device->bsrr_register) = device->bit_set;
  }
}

void M7219_sendDataRepeatLine(M7219Device* device, uint16_t module_count, const uint16_t* data) {
  for (uint16_t m=0; m<8; ++m) {
    (*device->bsrr_register) = device->bit_reset;

    for (uint16_t i=0; i<module_count; ++i) {
      while(((*device->status_register) & SPI_SR_TXE) == 0);
      (*device->data_register) = (data[i] & 0x00FF) | ((m + 1) << 8);
    }
    
    while(((*device->status_register) & SPI_SR_BSY) != 0);
    (*device->bsrr_register) = device->bit_set;
  }
}

void setMaxFreq(volatile uint32_t* SPI_CR_register, uint32_t bit0, uint32_t bit1, int frequency) {
  switch (frequency) {
  case M7219_SPI_FREQUENCY_50Mhz: (*SPI_CR_register) |= (bit1|bit0); break; 
  case M7219_SPI_FREQUENCY_10Mhz: (*SPI_CR_register) |=  bit0;       break; 
  case M7219_SPI_FREQUENCY_2Mhz:  (*SPI_CR_register) |=  bit1;       break;
  default: break;
  }
}