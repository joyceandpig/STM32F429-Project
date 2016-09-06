#ifndef _MYIIC_H
#define _MYIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//IIC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
/* Definition for I2Cx clock resources */
#define I2Cx                             I2C2
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_4
#define I2Cx_SCL_GPIO_PORT              GPIOH
#define I2Cx_SCL_AF                     GPIO_AF4_I2C2

#define I2Cx_SDA_PIN                    GPIO_PIN_5
#define I2Cx_SDA_GPIO_PORT              GPIOH
#define I2Cx_SDA_AF                     GPIO_AF4_I2C2

#define I2C_ADDRESS        0xA0

////IO��������
//#define SDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	//PH5����ģʽ
//#define SDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} //PH5���ģʽ
////IO����
//#define IIC_SCL   PHout(4) //SCL
//#define IIC_SDA   PHout(5) //SDA
//#define READ_SDA  PHin(5)  //����SDA

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
//void IIC_Start(void);				//����IIC��ʼ�ź�
//void IIC_Stop(void);	  			//����IICֹͣ�ź�
//void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
//u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
//u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
//void IIC_Ack(void);					//IIC����ACK�ź�
//void IIC_NAck(void);				//IIC������ACK�ź�


#endif

