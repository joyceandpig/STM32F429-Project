#ifndef __PAINT_H
#define __PAINT_H 	
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

////////////////////////////////////////////////////////////////////////////////////////////
//��ͼ��/ͼƬ·��
extern u8*const PAINT_COLOR_TBL_PIC;//��ɫ��·��
////////////////////////////////////////////////////////////////////////////////////////////

//RGB565λ����
typedef struct 
{
	u16 b:5;
	u16 g:6;
	u16 r:5; 
}PIX_RGB565;

void paint_new_pathname(u8 *pname);
void paint_show_colorval(u16 xr,u16 yr,u16 color);
u8 paint_pen_color_set(u16 x,u16 y,u16* color,u8*caption);
u8 paint_pen_size_set(u16 x,u16 y,u16 color,u8 *mode,u8*caption);
void paint_draw_point(u16 x,u16 y,u16 color,u8 mode);	 
u8 paint_play(void);


#endif























