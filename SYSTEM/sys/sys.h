#ifndef _SYS_H
#define _SYS_H
/*
#include "sys.h"
*/
///////////////////////////////////////////////////////////////////////////////////
#include "mytypes.h"  
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq);//ʱ��ϵͳ����
//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ 
#endif

