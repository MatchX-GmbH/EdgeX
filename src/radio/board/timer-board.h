#ifndef TIMER_BOARD_H
#define TIMER_BOARD_H

#include <stdint.h>
#include "mxtimer.h"

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

#endif
