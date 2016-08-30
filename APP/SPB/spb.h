#ifndef __SPB_H
#define	__SPB_H	   
#include "spblcd.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//SPBЧ��ʵ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved							  
//*******************************************************************************
//V1.1 20160627
//������ҳ����֧��							  
////////////////////////////////////////////////////////////////////////////////// 	

#define SPB_PAGE_NUM 		3 		//SPBҳ��,ÿҳ����8��ͼ��
#define SPB_ICOS_NUM 		21 		//SPB����������ͼ����

#define SPB_MOVE_WIN 		2 		//SPB������ⴰ��ֵ
#define SPB_MOVE_MIN		8		//SPB��Ϊ�ǻ�������Сֵ,��������Ϊ�ǻ���,���ǲ�һ���ᷭҳ	   
#define SPB_MOVE_ACT 		50 		//SPB����������ֵ,�������ֵ������з�ҳ


#define SPB_ALPHA_VAL 		18 		//SPBѡ��͸��������
#define SPB_ALPHA_COLOR		WHITE	//SPB͸��ɫ
#define SPB_FONT_COLOR 		BLUE 	//SPB������ɫ
#define SPB_MICO_BKCOLOR	0XA5BB	//micoͼ�걳��ɫ

extern u8*const spb_bkpic_path_tbl[5];
extern u8*const spb_icos_path_tbl[3][SPB_ICOS_NUM];
extern u8*const spb_micos_path_tbl[3][3];	
//SPB ͼ����ƽṹ��
__packed typedef struct _m_spb_icos
{										    
	u16 x;			//ͼ�����꼰�ߴ�
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//ͼ��·��ָ��
	u8 * name;		//ͼ������ָ��
}m_spb_icos;


//SPB ������
typedef struct _m_spb_dev
{										    
	u16 oldxpos;     				//��һ��TP��x����λ��
 	u16 curxpos;     				//��ǰtp���µ�x����
	u16 curypos;     				//��ǰtp���µ�y����
	u16	spbsta;						//spb״̬
									//[15]:��һ�ΰ��±�־
									//[14]:������־;					 
									//[13~0]:��������
	
	u16 pos;                   	 	//��ǰ֡��λ��(x����)
	u16 oldpos;				 		//��������ʱ��֡λ��
	
	u8 stabarheight;				//״̬���߶�
	u16 spbheight;					//SPB��������߶�
	u16 spbwidth;					//SPB����������,һ�����LCD�ֱ��ʿ��
	u16 spbahwidth;					//SPB����Ԥ�����,һ�����LCD��ȵ�1/4
	u16 spbfontsize;				//SPB�����С
 
	u8 selico;						//��ǰѡ�е�ͼ��.
									//0~8,��ѡ�е�ͼ����	 
									//����,û���κ�ͼ�걻ѡ��	  
	m_spb_icos icos[SPB_ICOS_NUM];	//��ͼ����
	m_spb_icos micos[3];			//3����ͼ��
}m_spb_dev;
extern m_spb_dev spbdev;


u8 spb_init(void);
void spb_delete(void);
u8 spb_load_icos(void);
u8 spb_load_micos(void);
void spb_gsm_signal_show(u16 x,u16 y,u8 signal);
void spb_stabar_msg_show(u8 clr);
u8 spb_load_mui(void);
void spb_frame_move(u8 dir,u8 skips,u16 pos);
void spb_unsel_micos(u8 selx);
void spb_set_sel(u8 sel);
u8 spb_move_chk(void);
#endif




