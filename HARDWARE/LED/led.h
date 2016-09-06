#ifndef _LED_H
#define _LED_H
#include "mytypes.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define LED_0_GPIO_PIN_NUM           0
#define LED_1_GPIO_PIN_NUM           1
#define LED_0_GPIO_PIN               GPIO_PIN_0
#define LED_1_GPIO_PIN               GPIO_PIN_1
#define LED_GPIO_GPIO_PORT           GPIOB


#define LED_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()


////IO��������											   
#define	LED0_ON()   HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_0_GPIO_PIN, GPIO_PIN_RESET) //���ݶ˿�	 
#define	LED0_OFF()  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_0_GPIO_PIN, GPIO_PIN_SET) //���ݶ˿�	 
#define	LED0_SET_STA(_STA_)  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_0_GPIO_PIN, _STA_) //���ݶ˿�

#define	LED1_ON()   HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_1_GPIO_PIN, GPIO_PIN_RESET) //���ݶ˿�	 
#define	LED1_OFF()  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_1_GPIO_PIN, GPIO_PIN_SET) //���ݶ˿�	 
#define	LED1_SET_STA(_STA_)  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_1_GPIO_PIN, _STA_) //���ݶ˿�


														
//#define LED0 PBout(1)   //LED0
//#define LED1 PBout(0)   //LED1

void LED_Init(void);
#endif
