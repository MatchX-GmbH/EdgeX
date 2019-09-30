#ifndef MXTIMER_H
#define MXTIMER_H

// Basic implementation for a simplified timer functionality
// Uses a hw timer to check for expired events at a defined frequency

typedef void (*mxtimer_cb)(void*);

typedef struct mxtimer_s {
  uint32_t id;
  uint64_t expiration;
  struct mxtimer_s *next;
  mxtimer_cb callback;
  const char * name;
} mxtimer_t;


void mxtimer_setup();
uint32_t mxtimer_start(uint32_t timeout_ms, mxtimer_cb callback, const char * name);
void mxtimer_stop(uint32_t timer_id);


#endif
