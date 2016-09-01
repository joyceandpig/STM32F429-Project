#include "gradienter.h"
#include "gyroscope.h"
#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-ˮƽ�� ����	   
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
  
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void grad_draw_hline(short x0,short y0,u16 len,u16 color)
{  
	short ex;
	ex=x0+len-1;
	if(y0<0)return;
	if(y0>(mlcddev.height-1))return;
	if(ex<0)return;
	if(ex>(mlcddev.width-1))ex=mlcddev.width-1;
	if(x0>(mlcddev.width-1))return;
	if(x0<0)x0=0;
	mlcd_fill(x0,y0,ex,y0,color);	
}

//��ʵ��Բ
//x0,y0:����
//r�뾶
//color:��ɫ
void grad_fill_circle(short x0,short y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	grad_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax) 
		{
			// draw lines from outside  
			if (x>imax) 
			{
				grad_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				grad_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		grad_draw_hline(x0-x,y0+i,2*x,color);
		grad_draw_hline(x0-x,y0-i,2*x,color);
	}
}

//��������
//����ֵ:0,����;
//    ����,ʧ��.
u8 grad_load_font(void)
{
	u8 rval=0;
	u8 res;
	FIL* f_grad=0;	 
	f_grad=(FIL *)gui_memin_malloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_grad==NULL)rval=1;							//����ʧ��
	else
	{
		if(lcddev.width<=272)
		{
			res=f_open(f_grad,(const TCHAR*)APP_ASCII_S7236,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_7236=(u8*)gui_memex_malloc(f_grad->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_7236==0)rval=1;
				else res=f_read(f_grad,asc2_7236,f_grad->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
			} 
		}else if(lcddev.width==320)
		{
			res=f_open(f_grad,(const TCHAR*)APP_ASCII_S8844,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_8844=(u8*)gui_memex_malloc(f_grad->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_8844==0)rval=1;
				else res=f_read(f_grad,asc2_8844,f_grad->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
			} 
		}else if(lcddev.width>=480)
		{
			res=f_open(f_grad,(const TCHAR*)APP_ASCII_S14472,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_14472=(u8*)gui_memex_malloc(f_grad->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_14472==0)rval=1;
				else res=f_read(f_grad,asc2_14472,f_grad->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
			}  
		} 
		if(res)rval=res;
	}
	gui_memin_free(f_grad);//�ͷ��ڴ�
	return rval;
}
//ɾ������ 
void grad_delete_font(void)
{
	if(lcddev.width==240)
	{
		gui_memex_free(asc2_7236);
		asc2_7236=0;
	}else if(lcddev.width==320)
	{
		gui_memex_free(asc2_8844);
		asc2_8844=0;
	}else if(lcddev.width==480)
	{
		gui_memex_free(asc2_14472);
		asc2_14472=0;
	}  
} 

//ˮƽ�ǹ���
u8 grad_play(void)
{
	
	u8 res,rval=0;   
	u8 t=0;
	u16 angy;				//��ʾ�Ƕȵ���������ʼλ��
	u8 fsize;
	u8 *buf;
	u8 len;  
	u8 stable;
	u16 bkcolor=BLACK;		//Ĭ�ϱ���ɫΪ��ɫ
	
	float pitch,roll,yaw; 	//ŷ����  	
	
	short xcenter,ycenter;	//��������
	short xr1,yr1,wr1;		//Բ1����ͱ߳�
	short xr2,yr2,wr2;		//Բ2����ͱ߳�
	short scalefac;			//��������
	short oldxr2,oldyr2;
	double angle;
	//��ʾ��ʼ���MPU9250 ��MPL
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)gyro_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	app_wm8978_volset(0);				//����
	printf("mpu_dmp_init\r\n");
	OSSchedLock(); 
	res=mpu_dmp_init();					//��ʼ��DMP  
	OSSchedUnlock();
	app_wm8978_volset(wm8978set.mvol);	//���»ָ�����	
	printf("grad init result:%d\r\n",res);
	if(res)
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)gyro_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);   
		rval=1;
	}else rval=grad_load_font();//�������� 
	printf("grad_load_font result:%d\r\n",rval);
	if(lcddev.width==240)
	{
		fsize=72;angy=230;
	}else if(lcddev.width==272)
	{
		fsize=72;angy=350;
	}else if(lcddev.width==320)
	{
		fsize=88;angy=370;
	}else if(lcddev.width==480)
	{
		fsize=144;angy=600;
	}else if(lcddev.width==600)
	{
		fsize=144;angy=750;
	}
	buf=gui_memin_malloc(32);	//�����ڴ�  
	if(rval==0) 
	{
		mlcd_init(lcddev.width,lcddev.height,1);//��ʼ��mlcd
		mlcd_clear(BLACK); 
		xcenter=lcddev.width/2;
		ycenter=lcddev.height/2; 
		wr1=lcddev.width/6;
		wr2=wr1+2; 
		scalefac=10;
		xr1=xcenter-wr1/2;
		yr1=ycenter-wr1/2;
		xr2=xcenter-wr2/2;
		yr2=ycenter-wr2/2;
		gui_phy.draw_point=(void(*)(u16,u16,u32))mlcd_draw_point;
		system_task_return=0;
		printf("loop get mpu data\r\n");
		while(1)
		{ 
			if(system_task_return)break;				//TPAD����  
			if(mpu_mpl_get_data(&pitch,&roll,&yaw)==0)
			{ 
				printf("loop get mpu data ok\r\n");
				if((t%10)==0)
				{  
					angle=sqrt(pitch*pitch+roll*roll); 
					if(angle>=0.1)
					{
						if(bkcolor!=BLACK)
						{
							bkcolor=BLACK; 
							oldxr2=0;oldyr2=0;
						}
						angle=-angle;
						stable=0;
					}else 
					{ 
						
						if(stable<10)stable++;	//�ȴ�״̬�ȶ�
						else
						{
							if(bkcolor!=GREEN)
							{
								bkcolor=GREEN; 
								oldxr2=0;oldyr2=0;
							} 
						}
					}
					yr1=ycenter+pitch*scalefac;
					yr2=ycenter-pitch*scalefac;
					xr1=xcenter+roll*scalefac;
					xr2=xcenter-roll*scalefac; 
					if(oldxr2!=xr2||oldyr2!=yr2)
					{  
						mlcd_clear(bkcolor);
						oldxr2=xr2;oldyr2=yr2; 
						grad_fill_circle(xr2,yr2,wr2,RED); 
						grad_fill_circle(xr1,yr1,wr1,BLUE); 
						sprintf((char*)buf,"%.1f",angle);
						len=strlen((char*)buf);
						buf[len]=95+' ';
						buf[len+1]=0; 
						gui_phy.back_color=bkcolor; 
						gui_show_strmid(0,angy,lcddev.width,fsize,WHITE,fsize,buf);	//��ʾ�½Ƕ� 
						mlcd_display_layer(0);
					}  
					t++;
					continue; 
				}
			}
			t++; 
			delay_ms(3);
		} 
		mlcd_delete();							//ɾ��mlcd
		gui_phy.draw_point=LCD_Fast_DrawPoint;	//�ָ�Ϊԭ���Ļ��㺯�� 
	}
	grad_delete_font();	//ɾ������
	gui_memin_free(buf);//�ͷ��ڴ�
	return rval;
}


