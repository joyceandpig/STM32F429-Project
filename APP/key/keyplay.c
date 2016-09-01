#include "keyplay.h" 
#include "gradienter.h" 
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-�������� ����	   
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
 
//��ʾԲ����ʾ��Ϣ
//x,y:Ҫ��ʾ��Բ��������
//r:�뾶
//fsize:�����С
//color:Բ����ɫ
//str:��ʾ���ַ���
void key_show_circle(u16 x,u16 y,u16 r,u8 fsize,u16 color,u8 *str)
{ 
	gui_fill_circle(x,y,r,color);
	gui_show_strmid(x-r,y-fsize/2,2*r,fsize,BLUE,fsize,str);//��ʾ����  
}


//��������
u8 key_play(void)
{
	u8 key;
	u16 k0x,k1x,k2x,kux;
	u16 kuy,koy;
	u16 kcr; 
	u8 fsize=0;				//key�����С
	
	u8 keyold=0XFF;			//������֮ǰ�İ���ֵ
	
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
	app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);			//�·ֽ���	 
	gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)APP_MFUNS_CAPTION_TBL[22][gui_phy.language]);//��ʾ����  
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
		if(system_task_return)break;		//TPAD����  
		delay_ms(10);
	}  	 
	return 0;
}







































