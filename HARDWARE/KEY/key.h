#ifndef _KEY_H
#define _KEY_H
#include "mytypes.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//KEY驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//下面的方式是通过位带操作方式读取IO
//#define KEY0        PHin(3) //KEY0按键PH3
//#define KEY1        PHin(2) //KEY1按键PH2
//#define KEY2        PCin(13)//KEY2按键PC13
//#define WK_UP       PAin(0) //WKUP按键PA0


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

//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN)  //KEY0按键PH3
#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)  //KEY1按键PH2
#define KEY2        HAL_GPIO_ReadPin(KEY2_GPIO_PORT, KEY2_GPIO_PIN) //KEY2按键PC13
#define WK_UP       HAL_GPIO_ReadPin(KEY_WK_UP_GPIO_PORT, KEY_WK_UP_GPIO_PIN)  //WKUP按键PA0

#define KEY0_PRES 	1
#define KEY1_PRES		2
#define KEY2_PRES		3
#define WKUP_PRES   4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif
