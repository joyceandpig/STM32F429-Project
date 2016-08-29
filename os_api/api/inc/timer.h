#ifndef _TIMER_H_
#define _TIMER_H_
#include "api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef OS_UCOS

typedef OS_TMR *timer_t;

#endif
	
#ifdef OS_FREE_RTOS
typedef void *timer_t;
#endif

typedef void(*timer_callback_func)(void *parg);

timer_t timer_setup(int time_val, int type, timer_callback_func callback, void *callback_arg);

int timer_pending(timer_t tmr);

int mod_timer(timer_t tmr, unsigned int expires);

int add_timer(timer_t tmr);

int del_timer(timer_t tmr);

int timer_free(timer_t tmr);

void sleep(uint32_t ms);


unsigned int os_time_get(void);

unsigned int ms2ticks(unsigned int ms);

extern volatile uint32_t jiffies, tick_ms;

#ifdef __cplusplus
}

#endif

#endif
