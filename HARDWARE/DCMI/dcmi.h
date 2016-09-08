#ifndef _DCMI_H
#define _DCMI_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//DCMI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//����˵��
//V1.1  20160302
//�޸�DCMI_DMA_Init��DCMI_Init������CR�Ĵ����Ĳ�����ʽ,��ֹ��������.
//V1.2  20160515
//�޸�DMA2_Stream1_IRQn�����ȼ�����ֹ���ݸ��ǣ�����ͼƬĩβ��λ.
////////////////////////////////////////////////////////////////////////////////// 	 
//PH8,AF13  DCMI_HSYNC
//PA6,AF13  DCMI_PCLK  
//PB7,AF13  DCMI_VSYNC  
//PC6,AF13  DCMI_D0
//PC7,AF13  DCMI_D1
//PC8,AF13  DCMI_D2
//PC9,AF13  DCMI_D3
//PC11,AF13 DCMI_D4 
//PD3,AF13  DCMI_D5
//PB8,AF13  DCMI_D6
//PB9,AF13  DCMI_D7

#define DCMI_HSYNC_GPIO_PIN    GPIO_PIN_8
#define DCMI_HSYNC_GPIO_PORT    GPIOH

#define DCMI_PCLK_GPIO_PIN    GPIO_PIN_6
#define DCMI_PCLK_GPIO_PORT    GPIOA

#define DCMI_VSYNC_GPIO_PIN    GPIO_PIN_7
#define DCMI_VSYNC_GPIO_PORT    GPIOB

#define DCMI_D0_GPIO_PIN    GPIO_PIN_6
#define DCMI_D0_GPIO_PORT    GPIOC

#define DCMI_D1_GPIO_PIN    GPIO_PIN_7
#define DCMI_D1_GPIO_PORT    GPIOC

#define DCMI_D2_GPIO_PIN    GPIO_PIN_8
#define DCMI_D2_GPIO_PORT    GPIOC

#define DCMI_D3_GPIO_PIN    GPIO_PIN_9
#define DCMI_D3_GPIO_PORT    GPIOC

#define DCMI_D4_GPIO_PIN    GPIO_PIN_11
#define DCMI_D4_GPIO_PORT    GPIOC

#define DCMI_D5_GPIO_PIN    GPIO_PIN_3
#define DCMI_D5_GPIO_PORT    GPIOD

#define DCMI_D6_GPIO_PIN    GPIO_PIN_8
#define DCMI_D6_GPIO_PORT    GPIOB

#define DCMI_D7_GPIO_PIN    GPIO_PIN_9
#define DCMI_D7_GPIO_PORT    GPIOB



extern void (*dcmi_rx_callback)(void);//DCMI DMA���ջص�����

void DCMI_Init(void);
void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u8 memblen,u8 meminc);
void DCMI_Start(void);
void DCMI_Stop(void);
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync);
#endif





















