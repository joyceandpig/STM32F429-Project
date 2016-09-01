#include "paint.h"	    
#include "calendar.h"	    
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

//��ɫ��·��
u8*const PAINT_COLOR_TBL_PIC="1:/SYSTEM/APP/PAINT/COLORTBL.bmp";

//������Ƕ����ɫ
#define PAINT_INWIN_FONT_COLOR		0X736C		//0XAD53		

//��������
u8*const paint_penset_tbl[GUI_LANGUAGE_NUM]=
{"��������","���P�O��","PEN SET"};
//ģʽѡ��
u8*const paint_mode_tbl[GUI_LANGUAGE_NUM][2]=
{
{"�½�����","��λͼ",},
{"�½�����","���_λ�D",},		 
{"Create new file","Open exist file",},
};

//��������ѡ��
u8*const pen_set_tbl[GUI_LANGUAGE_NUM][2]=
{
{"������ɫ����","���ʳߴ�����",},
{"���P�ɫ�O��","���P�ߴ��O��",},		 
{"Pen color Set","Pen size set",},
};	   

/////////////////////////////////////////////////////////////////////////////////
//ͨ��ʱ���ȡ�ļ���
//��ϳ�:����"0:PAINT/PAINT20120321210633.bmp"/"2:PAINT/PAINT20120321210633.bmp"���ļ���
void paint_new_pathname(u8 *pname)
{	 
	calendar_get_time(&calendar);
	calendar_get_date(&calendar); 
	if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PAINT/PAINT%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//��ѡ������SD��
	else if(gui_phy.memdevflag&(1<<3))sprintf((char*)pname,"3:PAINT/PAINT%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD��������,�򱣴���U��	
}

//��ʾRGBֵ
//xr,yr:��ɫ���ݵ����϶�������
//color:��ɫֵ
void paint_show_colorval(u16 xr,u16 yr,u16 color)
{
	PIX_RGB565 *rgb=(PIX_RGB565*)&color;//���r,g,bֵ    
	u8 cstr[7];	   
 	gui_phy.back_color=APP_WIN_BACK_COLOR;//���ñ���ɫ
	gui_show_num(xr,yr,2,PAINT_INWIN_FONT_COLOR,12,rgb->r,0X80);	//��ʾR
	gui_show_num(xr,yr+30,2,PAINT_INWIN_FONT_COLOR,12,rgb->g,0X80);	//��ʾG
	gui_show_num(xr,yr+60,2,PAINT_INWIN_FONT_COLOR,12,rgb->b,0X80);	//��ʾB
   	sprintf((char*)cstr,"0X%04X",color);
	gui_show_ptstr(xr-72,yr+13+5,xr-72+36,yr+13+5+12,0,PAINT_INWIN_FONT_COLOR,12,cstr,0);//��ʾ16λ��ɫֵ
 }		 

//x,y:���Ͻ�����
//color:������ɫָ��
//caption:��������				  
//����ֵ:0,�ɹ�����;
//    ����,����������
u8 paint_pen_color_set(u16 x,u16 y,u16* color,u8*caption)
{
 	u8 rval=0,res;
	u8 i=0;
	u8 rgbtemp[3];
	PIX_RGB565 *rgb=0;				//���r,g,bֵ
 	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//ȡ����ť		  
 	_btn_obj * okbtn=0;				//ȷ����ť		  
	_progressbar_obj * pencprgb[3];	//��ɫ������
 	u16 tempcolor=*color;
	u8 cmask=1;						//��ɫ�Ƿ��б䶯
  	twin=window_creat(x,y,180,220,0,1|1<<5,16);					//��������
 	for(i=0;i<3;i++)
	{
		pencprgb[i]=progressbar_creat(x+70,y+82+15+30*i,100,15,0X20);//����������
		if(pencprgb[i]==NULL)rval=1;
	}
	pencprgb[0]->totallen=31;	//��ɫ�������ֵ
 	pencprgb[1]->totallen=63;	//��ɫ�������ֵ
	pencprgb[2]->totallen=31;	//��ɫ�������ֵ

  	okbtn=btn_creat(x+20,y+182,60,30,0,0x02);		//������ť
 	rbtn=btn_creat(x+20+60+20,y+182,60,30,0,0x02);	//������ť
	if(twin==NULL||rbtn==NULL||okbtn==NULL||rval)rval=1;
	else
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
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

		rbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];	//����Ϊȡ��
 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];		//����Ϊȷ��
  		window_draw(twin);				//��������
		btn_draw(rbtn);					//����ť
		btn_draw(okbtn);				//����ť
		minibmp_decode((u8*)PAINT_COLOR_TBL_PIC,x+5,y+42,170,40,0,0);//��ʾ��ɫ��
  		gui_show_string("R:",x+70,y+82+2,24,12,12,PAINT_INWIN_FONT_COLOR);			//��ʾR:
  		gui_show_string("G:",x+70,y+82+2+30,24,12,12,PAINT_INWIN_FONT_COLOR);		//��ʾG:
 		gui_show_string("B:",x+70,y+82+2+60,24,12,12,PAINT_INWIN_FONT_COLOR);		//��ʾB:
  		gui_draw_rectangle(x+9,y+111+5,52,52,0X8452);		//Ԥ�����߿�
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
			res=btn_check(rbtn,&in_obj);			//ȡ����ť���
			if(res&&((rbtn->sta&0X80)==0))			//����Ч����
			{
				rval=1;
				break;//�˳�
			}
			res=btn_check(okbtn,&in_obj); 			//ȷ�ϰ�ť���
			if(res&&((okbtn->sta&0X80)==0))			//����Ч����
			{
				rval=0XFF;		   
				break;//ȷ����
			}
			if(cmask) //��������Ƿ���Ҫ����
			{
				if(cmask==1)//��������Ҫ����.
				{
					rgb=(PIX_RGB565*)&tempcolor;
					pencprgb[0]->curpos=rgb->r;
					pencprgb[1]->curpos=rgb->g;
					pencprgb[2]->curpos=rgb->b;		  
					rgbtemp[0]=rgb->r;			//��ʱ����
					rgbtemp[1]=rgb->g;
					rgbtemp[2]=rgb->b;
	 				for(i=0;i<3;i++)progressbar_draw_progressbar(pencprgb[i]);//���½�����
 				}
				cmask=0;
				paint_show_colorval(x+70+12,y+82+2,tempcolor);		//��ʾRGBֵ
				gui_fill_rectangle(x+10,y+112+5,50,50,tempcolor);	//��Ԥ��ͼ
 			}	  
			for(i=0;i<3;i++)//�������������
			{
				res=progressbar_check(pencprgb[i],&in_obj);
	 			if(res&&(rgbtemp[i]!=pencprgb[i]->curpos))	//�������Ķ���
				{
					rgbtemp[i]=pencprgb[i]->curpos;	 		//�������µĽ��
					rgb->r=rgbtemp[0];
					rgb->g=rgbtemp[1];
					rgb->b=rgbtemp[2];	   
					tempcolor=*(u16*)rgb;
					cmask=2;//������������,��������Ҫ����
 				}

			}
			if(app_tp_is_in_area(&tp_dev,x+5,y+42,170,40)&&(tp_dev.sta&TP_PRES_DOWN))
			{
				tempcolor=gui_phy.read_point(tp_dev.x[0],tp_dev.y[0]);	
				cmask=1;
			}	
 		}
	}
	window_delete(twin);							//ɾ������
	btn_delete(rbtn);								//ɾ����ť	
	btn_delete(okbtn);								//ɾ����ť	
	for(i=0;i<3;i++)progressbar_delete(pencprgb[i]);//ɾ��������  
	if(rval==0XFF)
	{
		*color=tempcolor;
		return 0;
 	}
	return rval;
}
//x,y:���Ͻ�����
//color:������ɫָ��
//caption:��������				  
//����ֵ:0,�ɹ�����;
//    ����,����������
u8 paint_pen_size_set(u16 x,u16 y,u16 color,u8 *mode,u8*caption)
{
 	u8 rval=0,res;
   	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//ȡ����ť		  
 	_btn_obj * okbtn=0;				//ȷ����ť		  
	_progressbar_obj * sizeprgb;	//�ߴ����ý�����
 	u8 smask=1;						//�ߴ��Ƿ��б䶯
	u8 tempsize=*mode;

  	twin=window_creat(x,y,180,162,0,1|1<<5,16);			//�������� ��
 	sizeprgb=progressbar_creat(x+10,y+82,160,20,0X20);	//����������
	if(sizeprgb==NULL)rval=1;
  	okbtn=btn_creat(x+20,y+122,60,30,0,0x02);		//������ť
 	rbtn=btn_creat(x+20+60+20,y+122,60,30,0,0x02);	//������ť
	if(twin==NULL||rbtn==NULL||okbtn==NULL||rval)rval=1;
	else
	{
		//���ڵ����ֺͱ���ɫ
		twin->caption=caption;
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

		sizeprgb->totallen=10;		//���뾶Ϊ10 
	   	sizeprgb->curpos=tempsize;	//��ǰ�ߴ�

		rbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];	//����Ϊȡ��
 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];		//����Ϊȷ��
  		window_draw(twin);				//��������
		btn_draw(rbtn);					//����ť
		btn_draw(okbtn);				//����ť
   		gui_show_string("SIZE:",x+10,y+42+12,40,16,16,PAINT_INWIN_FONT_COLOR);		//��ʾSIZE
  		gui_show_string("PREV:",x+170-64,y+42+12,40,16,16,PAINT_INWIN_FONT_COLOR);	//��ʾPREV
		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
			res=btn_check(rbtn,&in_obj);			//ȡ����ť���
			if(res&&((rbtn->sta&0X80)==0))			//����Ч����
			{
				rval=1;
				break;//�˳�
			}
			res=btn_check(okbtn,&in_obj); 			//ȷ�ϰ�ť���
			if(res&&((okbtn->sta&0X80)==0))			//����Ч����
			{
				rval=0XFF;		   
				break;//ȷ����
			}
			if(smask) //��������Ƿ���Ҫ����
			{
				if(smask==1)//��������Ҫ����.
				{
 	 				progressbar_draw_progressbar(sizeprgb);//���½�����
 				}
				smask=0;
  				gui_phy.back_color=APP_WIN_BACK_COLOR;//���ñ���ɫ
				gui_show_num(x+10+40,y+42+12,2,PAINT_INWIN_FONT_COLOR,16,tempsize,0X80);//��ʾ��ֵ
				gui_fill_rectangle(x+170-24,y+42+8,24,24,gui_phy.back_color);			//���Ԥ������	 
				paint_draw_point(x+170-12,y+42+20,color,tempsize);		//��Ԥ��ͼ
 			}	  
			res=progressbar_check(sizeprgb,&in_obj);
 			if(res&&(tempsize!=sizeprgb->curpos))	//�������Ķ���
			{
				tempsize=sizeprgb->curpos;	 		//�������µĽ��  
				smask=2;//������������,��������Ҫ����
			} 	
 		}
	}
	window_delete(twin);							//ɾ������
	btn_delete(rbtn);								//ɾ����ť	
	btn_delete(okbtn);								//ɾ����ť	
	progressbar_delete(sizeprgb);//ɾ��������  
	if(rval==0XFF)
	{
		*mode=tempsize;
		return 0;
 	}
	return rval;
} 
//x,y:����
//color:��ɫ
//mode:
//[7:4]:����
//[3:0]:�ߴ�.(Բ��,���뾶)
void paint_draw_point(u16 x,u16 y,u16 color,u8 mode)
{
	u8 size=mode&0X0F;//�õ��ߴ��С	    
	if(size==0)gui_phy.draw_point(x,y,color);
	else gui_fill_circle(x,y,size,color);		   
}
	  
