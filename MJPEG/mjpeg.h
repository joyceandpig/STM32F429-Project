#ifndef __MJPEG_H
#define __MJPEG_H 
#include "stdio.h" 
#include <cdjpeg.h> 
#include <sys.h> 
#include <setjmp.h>
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//MJPEG��Ƶ���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   



struct my_error_mgr {
  struct jpeg_error_mgr pub;	
  jmp_buf setjmp_buffer;		//for return to caller 
}; 
typedef struct my_error_mgr * my_error_ptr;

u8 mjpegdec_init(u16 offx,u16 offy);
void mjpegdec_free(void);
u8 mjpegdec_decode(u8* buf,u32 bsize);

#endif

