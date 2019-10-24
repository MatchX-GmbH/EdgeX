#ifndef PCONFIG_H
#define PCONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Persistent storage utility

typedef struct pconfig_s {
  uint8_t dev_eui[8];
  uint8_t app_eui[8];
  uint8_t app_key[16];
  bool print_debug;
} __attribute__((__packed__)) pconfig_t;

extern pconfig_t pconfig;

void pconfig_load(void);
void pconfig_save(void);

#endif
