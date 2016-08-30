#ifndef __MEMLCD_H
#define __MEMLCD_H			 
#include "guix.h"  
#include "stdlib.h" 

#define MEMLCD_MAX_LAYER	1			//MEM LCD��֧�ֵ��Դ����

//LCD��Ҫ������
typedef struct  
{		 	 
	u16 width;							//MEM LCD ���
	u16 height;							//MEM LCD �߶� 
	u8  dir;							//���������������ƣ�0��������1��������
	u16 *grambuf[MEMLCD_MAX_LAYER];		//MEM LCD GRAM
	u8 layer;							//������,0,��һ��;1,�ڶ���;2,������,�Դ�����.
}_mlcd_dev; 	  

extern _mlcd_dev mlcddev;



u8 mlcd_init(u16 width,u16 height,u8 lx);
void mlcd_delete(void);
void mlcd_draw_point(u16 x,u16 y,u16 color);
u16 mlcd_read_point(u16 x,u16 y);
void mlcd_clear(u16 color);
void mlcd_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void mlcd_display_layer(u8 lx);

#endif















