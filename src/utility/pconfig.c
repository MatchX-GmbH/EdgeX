#include "pconfig.h"
#include "w25qxx.h"
#include "stddef.h"
#include "spi.h"

static const size_t config_location = 0x700000;

pconfig_t pconfig;

void pconfig_load(void){
  w25qxx_init(SPI_DEVICE_3, SPI_CHIP_SELECT_0);
  w25qxx_enable_quad_mode();
  w25qxx_read_data(config_location, (uint8_t*)&pconfig, sizeof(pconfig_t), W25QXX_QUAD_FAST);
}

void pconfig_save(void){
  w25qxx_init(SPI_DEVICE_3, SPI_CHIP_SELECT_0);
  w25qxx_enable_quad_mode();
  w25qxx_write_data(config_location, (uint8_t*)&pconfig, sizeof(pconfig_t));
}
