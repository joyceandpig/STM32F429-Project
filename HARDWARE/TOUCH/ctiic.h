#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "mytypes.h"  
#include "sys.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//���ݴ�����-IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//��
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

//IO��������
#define CT_SDA_IN()  do{CT_IIC_GPIO_MODER_CLEAR(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												CT_IIC_GPIO_MODER_SET_IN(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												}while(0);	//PI3����ģʽ
#define CT_SDA_OUT() do{CT_IIC_GPIO_MODER_CLEAR(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												CT_IIC_GPIO_MODER_SET_OUT(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN_NUM);\
												}while(0); 	//PI3���ģʽ

////IO��������											   
#define	CT_IIC_SCL_SET     HAL_GPIO_WritePin(CT_IIC_GPIO_SCL_PORT, CT_IIC_GPIO_SCL_PIN, GPIO_PIN_SET) //SCL	 
#define	CT_IIC_SCL_CLEAR   HAL_GPIO_WritePin(CT_IIC_GPIO_SCL_PORT, CT_IIC_GPIO_SCL_PIN, GPIO_PIN_RESET) //SCL

#define	CT_IIC_SDA_SET     HAL_GPIO_WritePin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN, GPIO_PIN_SET) //SDA	  
#define	CT_IIC_SDA_CLEAR   HAL_GPIO_WritePin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN, GPIO_PIN_RESET) //SDA
#define	CT_IIC_SDA(_STA_)  HAL_GPIO_WritePin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_PIN, _STA_) //SDA	
												
#define	CT_READ_SDA_STA    HAL_GPIO_ReadPin(CT_IIC_GPIO_SDA_PORT, CT_IIC_GPIO_SDA_READ_PIN) //����SDA 	

 

//IIC���в�������
void CT_IIC_Init(void);                	//��ʼ��IIC��IO��				 
void CT_IIC_Start(void);				//����IIC��ʼ�ź�
void CT_IIC_Stop(void);	  				//����IICֹͣ�ź�
void CT_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC��ȡһ���ֽ�
u8 CT_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);					//IIC����ACK�ź�
void CT_IIC_NAck(void);					//IIC������ACK�ź�

#endif







