#ifndef __CAMERA_H
#define __CAMERA_H 	
#include "common.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-����� ����	   
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
 
extern vu8 hsync_int;					//֡�жϱ�־
extern vu32 jpeg_data_len; 				//buf�е�JPEG��Ч���ݳ��� 
extern vu8 jpeg_data_ok;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
										
extern u32 *jpeg_buf;		 			//JPEG���ݻ���buf,ͨ��malloc�����ڴ�


void jpeg_data_process(void);
void sw_ov5640_mode(void);
void sw_sdcard_mode(void);
void camera_new_pathname(u8 *pname,u8 mode);
u8 ov5640_jpg_photo(u8 *pname);
u8 camera_play(void);
#endif























