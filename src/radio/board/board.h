// The purpose of this file is to provide board functionality
// to the radio driver as close to the original interface as possible
// to make the process of possible upgrades to the driver as easy as possible
// This file defines the interfaces, which are implemented in the /board/ dir

#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include <sleep.h>
#include <stdio.h>
#include <gpiohs.h>
#include <spi.h>
#include "mxtimer.h"
#include "gpio-board.h"
#include "timer-board.h"
#include "spi-board.h"

//// Pinout

#define RADIO_NSS SX1261_SPI_NSS_GPIO
#define RADIO_BUSY SX1261_BUSY_GPIO
#define RADIO_DIO1 SX1261_DIO1_GPIO
#define RADIO_RESET SX1261_RESET_GPIO
#define ANT_SWITCH_POWER SX1261_FEM_CRX_GPIO


static void DelayMs(uint16_t ms){
  msleep(ms);
}

static void BoardEnableIrq(){
  sysctl_enable_irq();
}

static void BoardDisableIrq(){
  sysctl_disable_irq();
}

static void BoardCriticalSectionBegin(uint32_t* mask){
  sysctl_disable_irq();
}

static void BoardCriticalSectionEnd(uint32_t* mask){
  sysctl_enable_irq();
}

static void BoardLowPowerHandler(){}

static uint32_t BoardGetRandomSeed( void )
{
  return 0;
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
