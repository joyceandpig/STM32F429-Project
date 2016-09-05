#ifndef __SAI_H
#define __SAI_H
#include "sys.h"    		
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_dma_ex.h"
//#inlucde "stm32f4xx_hal.h"
//#inlucde "stm32f4xx_hal_sai.h"
//#inlucde "stm32f4xx_hal_sai_ex.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//SAI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/11
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved	
//********************************************************************************
//V1.2 20160111
//����SAIB_Init��SAIA_RX_DMA_Init��SAI_Rec_Start��SAI_Rec_Stop�Ⱥ���		
//V1.1 20160602
//SAIA_RX_DMA_Init��ΪSAIB_RX_DMA_Init								   
////////////////////////////////////////////////////////////////////////////////// 	

extern void (*sai_tx_callback)(void);		//sai tx�ص�����ָ��  
extern void (*sai_rx_callback)(void);		//sai rx�ص�����

void SAIA_Init(u8 mode,u8 cpol,u8 datalen);
void SAIB_Init(u8 mode,u8 cpol,u8 datalen);
u8 SAIA_SampleRate_Set(u32 samplerate);
void SAIA_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num,u8 width);
void SAIB_RX_DMA_Init(u8* buf0,u8 *buf1,u16 num,u8 width);
void SAI_Play_Start(void); 
void SAI_Play_Stop(void);  
void SAI_Rec_Start(void);
void SAI_Rec_Stop(void);
#endif





















