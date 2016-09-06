#ifndef _LED_H
#define _LED_H
#include "mytypes.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define LED_0_GPIO_PIN_NUM           0
#define LED_1_GPIO_PIN_NUM           1
#define LED_0_GPIO_PIN               GPIO_PIN_0
#define LED_1_GPIO_PIN               GPIO_PIN_1
#define LED_GPIO_GPIO_PORT           GPIOB


#define LED_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()


////IO操作函数											   
#define	LED0_ON()   HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_0_GPIO_PIN, GPIO_PIN_RESET) //数据端口	 
#define	LED0_OFF()  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_0_GPIO_PIN, GPIO_PIN_SET) //数据端口	 
#define	LED0_SET_STA(_STA_)  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_0_GPIO_PIN, _STA_) //数据端口

#define	LED1_ON()   HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_1_GPIO_PIN, GPIO_PIN_RESET) //数据端口	 
#define	LED1_OFF()  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_1_GPIO_PIN, GPIO_PIN_SET) //数据端口	 
#define	LED1_SET_STA(_STA_)  HAL_GPIO_WritePin(LED_GPIO_GPIO_PORT, LED_1_GPIO_PIN, _STA_) //数据端口


														
//#define LED0 PBout(1)   //LED0
//#define LED1 PBout(0)   //LED1

void LED_Init(void);
#endif
