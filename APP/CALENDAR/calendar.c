#include "calendar.h" 	      						  
#include "stdio.h"
#include "settings.h" 
#include "mpu9250.h"
#include "ds18b20.h"
#include "24cxx.h"
#include "math.h"
#include "rtc.h"
#include "camera.h" 
//#include "pcf8574.h"
#include "wm8978.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-���� ����	   
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
#define CALENDAR_ALARM_SET_ENABLE(HANDLE)  (HANDLE->ringsta |= (1<<7))
#define CALENDAR_ALARM_SET_DISABLE(HANDLE) (HANDLE->ringsta &= ~(1<<7))
#define CALENDAR_ALARM_GET_STATUS(HANDLE)  (HANDLE->ringsta>>7 | 0x01) 
 
_alarm_obj alarm;		//���ӽṹ��
_calendar_obj calendar;	//�����ṹ��

static u16 TIME_TOPY;		//	120
static u16 OTHER_TOPY;		//	200 	
 
u8*const calendar_week_table[GUI_LANGUAGE_NUM][7]=
{
{"������","����һ","���ڶ�","������","������","������","������"},
{"������","����һ","���ڶ�","������","������","������","������"},
{"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
};
//���ӱ���
u8*const calendar_alarm_caption_table[GUI_LANGUAGE_NUM]=
{
"����","�[�","ALARM",
};
//���찴ť
u8*const calendar_alarm_realarm_table[GUI_LANGUAGE_NUM]=
{
"����","���","REALARM",
}; 
u8*const calendar_loading_str[GUI_LANGUAGE_NUM][3]=
{
{
	"���ڼ���,���Ժ�...",
	"δ��⵽DS18B20!",
	"����MPU9250�¶ȴ�����...",
},
{
	"���ڼ���,���Ժ�...",
	"δ��⵽DS18B20!",
	"����MPU9250�¶ȴ�����...",
},
{
	"Loading...",
	"DS18B20 Check Failed!",
	"Use MPU9250 Temp Sensor...",
},
};	 
//���³�ʼ������		    
//alarmx:���ӽṹ��
//calendarx:�����ṹ��
void calendar_alarm_init(_alarm_obj *alarmx,_calendar_obj *calendarx) 
{	  
	u8 temp;
	calendar_get_date(calendarx);	//��ȡ��ǰ������Ϣ
	if(calendarx->week==7)temp=1<<0;
	else temp=1<<calendarx->week; 
	
	if(alarmx->weekmask&temp)		//��Ҫ����
	{ 
		RTC_Set_AlarmA(calendarx->week,alarmx->hour,alarmx->min,0);//��������ʱ��		
	} 
  
} 
//����������
//type:��������	   
//0,��.
//1,��.��.
//2,��.��.��
//4,��.��.��.��
void calendar_alarm_ring(u8 type)
{
	u8 i;	 
	for(i=0;i<(type+1);i++)
	{
//		PCF8574_WriteBit(BEEP_IO,0);
		delay_ms(50);
//		PCF8574_WriteBit(BEEP_IO,1);   
		delay_ms(70);
	}	 
}
//�õ�ʱ��
//calendarx:�����ṹ��
void calendar_get_time(_calendar_obj *calendarx)
{
	u8 ampm;
	RTC_Get_Time(&calendarx->hour,&calendarx->min,&calendarx->sec,&ampm);//�õ�ʱ��	
}
//�õ�����
//calendarx:�����ṹ��
void calendar_get_date(_calendar_obj *calendarx)
{
	u8 year;
	RTC_Get_Date(&year,&calendarx->w_month,&calendarx->w_date,&calendarx->week);
	calendar.w_year=year+2000;//��2000�꿪ʼ����
}
//���ݵ�ǰ������,����������.
void calendar_date_refresh(void) 
{
 	u8 weekn;   //�ܼĴ�
	u16 offx=(lcddev.width-240)/2;
 	//��ʾ����������
	POINT_COLOR=BRED; 
	BACK_COLOR=BLACK; 
	LCD_ShowxNum(offx+5,OTHER_TOPY+9,(calendar.w_year/100)%100,2,16,0);//��ʾ��  20/19  
	LCD_ShowxNum(offx+21,OTHER_TOPY+9,calendar.w_year%100,2,16,0);     //��ʾ��  
	LCD_ShowString(offx+37,OTHER_TOPY+9,lcddev.width,lcddev.height,16,"-"); //"-"
	LCD_ShowxNum(offx+45,OTHER_TOPY+9,calendar.w_month,2,16,0X80);     //��ʾ��
	LCD_ShowString(offx+61,OTHER_TOPY+9,lcddev.width,lcddev.height,16,"-"); //"-"
	LCD_ShowxNum(offx+69,OTHER_TOPY+9,calendar.w_date,2,16,0X80);      //��ʾ��	  
	//��ʾ�ܼ�?
	POINT_COLOR=RED;
  weekn=calendar.week;
	Show_Str(5+offx,OTHER_TOPY+35,lcddev.width,lcddev.height,(u8 *)calendar_week_table[gui_phy.language][weekn],16,0); //��ʾ�ܼ�?	
													 
}
//�������ݱ�����:SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings)+sizeof(_wm8978_obj)
//��ȡ����������Ϣ
//alarm:������Ϣ 
void calendar_read_para(_alarm_obj * alarm)
{
	AT24CXX_Read(SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings)+sizeof(_wm8978_obj),(u8*)alarm,sizeof(_alarm_obj));
}


