#ifndef __APPPLAY_H
#define __APPPLAY_H 	
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-Ӧ������ ����	   
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

#define APPPLAY_EX_BACKCOLOR	0X0000			//�����ⲿ����ɫ
#define APPPLAY_IN_BACKCOLOR	0X8C51			//�����ڲ�����ɫ
#define APPPLAY_NAME_COLOR		0X001F			//��������ɫ

#define APPPLAY_ALPHA_VAL 		18 				//APPѡ��͸��������
#define APPPLAY_ALPHA_COLOR		WHITE			//APP͸��ɫ
	
	
////////////////////////////////////////////////////////////////////////////////////////////
//��ͼ��/ͼƬ·��
extern u8 * const appplay_icospath_tbl[3][16];//icos��·����
////////////////////////////////////////////////////////////////////////////////////////////


//APPͼ���������
__packed typedef struct _m_app_icos
{										    
	u16 x;			//ͼ�����꼰�ߴ�
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//ͼ��·��
	u8 * name;		//ͼ������
}m_app_icos;

//APP������
typedef struct _m_app_dev
{										    
	u8 selico;					//��ǰѡ�е�ͼ��.
								//0~15,��ѡ�е�ͼ����	 
								//����,û���κ�ͼ�걻ѡ��	  
	m_app_icos icos[14];		//�ܹ�16��ͼ��
}m_app_dev;
		 

u8 app_play(void); 
#endif























