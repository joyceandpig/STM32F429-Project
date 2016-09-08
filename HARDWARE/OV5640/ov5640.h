#ifndef _OV5640_H
#define _OV5640_H
#include "sys.h"
#include "sccb.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//OV5640 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//*******************************************************************************
//修改信息
//20160226 V1.1
//新增OV5640_Exposure函数.
//20160515 V1.2
//1，修改ov5640_uxga_init_reg_tbl数组名字为：ov5640_init_reg_tbl
//2，修改相关注释
////////////////////////////////////////////////////////////////////////////////// 
#define OV5640_RST_GPIO_PIN_NUM           		 15

#define OV5640_RST_GPIO_PORT               		 GPIOA
#define OV5640_RST_GPIO_PIN           		 GPIO_PIN_15

#define OV5640_RST_GPIO_PORT_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()

////IO操作函数											   
#define	OV5640_RST_SET()     HAL_GPIO_WritePin(OV5640_RST_GPIO_PORT, OV5640_RST_GPIO_PIN, GPIO_PIN_SET) //SCL	 
#define	OV5640_RST_CLEAR()   HAL_GPIO_WritePin(OV5640_RST_GPIO_PORT, OV5640_RST_GPIO_PIN, GPIO_PIN_RESET) //SCL

//#define OV5640_PWDN  	PGout(9)			//POWER DOWN控制信号,由PCF8574_P2控制 
//#define OV5640_RST  	PAout(15)			//复位控制信号 

////////////////////////////////////////////////////////////////////////////////// 
#define OV5640_ID               0X5640  	//OV5640的芯片ID
 
#define OV5640_ADDR        			0X78		//OV5640的IIC地址
 
//OV5640相关寄存器定义  
#define OV5640_CHIPIDH          0X300A  	//OV5640芯片ID高字节
#define OV5640_CHIPIDL          0X300B  	//OV5640芯片ID低字节
 

								
u8 OV5640_WR_Reg(u16 reg,u8 data);
u8 OV5640_RD_Reg(u16 reg);
void OV5640_PWDN_Set(u8 sta);
u8 OV5640_Init(void);  
void OV5640_JPEG_Mode(void);
void OV5640_RGB565_Mode(void);
void OV5640_Exposure(u8 exposure);
void OV5640_Light_Mode(u8 mode);
void OV5640_Color_Saturation(u8 sat);
void OV5640_Brightness(u8 bright);
void OV5640_Contrast(u8 contrast);
void OV5640_Sharpness(u8 sharp);
void OV5640_Special_Effects(u8 eft);
void OV5640_Test_Pattern(u8 mode);
void OV5640_Flash_Ctrl(u8 sw);
u8 OV5640_OutSize_Set(u16 offx,u16 offy,u16 width,u16 height);
u8 OV5640_ImageWin_Set(u16 offx,u16 offy,u16 width,u16 height); 
u8 OV5640_Focus_Init(void);
u8 OV5640_Focus_Single(void);
u8 OV5640_Focus_Constant(void);
#endif





















