#ifndef __GSM_H__
#define __GSM_H__	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//GSMģ���ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved 
////////////////////////////////////////////////////////////////////////////////// 	
 
#define GSM_MAX_NEWMSG	10		//���10������Ϣ

typedef struct 
{							  
 	u8 status;		//gsm״̬
					//bit7:0,û���ҵ�ģ��;1,�ҵ�ģ����
					//bit6:0,SIM��������;1,SIM������
					//bit5:0,δ�����Ӫ������;1,�ѻ����Ӫ������
					//bit4:0,�й��ƶ�;1,�й���ͨ
					//[3:0]:����
	
	u8 mode;		//��ǰ����ģʽ
					//0,��������ģʽ/����ģʽ
					//1,������
					//2,ͨ����
					//3,������Ӧ��
	
	vu8 cmdon;		//����Ƿ���ָ���ڷ��͵ȴ�״̬
					//0,û��ָ���ڵȴ���Ӧ
					//1,��ָ���ڵȴ���Ӧ
	
	u8 csq;			//�ź�����
	
	vu8 newmsg;		//����Ϣ����,0,û������Ϣ;����,����Ϣ����
	u8 newmsgindex[GSM_MAX_NEWMSG];//�¶�����SIM���ڵ�����,���¼GSM_MAX_NEWMSG���¶���
	u8 incallnum[20];//������뻺����,�20λ
}__gsmdev; 

extern __gsmdev gsmdev;	//gsm������

 
 
#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//�ߵ��ֽڽ����궨��

 

u8* gsm_check_cmd(u8 *str);
u8 gsm_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void gsm_cmd_over(void);
u8 gsm_chr2hex(u8 chr);
u8 gsm_hex2chr(u8 hex);
void gsm_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
void gsm_cmsgin_check(void);
void gsm_status_check(void);
#endif





