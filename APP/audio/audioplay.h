#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H
#include "sys.h"
#include "includes.h" 	   	 
#include "common.h"
#include "ff.h"	
#include "gui.h"
#include "lyric.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//���ֲ����� Ӧ�ô���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/11
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//V1.1 20160530
//�޸Ĳ��ִ��룬��֧�����µ�fatfs��R0.12��
//V1.2 
//�޸Ĵ�����֧���ۺ�ʵ�飬�ҽ����ۺ�ʵ��ʹ��
////////////////////////////////////////////////////////////////////////////////// 	
 
////////////////////////////////////////////////////////////////////////////////////////////
//��ͼ��/ͼƬ·��  
extern u8*const AUDIO_BTN_PIC_TBL[2][5];  	//5��ͼƬ��ť��·��
extern u8*const AUDIO_BACK_PIC[5];			//5������ͼƬ
extern u8*const AUDIO_PLAYR_PIC;			//���� �ɿ�
extern u8*const AUDIO_PLAYP_PIC;			//���� ����
extern u8*const AUDIO_PAUSER_PIC;			//��ͣ �ɿ�
extern u8*const AUDIO_PAUSEP_PIC;			//��ͣ ����
////////////////////////////////////////////////////////////////////////////////////////////
#define AUDIO_TITLE_COLOR   	0XFFFF		//������������ɫ	
#define AUDIO_TITLE_BKCOLOR   	0X0000		//���������ⱳ��ɫ	

#define AUDIO_INFO_COLOR   		0X8410		//��Ϣ�������ɫ	
#define AUDIO_MAIN_BKCOLOR   	0X18E3		//������ɫ	
#define AUDIO_BTN_BKCOLOR   	0XDF9F		//5�����ư�ť����ɫ	
/////////////////////////////////////////////
#define AUDIO_LRC_MCOLOR       0XF810//0XFFE0		//��ǰ�����ɫΪ��ɫ
#define AUDIO_LRC_SCOLOR       0XFFFF//0X07FF		//ǰһ��ͺ�һ������ɫΪ��ɫ			    
 
//���ֲ��Ų����������
typedef enum {
	AP_OK=0X00,				//�����������
	AP_NEXT,				//������һ��
	AP_PREV,				//������һ��
	AP_ERR=0X80,			//�����д���(û����������,������ʾ����) 
}APRESULT;

//���ֲ��ſ�����
typedef __packed struct
{  
	//2��SAI�����BUF
	u8 *saibuf1;
	u8 *saibuf2; 
	u8 *tbuf;			//��ʱ����
	FIL *file;			//��Ƶ�ļ�ָ�� 	
	u32(*file_seek)(u32);//�ļ�������˺��� 

	vu8 status;			//bit0:0,��ͣ����;1,��������
						//bit1:0,��������;1,��������  
						//bit2~3:����
						//bit4:0,�����ֲ���;1,���ֲ����� (������)		
						//bit5:0,�޶���;1,ִ����һ���и����(������)
						//bit6:0,�޶���;1,������ֹ����(���ǲ�ɾ����Ƶ��������),������ɺ�,���������Զ������λ
 						//bit7:0,��Ƶ����������ɾ��/����ɾ��;1,��Ƶ����������������(�������ִ��)
	
	u8 mode;			//����ģʽ
						//0,ȫ��ѭ��;1,����ѭ��;2,�������;
	
	u8 *path;			//��ǰ�ļ���·��
	u8 *name;			//��ǰ���ŵ�MP3��������
	u16 namelen;		//name��ռ�ĵ���.
	u16 curnamepos;		//��ǰ��ƫ��

    u32 totsec ;		//���׸�ʱ��,��λ:��
    u32 cursec ;		//��ǰ����ʱ�� 
    u32 bitrate;	   	//������(λ��)
	u32 samplerate;		//������ 
	u16 bps;			//λ��,����16bit,24bit,32bit
	
	u16 curindex;		//��ǰ���ŵ���Ƶ�ļ�����
	u16 mfilenum;		//�����ļ���Ŀ	    
	u32 *mfindextbl;	//��Ƶ�ļ�������
	
}__audiodev; 
extern __audiodev audiodev;	//���ֲ��ſ�����

//���ֲ��Ž���,UIλ�ýṹ��
typedef __packed struct
{  
	//����������ز���
	u8 tpbar_height;	//�����������߶�
	u8 capfsize;		//���������С
	u8 msgfsize;		//��ʾ��Ϣ�����С(������/����/������/������/λ��/���ʵ�/����ʱ����)
	u8 lrcdheight;		//����м��
	
	//�м���Ϣ����ز���
	u8 msgbar_height;	//��Ϣ���߶�
	u8 nfsize;			//�����������С
	u8 xygap;			//x,y�����ƫ����,������/����ͼ��/λ����� 1 gap,������Ϣ,1/2 gap
	u16 vbarx;			//������x����
	u16 vbary;			//������y����
	u16 vbarwidth;		//����������
	u16 vbarheight;		//���������
	u8 msgdis;			//������(������) dis+����ͼ��+vbar+dis+������+λ��+dis
	
	//���Ž�����ز���  
	u8 prgbar_height;	//�������߶�
	u16 pbarwidth;		//����������
	u16 pbarheight;		//���������
	u16 pbarx;			//������x����
	u16 pbary;			//������y����
	
	//��ť����ز���
	u8 btnbar_height;	//��ť���߶� 
}__audioui; 
extern __audioui *aui;	//���ֲ��Ž��������

//ȡ2��ֵ����Ľ�Сֵ.
#ifndef AUDIO_MIN			
#define AUDIO_MIN(x,y)	((x)<(y)? (x):(y))
#endif

void audio_start(void);
void audio_stop(void); 

void music_play_task(void *pdata);
void audio_stop_req(__audiodev *audiodevx);
u8 audio_filelist(__audiodev *audiodevx);
void audio_load_ui(u8 mode);
void audio_show_vol(u8 pctx);
void audio_time_show(u16 sx,u16 sy,u16 sec); 
void audio_info_upd(__audiodev *audiodevx,_progressbar_obj* audioprgbx,_progressbar_obj* volprgbx,_lyric_obj* lrcx);
void audio_lrc_bkcolor_process(_lyric_obj* lrcx,u8 mode);
void audio_lrc_show(__audiodev *audiodevx,_lyric_obj* lrcx);
u8 audio_task_creat(void);
void audio_task_delete(void);
u8 audio_play(void);	    

#endif












