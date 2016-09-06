#ifndef _SDRAM_H
#define _SDRAM_H
#include "mytypes.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//SDRAM驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
////SRAM 端口 配置
#define SRAM_C_PORT    GPIOC
#define SRAM_D_PORT    GPIOD
#define SRAM_E_PORT    GPIOE
#define SRAM_F_PORT    GPIOF
#define SRAM_G_PORT    GPIOG


#define SRAM_FMC_PORT_CLK_ENABLE()		__HAL_RCC_FMC_CLK_ENABLE();  
#define SRAM_C_PORT_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE();
#define SRAM_D_PORT_CLK_ENABLE()			__HAL_RCC_GPIOD_CLK_ENABLE();
#define SRAM_E_PORT_CLK_ENABLE()			__HAL_RCC_GPIOE_CLK_ENABLE();
#define SRAM_F_PORT_CLK_ENABLE()			__HAL_RCC_GPIOF_CLK_ENABLE();
#define SRAM_G_PORT_CLK_ENABLE()			__HAL_RCC_GPIOG_CLK_ENABLE();

////SRAM 引脚 配置
#define SRAM_DQ0_FMC_D0      GPIO_PIN_14  //D
#define SRAM_DQ1_FMC_D1      GPIO_PIN_15  //D
#define SRAM_DQ2_FMC_D2      GPIO_PIN_0   //D
#define SRAM_DQ3_FMC_D3      GPIO_PIN_1   //D
#define SRAM_DQ4_FMC_D4      GPIO_PIN_7   //E
#define SRAM_DQ5_FMC_D5      GPIO_PIN_8   //E
#define SRAM_DQ6_FMC_D6      GPIO_PIN_9   //E
#define SRAM_DQ7_FMC_D7      GPIO_PIN_10   //E
#define SRAM_DQ8_FMC_D8      GPIO_PIN_11   //E
#define SRAM_DQ9_FMC_D9     	GPIO_PIN_12   //E
#define SRAM_DQ10_FMC_D10     GPIO_PIN_13   //E
#define SRAM_DQ11_FMC_D11      GPIO_PIN_14   //E
#define SRAM_DQ12_FMC_D12     GPIO_PIN_15  //E
#define SRAM_DQ13_FMC_D13     GPIO_PIN_8   //D
#define SRAM_DQ14_FMC_D14     GPIO_PIN_9   //D
#define SRAM_DQ15_FMC_D15     GPIO_PIN_10   //D

#define SRAM_A0_FMC_A0  GPIO_PIN_0//F
#define SRAM_A1_FMC_A1  GPIO_PIN_1//F
#define SRAM_A2_FMC_A2   GPIO_PIN_2//F
#define SRAM_A3_FMC_A3   GPIO_PIN_3//F
#define SRAM_A4_FMC_A4   GPIO_PIN_4//F
#define SRAM_A5_FMC_A5    GPIO_PIN_5//F
#define SRAM_A6_FMC_A6   GPIO_PIN_12//F
#define SRAM_A7_FMC_A7   GPIO_PIN_13//F
#define SRAM_A8_FMC_A8   GPIO_PIN_14//F
#define SRAM_A9_FMC_A9    GPIO_PIN_15//F
#define SRAM_A10_FMC_A10     GPIO_PIN_0//G
#define SRAM_A11_FMC_A11     GPIO_PIN_1//G
#define SRAM_A12_FMC_A12     GPIO_PIN_2//G

#define SRAM_WE_FMC_SDNWE       GPIO_PIN_0//C
#define SRAM_CAS_FMC_SDNCAS    GPIO_PIN_15//G
#define SRAM_RAS_FMSSDNRAS     GPIO_PIN_11//F
#define SRAM_CS_FMC_SDNE0         GPIO_PIN_2//C

#define SRAM_BA0_FMC_BA0      GPIO_PIN_4//G
#define SRAM_BA1_FMC_BA1      GPIO_PIN_5//G
#define SRAM_CKE_FMC_SDCKE0   GPIO_PIN_3//C
#define SRAM_CLK_FMC_SDCLK    GPIO_PIN_8//G
#define SRAM_LDQM_FMC_NBL0   GPIO_PIN_0//E
#define SRAM_UDQM_FMC_NBL1   GPIO_PIN_1//E



extern SDRAM_HandleTypeDef SDRAM_Handler;//SDRAM句柄
#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) //SDRAM开始地址

//SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((u16)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((u16)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((u16)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((u16)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((u16)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((u16)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((u16)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((u16)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((u16)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((u16)0x0200)

void SDRAM_Init(void);
void SDRAM_MPU_Config(void);
u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);
#endif
