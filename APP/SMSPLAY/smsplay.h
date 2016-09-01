#ifndef __SMSPLAY_H
#define __SMSPLAY_H 
#include "common.h"  
#include "gsm.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-���Ų��� ����	   
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
#define SMS_RMEMO_MAXLEN		2000		//����������2K�ֽ�����

#define SMS_PNUM_BACK_COLOR		0X42F0		//�ֻ�����(��ϵ��/�ռ���)���򱳾�ɫ
#define SMS_PNUM_NAME_COLOR		WHITE		//��ϵ��/�ռ���������ɫ
#define SMS_PNUM_NUM_COLOR		WHITE		//������ɫ

#define SMS_COM_RIM_COLOR		0X9D36		//ͨ�÷ָ���

#define SMS_RMSG_BACK_COLOR		0XBE3B		//���Ž���ʱ�����򱳾�ɫ
#define SMS_RMSG_FONT_COLOR		BLACK		//ʱ��������ɫ

#define SMS_RMEMO_BACK_COLOR	WHITE		//���յĶ���MEMO�ؼ�����ɫ
#define SMS_RMEMO_FONT_COLOR	BLACK		//���յĶ���������ɫ

#define SMS_SAREA_BACK_COLOR	0XA599  	//�������򱳾�ɫ
#define SMS_SMEMO_BACK_COLOR	WHITE		//���Ͷ���MEMO�ؼ�����ɫ
#define SMS_SMEMO_FONT_COLOR	BLACK		//���Ͷ���������ɫ


//��������ڵ�
typedef  struct __sms_node 
{	
	u8* pnum;	//�绰����ָ��
	u8* msg;	//��������ָ��
	u8* time;	//����ʱ��ָ��
	u8 *item;	//����չʾ��ָ��=pnum+msg+time
	u8 index;	//�ö�����SIM����λ��
	u8 readed;	//0,δ������;1,�Ѷ�����.
	struct __sms_node *next;//ָ����һ���ڵ�
}sms_node; 

sms_node* sms_node_creat(void);

void sms_node_free(sms_node * pnode,u8 mode);
u8 sms_node_insert(sms_node* head,sms_node *pnew,u16 i);
u8 sms_node_delete(sms_node* head,u16 i);
sms_node* sms_node_getnode(sms_node* head,u16 i);
u16 sms_node_getsize(sms_node* head);
void sms_node_destroy(sms_node* head);

void sms_remind_msg(u8 mode);
u8 sms_read_sms(sms_node *pnode,u16 index,u8 mode);
u8 sms_delete_sms(sms_node **head,u16 index);
u8 sms_get_smsnum(u16 *num,u16 *max);
u8 sms_msg_select(u16 *sel,u16 *top,u8 * caption,sms_node* head);
u16 sms_read_all_sms(sms_node *head,u16 *maxnum);
void sms_send_sms(sms_node* pnode,u8 mode);
u8 sms_play(void);
#endif



















