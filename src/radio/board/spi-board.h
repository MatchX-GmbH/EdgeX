#ifndef SPI_H
#define SPI_H

#include <sysctl.h>
#include "dbprintf.h"

// Placeholder to keep the api
typedef struct {} Spi_t;

#define SPI_CHANNEL 1
#define SPI_CHIP 1

static void SpiInit( Spi_t *obj, uint8_t mosi, uint8_t miso, uint8_t sclk, uint8_t nss )
{}

static void SpiDeInit()
{}

static void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{}

static void SpiFrequency( Spi_t *obj, uint32_t hz )
{}

static uint8_t SpiInOut( Spi_t *obj, uint8_t data ){
  uint8_t rxData = 0;

  sysctl_disable_irq();

  spi_dup_send_receive_data_dma(DMAC_CHANNEL1, DMAC_CHANNEL2, SPI_CHANNEL, SPI_CHIP, &data, 1, &rxData, 1);

  sysctl_enable_irq();

  dbprintf("SPI i/o %02x %02x\n", data, rxData);

  return rxData;
}

#endif