//��ͼ����
//���л�ͼ�ļ�,��������SD��PAINT�ļ�����.
u8 paint_play(void)
{
 	DIR paintdir;					//paintdirר��	  
	FILINFO *paintinfo;		 	   
  	_btn_obj* rbtn=0;				//���ذ�ť�ؼ�
   	_filelistbox_obj * flistbox=0;
	_filelistbox_list * filelistx=0;//�ļ���
 	u16 lastpos[5][2];				//���һ�ε����� 
	
 	u16 pencolor=RED;
   	u8 *pname=0;
	u8 *fn;	 
	u8 res;
	u8 rval=0;
   	u8 key=0;
	u8 mode=0X01;					//��ͼģʽ				 
									//[7:4]:����
									//[3:0]:��뾶
	u8 editmask=0;					//�Ķ���־λ
  	u16 *pset_bkctbl=0;				//����ʱ����ɫָ��  
 	br=0;
	if(lcddev.width>=480)mode=0X02;	//�ֱ��ʴ���800*480����Ĭ�����û��ʳߴ�Ϊ2
	//��ѡ��ģʽ                                                                                                
 	res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)paint_mode_tbl[gui_phy.language],2,(u8*)&br,0X90,(u8*)APP_MODESEL_CAPTION_TBL[gui_phy.language]);//2��ѡ��
 	if(res==0)//ȷ�ϼ�������,ͬʱ������������
	{	  	
		app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);	//�Ϸֽ���
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
		paintinfo=(FILINFO*)gui_memin_malloc(sizeof(FILINFO));//����FILENFO�ڴ�
		if(!paintinfo||!rbtn)rval=1;//û���㹻�ڴ湻����
		else
		{																				
		 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//���� 
			rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
			rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
			rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
 			btn_draw(rbtn);			//�ػ���ť
		}
  		LCD_Clear(0XFFFF);				//����
 		if(br==0)						//�½�ͼƬ�ļ�
		{
			if(gui_phy.memdevflag&(1<<0))f_mkdir("0:PAINT");		//ǿ�ƴ����ļ���,����ͼ��
			if(gui_phy.memdevflag&(1<<3))f_mkdir("3:PAINT");		//ǿ�ƴ����ļ���,����ͼ��
			pname=gui_memin_malloc(40);	//����40���ֽ��ڴ�,����"0:PAINT/PAINT20120321210633.bmp"
			pname[0]='\0';//��ӽ�����
			paint_new_pathname(pname);
			if(pname==NULL)rval=1;
 		}else	//ѡ��һ��BMP�ļ���
		{
			app_filebrower((u8*)paint_mode_tbl[gui_phy.language][1],0X07);//��ʾ��Ϣ
			btn_draw(rbtn);//����ť 
			flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//����һ��filelistbox
			if(flistbox==NULL)rval=1;							//�����ڴ�ʧ��.
			else  
			{
				flistbox->fliter=FLBOX_FLT_PICTURE;	//����ͼƬ�ļ�
				flistbox->fliterex=1<<0;			//��������λͼ�ļ�
				filelistbox_add_disk(flistbox);		//��Ӵ���·��
				filelistbox_draw_listbox(flistbox);
			} 	  
	 		for(res=0;res<5;res++)lastpos[res][0]=0XFFFF; //�����趨
		   	while(rval==0)
			{
				tp_dev.scan(0);    
				in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
				delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
    			if(system_task_return)				   	//TPAD����
				{
					rval=1;
					break;			
				}
				filelistbox_check(flistbox,&in_obj);	//ɨ���ļ�
				res=btn_check(rbtn,&in_obj);
				if(res)
				{
					if(((rbtn->sta&0X80)==0))//��ť״̬�ı���
					{
						if(flistbox->dbclick!=0X81)
						{
		 					filelistx=filelist_search(flistbox->list,flistbox->selindex);//�õ���ʱѡ�е�list����Ϣ
							if(filelistx->type==FICO_DISK)
							{
								rval=1;
								break;	//�Ѿ�������������
						 	}
							else filelistbox_back(flistbox);	//�˻���һ��Ŀ¼	 
						} 
		 			}	 
				}
				if(flistbox->dbclick==0X81)//˫���ļ���
				{	    
 		     		rval=f_opendir(&paintdir,(const TCHAR*)flistbox->path); //��ѡ�е�Ŀ¼
					if(rval)break;	 			   
					dir_sdi(&paintdir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
			 		rval=f_readdir(&paintdir,paintinfo);//��ȡ�ļ���Ϣ
					if(rval)break;//�򿪳ɹ�    
					fn=(u8*)(paintinfo->fname);
					pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//�����ڴ�
					if(pname==NULL)rval=1;//����ʧ��
					{
						pname=gui_path_name(pname,flistbox->path,fn);	//�ļ�������·��  
						LCD_Clear(WHITE);
						ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//�������ͼƬ
    					break;
					} 
				}
			}
		}	  
 		pset_bkctbl=gui_memex_malloc(180*220*2);//Ϊ����ʱ�ı���ɫ�������ڴ�
		if(pset_bkctbl==NULL)rval=1;//�����ڴ����   
   	 	while(rval==0) 
		{		 
			tp_dev.scan(0); 		 
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ 	  
    		if(system_task_return)break;			//TPAD����	 
 			key=KEY_Scan(0);				//ɨ�谴��
			if(key==4)						//WK_UP����
			{
				app_read_bkcolor((lcddev.width-180)/2,(lcddev.height-240)/2,180,220,pset_bkctbl);//��ȡ����ɫ
				//�����Ի���	 																				   
				res=app_items_sel((lcddev.width-180)/2,(lcddev.height-240)/2+20,180,72+40*2,(u8**)pen_set_tbl[gui_phy.language],2,(u8*)&br,0X90,(u8*)paint_penset_tbl[gui_phy.language]);//2��ѡ��
				if(res==0)//ȷ�ϼ�����
				{
					if(br==0)	//���û�����ɫ
					{
						paint_pen_color_set((lcddev.width-180)/2,(lcddev.height-240)/2,&pencolor,(u8*)pen_set_tbl[gui_phy.language][0]);
					}else 		//���û��ʳߴ�
					{
						paint_pen_size_set((lcddev.width-180)/2,(lcddev.height-240)/2+20,pencolor,&mode,(u8*)pen_set_tbl[gui_phy.language][1]);
					}
				}
				app_recover_bkcolor((lcddev.width-180)/2,(lcddev.height-240)/2,180,220,pset_bkctbl);//�ָ�����ɫ
 			} 
			//����������
			if(tp_dev.sta&TP_PRES_DOWN)			
			{	
				editmask=1;
				paint_draw_point(tp_dev.x[0],tp_dev.y[0],pencolor,mode);	//������	
			}else delay_ms(1000/OS_TICKS_PER_SEC);	//û�а������µ�ʱ�� ��ʱһ��ʱ�ӽ���	 
			//������
			if(tp_dev.touchtype&0X80)		//������
			{
				for(res=0;res<CT_MAX_TOUCH; res++)
				{
					if((tp_dev.sta)&(1<<res))
					{
						if(tp_dev.x[res]<lcddev.width&&tp_dev.y[res]<lcddev.height)
						{
							if(lastpos[res][0]==0XFFFF)
							{
								lastpos[res][0] = tp_dev.x[res];
								lastpos[res][1] = tp_dev.y[res];
							}
							gui_draw_bline(lastpos[res][0],lastpos[res][1],tp_dev.x[res],tp_dev.y[res],mode&0X0F,pencolor);//����
							lastpos[res][0]=tp_dev.x[res];
							lastpos[res][1]=tp_dev.y[res];  
						}
					}else lastpos[res][0]=0XFFFF;	                                                                                                                 
				} 
				if((tp_dev.sta&0X1F)==0)delay_ms(1000/OS_TICKS_PER_SEC);//û�а������µ�ʱ�� ��ʱһ��ʱ�ӽ���	 
			} 
		}
 	}	 			 						  
	if(editmask)//�б༭��,��Ҫ����
	{	   
 		res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)APP_SAVE_CAPTION_TBL[gui_phy.language],12,0,0X03|1<<6,0);
 		if(res==1)//��Ҫ����
		{
			if(bmp_encode(pname,0,0,lcddev.width,lcddev.height,1))//����BMP�ļ�
			{
				window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)APP_CREAT_ERR_MSG_TBL[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//��ʾSD���Ƿ����
				delay_ms(2000);//�ȴ�2����
		 	}
		}
	}
	filelistbox_delete(flistbox);
 	btn_delete(rbtn);
   	gui_memex_free(pset_bkctbl);
  	gui_memin_free(paintinfo); 
  	gui_memin_free(pname); 
	return rval;

}








