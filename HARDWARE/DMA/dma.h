#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//DMA ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
#define __HAL_DMA_TypeDef_SET_LISR(__HANDLE__, __VAL__)   ((uint32_t)((__HANDLE__)->LISR   |= (__VAL__)))
#define __HAL_DMA_TypeDef_SET_HISR(__HANDLE__, __VAL__)   ((uint32_t)((__HANDLE__)->HISR   |= (__VAL__)))
#define __HAL_DMA_TypeDef_SET_LIFCR(__HANDLE__, __VAL__)  ((uint32_t)((__HANDLE__)->LIFCR |= (__VAL__)))
#define __HAL_DMA_TypeDef_SET_HIFCR(__HANDLE__, __VAL__)  ((uint32_t)((__HANDLE__)->HIFCR |= (__VAL__)))


void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr);//����DMAx_CHx
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//ʹ��һ��DMA����		   
#endif






























