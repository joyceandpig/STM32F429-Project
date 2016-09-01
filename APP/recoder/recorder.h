#ifndef __RECORDER_H
#define __RECORDER_H 	
#include "common.h"
#include "wavplay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-¼���� ����	   
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

#define SAI_RX_DMA_BUF_SIZE    	4096		//����RX DMA �����С
#define SAI_RX_FIFO_SIZE		10			//�������FIFO��С

////////////////////////////////////////////////////////////////////////////////////////////
//��ͼ��/ͼƬ·��
extern u8*const RECORDER_DEMO_PIC;			//demoͼƬ·�� 	      
extern u8*const RECORDER_RECR_PIC;			//¼�� �ɿ�
extern u8*const RECORDER_RECP_PIC;			//¼�� ����
extern u8*const RECORDER_PAUSER_PIC;		//��ͣ �ɿ�
extern u8*const RECORDER_PAUSEP_PIC;		//��ͣ ����
extern u8*const RECORDER_STOPR_PIC;			//ֹͣ �ɿ�
extern u8*const RECORDER_STOPP_PIC;			//ֹͣ ����
////////////////////////////////////////////////////////////////////////////////////////////
 
	 		
u8 rec_sai_fifo_read(u8 **buf);
u8 rec_sai_fifo_write(u8 *buf);

void rec_sai_dma_rx_callback(void); 
void recorder_enter_rec_mode(void);
void recorder_stop_rec_mode(void);
void recoder_set_samplerate(__WaveHeader* wavhead,u16 samplerate); 
void recorder_wav_init(__WaveHeader* wavhead);//��ʼ��WAVͷ	
u8 recorder_vu_get(u16 signallevel);
void recorder_vu_meter(u16 x,u16 y,u8 level);
void recorder_show_time(u16 x,u16 y,u32 tsec);
void recorder_show_name(u16 x,u16 y,u8 *name);
void recorder_show_samplerate(u16 x,u16 y,u16 samplerate);
void recorder_load_ui(void);
void recorder_new_pathname(u8 *pname);
void recorder_show_agc(u16 x,u16 y,u8 agc);
u8 recorder_agc_set(u16 x,u16 y,u8 *agc,u8*caption);
u8 recorder_play(void);		   			   
#endif























