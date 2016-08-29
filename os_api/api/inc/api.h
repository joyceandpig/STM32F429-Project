#ifndef _API_H_
#define _API_H_

#include "stm32f4xx.h"

//kernel
#ifdef OS_FREE_RTOS
	#include "FreeRTOS.h"
	#include "task.h"
	#include "semphr.h"
	#include "queue.h"
	#include "timers.h"
#endif

#ifdef OS_UCOS
	#ifdef UCOS_V3
		#include "os.h"
		#include "os_cfg_app.h"
	#else
		#include "ucos_ii.h"
	#endif
#endif

#include "atomic.h"
//#include "mbox.h"
//#include "mutex.h"
//#include "sem.h"
//#include "wait.h"
#include "task_api.h"
//#include "timer.h"
//#include "msg_q.h"


#ifdef __cplusplus
extern "C"
{
#endif

extern volatile uint32_t jiffies;
#ifdef OS_FREE_RTOS
extern volatile int           OSIntNesting;      
#endif
	
__inline void enter_interrupt(void)
{
#ifdef OS_UCOS
	#ifdef UCOS_V3
		OSIntNestingCtr++;
	#else
		OSIntNesting++;
	#endif
#endif
	
#ifdef OS_FREE_RTOS
	OSIntNesting++;
#endif
}

__inline void exit_interrupt(int need_sched)
{
#ifdef	OS_UCOS
	if (need_sched)
		OSIntExit();
	else{
#ifdef UCOS_V3
		OSIntNestingCtr--;
#else
		OSIntNesting--;
#endif
	}
#endif
	
#ifdef OS_FREE_RTOS
	OSIntNesting--;
	portYIELD_FROM_ISR(need_sched);
#endif
}

#ifdef __cplusplus
}

#endif

#endif