//д������������Ϣ
//alarm:������Ϣ 
void calendar_save_para(_alarm_obj * alarm)
{
  OS_CPU_SR cpu_sr=0;
//	alarm->ringsta&=0X7F;	//������λ
	CALENDAR_ALARM_SET_DISABLE(alarm);
	OS_ENTER_CRITICAL();	//�����ٽ���(�޷����жϴ��) 
	AT24CXX_Write(SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings)+sizeof(_wm8978_obj),(u8*)alarm,sizeof(_alarm_obj));
	OS_EXIT_CRITICAL();		//�˳��ٽ���(���Ա��жϴ��)
} 

//���崦��(�ߴ�:200*160)
//x,y:����
//����ֵ,������
u8 calendar_alarm_msg(u16 x,u16 y)
{
	u8 rval=0; 
	vu8 res=0;
	u32 abr;						    
	FIL* falarm=0;
	u16 tempcolor=gui_phy.back_color;//����֮ǰ�ı���ɫ
	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//ȡ����ť		  
 	_btn_obj * okbtn=0;				//ȷ����ť	
	u8 freadflag=0;
	u8 dcmiflag=0;
	
	if(DCMI->CR&0X01)//����ͷ���ڹ���?
	{
		dcmiflag=1;
		DCMI_Stop();//�ر�����ͷ
		sw_sdcard_mode();//�л�ΪSD��ģʽ
	}
	OSTaskSuspend(6); //����������
  twin=window_creat(x,y,200,160,0,1|1<<5|1<<6,16);//��������,��ȡ����ɫ
  okbtn=btn_creat(x+20,y+120,70,30,0,0x02);		//������ť
 	rbtn=btn_creat(x+20+70+20,y+120,70,30,0,0x02);	//������ť
 	falarm=(FIL *)gui_memin_malloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(twin==NULL||rbtn==NULL||okbtn==NULL||falarm==NULL)rval=1; 
	else
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=(u8*)calendar_alarm_caption_table[gui_phy.language];
		twin->windowbkc=APP_WIN_BACK_COLOR;	
		//���ذ�ť����ɫ    
		rbtn->bkctbl[0]=0X8452;		//�߿���ɫ
		rbtn->bkctbl[1]=0XAD97;		//��һ�е���ɫ				
		rbtn->bkctbl[2]=0XAD97;		//�ϰ벿����ɫ
		rbtn->bkctbl[3]=0X8452;		//�°벿����ɫ
		okbtn->bkctbl[0]=0X8452;	//�߿���ɫ
		okbtn->bkctbl[1]=0XAD97;	//��һ�е���ɫ				
		okbtn->bkctbl[2]=0XAD97;	//�ϰ벿����ɫ
		okbtn->bkctbl[3]=0X8452;	//�°벿����ɫ

		rbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];		//ȡ����ť
 		okbtn->caption=(u8*)calendar_alarm_realarm_table[gui_phy.language];	//���찴ť

		if(asc2_s6030==0)//֮ǰ��û���ֿ��?
		{
			freadflag=1;	//��Ƕ�ȡ���ֿ�
			res=f_open(falarm,(const TCHAR*)APP_ASCII_S6030,FA_READ);//���ļ� 
			if(res==0)
			{
				asc2_s6030=(u8*)gui_memex_malloc(falarm->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_s6030==0)rval=1;
				else 
				{
					res=f_read(falarm,asc2_s6030,falarm->obj.objsize,(UINT*)&abr);	//һ�ζ�ȡ�����ļ�
				}
				f_close(falarm);
			}
		}
 		window_draw(twin);						//��������
		btn_draw(rbtn);							//����ť
		btn_draw(okbtn);						//����ť		
 		if(res)rval=res;
		else 									//��ʾ����ʱ��
		{
			gui_phy.back_color=APP_WIN_BACK_COLOR;
			gui_show_num(x+15,y+32+14,2,BLUE,60,alarm.hour,0X80);	//��ʾʱ
			gui_show_ptchar(x+15+60,y+32+14,lcddev.width,lcddev.height,0,BLUE,60,':',0);	//":"
			gui_show_num(x+15+90,y+32+14,2,BLUE,60,alarm.min,0X80);	//��ʾ�� 
		}
		system_task_return=0;
 		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ  
			delay_ms(5); 
 	 		if(system_task_return)
			{
				rval=1;			//ȡ��
				break;			//TPAD����	
			}
			res=btn_check(rbtn,&in_obj);			//ȡ����ť���
			if(res&&((rbtn->sta&0X80)==0))			//����Ч����
			{
				rval=1;
				break;//�˳�
			}
	 		res=btn_check(okbtn,&in_obj);			//���찴ť���
	 		if(res&&((okbtn->sta&0X80)==0))			//����Ч����
			{		 
				rval=0XFF;//		   
				break;//�˳�
			}    
		}	 
	}
	alarm.ringsta&=~(1<<7);	//ȡ������
