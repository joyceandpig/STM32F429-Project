#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "mytypes.h"  
#include "sys.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//电容触摸屏-IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//无
////////////////////////////////////////////////////////////////////////////////// 	

/* Definition for DS18B20_GPIO clock resources */
#define CT_IIC_GPIO_SDA_PIN_NUM           		 3
#define CT_IIC_GPIO_SCL_PORT               		 GPIOH
#define CT_IIC_GPIO_SCL_PIN               		 GPIO_PIN_6



#define CT_IIC_GPIO_SDA_PORT              		 GPIOI
#define CT_IIC_GPIO_SDA_PIN               		 GPIO_PIN_3
#define CT_IIC_GPIO_SDA_READ_PIN               GPIO_PIN_3

#define CT_IIC_GPIO_SDA_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define CT_IIC_GPIO_SCL_CLK_ENABLE()           __HAL_RCC_GPIOH_CLK_ENABLE()

#define CT_IIC_GPIO_MODER_CLEAR(_PORT_,_PIN_)    (_PORT_->MODER &= ~(3<<(_PIN_*2)))
#define CT_IIC_GPIO_MODER_SET_IN(_PORT_,_PIN_)   (_PORT_->MODER |=  (0<<(_PIN_*2)))
#define CT_IIC_GPIO_MODER_SET_OUT(_PORT_,_PIN_)  (_PORT_->MODER |=  (1<<(_PIN_*2)))

//IO方向设置
#define CT_SDA_IN()  do{CT_IIC_GPIO_MODER_CLEAR(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												CT_IIC_GPIO_MODER_SET_IN(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												}while(0);	//PI3输入模式
#define CT_SDA_OUT() do{CT_IIC_GPIO_MODER_CLEAR(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												CT_IIC_GPIO_MODER_SET_OUT(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												}while(0); 	//PI3输出模式

////IO操作函数											   
#define	CT_IIC_SCL_SET     HAL_GPIO_WritePin(CT_IIC_GPIO_SCL_PORT, CT_IIC_GPIO_SCL_PIN, GPIO_PIN_SET) //SCL	 
#define	CT_IIC_SCL_CLEAR   HAL_GPIO_WritePin(CT_IIC_GPIO_SCL_PORT, CT_IIC_GPIO_SCL_PIN, GPIO_PIN_RESET) //SCL

#define	CT_IIC_SDA_SET     HAL_GPIO_WritePin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN, GPIO_PIN_SET) //SDA	  
#define	CT_IIC_SDA_CLEAR   HAL_GPIO_WritePin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN, GPIO_PIN_RESET) //SDA
#define	CT_IIC_SDA(_STA_)  HAL_GPIO_WritePin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN, _STA_) //SDA	
												
#define	CT_READ_SDA_STA    HAL_GPIO_ReadPin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_READ_PIN) //输入SDA 	

 

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口				 
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif







