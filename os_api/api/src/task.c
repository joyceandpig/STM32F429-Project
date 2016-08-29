#include "api.h"
#include "malloc.h"	

#define mem_calloc(x,y)		mymalloc(SRAMIN,x*y)
#define mem_free(x)			myfree(SRAMIN,x)

#ifdef	OS_UCOS
#ifdef	CPU_CFG_CRITICAL_METHOD	
	#define UCOS_V3
#endif

#ifdef	OS_CRITICAL_METHOD
	#undef	UCOS_V3
#endif

#endif


int thread_create(void(*task)(void *p_arg), void *p_arg, unsigned int prio, unsigned int *pbos, unsigned int stk_size, char *name)
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	OS_ERR err;
	OS_TCB *TaskTCB=NULL;
	
	if (pbos == 0)
	{
		pbos = (CPU_STK *)mem_calloc(stk_size, sizeof(CPU_STK));
		if (!pbos)
			return NULL;
		TaskTCB=(OS_TCB *)mem_calloc(1, sizeof(OS_TCB));
		if (!TaskTCB)
			return NULL;
	}

	OSTaskCreate(	(OS_TCB 	*)TaskTCB,
								(CPU_CHAR 	*)name,
								(OS_TASK_PTR)task,
								(void 		*)p_arg,
								(OS_PRIO	) prio,
								(CPU_STK	*)&pbos[0],
								(CPU_STK_SIZE)stk_size/10,
								(CPU_STK_SIZE)stk_size,
								(OS_MSG_QTY)0,
								(OS_TICK	)0,
								(void		*)0,
								(OS_OPT		)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
								(OS_ERR		*)&err);
								
	if (err == OS_ERR_NONE)
	{
		return (int)TaskTCB;
	}else{
			if(pbos)
				mem_free(pbos);
			if(TaskTCB)
				mem_free(TaskTCB);
	}
#else
	INT8U ret, need_free_stack = 0;
	if (pbos == 0)
	{
		need_free_stack = 1;
		pbos = (OS_STK*)mem_calloc(stk_size, sizeof(OS_STK));
		if (!pbos)
			return -1;
	}
	ret = OSTaskCreateExt(task, p_arg, &pbos[stk_size - 1], prio, prio, pbos, stk_size, NULL, (INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	if (ret == OS_ERR_NONE)
	{
		OSTaskNameSet(prio, (INT8U*)name, (INT8U*) &ret);
		return prio;
	}else{
		if(need_free_stack && pbos)
			mem_free(pbos);
	}
#endif
	return  - 1;
#endif
	
#ifdef OS_FREE_RTOS
	TaskHandle_t xHandle = NULL;
	xTaskCreate(task, name, stk_size, p_arg, prio, &xHandle);
	return (int)xHandle;
#endif
}

int thread_exit(int thread_id)
{
	int ret = 0;
#ifdef OS_UCOS
#ifdef UCOS_V3
	OSTaskDel((OS_TCB *)thread_id,(OS_ERR*)&ret);
	mem_free(((OS_TCB *)thread_id)->StkBasePtr);//mark
	mem_free((void*)thread_id);
#else
	ret = OSTaskDel(thread_id);
#endif
#endif
#ifdef OS_FREE_RTOS
	vTaskDelete((TaskHandle_t)thread_id);
#endif
	return ret;
}

//返回任务自身的ID号
int thread_myself()
{
#ifdef OS_UCOS
#ifdef UCOS_V3
	return (int)OSTCBCurPtr;
#else
	return OSTCBCur->OSTCBId;
#endif
#endif
#ifdef OS_FREE_RTOS
	return (int)xTaskGetCurrentTaskHandle();
#endif
}
