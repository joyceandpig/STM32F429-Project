#ifndef __GT9147_H
#define __GT9147_H	
#include "mytypes.h"  
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//4.3寸电容触摸屏-GT9147 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved		 
////////////////////////////////////////////////////////////////////////////////// 

#define GT_RST_GPIO_PIN_NUM           		 8
#define GT_INT_GPIO_PIN_NUM           		 7

#define GT_RST_GPIO_PIN           		 GPIO_PIN_8
#define GT_INT_GPIO_PIN           		 GPIO_PIN_7

#define GT_RST_GPIO_PORT               		 GPIOI
#define GT_INT_GPIO_PORT               		 GPIOH

#define GT_RST_GPIO_PORT_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define GT_INT_GPIO_PORT_CLK_ENABLE()           __HAL_RCC_GPIOH_CLK_ENABLE()

////IO操作函数											   
#define	GT_RST_SET()     HAL_GPIO_WritePin(GT_RST_GPIO_PORT, GT_RST_GPIO_PIN, GPIO_PIN_SET) //SCL	 
#define	GT_RST_CLEAR()   HAL_GPIO_WritePin(GT_RST_GPIO_PORT, GT_RST_GPIO_PIN, GPIO_PIN_RESET) //SCL

#define	GT_INT()     HAL_GPIO_ReadPin(GT_INT_GPIO_PORT, GT_INT_GPIO_PIN) //SDA	  

////IO操作函数	 
//#define GT_RST  PIout(8) //GT9147复位引脚
//#define GT_INT  PHin(7)  //GT9147中断引脚		
 
//I2C读写命令	
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令
  
//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  
 
 
u8 GT9147_Send_Cfg(u8 mode);
u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len);
void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len); 
u8 GT9147_Init(void);
u8 GT9147_Scan(u8 mode); 
#endif













