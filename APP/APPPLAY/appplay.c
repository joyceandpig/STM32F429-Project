#include "appplay.h"
//#include "appplay_remote.h" 
//#include "appplay_wlvideo.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-Ӧ������ ����	   
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

#define APPPLAY_EX_BACKCOLOR	0X0000			//�����ⲿ����ɫ
#define APPPLAY_IN_BACKCOLOR	0X8C51			//�����ڲ�����ɫ
#define APPPLAY_NAME_COLOR		0X001F			//��������ɫ

#define APPPLAY_ALPHA_VAL 		18 				//APPѡ��͸��������
#define APPPLAY_ALPHA_COLOR		WHITE			//APP͸��ɫ
	

//Ӧ�ó������б�
//�����б�Ϊ16��,���ֳ��Ȳ�Ҫ����8���ֽ�(4������)
u8* const appplay_appname_tbl[3][16]=
{
{
	"����ң��","Ӧ�ó���","Ӧ�ó���","Ӧ�ó���",
	"Ӧ�ó���","Ӧ�ó���","Ӧ�ó���","Ӧ�ó���",
	"Ӧ�ó���","Ӧ�ó���","Ӧ�ó���","Ӧ�ó���",
	"Ӧ�ó���","Ӧ�ó���","Ӧ�ó���","Ӧ�ó���",
},
{
	"�t���b��","���ó���","���ó���","���ó���",
	"���ó���","���ó���","���ó���","���ó���",
	"���ó���","���ó���","���ó���","���ó���",
	"���ó���","���ó���","���ó���","���ó���",
},
{
	"REMOTE","APP","APP","APP",
	"APP","APP","APP","APP",
	"APP","APP","APP","APP",   
	"APP","APP","APP","APP",
},
};
 
//appplay��ͼ��·����
u8* const appplay_icospath_tbl[3][16]=
{
{
"1:/SYSTEM/APP/APPS/ICOS/remote_48.bmp", 
"1:/SYSTEM/APP/APPS/ICOS/app01_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app02_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app03_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app04_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app05_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app06_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app07_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app08_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app09_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app10_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app11_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app12_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app13_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app14_48.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app15_48.bmp",
},

{
"1:/SYSTEM/APP/APPS/ICOS/remote_60.bmp", 
"1:/SYSTEM/APP/APPS/ICOS/app01_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app02_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app03_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app04_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app05_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app06_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app07_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app08_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app09_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app10_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app11_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app12_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app13_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app14_60.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app15_60.bmp",
},
{
"1:/SYSTEM/APP/APPS/ICOS/remote_90.bmp", 
"1:/SYSTEM/APP/APPS/ICOS/app01_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app02_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app03_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app04_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app05_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app06_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app07_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app08_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app09_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app10_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app11_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app12_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app13_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app14_90.bmp",
"1:/SYSTEM/APP/APPS/ICOS/app15_90.bmp",
},
}; 

