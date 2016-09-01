#ifndef __PHONEPLAY_H
#define __PHONEPLAY_H 	
#include "common.h"  
#include "gsm.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-������ ����	   
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

#define PHONE_MAX_INPUT		200		//�������200�����볤�� 

 

extern u8*const PHONE_CALL_PIC[3];
extern u8*const PHONE_HANGUP_PIC[3];
extern u8*const PHONE_DEL_PIC[3];

__packed typedef struct 
{		
	u16 xoff;		//x����ƫ��
	u16 yoff;		//y����ƫ��
	u16 width;		//���
	u16 height;		//�߶�
	
	u16 time;		//ͨ��ʱ��
	
	u8 *inbuf;		//�������ֻ�����
	u8 inlen;		//�������ֳ���
	u8 *phonebuf;	//���뻺����
	u8 plen;		//���볤��
	u8 psize;		//���������С:28,36,54 
	u8 tsize;		//ʱ�������С:12,16,24 
}_phonedis_obj;
extern _phonedis_obj *pdis;


void phone_show_clear(_phonedis_obj *pdis);
void phone_show_time(_phonedis_obj *pdis);
void phone_show_calling(_phonedis_obj *pdis,__gsmdev *gsmx);
void phone_show_phone(_phonedis_obj *pdis,__gsmdev*gsmx);
void phone_show_input(_phonedis_obj *pdis,__gsmdev*gsmx);
void phone_ring(void);
void phone_incall_task(void *pdata);
u8 phone_incall_task_creat(void);
void phone_incall_task_delete(void);
u8 phone_play(void); 
#endif