//	CALENDAR_ALARM_SET_DISABLE(&alarm);
	if(rval==0XFF)			//�Ժ�����
	{
		alarm.min+=5; 		//�Ƴ�5����
		if(alarm.min>59)
		{
			alarm.min=alarm.min%60;
			alarm.hour++;
			if(alarm.hour>23)alarm.hour=0;
		}				  
		calendar_alarm_init((_alarm_obj*)&alarm,&calendar);//���³�ʼ������
 	}
	window_delete(twin);
	btn_delete(rbtn);
	btn_delete(okbtn); 	  
	gui_memin_free(falarm);
	if(freadflag)//��ȡ���ֿ�?�ͷ�
	{
		gui_memex_free(asc2_s6030);
		asc2_s6030=0;
	}
	system_task_return=0;	//ȡ��TPAD
	gui_phy.back_color=tempcolor;//�ָ�����ɫ
	OSTaskResume(6); 		//�ָ�������
	if(dcmiflag)
	{
		sw_ov5640_mode();
		DCMI_Start();//������������ͷ
	}
	return rval;
} 
//��Բ��ָ�����
//x,y:�������ĵ�
//size:���̴�С(ֱ��)
//d:���̷ָ�,���ӵĸ߶�
void calendar_circle_clock_drawpanel(u16 x,u16 y,u16 size,u16 d)
{
	u16 r=size/2;//�õ��뾶 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1; 
	u16 i; 
	gui_fill_circle(x,y,r,WHITE);		//����Ȧ
	gui_fill_circle(x,y,r-4,BLACK);		//����Ȧ
	for(i=0;i<60;i++)//�����Ӹ�
	{ 
		px0=sx+r+(r-4)*sin((app_pi/30)*i); 
		py0=sy+r-(r-4)*cos((app_pi/30)*i); 
		px1=sx+r+(r-d)*sin((app_pi/30)*i); 
		py1=sy+r-(r-d)*cos((app_pi/30)*i);  
		gui_draw_bline1(px0,py0,px1,py1,0,WHITE);		
	}
	for(i=0;i<12;i++)//��Сʱ��
	{ 
		px0=sx+r+(r-5)*sin((app_pi/6)*i); 
		py0=sy+r-(r-5)*cos((app_pi/6)*i); 
		px1=sx+r+(r-d)*sin((app_pi/6)*i); 
		py1=sy+r-(r-d)*cos((app_pi/6)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);		
	}
	for(i=0;i<4;i++)//��3Сʱ��
	{ 
		px0=sx+r+(r-5)*sin((app_pi/2)*i); 
		py0=sy+r-(r-5)*cos((app_pi/2)*i); 
		px1=sx+r+(r-d-3)*sin((app_pi/2)*i); 
		py1=sy+r-(r-d-3)*cos((app_pi/2)*i);  
		gui_draw_bline1(px0,py0,px1,py1,2,YELLOW);		
	}
	gui_fill_circle(x,y,d/2,WHITE);		//������Ȧ
}
//��ʾʱ��
//x,y:�������ĵ�
//size:���̴�С(ֱ��)
//d:���̷ָ�,���ӵĸ߶�
//hour:ʱ��
//min:����
//sec:����
void calendar_circle_clock_showtime(u16 x,u16 y,u16 size,u16 d,u8 hour,u8 min,u8 sec)
{
	static u8 oldhour=0;	//���һ�ν���ú�����ʱ������Ϣ
	static u8 oldmin=0;
	static u8 oldsec=0;
	float temp;
	u16 r=size/2;//�õ��뾶 
	u16 sx=x-r;
	u16 sy=y-r;
	u16 px0,px1;
	u16 py0,py1;  
	u8 r1; 
	if(hour>11)hour-=12;
///////////////////////////////////////////////
	//���Сʱ
	r1=d/2+4;
	//�����һ�ε�����
	temp=(float)oldmin/60;
	temp+=oldhour;
	px0=sx+r+(r-3*d-7)*sin((app_pi/6)*temp); 
	py0=sy+r-(r-3*d-7)*cos((app_pi/6)*temp); 
	px1=sx+r+r1*sin((app_pi/6)*temp); 
	py1=sy+r-r1*cos((app_pi/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,BLACK);
	//�������
	r1=d/2+3;
	temp=(float)oldsec/60;
	temp+=oldmin;
	//�����һ�ε�����
	px0=sx+r+(r-2*d-7)*sin((app_pi/30)*temp); 
	py0=sy+r-(r-2*d-7)*cos((app_pi/30)*temp); 
	px1=sx+r+r1*sin((app_pi/30)*temp); 
	py1=sy+r-r1*cos((app_pi/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,BLACK); 
	//������� 
	r1=d/2+3;
	//�����һ�ε�����
	px0=sx+r+(r-d-7)*sin((app_pi/30)*oldsec); 
	py0=sy+r-(r-d-7)*cos((app_pi/30)*oldsec); 
	px1=sx+r+r1*sin((app_pi/30)*oldsec); 
	py1=sy+r-r1*cos((app_pi/30)*oldsec); 
	gui_draw_bline1(px0,py0,px1,py1,0,BLACK); 
///////////////////////////////////////////////
	//��ʾСʱ 
	r1=d/2+4; 
	//��ʾ�µ�ʱ��
	temp=(float)min/60;
	temp+=hour;
	px0=sx+r+(r-3*d-7)*sin((app_pi/6)*temp); 
	py0=sy+r-(r-3*d-7)*cos((app_pi/6)*temp); 
	px1=sx+r+r1*sin((app_pi/6)*temp); 
	py1=sy+r-r1*cos((app_pi/6)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,2,YELLOW); 
	//��ʾ���� 
	r1=d/2+3; 
	temp=(float)sec/60;
	temp+=min;
	//��ʾ�µķ���
	px0=sx+r+(r-2*d-7)*sin((app_pi/30)*temp); 
	py0=sy+r-(r-2*d-7)*cos((app_pi/30)*temp); 
	px1=sx+r+r1*sin((app_pi/30)*temp); 
	py1=sy+r-r1*cos((app_pi/30)*temp); 
	gui_draw_bline1(px0,py0,px1,py1,1,GREEN); 	
	//��ʾ����  
	r1=d/2+3;
	//��ʾ�µ�����
	px0=sx+r+(r-d-7)*sin((app_pi/30)*sec); 
	py0=sy+r-(r-d-7)*cos((app_pi/30)*sec); 
	px1=sx+r+r1*sin((app_pi/30)*sec); 
	py1=sy+r-r1*cos((app_pi/30)*sec); 
	gui_draw_bline1(px0,py0,px1,py1,0,RED); 
	oldhour=hour;	//����ʱ
	oldmin=min;		//�����
	oldsec=sec;		//������
}	    
//ʱ����ʾģʽ    
u8 calendar_play(void)
{
	u8 second=0;
	short temperate=0;	//�¶�ֵ		   
	u8 t=0;
	u8 tempdate=0;
	u8 rval=0;			//����ֵ	
	u8 res;
	u16 xoff=0;
	u16 yoff=0;	//����yƫ����
	u16 r=0;	//���̰뾶
	u8 d=0;		//ָ�볤��
	  
	u8 TEMP_SEN_TYPE=0;	//Ĭ��ʹ��DS18B20
	FIL* f_calendar=0;	 
	
  	f_calendar=(FIL *)gui_memin_malloc(sizeof(FIL));//����FIL�ֽڵ��ڴ����� 
	if(f_calendar==NULL)rval=1;		//����ʧ��
	else
	{
		res=f_open(f_calendar,(const TCHAR*)APP_ASCII_S6030,FA_READ);//���ļ� 
		if(res==FR_OK)
		{
			asc2_s6030=(u8*)gui_memex_malloc(f_calendar->obj.objsize);	//Ϊ�����忪�ٻ����ַ
			if(asc2_s6030==0)rval=1;
			else 
			{
				res=f_read(f_calendar,asc2_s6030,f_calendar->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
 			}
			f_close(f_calendar);
		} 
		if(res)rval=res;
	} 	    
	if(rval==0)//�޴���
	{	  
 		LCD_Clear(BLACK);//�����    	  
		second=calendar.sec;//�õ��˿̵�����
		POINT_COLOR=GBLUE;
		Show_Str(48,60,lcddev.width,lcddev.height,(u8*)calendar_loading_str[gui_phy.language][0],16,0x01); //��ʾ������Ϣ	    
//		PCF8574_ReadBit(EX_IO);//��ȡһ��PCF8574,�ͷ�INT��
		if(DS18B20_Init())
		{
			Show_Str(48,76,lcddev.width,lcddev.height,(u8*)calendar_loading_str[gui_phy.language][1],16,0x01);  
			delay_ms(500);
			Show_Str(48,92,lcddev.width,lcddev.height,(u8*)calendar_loading_str[gui_phy.language][2],16,0x01);  
			TEMP_SEN_TYPE=1; 
		}   	    
		delay_ms(1100);//�ȴ�1.1s 
		BACK_COLOR= BLACK;
		LCD_Clear(BLACK);//����� 
		r=lcddev.width/3;
		d=lcddev.width/40; 
		yoff=(lcddev.height-r*2-140)/2;
		TIME_TOPY=yoff+r*2+10;
		OTHER_TOPY=TIME_TOPY+60+10;
		xoff=(lcddev.width-240)/2;   
		calendar_circle_clock_drawpanel(lcddev.width/2,yoff+r,r*2,d);//��ʾָ��ʱ�ӱ��� 
		calendar_date_refresh();  //��������
		tempdate=calendar.w_date;//�����ݴ��� 		
		gui_phy.back_color=BLACK;
		gui_show_ptchar(xoff+70-4,TIME_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,':',0);	//":"
		gui_show_ptchar(xoff+150-4,TIME_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,':',0);	//":" 
	}
  	while(rval==0)
	{	
		calendar_get_time(&calendar);	//����ʱ��
		if(system_task_return)break;	//��Ҫ����	  
 		if(second!=calendar.sec)//���Ӹı���
		{ 	
  			second=calendar.sec;  
			calendar_circle_clock_showtime(lcddev.width/2,yoff+r,r*2,d,calendar.hour,calendar.min,calendar.sec);//ָ��ʱ����ʾʱ��		
			gui_phy.back_color=BLACK;
			gui_show_num(xoff+10,TIME_TOPY,2,GBLUE,60,calendar.hour,0X80);	//��ʾʱ
			gui_show_num(xoff+90,TIME_TOPY,2,GBLUE,60,calendar.min,0X80);	//��ʾ��
			gui_show_num(xoff+170,TIME_TOPY,2,GBLUE,60,calendar.sec,0X80);	//��ʾ�� 					   
			if(t%2==0)//�ȴ�2����
			{		 
  				if(TEMP_SEN_TYPE)temperate=MPU_Get_Temperature()/10;//�õ�MPU6050�ɼ������¶�,0.1��
				else temperate=DS18B20_Get_Temp();//�õ�18b20�¶�
				if(temperate<0)//�¶�Ϊ������ʱ�򣬺�ɫ��ʾ
				{
					POINT_COLOR=RED;
					temperate=-temperate;	//��Ϊ���¶�
				}else POINT_COLOR=BRRED;	//����Ϊ�غ�ɫ������ʾ		
				gui_show_num(xoff+90,OTHER_TOPY,2,GBLUE,60,temperate/10,0X80);	//XX					   
				gui_show_ptchar(xoff+150,OTHER_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,'.',0);	//"." 
				gui_show_ptchar(xoff+180-15,OTHER_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,temperate%10+'0',0);//��ʾС��
				gui_show_ptchar(xoff+210-10,OTHER_TOPY,lcddev.width,lcddev.height,0,GBLUE,60,95+' ',0);//��ʾ��
				if(t>0)t=0;			 
			} 
			calendar_get_date(&calendar);	//��������		
			if(calendar.w_date!=tempdate)
			{
				calendar_date_refresh();	//�����仯��,��������.  
				tempdate=calendar.w_date;	//�޸�tempdate����ֹ�ظ�����
			}
			t++;   
 		} 
		delay_ms(20);
 	};
 	while(tp_dev.sta&TP_PRES_DOWN)tp_dev.scan(0);//�ȴ�TP�ɿ�.
 	gui_memex_free(asc2_s6030);	//ɾ��������ڴ�
	asc2_s6030=0;				//����
	gui_memin_free(f_calendar);	//ɾ��������ڴ�
	POINT_COLOR=BLUE;
	BACK_COLOR=WHITE ;	
	return rval;
}




















