#ifndef __QRPLAY_H
#define __QRPLAY_H 	
#include "common.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-��ά��ʶ��&���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define QR_BACK_COLOR			0XA599		//����ɫ

//��qrplay.c���涨��
extern vu8 qr_dcmi_rgbbuf_sta;				//RGB BUF״̬
extern u16 qr_dcmi_curline;					//����ͷ�������,��ǰ�б��  

void qr_cursor_show(u8 csize);
void qr_dcmi_rx_callback(void);
void qr_decode_show_result(u8* result);
void qr_decode_play(void);
void qr_encode_play(void);
void qr_play(void);
#endif























