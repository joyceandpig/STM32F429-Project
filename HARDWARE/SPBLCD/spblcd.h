#ifndef __SPBLCD_H
#define	__SPBLCD_H	   
#include "sys.h"  
#include "lcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//SPBLCD ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/20
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved				
//********************************************************************************
//�޸�˵�� 
//V1.1  20160320 
//�޸�Ϊ֧��3ҳ����.
//////////////////////////////////////////////////////////////////////////////////	  


#define SLCD_DMA_MAX_TRANS	60*1024		//DMAһ����ഫ��60K�ֽ�	
extern u16 *sramlcdbuf;					//SRAMLCD����,����SRAM ���潫ͼƬ����,������ͼ���Լ����ֵ���Ϣ

										
void slcd_draw_point(u16 x,u16 y,u16 color);
u16 slcd_read_point(u16 x,u16 y);
void slcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color);
void slcd_frame_sram2spi(u8 frame); 
void slcd_spi1_mode(u8 mode);
void slcd_dma_init(void);
void slcd_dma_enable(u32 x);  
void slcd_frame_show(u32 x);


#endif

























