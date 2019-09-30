#include "../sx126x/board.h"
#include "rtc.h"

static volatile rtc_t * rtc_driver = (void*)RTC_BASE_ADDR;

void TimerInit( TimerEvent_t *obj, void ( *callback )( void* ) ){
  printf("TimerInit %s \n", obj->name);
  obj->callback = callback;
}

void TimerStart( TimerEvent_t *obj ){
  printf("TimerStart %s \n", obj->name);
  obj->id = mxtimer_start(obj->timeout, obj->callback, obj->name);
}

void TimerStop( TimerEvent_t *obj ){
  printf("TimerStop %s \n", obj->name);
  mxtimer_stop(obj->id);
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value ){
  printf("TimerSetVal %s %u \n", obj->name, value);
  //xTimerChangePeriod(obj->handle, value / portTICK_PERIOD_MS, BLOCKING_TIME);
  obj->timeout = value;
}

TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime ){
  return TimerGetCurrentTime() - savedTime;
}

// TODO: Not sure if this is intended implementation
TimerTime_t TimerGetCurrentTime( void ){
  // get ms
  const uint32_t rtc_ticks_per_s = 26 * 000 * 000;
  uint32_t milliseconds = (double)rtc_driver->current_count.count / rtc_ticks_per_s * 1000;

  // get s
  struct tm get_time;
  rtc_timer_get(&get_time.tm_year, &get_time.tm_mon, &get_time.tm_mday, &get_time.tm_hour, &get_time.tm_min, &get_time.tm_sec);
  time_t seconds = mktime(&get_time);

  return seconds * 1000 + milliseconds;
}
