#ifndef __W25QXX_H
#define __W25QXX_H
#include "mytypes.h"
#include "sys.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������ 
//W25QXX��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/28
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1�޸�˵�� 20160107
//1,W25QXX_Write���Ӷ�̬�ڴ����
////////////////////////////////////////////////////////////////////////////////// 	
 
//W25Xϵ��/Qϵ��оƬ�б�	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
//W25Q256 ID  0XEF18
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern u16 W25QXX_TYPE;					//����W25QXXоƬ�ͺ�		

#define W25QXX_CS_GPIO_PIN_NUM           		 6
#define W25QXX_CS_GPIO_PIN           		 GPIO_PIN_6
#define W25QXX_CS_GPIO_PORT               		 GPIOF
#define W25QXX_CS_GPIO_PORT_CLK_ENBALE()        __HAL_RCC_GPIOF_CLK_ENABLE()
////IO��������											   
#define	W25QXX_CS_SET()     HAL_GPIO_WritePin(W25QXX_CS_GPIO_PORT, W25QXX_CS_GPIO_PIN_NUM, GPIO_PIN_SET) //SCL	 
#define	W25QXX_CS_CLEAR()   HAL_GPIO_WritePin(W25QXX_CS_GPIO_PORT, W25QXX_CS_GPIO_PIN_NUM, GPIO_PIN_RESET) //SCL

//#define	W25QXX_CS 			PFout(6)	//W25QXX��Ƭѡ�ź�

#define W25QXX_USE_MALLOC	1			//�����Ƿ�ʹ�ö�̬�ڴ����

////////////////////////////////////////////////////////////////////////////////// 
//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//��ȡFLASH ID
u8 W25QXX_ReadSR(u8 regno);             //��ȡ״̬�Ĵ��� 
void W25QXX_4ByteAddr_Enable(void);     //ʹ��4�ֽڵ�ַģʽ
void W25QXX_Write_SR(u8 regno,u8 sr);   //д״̬�Ĵ���
void W25QXX_Write_Enable(void);  		//дʹ�� 
void W25QXX_Write_Disable(void);		//д����
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void W25QXX_Erase_Chip(void);    	  	//��Ƭ����
void W25QXX_Erase_Sector(u32 Dst_Addr);	//��������
void W25QXX_Wait_Busy(void);           	//�ȴ�����
void W25QXX_PowerDown(void);        	//�������ģʽ
void W25QXX_WAKEUP(void);				//����

#ifdef DEBUG
void W25QXX_Test(u8 n);
#endif

#endif
