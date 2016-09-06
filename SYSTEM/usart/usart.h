#ifndef _USART_H
#define _USART_H 
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "ucos_ii.h"
#include "mytypes.h" 
#include "user_config.h"
#include "usmart.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"


///////////////////////////////宏定义操作////////////////////////////////////////
#define __HAL_USART_SET_DAT_TO_DR(__HANDLE__, __VAL__) (((__HANDLE__)->Instance->DR) == (__VAL__))


///////////////////////////////////////////////////////////////
//串口1定义及数据区域
#ifdef USART1_EN
	#define USART1_TX_EN      1
	#define USART1_RX_EN      1
	#define USART1_IRQn_EN    1
	
	#define USART1_TX_GPIO_PIN               	 GPIO_PIN_9 
	#define USART1_RX_GPIO_PIN           		   GPIO_PIN_10
	#define USART1_GPIO_PORT           				 GPIOA   
	#define USART1_GPIO_PORT_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
	#define USART1_RCC_CLK_ENABLE()      			 __HAL_RCC_USART1_CLK_ENABLE()
	

	extern UART_HandleTypeDef USART1_Handler; //UART句柄
	extern u8 aRxBuffer;//HAL库使用的串口接收缓冲
	extern char aTxBuffer[1024];
	//如果想串口中断接收，请不要注释以下宏定义
	void uart1_init(u32 bound);
#endif  //end USART1_EN

////////////////////////////////////////////////////////////////
//串口2定义及数据区域
#ifdef USART2_EN
	#define USART2_TX_EN    1
	#define USART2_RX_EN    1
	#define USART2_IRQn_EN  1
	
	#define USART2_TX_GPIO_PIN               	 GPIO_PIN_9 
	#define USART2_RX_GPIO_PIN           		   GPIO_PIN_10
	#define USART2_GPIO_PORT           				 GPIOA   
	#define USART2_GPIO_PORT_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
	#define USART2_RCC_CLK_ENABLE()      			 __HAL_RCC_USART1_CLK_ENABLE()
#endif   //end USART2_EN

////////////////////////////////////////////////////////////////
//串口3定义及数据区域
#ifdef USART3_EN
	#define USART3_TX_EN  1
	#define USART3_RX_EN	1           //0,不接收;1,接收.
	#define USART3_IRQn_EN  1
	
	#define USART3_TX_GPIO_PIN               	 GPIO_PIN_10 
	#define USART3_RX_GPIO_PIN           		   GPIO_PIN_11
	#define USART3_GPIO_PORT           				 GPIOB   
	#define USART3_GPIO_PORT_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
	#define USART3_RCC_CLK_ENABLE()      			 __HAL_RCC_USART3_CLK_ENABLE()

	#define USART3_MAX_RECV_LEN		400					//最大接收缓存字节数
	#define USART3_MAX_SEND_LEN		400					//最大发送缓存字节数			
	
	extern UART_HandleTypeDef USART3_Handler; //UART句柄
	extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
	extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
	extern vu16 USART3_RX_STA;   						//接收数据状态

	void uart3_init(u32 bound);
	void u3_printf(char* fmt,...);
	#endif //end USART3_EN

#endif  //end _USART_H
