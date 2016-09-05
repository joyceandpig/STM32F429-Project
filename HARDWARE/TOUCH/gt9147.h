#ifndef __GT9147_H
#define __GT9147_H	
#include "mytypes.h"  
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//4.3����ݴ�����-GT9147 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/28
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved		 
////////////////////////////////////////////////////////////////////////////////// 

#define GT_RST_GPIO_PIN_NUM           		 8
#define GT_INT_GPIO_PIN_NUM           		 7

#define GT_RST_GPIO_PORT               		 GPIOI
#define GT_INT_GPIO_PORT               		 GPIOH

#define GT_RST_GPIO_PORT_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define GT_INT_GPIO_PORT_CLK_ENABLE()           __HAL_RCC_GPIOH_CLK_ENABLE()

////IO��������											   
#define	GT_RST_SET()     HAL_GPIO_WritePin(GT_RST_GPIO_PORT, GT_RST_GPIO_PIN_NUM, GPIO_PIN_SET) //SCL	 
#define	GT_RST_CLEAR()   HAL_GPIO_WritePin(GT_RST_GPIO_PORT, GT_RST_GPIO_PIN_NUM, GPIO_PIN_RESET) //SCL

#define	GT_INT()     HAL_GPIO_ReadPin(GT_INT_GPIO_PORT, GT_INT_GPIO_PIN_NUM) //SDA	  

////IO��������	 
//#define GT_RST  PIout(8) //GT9147��λ����
//#define GT_INT  PHin(7)  //GT9147�ж�����		
 
//I2C��д����	
#define GT_CMD_WR 		0X28     	//д����
#define GT_CMD_RD 		0X29		//������
  
//GT9147 ���ּĴ������� 
#define GT_CTRL_REG 	0X8040   	//GT9147���ƼĴ���
#define GT_CFGS_REG 	0X8047   	//GT9147������ʼ��ַ�Ĵ���
#define GT_CHECK_REG 	0X80FF   	//GT9147У��ͼĴ���
#define GT_PID_REG 		0X8140   	//GT9147��ƷID�Ĵ���

#define GT_GSTID_REG 	0X814E   	//GT9147��ǰ��⵽�Ĵ������
#define GT_TP1_REG 		0X8150  	//��һ�����������ݵ�ַ
#define GT_TP2_REG 		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG 		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG 		0X8170		//��������������ݵ�ַ  
 
 
u8 GT9147_Send_Cfg(u8 mode);
u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len);
void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len); 
u8 GT9147_Init(void);
u8 GT9147_Scan(u8 mode); 
#endif













