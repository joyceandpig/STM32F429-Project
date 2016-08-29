#ifndef _USART_H
#define _USART_H
#include "mytypes.h"  	

extern UART_HandleTypeDef UART1_Handler; //UART句柄

extern u8 aRxBuffer;//HAL库使用的串口接收缓冲
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);


#endif