//����������
//����ֵ:0,�ɹ�;����,ʧ��;
u8 appplay_load_ui(m_app_dev *appdev) 
{
	u8 i;
	u8 xdis,ydis;
	u8 wxoff,wyoff;
	u8 width,height;
	u8 icowidth;
	u8 icofsize;
	u8 dis;
	u8 lcdtype=0;
	
	u8 rval=0;
 	_window_obj* twin=0;	//����
	if(lcddev.width==240)
	{
		wxoff=4;
		wyoff=4;
		xdis=4;
		ydis=5;
		icowidth=48;
		icofsize=12;
		width=48+3*2;
		height=48+12+3*2+2; 
		lcdtype=0;
	}else if(lcddev.width==272)
	{
		wxoff=6;
		wyoff=6;
		xdis=7;
		ydis=7;
		icowidth=48;
		icofsize=12;
		width=48+5*2;
		height=48+12+5*2+2; 
		lcdtype=0;
	}else if(lcddev.width==320)
	{
		wxoff=8;
		wyoff=8;
		xdis=6;
		ydis=6;
		icowidth=60;
		icofsize=12;
		width=60+5*2;
		height=60+12+5*2+2; 
		lcdtype=1;
	}else if(lcddev.width==480)
	{
		wxoff=10;
		wyoff=10;
		xdis=9;
		ydis=8;
		icowidth=90;
		icofsize=16;
		width=90+8*2;
		height=90+16+8*2+2; 
		lcdtype=2;
	}else if(lcddev.width==600)
	{
		wxoff=20;
		wyoff=20;
		xdis=30;
		ydis=10;
		icowidth=90;
		icofsize=16;
		width=90+10*2;
		height=90+16+10*2+2; 
		lcdtype=2;
	}
	dis=(width-icowidth)/2;
	twin=window_creat(wxoff,gui_phy.tbheight+wyoff,lcddev.width-2*wxoff,lcddev.height-gui_phy.tbheight-2*wyoff,0,0X01,16);//��������
	if(twin)
	{
		twin->captionbkcu=APPPLAY_IN_BACKCOLOR;		//Ĭ��caption�ϲ��ֱ���ɫ
		twin->captionbkcd=APPPLAY_IN_BACKCOLOR;		//Ĭ��caption�²��ֱ���ɫ
		twin->captioncolor=APPPLAY_IN_BACKCOLOR;	//Ĭ��caption����ɫ
	 	twin->windowbkc=APPPLAY_IN_BACKCOLOR;	 	//Ĭ�ϱ���ɫ
	
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,APPPLAY_EX_BACKCOLOR);//��䱳��ɫ
	  	app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);//�·ֽ���	 
 		gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize
		,(u8*)APP_MFUNS_CAPTION_TBL[25][gui_phy.language]);//��ʾ����  
		window_draw(twin);
		for(i=0;i<16;i++)
		{			 
			appdev->icos[i].x=wxoff+xdis/2+(i%4)*(width+xdis);		
			appdev->icos[i].y=gui_phy.tbheight+wxoff+ydis/2+(i/4)*(height+ydis);	
			appdev->icos[i].width=width;
			appdev->icos[i].height=height; 
			appdev->icos[i].path=(u8*)appplay_icospath_tbl[lcdtype][i];
			appdev->icos[i].name=(u8*)appplay_appname_tbl[gui_phy.language][i];
			rval=minibmp_decode(appdev->icos[i].path,appdev->icos[i].x,appdev->icos[i].y+dis,appdev->icos[i].width,icowidth,0,0);
			if(rval)break;//���������
			gui_show_strmid(appdev->icos[i].x,appdev->icos[i].y+dis+icowidth+2,appdev->icos[i].width,icofsize,APPPLAY_NAME_COLOR,icofsize,appdev->icos[i].name);
	 	}
		appdev->selico=0XFF;//Ĭ�ϲ�ѡ���κ�һ��
	}else rval=1;
	window_delete(twin);
	return rval;
}	   
//����ѡ���ĸ�ͼ��
//sel:0~15����ǰҳ��ѡ��ico
void appplay_set_sel(m_app_dev* appdev,u8 sel)
{
	u8 icowidth;
	u8 icofsize;
	u8 dis;
	if(sel>=16)return;//�Ƿ�������	
	if(lcddev.width<=272)
	{ 
		icowidth=48;
		icofsize=12;
	}else if(lcddev.width==320)
	{ 
		icowidth=60; 
		icofsize=12;
	}else if(lcddev.width>=480)
	{ 
		icowidth=90; 
		icofsize=16;
	}
	if(appdev->selico<16)
	{
		dis=(appdev->icos[appdev->selico].width-icowidth)/2;
		gui_fill_rectangle(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_IN_BACKCOLOR);//���֮ǰ��ͼƬ
		minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis,appdev->icos[appdev->selico].width,icowidth,0,0);
 		gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis+icowidth+2,appdev->icos[appdev->selico].width,icofsize,APPPLAY_NAME_COLOR,icofsize,appdev->icos[appdev->selico].name);
	};				   
	appdev->selico=sel;
	dis=(appdev->icos[appdev->selico].width-icowidth)/2;
	gui_alphablend_area(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y,appdev->icos[appdev->selico].width,appdev->icos[appdev->selico].height,APPPLAY_ALPHA_COLOR,APPPLAY_ALPHA_VAL);
	minibmp_decode(appdev->icos[appdev->selico].path,appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis,appdev->icos[appdev->selico].width,icowidth,0,0);
	gui_show_strmid(appdev->icos[appdev->selico].x,appdev->icos[appdev->selico].y+dis+icowidth+2,appdev->icos[appdev->selico].width,icofsize,APPPLAY_NAME_COLOR,icofsize,appdev->icos[appdev->selico].name);
}
//������ɨ��
//����ֵ:0~15,��Ч����.����,��Ч
u8 appplay_tpscan(m_app_dev *appdev)
{
	static u8 firsttpd=0;	//���������־,��ֹһ�ΰ���,��η���
	u8 i=0XFF;
	tp_dev.scan(0);//ɨ��																		 
	if((tp_dev.sta&TP_PRES_DOWN))//�а���������
	{
		if(firsttpd==0)//��һ�δ���?
		{
			firsttpd=1;	  		//����Ѿ������˴˴ΰ���
			for(i=0;i<16;i++)
			{
				if((tp_dev.x[0]>appdev->icos[i].x)&&(tp_dev.x[0]<appdev->icos[i].x+appdev->icos[i].width)
				 &&(tp_dev.y[0]>appdev->icos[i].y)&&(tp_dev.y[0]<appdev->icos[i].y+appdev->icos[i].height))//��������
				{
					break;//�õ�ѡ�еı��	
				}
			}
		}
	}else firsttpd=0;//�ɿ���
	return i;
}

//appӦ��
u8 app_play(void)
{		
	u8 selx=0XFF;
	u8 rval=0;
	m_app_dev *appdev;
	appdev=(m_app_dev*)gui_memin_malloc(sizeof(m_app_dev));
	if(appdev==NULL)rval=1;
	else if(appplay_load_ui(appdev))rval=1;
 	while(rval==0)
	{
		selx=appplay_tpscan(appdev);
		if(selx<16)//����Ч����
		{
			if(selx==appdev->selico)//�ڶ���ѡ�����Ŀ
			{
				//printf("selx:%d\r\n",appdev->selico);
				switch(selx)
				{
					case 0://����ң����
//						appplay_remote(appdev->icos[selx].name); 
						break; 
					case 1: 
						break;  				
				} 
				appplay_load_ui(appdev);//���¼���������
			}
			appplay_set_sel(appdev,selx);		
			system_task_return=0; 
		}
		if(system_task_return)break;//TPAD����
		delay_ms(1000/OS_TICKS_PER_SEC);//��ʱһ��ʱ�ӽ���
 	}
	gui_memin_free(appdev);//�ͷ��ڴ�
	return 0;
}























