#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

/*
#include "user_config.h"
*/

#ifdef __cplusplus
extern "C" {
#endif
	
#include "api.h"
#include "mytypes.h"
#include "sys.h"
#include "includes.h"
#include "delay.h"

	
#define Sleep(x) delay_ms(x)
	
	
	
#define USART1_EN
#define USART3_EN
	
/*
*********************************************************************************************************
*                                            TASK PRIORITIES
* UCOS每个线程优先级不能相同，放在这里统一管理
*********************************************************************************************************
*/
#ifdef OS_UCOS
enum TASK_PRIO{
	TASK_UNUSED_PRIO = 10,
	//以上顺序勿随意改动

	//user task	
	
	TASK_MAIN_PRIO ,
	TASK_LED_PRIO,
	TASK_USB_PRIO,
	TASK_PICTURE_PRIO,
	TASK_KEY_PRIO,
	
	
	/////
	OS_TASK_TMR_PRIO = (OS_LOWEST_PRIO - 2u)/**/
};
#endif

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*单位WORD
*********************************************************************************************************
*/
//Sys Task!!!

//User Task
#define TASK_MAIN_STACK_SIZE				1200
#define TASK_LED_STACK_SIZE					512
#define TASK_USB_STACK_SIZE					512
#define TASK_PICTURE_STACK_SIZE			512
#define TASK_KEY_STACK_SIZE					512
/*
*********************************************************************************************************
*                               		IRQ_PRIORITY
*如果中断函数里需要打印信息，则此中断优先级要低于uart中断
*********************************************************************************************************
*/

enum IRQ_PRIORITY{
	EXTI0_IRQn_Priority = 0,
	OTG_FS_IRQn_Priority,	
	SPI1_IRQn_Priority,	
	TIM4_IRQn_Priority,	
	USART1_IRQn_Priority,
	
	USART2_IRQn_Priority,	
	USART3_IRQn_Priority,

};


/***************************************
WWDG_IRQn_Priority,
PVD_IRQn_Priority,
TAMP_STAMP_IRQn_Priority,
RTC_WKUP_IRQn_Priority,	
FLASH_IRQn_Priority,	
RCC_IRQn_Priority,	
EXTI0_IRQn_Priority,	
EXTI1_IRQn_Priority,	
EXTI2_IRQn_Priority,	
EXTI3_IRQn_Priority,	
EXTI4_IRQn_Priority,	
DMA1_Stream0_IRQn_Priority,	
DMA1_Stream1_IRQn_Priority,	
DMA1_Stream2_IRQn_Priority,	
DMA1_Stream3_IRQn_Priority,	
DMA1_Stream4_IRQn_Priority,	
DMA1_Stream5_IRQn_Priority,	
DMA1_Stream6_IRQn_Priority,	
ADC_IRQn_Priority,	
CAN1_TX_IRQn_Priority,	
CAN1_RX0_IRQn_Priority,	
CAN1_RX1_IRQn_Priority,	
CAN1_SCE_IRQn_Priority,	
EXTI9_5_IRQn_Priority,	
TIM1_BRK_TIM9_IRQn_Priority,	
TIM1_UP_TIM10_IRQn_Priority,	
TIM1_TRG_COM_TIM11_IRQn_Priority,	
TIM1_CC_IRQn_Priority,	
TIM2_IRQn_Priority,	
TIM3_IRQn_Priority,	
TIM4_IRQn_Priority,	
I2C1_EV_IRQn_Priority,	
I2C1_ER_IRQn_Priority,	
I2C2_EV_IRQn_Priority,	
I2C2_ER_IRQn_Priority,	
SPI1_IRQn_Priority,	
SPI2_IRQn_Priority,	
USART1_IRQn_Priority,	
USART2_IRQn_Priority,	
USART3_IRQn_Priority,	
EXTI15_10_IRQn_Priority,	
RTC_Alarm_IRQn_Priority,	
OTG_FS_WKUP_IRQn_Priority,	
TIM8_BRK_TIM12_IRQn_Priority,	
TIM8_UP_TIM13_IRQn_Priority,	
TIM8_TRG_COM_TIM14_IRQn_Priority,	
TIM8_CC_IRQn_Priority,	
DMA1_Stream7_IRQn_Priority,	
FMC_IRQn_Priority,	
SDIO_IRQn_Priority,	
TIM5_IRQn_Priority,	
SPI3_IRQn_Priority,	
UART4_IRQn_Priority,	
UART5_IRQn_Priority,	
TIM6_DAC_IRQn_Priority,	
TIM7_IRQn_Priority,	
DMA2_Stream0_IRQn_Priority,	
DMA2_Stream1_IRQn_Priority,	
DMA2_Stream2_IRQn_Priority,	
DMA2_Stream3_IRQn_Priority,	
DMA2_Stream4_IRQn_Priority,	
ETH_IRQn_Priority,	
ETH_WKUP_IRQn_Priority,	
CAN2_TX_IRQn_Priority,	
CAN2_RX0_IRQn_Priority,	
CAN2_RX1_IRQn_Priority,	
CAN2_SCE_IRQn_Priority,	
OTG_FS_IRQn_Priority,	
DMA2_Stream5_IRQn_Priority,	
DMA2_Stream6_IRQn_Priority,	
DMA2_Stream7_IRQn_Priority,	
USART6_IRQn_Priority,	
I2C3_EV_IRQn_Priority,	
I2C3_ER_IRQn_Priority,	
OTG_HS_EP1_OUT_IRQn_Priority,	
OTG_HS_EP1_IN_IRQn_Priority,	
OTG_HS_WKUP_IRQn_Priority,	
OTG_HS_IRQn_Priority,	
DCMI_IRQn_Priority,	
HASH_RNG_IRQn_Priority,	
FPU_IRQn_Priority,	
UART7_IRQn_Priority,	
UART8_IRQn_Priority,	
SPI4_IRQn_Priority,	
SPI5_IRQn_Priority,	
SPI6_IRQn_Priority,	
SAI1_IRQn_Priority,	
LTDC_IRQn_Priority,	
LTDC_ER_IRQn_Priority,	
DMA2D_IRQn_Priority,
******************************************/
#ifdef __cplusplus
}
#endif

#endif

