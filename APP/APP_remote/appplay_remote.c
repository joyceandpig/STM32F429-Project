#include "appplay_remote.h"
#include "remote.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-����ң���� ����	   
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

///////////////////////////////////////////////////////////////////////////////////
//����ң����Ŀ��Ϣ�б�
u8*const appplay_remote_items_tbl[GUI_LANGUAGE_NUM][3]=
{
{"  ��ֵ:","  ����:","  ����:",},
{"  �Iֵ:","  �Δ�:","  ��̖:",},
{"KEYVAL:","KEYCNT:","SYMBOL:",},
};

u8 appplay_remote(u8* caption)
{
	u8 rval=0;
	u8 key;		 
	u16 sx=(lcddev.width-120)/2;
	u16 sy=(lcddev.height-100)/2;
	
 	_window_obj* twin=0;	//����
 	twin=window_creat(sx,sy,120,100,0,1<<6|1<<0,16);//��������
	if(twin)
	{	
		twin->caption=caption;					 
	 	twin->windowbkc=APP_WIN_BACK_COLOR;	 								//������ɫ
		window_draw(twin);													//������
	 	app_draw_smooth_line(sx+5,sy+32+1+22,110,1,0Xb1ffc4,0X1600b1);		//������
	 	app_draw_smooth_line(sx+5,sy+32+1+22+22,110,1,0Xb1ffc4,0X1600b1);	//������
		BACK_COLOR=APP_WIN_BACK_COLOR;
 		//��ʾ��Ŀ��Ϣ
		gui_show_ptstr(sx+8,sy+32+1+3,sx+8+56,sy+32+1+3+16,0,BLACK,16,(u8*)appplay_remote_items_tbl[gui_phy.language][0],1);
		gui_show_ptstr(sx+8,sy+32+1+3+22,sx+8+56,sy+32+1+3+22+16,0,BLACK,16,(u8*)appplay_remote_items_tbl[gui_phy.language][1],1);
		gui_show_ptstr(sx+8,sy+32+1+3+44,sx+8+56,sy+32+1+3+44+16,0,BLACK,16,(u8*)appplay_remote_items_tbl[gui_phy.language][2],1);
 	}else rval=1;
	if(rval==0)
	{
		BACK_COLOR=APP_WIN_BACK_COLOR;	//����ɫΪ������ɫ
		POINT_COLOR=RED;				//��ɫ����
		Remote_Init();//��ʼ���������
		while(1)
		{	  
			key=Remote_Scan();		 
			if(system_task_return)break;//TPAD����
			if(key)
			{				 	 			   
				LCD_ShowNum(sx+8+56,sy+32+1+3,key,3,16);		//��ʾ��ֵ
				LCD_ShowNum(sx+8+56,sy+32+1+3+22,RmtCnt,3,16);	//��ʾ��������
 				switch(key)
				{			    
					case 162: 
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"POWER");	  
						break;
					case 98:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"UP    ");
						break;
					case 2:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"PLAY  ");
						break;
					case 226:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"ALIEN ");
						break;
					case 194:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"RIGHT ");
						break;
					case 34:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"LEFT  ");
						break;
					case 56:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"8     ");
						break;
					case 224:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"VOL-  ");
						break;
					case 168:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"DOWN  ");
						break;
					case 144:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"VOL+  ");
						break;
					case 104:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"1     ");
						break;
					case 152:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"2     ");
						break;
					case 176:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"3     ");
						break;
					case 48:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"4     ");
						break;
					case 24:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"5     ");
						break;
					case 122:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"6     ");
						break;
					case 16:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"7     ");
						break;
					case 90:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"9     ");
						break;
					case 66:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"0     ");
						break;
					case 82:
						LCD_ShowString(sx+8+56,sy+32+1+3+44,240,320,16,"DELETE");
						break;	    
				}
			} 
			delay_ms(10); 
		}
	}
	TIM1->CR1&=~(1<<0);//�رն�ʱ��1
	window_delete(twin);
	return rval;
}



