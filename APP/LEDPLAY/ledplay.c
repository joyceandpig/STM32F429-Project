#include "ledplay.h" 
#include "gradienter.h" 
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-LED���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/6/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 

//DS0��ť����
u8*const ds0_btncaption_tbl[2][GUI_LANGUAGE_NUM]=
{ 
{"DS0��","DS0��","DS0 ON",},
{"DS0��","DS0��","DS0 OFF",},  
};
//DS1��ť����
u8*const ds1_btncaption_tbl[2][GUI_LANGUAGE_NUM]=
{ 
{"DS1��","DS1��","DS1 ON",},
{"DS1��","DS1��","DS1 OFF",},  
};

extern vu8 ledplay_ds0_sta;		//ledplay����,DS0�Ŀ���״̬
//LED����
u8 led_play(void)
{
	
	u8 res,rval=0;    
	u8 ds0sta=1,ds1sta=1;
	
 	_btn_obj* ds0btn=0;			//���ư�ť
 	_btn_obj* ds1btn=0;			//���ư�ť
	
	u16 btnw,btnh;				//��ť����
	u16 btnds0x,btnds0y,btnds1x,btnds1y;	//��ť�������
	  
	u16 cds0x,cds0y,cds1x,cds1y,cr; 		//Բ�������
	
	u8 btnfsize;				//�����С   
	if(lcddev.width>=480)
	{
		btnfsize=32;	
	}else if(lcddev.width>=320)
	{
		btnfsize=24;	
	}else if(lcddev.width>=240)
	{
		btnfsize=16;	
	}
	
	btnw=lcddev.width*2/5; 
	btnh=btnw/4;
	
	cr=btnw/2;
	cds0x=lcddev.width/20+cr;
	cds1x=cds0x+cr*2+lcddev.width/10;
	
	cds0y=(lcddev.height-cr*2-2*btnh)/2+cr;
	cds1y=cds0y; 
	
	btnds0x=lcddev.width/20;
	btnds0y=(lcddev.height-cr*2-2*btnh)/2+2*cr+btnh;
	
	btnds1x=btnds0x+lcddev.width/10+btnw;
	btnds1y=btnds0y;
    
	ds0btn=btn_creat(btnds0x,btnds0y,btnw,btnh,0,0);
	ds1btn=btn_creat(btnds1x,btnds1y,btnw,btnh,0,0);
	if(ds0btn&&ds1btn)
	{
		LCD_Clear(LGRAY);
		app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);			//�·ֽ���	 
		gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)APP_MFUNS_CAPTION_TBL[23][gui_phy.language]);//��ʾ����  
 	
		ds0btn->caption=ds0_btncaption_tbl[0][gui_phy.language];
		ds0btn->font=btnfsize;
		ds1btn->caption=ds1_btncaption_tbl[0][gui_phy.language];
		ds1btn->font=btnfsize;
		
		
		btn_draw(ds0btn);		//����ť
		btn_draw(ds1btn);		//����ť
		
		ds0btn->caption=ds0_btncaption_tbl[1][gui_phy.language];
		ds1btn->caption=ds1_btncaption_tbl[1][gui_phy.language];
		gui_fill_circle(cds0x,cds0y,cr,WHITE); 
		gui_fill_circle(cds1x,cds1y,cr,WHITE); 
		system_task_return=0;
		while(1)
		{ 
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			res=btn_check(ds0btn,&in_obj);   
			if(res&&((ds0btn->sta&(1<<7))==0)&&(ds0btn->sta&(1<<6)))//������,�а����������ɿ�,����TP�ɿ���
			{    
				ds0sta=!ds0sta;
				ds0btn->caption=ds0_btncaption_tbl[ds0sta][gui_phy.language]; 
				if(ds0sta)
				{     
					gui_fill_circle(cds0x,cds0y,cr,WHITE);
				}else 
				{  
					gui_fill_circle(cds0x,cds0y,cr,RED);    
				}
//				LED0=ds0sta;
				LED0_SET_STA(ds0sta);
				ledplay_ds0_sta=!ds0sta;
			}			
			res=btn_check(ds1btn,&in_obj);   
			if(res&&((ds1btn->sta&(1<<7))==0)&&(ds1btn->sta&(1<<6)))//������,�а����������ɿ�,����TP�ɿ���
			{    
				ds1sta=!ds1sta;
				ds1btn->caption=ds0_btncaption_tbl[ds1sta][gui_phy.language]; 
				if(ds1sta)
				{  
					gui_fill_circle(cds1x,cds1y,cr,WHITE);  
				}else 
				{ 
					gui_fill_circle(cds1x,cds1y,cr,GREEN); 
				}
//				LED1=ds1sta;
				LED1_SET_STA(ds1sta);
			}	 
			if(system_task_return)break;		//TPAD����  
			delay_ms(10);
		}   
	}
	ledplay_ds0_sta=0;
//	LED0=LED1=1;		//�ر�LED
	LED0_OFF();
	LED1_OFF();
	btn_delete(ds0btn);	//ɾ����ť
	btn_delete(ds1btn);	//ɾ����ť 
	return rval;
}







