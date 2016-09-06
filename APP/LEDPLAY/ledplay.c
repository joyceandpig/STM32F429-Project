#include "ledplay.h" 
#include "gradienter.h" 
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-LED测试 代码	   
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
 

//DS0按钮标题
u8*const ds0_btncaption_tbl[2][GUI_LANGUAGE_NUM]=
{ 
{"DS0亮","DS0亮","DS0 ON",},
{"DS0灭","DS0滅","DS0 OFF",},  
};
//DS1按钮标题
u8*const ds1_btncaption_tbl[2][GUI_LANGUAGE_NUM]=
{ 
{"DS1亮","DS1亮","DS1 ON",},
{"DS1灭","DS1滅","DS1 OFF",},  
};

extern vu8 ledplay_ds0_sta;		//ledplay任务,DS0的控制状态
//LED测试
u8 led_play(void)
{
	
	u8 res,rval=0;    
	u8 ds0sta=1,ds1sta=1;
	
 	_btn_obj* ds0btn=0;			//控制按钮
 	_btn_obj* ds1btn=0;			//控制按钮
	
	u16 btnw,btnh;				//按钮参数
	u16 btnds0x,btnds0y,btnds1x,btnds1y;	//按钮坐标参数
	  
	u16 cds0x,cds0y,cds1x,cds1y,cr; 		//圆坐标参数
	
	u8 btnfsize;				//字体大小   
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
		app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);			//下分界线	 
		gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)APP_MFUNS_CAPTION_TBL[23][gui_phy.language]);//显示标题  
 	
		ds0btn->caption=ds0_btncaption_tbl[0][gui_phy.language];
		ds0btn->font=btnfsize;
		ds1btn->caption=ds1_btncaption_tbl[0][gui_phy.language];
		ds1btn->font=btnfsize;
		
		
		btn_draw(ds0btn);		//画按钮
		btn_draw(ds1btn);		//画按钮
		
		ds0btn->caption=ds0_btncaption_tbl[1][gui_phy.language];
		ds1btn->caption=ds1_btncaption_tbl[1][gui_phy.language];
		gui_fill_circle(cds0x,cds0y,cr,WHITE); 
		gui_fill_circle(cds1x,cds1y,cr,WHITE); 
		system_task_return=0;
		while(1)
		{ 
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			res=btn_check(ds0btn,&in_obj);   
			if(res&&((ds0btn->sta&(1<<7))==0)&&(ds0btn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
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
			if(res&&((ds1btn->sta&(1<<7))==0)&&(ds1btn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
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
			if(system_task_return)break;		//TPAD返回  
			delay_ms(10);
		}   
	}
	ledplay_ds0_sta=0;
//	LED0=LED1=1;		//关闭LED
	LED0_OFF();
	LED1_OFF();
	btn_delete(ds0btn);	//删除按钮
	btn_delete(ds1btn);	//删除按钮 
	return rval;
}







