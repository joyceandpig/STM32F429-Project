#include "keyplay.h" 
#include "gradienter.h" 
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-按键测试 代码	   
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
 
//显示圆形提示信息
//x,y:要显示的圆中心坐标
//r:半径
//fsize:字体大小
//color:圆的颜色
//str:显示的字符串
void key_show_circle(u16 x,u16 y,u16 r,u8 fsize,u16 color,u8 *str)
{ 
	gui_fill_circle(x,y,r,color);
	gui_show_strmid(x-r,y-fsize/2,2*r,fsize,BLUE,fsize,str);//显示标题  
}


//按键测试
u8 key_play(void)
{
	u8 key;
	u16 k0x,k1x,k2x,kux;
	u16 kuy,koy;
	u16 kcr; 
	u8 fsize=0;				//key字体大小
	
	u8 keyold=0XFF;			//按键和之前的按键值
	
	kcr=lcddev.width/10;
	k2x=2*kcr;
	k1x=k2x+3*kcr;
	k0x=k1x+3*kcr;
	kux=k1x;
	kuy=(lcddev.height-5*kcr)/2+kcr;
	koy=kuy+3*kcr;
	
	if(lcddev.width<=272)
	{
		fsize=12;	
	}else if(lcddev.width==320)
	{
		fsize=16;	
	}else if(lcddev.width>=480)
	{
		fsize=24;	
	}
	LCD_Clear(LGRAY);
	app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);			//下分界线	 
	gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)APP_MFUNS_CAPTION_TBL[22][gui_phy.language]);//显示标题  
	system_task_return=0;
	while(1)
	{  
		key=KEY_Scan(1); 
		if(key!=keyold)
		{ 
			keyold=key;
			if(key==KEY0_PRES)key_show_circle(k0x,koy,kcr,fsize,RED,(u8*)"KEY0");
			else key_show_circle(k0x,koy,kcr,fsize,YELLOW,(u8*)"KEY0"); 
			if(key==KEY1_PRES)key_show_circle(k1x,koy,kcr,fsize,RED,(u8*)"KEY1");
			else key_show_circle(k1x,koy,kcr,fsize,YELLOW,(u8*)"KEY1"); 
			if(key==KEY2_PRES)key_show_circle(k2x,koy,kcr,fsize,RED,(u8*)"KEY2");
			else key_show_circle(k2x,koy,kcr,fsize,YELLOW,(u8*)"KEY2"); 
			if(key==WKUP_PRES)key_show_circle(kux,kuy,kcr,fsize,RED,(u8*)"KEYUP");
			else key_show_circle(kux,kuy,kcr,fsize,YELLOW,(u8*)"KEYUP");  
		}			
		if(system_task_return)break;		//TPAD返回  
		delay_ms(10);
	}  	 
	return 0;
}







































