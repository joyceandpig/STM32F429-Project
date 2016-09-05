#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"  
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////

/* Definition for DS18B20_GPIO clock resources */
#define DS18B20_TRANS_FACTOR                    0.625   //�¶�ת��ϵ��
#define DS18B20_GPIO_DATA_CMD_PIN_NUM           12
#define DS18B20_GPIO_SDA_GPIO_PORT              GPIOB
#define DS18B20_GPIO_DATA_CMD_PIN               GPIO_PIN_12
#define DS18B20_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()


#define DS18B20_GPIO_MODER_CLEAR(_PORT_,_PIN_)    (_PORT_->MODER &= ~(3<<(_PIN_*2)))
#define DS18B20_GPIO_MODER_SET_IN(_PORT_,_PIN_)  	(_PORT_->MODER |=  (0<<(_PIN_*2)))
#define DS18B20_GPIO_MODER_SET_OUT(_PORT_,_PIN_)  (_PORT_->MODER |=  (1<<(_PIN_*2)))

//IO��������
#define DS18B20_IO_IN()  do{DS18B20_GPIO_MODER_CLEAR(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN_NUM);\
														DS18B20_GPIO_MODER_SET_IN(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN_NUM);\
														}while(0);	//PB12����ģʽ
#define DS18B20_IO_OUT() do{DS18B20_GPIO_MODER_CLEAR(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN_NUM);\
														DS18B20_GPIO_MODER_SET_OUT(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN_NUM);\
														}while(0); 	//PB12���ģʽ

////IO��������											   
#define	DS18B20_DQ_OUT_SET   HAL_GPIO_WritePin(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN,GPIO_PIN_SET) //���ݶ˿�	 
#define	DS18B20_DQ_OUT_CLEAR HAL_GPIO_WritePin(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN,GPIO_PIN_RESET) //���ݶ˿�	 
#define	DS18B20_DQ_IN_STA    HAL_GPIO_ReadPin(DS18B20_GPIO_SDA_GPIO_PORT,DS18B20_GPIO_DATA_CMD_PIN)  //���ݶ˿�	


//////ROM FUNCTION COMMANDS		
#define READ_ROM 						0x33
#define MATCH_ROM 					0x55
#define SKIP_ROM 						0xcc
#define SEARCH_ROM 					0xf0
#define ALARM_SEARCH_ROM 		0xec
														
///MEMORY COMMAND FUNCTIONS
#define WRITE_SCRATCHPAD 		0x4e
#define READ_SCRATCHPAD 		0xbe
#define COPY_SCRATCHPAD 		0x48
#define CONVERT 						0x44													
#define RECALL_E2 					0xb8
#define READ_POWER_SUPPLY 	0xb4
														
													
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif















