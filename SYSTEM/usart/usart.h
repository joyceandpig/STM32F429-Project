#ifndef _USART_H
#define _USART_H
#include "mytypes.h"  	

extern UART_HandleTypeDef UART1_Handler; //UART���

extern u8 aRxBuffer;//HAL��ʹ�õĴ��ڽ��ջ���
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);


#endif
