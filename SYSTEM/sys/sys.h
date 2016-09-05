#ifndef _SYS_H
#define _SYS_H
/*
#include "sys.h"
*/
///////////////////////////////////////////////////////////////////////////////////
#include "mytypes.h"  


void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq);//时钟系统配置
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif

