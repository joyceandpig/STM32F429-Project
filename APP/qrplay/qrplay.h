#ifndef __QRPLAY_H
#define __QRPLAY_H 	
#include "common.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-二维码识别&编码 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define QR_BACK_COLOR			0XA599		//背景色

//在qrplay.c里面定义
extern vu8 qr_dcmi_rgbbuf_sta;				//RGB BUF状态
extern u16 qr_dcmi_curline;					//摄像头输出数据,当前行编号  

void qr_cursor_show(u8 csize);
void qr_dcmi_rx_callback(void);
void qr_decode_show_result(u8* result);
void qr_decode_play(void);
void qr_encode_play(void);
void qr_play(void);
#endif























