#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//OVϵ������ͷ SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define SCCB_SCL_GPIO_PIN_NUM           		 4
#define SCCB_SDA_GPIO_PIN_NUM           		 3
#define SCCB_READ_SDA_GPIO_PIN_NUM           3

#define SCCB_GPIO_SCL_PORT               		 GPIOB
#define SCCB_SCL_GPIO_PIN           		 GPIO_PIN_4
#define SCCB_SDA_GPIO_PIN          		 	 GPIO_PIN_3
#define SCCB_READ_SDA_GPIO_PIN           GPIO_PIN_3

#define SCCB_GPIO_SDA_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()


#define SCCB_GPIO_MODER_CLEAR(_PORT_,_PIN_)    (_PORT_->MODER &= ~(3<<(_PIN_*2)))
#define SCCB_GPIO_MODER_SET_IN(_PORT_,_PIN_)   (_PORT_->MODER |=  (0<<(_PIN_*2)))
#define SCCB_GPIO_MODER_SET_OUT(_PORT_,_PIN_)  (_PORT_->MODER |=  (1<<(_PIN_*2)))

//IO��������
#define 	SCCB_SDA_IN()  do{SCCB_GPIO_MODER_CLEAR(SCCB_GPIO_SCL_PORT, SCCB_SDA_GPIO_PIN_NUM);\
												SCCB_GPIO_MODER_SET_IN(SCCB_GPIO_SCL_PORT, SCCB_SDA_GPIO_PIN_NUM);\
												}while(0);	//PI3����ģʽ
#define 	SCCB_SDA_OUT() do{SCCB_GPIO_MODER_CLEAR(SCCB_GPIO_SCL_PORT, SCCB_SDA_GPIO_PIN_NUM);\
												SCCB_GPIO_MODER_SET_OUT(SCCB_GPIO_SCL_PORT, SCCB_SDA_GPIO_PIN_NUM);\
												}while(0); 	//PI3���ģʽ

////IO��������											   
#define	SCCB_SCL_SET()     HAL_GPIO_WritePin(SCCB_GPIO_SCL_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_SET) //SCL, GPIO_PIN_SET	 
#define	SCCB_SCL_CLEAR()   HAL_GPIO_WritePin(SCCB_GPIO_SCL_PORT, SCCB_SCL_GPIO_PIN, GPIO_PIN_RESET) //SCL, GPIO_PIN_RESET

#define	SCCB_SDA_SET()     HAL_GPIO_WritePin(SCCB_GPIO_SCL_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_SET) //SDA	  
#define	SCCB_SDA_CLEAR()   HAL_GPIO_WritePin(SCCB_GPIO_SCL_PORT, SCCB_SDA_GPIO_PIN, GPIO_PIN_RESET) //SDA	 

#define	SCCB_READ_SDA_STA()    HAL_GPIO_ReadPin(SCCB_GPIO_SCL_PORT, SCCB_READ_SDA_GPIO_PIN) //����SDA 	

////IO��������
//#define SCCB_SDA_IN()  {GPIOB->MODER&=~(3<<(3*2));GPIOB->MODER|=0<<3*2;}	//PB3 ����
//#define SCCB_SDA_OUT() {GPIOB->MODER&=~(3<<(3*2));GPIOB->MODER|=1<<3*2;} 	//PB3 ���

////IO��������	 
//#define SCCB_SCL    		PBout(4)	 	//SCL
//#define SCCB_SDA    		PBout(3) 		//SDA	 
//#define SCCB_READ_SDA    	PBin(3)  		//����SDA     

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void); 
#endif













