#ifndef __GRADIENTER_H
#define __GRADIENTER_H 	
#include "common.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-ˮƽ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/6/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
void grad_draw_hline(short x0,short y0,u16 len,u16 color);
void grad_fill_circle(short x0,short y0,u16 r,u16 color);
u8 grad_load_font(void);
void grad_delete_font(void);

u8 grad_play(void); 
#endif



