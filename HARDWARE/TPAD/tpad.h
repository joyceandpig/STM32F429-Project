#ifndef __TPAD_H
#define __TPAD_H
#include "sys.h"
#include "timer.h"		
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//TPAD��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved		
//********************************************************************************
//�޸�˵��
//V1.1  20160218
//1,ȥ��TPAD_GATE_VAL�Ķ���,��Ϊ�ж�tpad_default_val��4/3Ϊ����
//2,�޸�TPAD_Get_MaxVal������ʵ�ַ�ʽ,��߿�������
////////////////////////////////////////////////////////////////////////////////// 	
   
//���ص�ʱ��(û���ְ���),��������Ҫ��ʱ��
//���ֵӦ����ÿ�ο�����ʱ�򱻳�ʼ��һ��


#define TPAD_RST_GPIO_PIN_NUM           		 5

#define TPAD_RST_GPIO_PORT               		 GPIOA
#define TPAD_RST_GPIO_PIN           		 GPIO_PIN_5

#define TPAD_RST_GPIO_PORT_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()

////IO��������											   
#define	TPAD_RST_SET()     HAL_GPIO_WritePin(TPAD_RST_GPIO_PORT, TPAD_RST_GPIO_PIN, GPIO_PIN_SET) //SCL	 
#define	TPAD_RST_CLEAR()   HAL_GPIO_WritePin(TPAD_RST_GPIO_PORT, TPAD_RST_GPIO_PIN, GPIO_PIN_RESET) //SCL


extern vu16 tpad_default_val;
							   	    
void TPAD_Reset(void);
u16  TPAD_Get_Val(void);
u16 TPAD_Get_MaxVal(u8 n);
u8   TPAD_Init(u8 systick);
u8   TPAD_Scan(u8 mode);
void TIM2_CH1_Cap_Init(u32 arr,u16 psc);    
#endif























