#ifndef __CALCULATOR_H
#define __CALCULATOR_H 	
#include "common.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-��ѧ������ ����	   
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
extern u8*const CALC_UP_BTN_TBL[29];	//�ɿ�ʱ����ͼ��·��
extern u8*const CALC_DOWN_BTN_TBL[29];	//����ʱ����ͼ��·��
////////////////////////////////////////////////////////////////////////////////////////////
__packed typedef struct 
{							  
	u16 xoff;	//x����ƫ��:2,10,10
	u16 yoff;	//y����ƫ��:10,20,50
	u16 width;	//���
	u16 height;	//�߶�
	u8 xdis;	//��������x��������С:2,4,2
	u8 ydis;	//��������y��������С:5,11,19
	u8 fsize;	//����������С:28,36,60 
}_calcdis_obj;
extern _calcdis_obj *cdis;


u8 calc_play(void);
u8 calc_show_res(_calcdis_obj*calcdis,double res);
void calc_input_fresh(_calcdis_obj*calcdis,u8* calc_sta,u8 *inbuf,u8 len);
void calc_ctype_show(_calcdis_obj*calcdis,u8 ctype);
void calc_show_flag(_calcdis_obj*calcdis,u8 fg);
void calc_show_exp(_calcdis_obj*calcdis,short exp);
u8 calc_exe(_calcdis_obj*calcdis,double *x1,double *x2,u8 *buf,u8 ctype,u8 *calc_sta);
void calc_show_inbuf(_calcdis_obj*calcdis,u8 *buf);
void calc_load_ui(_calcdis_obj*calcdis);
					   
#endif























