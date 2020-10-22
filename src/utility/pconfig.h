#ifndef PCONFIG_H
#define PCONFIG_H
// Persistent storage utility

#include <stdint.h>
#include <stdbool.h>

#define MAX_PARKING_SPOTS 20
typedef struct parking_slot_s{
  uint16_t x;
  uint16_t y;
  uint16_t size;
} __attribute__((__packed__)) parking_slot_t;

//TODO: check for data integrity
typedef struct pconfig_s {
  uint8_t dev_eui[8];
  uint8_t app_eui[8];
  uint8_t app_key[16];
  bool print_debug;
  bool print_echo;
  // TODO: decouple custom settings?
  parking_slot_t parking_slots[MAX_PARKING_SPOTS];
} __attribute__((__packed__)) pconfig_t;

extern pconfig_t pconfig;

void pconfig_load(void);
void pconfig_save(void);

#endif
