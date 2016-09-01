#ifndef __VIDEOPLAY_H
#define __VIDEOPLAY_H 
#include <stm32f4xx.h> 
#include "avi.h"
#include "ff.h"
#include "includes.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-��Ƶ������ ����	   
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
 
#define AVI_AUDIO_BUF_SIZE    1024*5		//����avi����ʱ,��Ƶbuf��С.
#define AVI_VIDEO_BUF_SIZE    1024*60		//����avi����ʱ,��Ƶbuf��С.

//��Ƶ���ſ�����
typedef __packed struct
{    
	u8 *path;			//��ǰ�ļ���·�� 
	u8 *name;			//��ǰ��������
	vu8 status;			//bit0:0,��ͣ����;1,��������
						//bit1:0,���/������;1,��������
						//����,����
	
	u16 curindex;		//��ǰ���ŵ���Ƶ�ļ�����
	u16 mfilenum;		//��Ƶ�ļ���Ŀ	    
	u32 *mfindextbl;	//��Ƶ�ļ�������
	
	FIL *file;			//��Ƶ�ļ�ָ�� 	
	vu8 saiplaybuf;		//�������ŵ���Ƶ֡������
	u8* saibuf[4]; 		//��Ƶ����֡,��4֡,4*AVI_AUDIO_BUF_SIZE
}__videodev; 
extern __videodev videodev;//��Ƶ���ſ�����

//��Ƶ���Ž���,UIλ�ýṹ��
typedef __packed struct
{  
	//����������ز���
	u8 tpbar_height;	//�����������߶�
	u8 capfsize;		//���������С
	u8 msgfsize;		//��ʾ��Ϣ�����С(��Ƶ��/����/��Ƶ��/��Ƶ������/֡��/�ֱ���/����ʱ����)
	
	//�м���Ϣ����ز���
	u8 msgbar_height;	//��Ϣ���߶�
	u8 nfsize;			//��Ƶ�������С
	u8 xygap;			//x,y�����ƫ����,������/����ͼ��/֡�ʼ�� 1 gap,������Ϣ,1/2 gap
	u16 vbarx;			//������x����
	u16 vbary;			//������y����
	u16 vbarwidth;		//����������
	u16 vbarheight;		//���������
	u8 msgdis;			//������(������) dis+����ͼ��+vbar+dis+������+֡��+dis
	
	//���Ž�����ز���  
	u8 prgbar_height;	//�������߶�
	u16 pbarwidth;		//����������
	u16 pbarheight;		//���������
	u16 pbarx;			//������x����
	u16 pbary;			//������y����
	
	//��ť����ز���
	u8 btnbar_height;	//��ť���߶� 
}__videoui; 
extern __videoui *vui;				//��Ƶ���Ž��������

 
void video_time_show(u16 sx,u16 sy,u16 sec);
void video_load_ui(void);
void video_show_vol(u8 pctx);
void video_info_upd(__videodev *videodevx,_progressbar_obj* videoprgbx,_progressbar_obj* volprgbx,AVI_INFO *aviinfo,u8 flag);
u8 video_play(void);
u8 video_play_mjpeg(void); 

u8 video_seek(__videodev *videodevx,AVI_INFO *aviinfo,u8 *mbuf,u32 dstpos);

#endif











