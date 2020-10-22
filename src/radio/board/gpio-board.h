#ifndef GPIO_BOARD_H
#define GPIO_BOARD_H

#include <gpio_common.h>
#include <gpio.h>

// Placeholder to keep the api
typedef struct {} Gpio_t;

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

#endif
