#ifndef _WAIT_H_
#define _WAIT_H_
#include "api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef OS_UCOS
#ifdef UCOS_V3
typedef OS_SEM *wait_event_t;
#else
typedef OS_EVENT *wait_event_t;
#endif
#endif
#ifdef OS_FREE_RTOS
typedef void *wait_event_t;
#endif
#define WAIT_EVENT_TIMEOUT 1

wait_event_t init_event(void);

int wait_event(wait_event_t wq);

int wait_event_timeout(wait_event_t wq, unsigned int timeout);

void wake_up(wait_event_t wq);
void del_event(wait_event_t wq);

void clear_wait_event(wait_event_t wq);

#define init_waitqueue_head(x)		*(x) = init_event()
#define wait_event_interruptible(x,y)	wait_event(x)
#define wait_event_interruptible_timeout(x,y,z)	wait_event_timeout(x, z)

#define wake_up_interruptible	wake_up

#ifdef __cplusplus
}

#endif

#endif
