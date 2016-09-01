#ifndef __LYRIC_H
#define __LYRIC_H
#include "sys.h"
#include "includes.h" 	   	 
#include "common.h"	    
#include "ff.h"			
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//�����ʾʵ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 			    
#define LYRIC_BUFFER_SIZE		512		//��ʻ���,512�ֽ�
#define LYRIC_MAX_SIZE			200		//���������
//LYRIC�ṹ
__packed typedef struct 
{ 
	u32 time_tbl[LYRIC_MAX_SIZE];	//LRCʱ���
	u16 addr_tbl[LYRIC_MAX_SIZE];	//LRC��ַ��
	u16 indexsize; 					//LRC��С,���������(�����ظ���).
	u16 curindex;					//��ǰλ��
	u32	curtime;					//��ǰʱ��,��λ10ms
 	//oldostime,����ʱ����
 	u32	oldostime;					//��һ��ucos��ʱ��
	//ʵ�ֹ���Ч����4����Ҫ����
 	u8  detatime;					//��һ�ι�����,������10ms����
	u8  updatetime;					//����ʱ��:����ǰ����ǰ�������ʵ�ʱ���,�Լ������ʵĳ���ȷ���Ĺ���ʱ����
  	u16 namelen;
	u16 curnamepos;
	//
	//u16 bkcolor;					//��ʱ���ɫ
	u16 color;						//�����ɫ
	u8 	font;						//�������

	FIL* flrc;						//LRC�ļ�
	u8 buf[LYRIC_BUFFER_SIZE];		//LRC����ļ�������	
	u16 *lrcbkcolor[7];				//7����ʵı���ɫ��
}_lyric_obj;


void lrc_chg_suffix(u8*name,u8*sfx);
_lyric_obj* lrc_creat(void);
void lrc_delete(_lyric_obj* lcrdelete);
u8 lrc_str2num(u8 *str);
void lrc_analyze(_lyric_obj* lrcx,u16 lrcpos,u8*str);
void lrc_sequence(_lyric_obj* lrcx);
u8 lrc_read(_lyric_obj* lrcx,u8*path,u8 *name);
void lrc_show_linelrc(_lyric_obj* lrcx,u16 x,u16 y,u16 width,u16 height);


u8 lrc_test(u8 * path,u8 *name); 
#endif












