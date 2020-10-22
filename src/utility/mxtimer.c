#include <timer.h>
#include <stdint.h>
#include "mxtimer.h"

#define mxtimer_count 8
static const uint16_t mxtimer_freq_hz = 100;
static const timer_device_number_t mxtimer_hw_timer_dev = TIMER_DEVICE_0;
static const timer_channel_number_t mxtimer_hw_timer_ch = TIMER_CHANNEL_0;
static const uint8_t mxtimer_irq_prio = 1;

static const uint32_t interval_ns = 1000 * 1000 * 1000 / mxtimer_freq_hz;
static const float interval_ms = (float)interval_ns / (1000 * 1000);
static const float ticks_per_ms = 1.f / interval_ms;

static mxtimer_t timer_array[mxtimer_count];
static mxtimer_t* free_head = NULL;
static mxtimer_t* expiration_head = NULL;

static uint64_t current_tick = 0;
static uint64_t id_count = 0;

static void dump_timers(){
#if DUMP_TIMERS
  dbprintk("\n\n\nTO_EXPIRE:\n");
  mxtimer_t* head = expiration_head;
  while(head){
    dbprintk("name: %s, expiration %u, cb 0x%08x \n", head->name, head->expiration, head->callback);
    head = head->next;
  }
  dbprintk("\n\nFREE:\n");
  head = free_head;
  while(head){
    dbprintk("name: %s, expiration %u, cb 0x%08x \n", head->name, head->expiration, head->callback);
    head = head->next;
  }
  dbprintk("\n\n\n");
#endif
}

void mxtimer_timeout(void * ctx){

  sysctl_disable_irq();
  current_tick++;

  while(expiration_head){
    if(expiration_head->expiration <= current_tick){

      dump_timers();

      mxtimer_t* expired = expiration_head;

      dbprintk("timer shot %s! at %u \n", expired->name, (uint32_t)expired->callback);

      expiration_head = expired->next;
      expired->next = free_head;
      free_head = expired;


      // We invoke it afterwards, since the handle can try to modify timers
      // in which case we loose consistency w/ pointers
      expired->callback(NULL);

      dump_timers();
    }else{
      break;
    }
  }

  sysctl_enable_irq();
}

void mxtimer_setup(){
  void* const null_ctx = NULL;
  const uint8_t not_single_shot = 0;

  timer_init(mxtimer_hw_timer_dev);
  timer_set_interval(mxtimer_hw_timer_dev, mxtimer_hw_timer_ch, interval_ns);
  dbprintk("setting interval %u \n", interval_ns);
  timer_irq_register(mxtimer_hw_timer_dev, mxtimer_hw_timer_ch, not_single_shot,
                     mxtimer_irq_prio, mxtimer_timeout, null_ctx);
  timer_set_enable(mxtimer_hw_timer_dev, mxtimer_hw_timer_ch, 10);

  for(int i = 0; i < mxtimer_count - 1; ++i){
    timer_array[i].next = timer_array + i + 1;
  }

  free_head = timer_array;
}

uint32_t mxtimer_start(uint32_t timeout_ms, mxtimer_cb callback, const char* name){
  dump_timers();

  sysctl_disable_irq();

  uint32_t timer_id = 0;

  if(free_head){
    mxtimer_t* timer = free_head;
    free_head = timer->next;

    timer->expiration = current_tick + (timeout_ms * ticks_per_ms);
    timer->callback = callback;
    timer->name = name;
    timer->id = timer_id = ++id_count;
    dbprintk("timeout in %u\n", (uint32_t)(timeout_ms * ticks_per_ms) );

    if(expiration_head == NULL || expiration_head->expiration >= timer->expiration){
      timer->next = expiration_head;
      expiration_head = timer;
    }else{
      mxtimer_t* insert = expiration_head;
      while(insert->next && insert->next->expiration < timer->expiration)
        insert = insert->next;
      timer->next = insert->next;
      insert->next = timer;
    }
  }

  sysctl_enable_irq();

  dump_timers();

  return timer_id;
}

void mxtimer_stop(uint32_t timer_id){
  dump_timers();

  sysctl_disable_irq();
  mxtimer_t* previous = NULL;
  mxtimer_t* head = expiration_head;

  while(head){
    if(head->id == timer_id){

      if(head->next){
        dbprintk("stopped timer %s with next cb %u \n", head->name, (uint32_t)head->next->callback);
      }else{
        dbprintk("stopped timer with no next \n", head->name);
      }

      if(previous){
        previous->next = head->next;
      }else{
        expiration_head = head->next;
      }
      head->next = free_head;
      free_head = head;
      break;
    }
    previous = head;
    head = head->next;
  }

  sysctl_enable_irq();

  dump_timers();
}

void mxtimer_clear_all(){
  sysctl_disable_irq();

  if(expiration_head){
      mxtimer_t* last_to_expire = expiration_head;
      while(last_to_expire->next)
        last_to_expire = last_to_expire->next;

      last_to_expire->next = free_head;
      free_head = expiration_head;
      expiration_head = NULL;
  }

  sysctl_enable_irq();
}
