#ifndef __GRADIENTER_H
#define __GRADIENTER_H 	
#include "common.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-水平仪 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/6/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
void grad_draw_hline(short x0,short y0,u16 len,u16 color);
void grad_fill_circle(short x0,short y0,u16 r,u16 color);
u8 grad_load_font(void);
void grad_delete_font(void);

u8 grad_play(void); 
#endif



