// The purpose of this file is to provide board functionality
// to the radio driver as close to the original interface as possible
// to make the process of possible upgrades to the driver as easy as possible
// This file defines the interfaces, which are implemented in the /board/ dir

#ifndef __BOARD_H__
#define __BOARD_H__

//TODO: instability workaround!
#define printf printk

#include <stdint.h>
#include <stdbool.h>
#include <sleep.h>
#include <stdio.h>
#include <gpiohs.h>
#include <spi.h>
#include "mxtimer.h"

//// Pinout

#define RADIO_NSS SX1261_SPI_NSS_GPIO
#define RADIO_BUSY SX1261_BUSY_GPIO
#define RADIO_DIO1 SX1261_DIO1_GPIO
#define RADIO_RESET SX1261_RESET_GPIO
#define ANT_SWITCH_POWER SX1261_FEM_CRX_GPIO

// Placeholder to keep the api
typedef struct {} Gpio_t;

// Placeholder to keep the api
typedef struct {} Spi_t;

#define SPI_CHANNEL 1
#define SPI_CHIP 0

static void GpioMode(uint8_t pin, gpio_drive_mode_t mode){
  gpiohs_set_drive_mode(pin, mode);
}

static void GpioWrite(uint8_t pin, gpio_pin_value_t value){
  gpiohs_set_pin(pin, value);
}

static gpio_pin_value_t GpioRead(uint8_t pin){
  return gpiohs_get_pin(pin);
}

static void GpioSetInterrupt(uint8_t pin, gpio_pin_edge_t edge, void (*dio_irq)){
  gpiohs_set_pin_edge(pin, edge);
  gpiohs_set_irq(pin, 5, dio_irq);
}

static void DelayMs(uint16_t ms){
  usleep(ms * 1000);
}

static void BoardEnableIrq(){
  //vTaskExitCritical();
}

static void BoardDisableIrq(){
  //vTaskEnterCritical();
}

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

  BoardDisableIrq();

  spi_dup_send_receive_data_dma(DMAC_CHANNEL0, DMAC_CHANNEL1, SPI_CHANNEL, SPI_CHIP, &data, 1, &rxData, 1);

  BoardEnableIrq();

  printf("SPI i/o %02x %02x\n", data, rxData);

  return rxData;
}

//// Timer

typedef struct TimerEvent_s
{
  const char * name;
  mxtimer_cb * callback;
  uint32_t id;
  uint32_t timeout;
} TimerEvent_t;
typedef uint32_t TimerTime_t;
#define TIMERTIME_T_MAX  ( ( uint32_t )~0 )

void TimerInit( TimerEvent_t *obj, mxtimer_cb callback);
void TimerStart( TimerEvent_t *obj );
void TimerStop( TimerEvent_t *obj );
TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime );
TimerTime_t TimerGetCurrentTime( void );
void TimerSetValue( TimerEvent_t *obj, uint32_t value );

static void BoardCriticalSectionBegin(uint32_t* mask){
  
}

static void BoardCriticalSectionEnd(uint32_t* mask){
  
}

static void BoardLowPowerHandler(){
  
}

static uint32_t BoardGetRandomSeed( void )
{
  return 7;
}

static void BoardGetUniqueId( uint8_t *id )
{
  id[7] = 0;
  id[6] = 0;
  id[5] = 0;
  id[4] = 0;
  id[3] = 6;
  id[2] = 9;
  id[1] = 9;
  id[0] = 1;
}

#endif
