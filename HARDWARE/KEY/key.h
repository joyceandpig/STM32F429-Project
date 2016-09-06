#ifndef _KEY_H
#define _KEY_H
#include "mytypes.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//KEY��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//����ķ�ʽ��ͨ��λ��������ʽ��ȡIO
//#define KEY0        PHin(3) //KEY0����PH3
//#define KEY1        PHin(2) //KEY1����PH2
//#define KEY2        PCin(13)//KEY2����PC13
//#define WK_UP       PAin(0) //WKUP����PA0


#define KEY0_GPIO_PIN        			GPIO_PIN_3
#define KEY1_GPIO_PIN        			GPIO_PIN_2
#define KEY2_GPIO_PIN        			GPIO_PIN_13
#define KEY_WK_UP_GPIO_PIN        GPIO_PIN_0

#define KEY0_GPIO_PORT       			GPIOH
#define KEY1_GPIO_PORT       			GPIOH
#define KEY2_GPIO_PORT       			GPIOC
#define KEY_WK_UP_GPIO_PORT       GPIOA

#define KEY0_GPIO_PORT_CLK_ENABLE()    			__HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY1_GPIO_PORT_CLK_ENABLE()    			__HAL_RCC_GPIOH_CLK_ENABLE()
#define KEY2_GPIO_PORT_CLK_ENABLE()   		  __HAL_RCC_GPIOC_CLK_ENABLE()
#define KEY_WK_UP_GPIO_PORT_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()

//����ķ�ʽ��ͨ��ֱ�Ӳ���HAL�⺯����ʽ��ȡIO
#define KEY0        HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN)  //KEY0����PH3
#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)  //KEY1����PH2
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) //KEY2����PC13
#define WK_UP       HAL_GPIO_ReadPin(KEY_WK_UP_GPIO_PORT, KEY_WK_UP_GPIO_PIN)  //WKUP����PA0

#define KEY0_PRES 	1
#define KEY1_PRES		2
#define KEY2_PRES		3
#define WKUP_PRES   4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
