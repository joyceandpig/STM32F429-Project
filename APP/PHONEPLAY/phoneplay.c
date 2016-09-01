#include "phoneplay.h"
#include "spb.h" 
//#include "camera.h" 
//#include "sai.h" 
//#include "audioplay.h" 
#include "usart.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-������ ����	   
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

//��������
//�����������ȼ�
#define PHONE_INCALL_TASK_PRIO      	4
//���������ջ��С
#define PHONE_INCALL_STK_SIZE  		    512
//�����ջ�������ڴ����ķ�ʽ��������	
OS_STK * PHONE_INCALL_TASK_STK=0;	
//������
void phone_incall_task(void *pdata);


//��绰ͼƬ
u8*const PHONE_CALL_PIC[3]=
{
	"1:/SYSTEM/APP/PHONE/call_28.bmp",
	"1:/SYSTEM/APP/PHONE/call_36.bmp",
	"1:/SYSTEM/APP/PHONE/call_54.bmp",		
};
//�ҵ绰ͼƬ
u8*const PHONE_HANGUP_PIC[3]=
{
"1:/SYSTEM/APP/PHONE/hangup_28.bmp",
"1:/SYSTEM/APP/PHONE/hangup_36.bmp",
"1:/SYSTEM/APP/PHONE/hangup_54.bmp",		
};
//�˸�ͼƬ
u8*const PHONE_DEL_PIC[3]=
{
"1:/SYSTEM/APP/PHONE/del_28.bmp",
"1:/SYSTEM/APP/PHONE/del_36.bmp",	
"1:/SYSTEM/APP/PHONE/del_54.bmp",		
};
//�绰��ʾ��Ϣ
u8*const phone_remind_tbl[4][GUI_LANGUAGE_NUM]=
{
"������...","��̖��...","Dialing...",
"����...","����...","Calling...",
"δ�ҵ�GSMģ��,����!","δ�ҵ�GSMģ�K,Ո�z��!","GSM Module Error!Pleas Check!",
"SIM���쳣,����!","SIM������,Ո�z��!","SIM Card Error!Pleas Check!",
}; 
//�������Ʊ�
u8*const call_btnstr_tbl[14]=
{   						   
"1","2","3",
"4","5","6",
"7","8","9",
"*","0","#", 
"call","del",	
};
//��ʾ��������
void phone_show_clear(_phonedis_obj *pdis)
{
	gui_fill_rectangle(pdis->xoff,pdis->yoff,pdis->width,pdis->height,BLACK);//���ԭ������ʾ
}
//��ʾͨ��ʱ��
//pdis:��ʾ�ṹ��
void phone_show_time(_phonedis_obj *pdis)
{
	u8 *pbuf;
	u16 yoff;
	u16 xoff;
	pbuf=gui_memin_malloc(100);
	if(pbuf==0)return ;
	sprintf((char*)pbuf,"%03d:%02d",pdis->time/60,pdis->time%60); 
	gui_phy.back_color=BLACK;
	xoff=pdis->xoff+pdis->width/2-5*pdis->tsize/2;
	yoff=pdis->yoff+pdis->height/2-pdis->tsize/2;
	gui_fill_rectangle(xoff,yoff,5*pdis->tsize,pdis->tsize,BLACK);//���ԭ������ʾ
	gui_show_strmid(xoff,yoff,5*pdis->tsize,pdis->tsize,WHITE,pdis->tsize,pbuf); 
	gui_memin_free(pbuf);//�ͷ��ڴ�
}
//��ʾ������
//pdis:��ʾ�ṹ��
//gsmx:GSM״̬�ṹ��
void phone_show_calling(_phonedis_obj *pdis,__gsmdev *gsmx)
{
	u16 yoff;
	u16 xoff; 
	gui_phy.back_color=BLACK;
	xoff=pdis->xoff+pdis->width/2-5*pdis->tsize/2;
	yoff=pdis->yoff+pdis->height/2-pdis->tsize/2;
	gui_fill_rectangle(xoff,yoff,5*pdis->tsize,pdis->tsize,BLACK);//���ԭ������ʾ
	if(gsmx->mode==3)gui_show_strmid(xoff,yoff,5*pdis->tsize,pdis->tsize,WHITE,pdis->tsize,phone_remind_tbl[1][gui_phy.language]); 
	else gui_show_strmid(xoff,yoff,5*pdis->tsize,pdis->tsize,WHITE,pdis->tsize,phone_remind_tbl[0][gui_phy.language]); 
}
//��ʾ�绰����
//pdis:��ʾ�ṹ��
//gsmx:GSM״̬�ṹ��
void phone_show_phone(_phonedis_obj *pdis,__gsmdev*gsmx)
{
	u16 yoff;
	u16 xoff;
	u8 maxnum;
	maxnum=pdis->width/(pdis->psize/2);
	if(pdis->plen>maxnum)maxnum=pdis->plen-maxnum;
	else maxnum=0;
	if(gsmx->mode==0)//��������ģʽ,�������ڵ绰������ʾ
	{
		xoff=pdis->xoff;
		yoff=pdis->yoff+pdis->height/2-pdis->psize/2;
		gui_fill_rectangle(xoff,yoff,pdis->width,pdis->psize,BLACK);//���ԭ������ʾ
		gui_show_strmid(xoff,yoff,pdis->width,pdis->psize,WHITE,pdis->psize,pdis->phonebuf+maxnum);//��ʾ�绰����
	}else
	{
		xoff=pdis->xoff;
		yoff=pdis->yoff+pdis->height/2-pdis->tsize/2-pdis->psize;
		gui_fill_rectangle(xoff,yoff,pdis->width,pdis->psize,BLACK);//���ԭ������ʾ
		gui_show_strmid(xoff,yoff,pdis->width,pdis->psize,WHITE,pdis->psize,pdis->phonebuf+maxnum);//��ʾ�绰���� 
	}
}
//��ʾ�����ַ���
//pdis:��ʾ�ṹ��
//gsmx:GSM״̬�ṹ��
void phone_show_input(_phonedis_obj *pdis,__gsmdev*gsmx)
{	
	u16 yoff;
	u16 xoff;
	u8 maxnum;
	if(gsmx->mode==2)//���ڵ绰��ͨ����Ч
	{
		maxnum=pdis->width/(pdis->psize/2);
		if(pdis->inlen>maxnum)maxnum=pdis->inlen-maxnum;
		else maxnum=0;
		xoff=pdis->xoff;
		yoff=pdis->yoff+pdis->height/2+pdis->tsize;
		gui_fill_rectangle(xoff,yoff,pdis->width,pdis->psize,BLACK);//���ԭ������ʾ
		gui_show_strmid(xoff,yoff,pdis->width,pdis->psize,WHITE,pdis->psize,pdis->inbuf+maxnum);//��ʾ��������
	}
} 
//�绰��ʾ��
void phone_ring(void)
{
	u8 i;	 
	for(i=0;i<2;i++)
	{
//		PCF8574_WriteBit(BEEP_IO,0);
		delay_ms(100);
//		PCF8574_WriteBit(BEEP_IO,1);
		delay_ms(50);
	}	 
}
//��������,ר�Ŵ���绰����.ͨ���������򴴽�
void phone_incall_task(void *pdata)
{ 
	u16 *bkcolor;
	_btn_obj* p_btn[14];//�ܹ�14������ 
	_phonedis_obj *pdis;
	u8 btnxsize,btnysize; 
	u8 btnxydis;
	u8 fsize;
	u8 lcdtype;
	u16 yoff;
	 
 	u8 i,j;
	u8 rval=0;
	u8 res=0;  
	FIL* f_phone=0;	 
	u32 calltime;
	u8 ascflag=0;
	u8 dcmiflag=0;
	
	bkcolor=gui_memex_malloc(lcddev.width*lcddev.height*2); 
	pdis=gui_memin_malloc(sizeof(_phonedis_obj));
	if(pdis)
	{
		memset(pdis,0,sizeof(_phonedis_obj));			//�������� 
		pdis->inbuf=gui_memin_malloc(PHONE_MAX_INPUT);
		pdis->phonebuf=gui_memin_malloc(PHONE_MAX_INPUT); 
	}
	f_phone=(FIL *)gui_memin_malloc(sizeof(FIL));//����FIL�ֽڵ��ڴ����� 	
	if(bkcolor&&pdis->phonebuf&&f_phone)//�ڴ�����ɹ���
	{
		OSTaskSuspend(6); 	//����������
//		if(audiodev.status&(1<<4))
//		{
//			app_wm8978_volset(0);	 
//			SAI_Play_Stop();//ֹͣ��Ƶ����
//		} 
		if(DCMI->CR&0X01)//����ͷ���ڹ���?
		{
			dcmiflag=1;
//			DCMI_Stop();//�ر�����ͷ
//			sw_sdcard_mode();//�л�ΪSD��ģʽ
		}
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//����Ϊȫ������
		app_read_bkcolor(0,0,lcddev.width,lcddev.height,bkcolor);	//��ȡ����ɫ 
		if(lcddev.width<=272&&asc2_2814==0)
		{ 
			ascflag=1;//��������ַ����ڴ�
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_2814,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_2814=(u8*)gui_memex_malloc(f_phone->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_2814==0)rval=1;
				else res=f_read(f_phone,asc2_2814,f_phone->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
				f_close(f_phone);
			} 
		}else if(lcddev.width==320&&asc2_3618==0)
		{
			ascflag=1;//��������ַ����ڴ�
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_3618,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_3618=(u8*)gui_memex_malloc(f_phone->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_3618==0)rval=1;
				else res=f_read(f_phone,asc2_3618,f_phone->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
				f_close(f_phone);
			} 
		}else if(lcddev.width>=480&&asc2_5427==0)
		{
			ascflag=1;//��������ַ����ڴ�
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_5427,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_5427=(u8*)gui_memex_malloc(f_phone->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_5427==0)rval=1;
				else res=f_read(f_phone,asc2_5427,f_phone->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
				f_close(f_phone);
			}
		}
		if(res)rval=res; 
		if(rval==0)//����ɹ�
		{
			LCD_Clear(BLACK);
			if(lcddev.width==240)
			{ 
				btnxsize=78; 
				btnysize=40;
				btnxydis=2;
				fsize=28; 
				pdis->psize=28;
				pdis->tsize=12;
				lcdtype=0;
			}if(lcddev.width==272)
			{ 
				btnxsize=86; 
				btnysize=50;
				btnxydis=4;
				fsize=28; 
				pdis->psize=28;
				pdis->tsize=12;
				lcdtype=0;
			}else if(lcddev.width==320)
			{ 
				btnxsize=102; 
				btnysize=60;
				btnxydis=4;
				fsize=36;
				pdis->psize=36;
				pdis->tsize=16;
				lcdtype=1;
			}else if(lcddev.width==480)
			{ 
				btnxsize=154; 
				btnysize=100;
				btnxydis=6; 
				fsize=54;
				pdis->psize=54;
				pdis->tsize=24;
				lcdtype=2;
			}else if(lcddev.width==600)
			{ 
				btnxsize=190; 
				btnysize=125;
				btnxydis=10; 
				fsize=54;
				pdis->psize=54;
				pdis->tsize=24;
				lcdtype=2;
			}
			pdis->xoff=0;
			pdis->yoff=0;
			pdis->width=lcddev.width;
			pdis->height=lcddev.height-5*btnysize-6*btnxydis;
			yoff=lcddev.height-btnysize*5-btnxydis*6;
			for(i=0;i<5;i++)//����14����ť
			{
				for(j=0;j<3;j++)
				{
					res=i*3+j; 
					if(i==4)
					{
						if(j==2)break;
						p_btn[res]=btn_creat(j*(btnxsize+btnxsize/2+(btnxydis*3)/2)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,(btnxsize*3)/2+btnxydis/2,btnysize,0,1);	//����ͼƬ��ť
						if(j==0)
						{
							p_btn[res]->bcfdcolor=0X76B2;;	//����ʱ�ı���ɫ
							p_btn[res]->bcfucolor=0X3DEA;	//�ɿ�ʱ����ɫ 
							p_btn[res]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
							p_btn[res]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
						}else
						{
							p_btn[res]->bcfdcolor=0XFBEF;;	//����ʱ�ı���ɫ
							p_btn[res]->bcfucolor=0XFA69;	//�ɿ�ʱ����ɫ 
							p_btn[res]->picbtnpathu=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[res]->picbtnpathd=(u8*)PHONE_HANGUP_PIC[lcdtype];
						} 
					}else 
					{
						p_btn[res]=btn_creat(j*(btnxsize+btnxydis)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,btnxsize,btnysize,0,2);//����Բ�ǰ�ť
						p_btn[res]->bkctbl[0]=0X39E7;//�߿���ɫ
						p_btn[res]->bkctbl[1]=0X73AE;//��һ�е���ɫ				
						p_btn[res]->bkctbl[2]=0X4208;//�ϰ벿����ɫ
						p_btn[res]->bkctbl[3]=0X3186;//�°벿����ɫ	 
						p_btn[res]->bcfucolor=WHITE;//�ɿ�ʱΪ��ɫ
						p_btn[res]->bcfdcolor=BLACK;//����ʱΪ��ɫ
						p_btn[res]->caption=(u8*)call_btnstr_tbl[res]; 
					} 
					if(lcddev.width<=272)p_btn[res]->font=16;
					else p_btn[res]->font=24;	 
					if(p_btn[res]==NULL){rval=1;break;}//����ʧ��. 
					p_btn[res]->sta=0;
					p_btn[res]->font=fsize;
					btn_draw(p_btn[res]); 
				}	 
			}
		} 
		memset(pdis->inbuf,0,PHONE_MAX_INPUT);
		memset(pdis->phonebuf,0,PHONE_MAX_INPUT);
		strcpy((char*)pdis->phonebuf,(char*)gsmdev.incallnum);//�������뵽phonebuf����
		pdis->plen=strlen((char*)pdis->phonebuf);//�õ����볤��
		phone_show_phone(pdis,&gsmdev);	//��ʾ�绰����
		phone_show_calling(pdis,&gsmdev);//������
		while(gsmdev.mode!=0)//�ǲ���ģʽ
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(5);							//��ʱһ��ʱ�ӽ���
			if(system_task_return)break;			//TPAD����,���ܾ�����
			if(USART3_RX_STA&0X8000)				//���յ�����
			{  
				if(gsm_check_cmd("NO CARRIER"))gsmdev.mode=0;	//����ʧ��
				if(gsm_check_cmd("NO ANSWER"))gsmdev.mode=0;		//����ʧ��
				if(gsm_check_cmd("ERROR"))gsmdev.mode=0;			//����ʧ�� 
				USART3_RX_STA=0; 
			}
			if(gsmdev.mode==2)//ͨ����
			{
				if((OSTime-calltime)>=200)//����1������
				{
					calltime=OSTime;
					pdis->time++;
					phone_show_time(pdis);//��ʾʱ��
				}
			} 
			for(i=0;i<14;i++)
			{
				res=btn_check(p_btn[i],&in_obj);   
				if(res&&((p_btn[i]->sta&(1<<7))==0)&&(p_btn[i]->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
				{
					if(i<12)//1~#
					{
						if(gsmdev.mode==2)//����ͨ������Ч
						{
							if(pdis->inlen<PHONE_MAX_INPUT-1)
							{
								u3_printf("AT+CLDTMF=2,\"%c\"\r\n",call_btnstr_tbl[i][0]);
								delay_ms(100);
								u3_printf("AT+VTS=%c\r\n",call_btnstr_tbl[i][0]); 
								pdis->inbuf[pdis->inlen]=call_btnstr_tbl[i][0];	//����ַ�
								pdis->inlen++;
								pdis->inbuf[pdis->inlen]=0;//��ӽ�����
							}
							phone_show_input(pdis,&gsmdev);//��ʾ�������� 
						}
					}
					if(i==12)//��������
					{
						if(gsmdev.mode==3)
						{
							gsmdev.mode=2;//ͨ����
							p_btn[12]->bcfdcolor=0XFBEF;;	//����ʱ�ı���ɫ
							p_btn[12]->bcfucolor=0XFA69;	//�ɿ�ʱ����ɫ 
							p_btn[12]->picbtnpathu=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[13]->bcfdcolor=0X630C;;	//����ʱ�ı���ɫ
							p_btn[13]->bcfucolor=0X4208;	//�ɿ�ʱ����ɫ 
							p_btn[13]->picbtnpathu=(u8*)PHONE_DEL_PIC[lcdtype];
							p_btn[13]->picbtnpathd=(u8*)PHONE_DEL_PIC[lcdtype];
							btn_draw(p_btn[13]);
							gsm_send_cmd("ATA","OK",200);//�����绰
							gsm_cmd_over();	//�������
							calltime=OSTime;	//��¼�ոս���ͨ��ʱ��ʱ�� 
						}else 
						{
 							gsmdev.mode=0;//�ָ�����ģʽ
							p_btn[12]->bcfdcolor=0X76B2;;	//����ʱ�ı���ɫ
							p_btn[12]->bcfucolor=0X3DEA;	//�ɿ�ʱ����ɫ 
							p_btn[12]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
							phone_show_clear(pdis);			//���ԭ������ʾ
							pdis->inlen=0;
							pdis->inbuf[pdis->inlen]=0;		//������
							gsmdev.mode=0;				//���벦��ģʽ
							USART3_RX_STA=0;
						}
						btn_draw(p_btn[12]);
					}
					if(i==13)//�ܾ�����
					{  
 						if(gsmdev.mode==2)//ͨ����
						{ 
							if(pdis->inlen)pdis->inlen--; 
							pdis->inbuf[pdis->inlen]=0;		//������
						}else
						{
							gsmdev.mode=0;	//���벦��ģʽ
							break;
						}
					}  
				}
			} 
		} 
		gsm_send_cmd("ATH","OK",200);	//�һ�
		gsm_cmd_over();//�������
		LCD_Clear(BLACK);
		app_recover_bkcolor(0,0,lcddev.width,lcddev.height,bkcolor);//�ָ�����ɫ
//		if(audiodev.status&(1<<4))
//		{
//			SAI_Play_Start();//������Ƶ����
//			app_wm8978_volset(wm8978set.mvol);	 
//		} 
		OSTaskResume(6); 		//�ָ�������
		for(i=0;i<14;i++)btn_delete(p_btn[i]);//ɾ����ť
	}  
	gui_memin_free(f_phone);		//�ͷ��ڴ�
	gui_memin_free(pdis->inbuf);	//�ͷ��ڴ�
	gui_memin_free(pdis->phonebuf);	//�ͷ��ڴ�
	gui_memin_free(pdis);			//�ͷ��ڴ�
	if(ascflag)//���ڴ�����,���ͷ�
	{
		if(lcddev.width<=272)
		{
			gui_memex_free(asc2_2814);
			asc2_2814=0;
		}else if(lcddev.width==320)
		{
			gui_memex_free(asc2_3618);
			asc2_3618=0;
		}else if(lcddev.width>=480)
		{
			gui_memex_free(asc2_5427);
			asc2_5427=0;
		} 
	}
	if(dcmiflag)
	{
//		sw_ov5640_mode();
//		DCMI_Start();	//������������ͷ
	}
	gui_memex_free(bkcolor);	//�ͷ��ڴ�
	gsmdev.mode=0;			//���벦��ģʽ
	system_task_return=0;		//��ֹ�˳���������
	phone_incall_task_delete();	//ɾ������ 
} 

//������������
//����ֵ:0,�ɹ�
//    ����,�������
u8 phone_incall_task_creat(void)
{ 
    OS_CPU_SR cpu_sr=0; 	
	u8 res;
	if(PHONE_INCALL_TASK_STK)return 0;//�����Ѿ�������
 	PHONE_INCALL_TASK_STK=gui_memin_malloc(PHONE_INCALL_STK_SIZE*sizeof(OS_STK));
	if(PHONE_INCALL_TASK_STK==0)return 1;	//�ڴ�����ʧ��
	OS_ENTER_CRITICAL();//�����ٽ���(�޷����жϴ��)    
	res=OSTaskCreate(phone_incall_task,(void *)0,(OS_STK*)&PHONE_INCALL_TASK_STK[PHONE_INCALL_STK_SIZE-1],PHONE_INCALL_TASK_PRIO);						   
	OS_EXIT_CRITICAL();	//�˳��ٽ���(���Ա��жϴ��) 
	return res;
}
//ɾ����������
void phone_incall_task_delete(void)
{  
	gui_memin_free(PHONE_INCALL_TASK_STK);//�ͷ��ڴ�
	PHONE_INCALL_TASK_STK=0;
	OSTaskDel(PHONE_INCALL_TASK_PRIO);	//ɾ�����ֲ������� 
}
//�绰����������
u8 phone_play(void)
{
	_btn_obj* p_btn[14];//�ܹ�14������ 
	_phonedis_obj *pdis;
	u8 btnxsize,btnysize; 
	u8 btnxydis;
	u8 fsize;
	u8 lcdtype;
	u16 yoff;
	 
 	u8 i,j;
	u8 rval=0;
	u8 res=0;  
	FIL* f_phone=0;	 
	u32 calltime;
	
	pdis=gui_memin_malloc(sizeof(_phonedis_obj));
	if(pdis==NULL)return 1;								//���벻�ɹ�,ֱ���˳�
	memset(pdis,0,sizeof(_phonedis_obj));				//��������
	pdis->inbuf=gui_memin_malloc(PHONE_MAX_INPUT);
	pdis->phonebuf=gui_memin_malloc(PHONE_MAX_INPUT); 
   	f_phone=(FIL *)gui_memin_malloc(sizeof(FIL));		//����FIL�ֽڵ��ڴ����� 
	if(!pdis->phonebuf||!pdis->inbuf||!f_phone)rval=1;	//����ʧ��
	else
	{
		if(lcddev.width<=272)
		{
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_2814,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_2814=(u8*)gui_memex_malloc(f_phone->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_2814==0)rval=1;
				else res=f_read(f_phone,asc2_2814,f_phone->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
				f_close(f_phone);
			} 
		}else if(lcddev.width==320)
		{
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_3618,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_3618=(u8*)gui_memex_malloc(f_phone->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_3618==0)rval=1;
				else res=f_read(f_phone,asc2_3618,f_phone->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
				f_close(f_phone);
			} 
		}else if(lcddev.width>=480)
		{
			res=f_open(f_phone,(const TCHAR*)APP_ASCII_5427,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_5427=(u8*)gui_memex_malloc(f_phone->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_5427==0)rval=1;
				else res=f_read(f_phone,asc2_5427,f_phone->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
				f_close(f_phone);
			}
		} 
		if(res)rval=res;
	}  
	if(rval==0)//����ɹ�
	{
		LCD_Clear(BLACK);
		if(lcddev.width==240)
		{ 
			btnxsize=78; 
			btnysize=40;
			btnxydis=2;
			fsize=28; 
			pdis->psize=28;
			pdis->tsize=12;
			lcdtype=0;
		}if(lcddev.width==272)
		{ 
			btnxsize=86; 
			btnysize=50;
			btnxydis=4;
			fsize=28; 
			pdis->psize=28;
			pdis->tsize=12;
			lcdtype=0;
		}else if(lcddev.width==320)
		{ 
			btnxsize=102; 
			btnysize=60;
			btnxydis=4;
			fsize=36;
			pdis->psize=36;
			pdis->tsize=16;
			lcdtype=1;
		}else if(lcddev.width==480)
		{ 
			btnxsize=154; 
			btnysize=100;
			btnxydis=6; 
			fsize=54;
			pdis->psize=54;
			pdis->tsize=24;
			lcdtype=2;
		}else if(lcddev.width==600)
		{ 
			btnxsize=190; 
			btnysize=125;
			btnxydis=10; 
			fsize=54;
			pdis->psize=54;
			pdis->tsize=24;
			lcdtype=2;
		}
		pdis->xoff=0;
		pdis->yoff=0;
		pdis->width=lcddev.width;
		pdis->height=lcddev.height-5*btnysize-6*btnxydis;
		yoff=lcddev.height-btnysize*5-btnxydis*6;
		for(i=0;i<5;i++)//����14����ť
		{
			for(j=0;j<3;j++)
			{
				res=i*3+j; 
				if(i==4)
				{
					if(j==2)break;
					p_btn[res]=btn_creat(j*(btnxsize+btnxsize/2+(btnxydis*3)/2)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,(btnxsize*3)/2+btnxydis/2,btnysize,0,1);	//����ͼƬ��ť
 					if(j==0)
					{
						p_btn[res]->bcfdcolor=0X76B2;;	//����ʱ�ı���ɫ
						p_btn[res]->bcfucolor=0X3DEA;	//�ɿ�ʱ����ɫ 
						p_btn[res]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
						p_btn[res]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
					}else
					{
						p_btn[res]->bcfdcolor=0X630C;;	//����ʱ�ı���ɫ
						p_btn[res]->bcfucolor=0X4208;	//�ɿ�ʱ����ɫ 
						p_btn[res]->picbtnpathu=(u8*)PHONE_DEL_PIC[lcdtype];
						p_btn[res]->picbtnpathd=(u8*)PHONE_DEL_PIC[lcdtype];
					} 
				}else 
				{
					p_btn[res]=btn_creat(j*(btnxsize+btnxydis)+btnxydis/2,yoff+i*(btnysize+btnxydis)+btnxydis,btnxsize,btnysize,0,2);//����Բ�ǰ�ť
					p_btn[res]->bkctbl[0]=0X39E7;//�߿���ɫ
					p_btn[res]->bkctbl[1]=0X73AE;//��һ�е���ɫ				
					p_btn[res]->bkctbl[2]=0X4208;//�ϰ벿����ɫ
					p_btn[res]->bkctbl[3]=0X3186;//�°벿����ɫ	 
					p_btn[res]->bcfucolor=WHITE;//�ɿ�ʱΪ��ɫ
					p_btn[res]->bcfdcolor=BLACK;//����ʱΪ��ɫ
					p_btn[res]->caption=(u8*)call_btnstr_tbl[res]; 
				} 
				if(lcddev.width<=272)p_btn[res]->font=16;
				else p_btn[res]->font=24;	 
				if(p_btn[res]==NULL){rval=1;break;}//����ʧ��. 
				p_btn[res]->sta=0;
				p_btn[res]->font=fsize;
			}	 
		}
	}
	if(rval==0)
	{ 
		for(i=0;i<14;i++)btn_draw(p_btn[i]);
		gsmdev.mode=0;
		memset(pdis->inbuf,0,PHONE_MAX_INPUT);
		memset(pdis->phonebuf,0,PHONE_MAX_INPUT);
		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(5);							//��ʱһ��ʱ�ӽ���
			if(system_task_return)break;	  		//TPAD����
			if(USART3_RX_STA&0X8000)				//���յ�����
			{
				if(gsmdev.mode==1||gsmdev.mode==2)
				{
					if(gsmdev.mode==1)if(gsm_check_cmd("+COLP:"))
					{
						pdis->time=0;
						calltime=OSTime;//��¼�ոս���ͨ��ʱ��ʱ��
						gsmdev.mode=2;	//���ųɹ�
					}
					if(gsm_check_cmd("NO CARRIER"))gsmdev.mode=0;	//����ʧ��
					if(gsm_check_cmd("NO ANSWER"))gsmdev.mode=0;		//����ʧ��
					if(gsm_check_cmd("ERROR"))gsmdev.mode=0;			//����ʧ��
					if(gsmdev.mode==0)
					{
						phone_show_clear(pdis);			//���ԭ������ʾ
						p_btn[12]->bcfdcolor=0X76B2;;	//����ʱ�ı���ɫ
						p_btn[12]->bcfucolor=0X3DEA;	//�ɿ�ʱ����ɫ 
						p_btn[12]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
						p_btn[12]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
						btn_draw(p_btn[12]);
						pdis->inlen=0;
						pdis->inbuf[pdis->inlen]=0;		//������
						phone_show_phone(pdis,&gsmdev);//��ʾ�绰����
					}
					gsm_cmd_over();//�������;
				}
			}
			if(gsmdev.mode==2)//ͨ����
			{
				if((OSTime-calltime)>=OS_TICKS_PER_SEC)//����1������
				{
					calltime=OSTime;
					pdis->time++;
					phone_show_time(pdis);//��ʾʱ��
				}
			}
	 		for(i=0;i<14;i++)
			{
				res=btn_check(p_btn[i],&in_obj);   
				if(res&&((p_btn[i]->sta&(1<<7))==0)&&(p_btn[i]->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
				{
					if(i<12)//1~#
					{
						if(gsmdev.mode==0)//��������ģʽ
						{
							if(pdis->plen<PHONE_MAX_INPUT-1)
							{
								u3_printf("AT+CLDTMF=2,\"%c\"\r\n",call_btnstr_tbl[i][0]);
								pdis->phonebuf[pdis->plen]=call_btnstr_tbl[i][0];//����ַ�
								pdis->plen++;
								pdis->phonebuf[pdis->plen]=0;//��ӽ�����
							}
						
						}else if(gsmdev.mode==2)//����ͨ������Ч
						{
							if(pdis->inlen<PHONE_MAX_INPUT-1)
							{
								u3_printf("AT+CLDTMF=2,\"%c\"\r\n",call_btnstr_tbl[i][0]);
								delay_ms(150);
								u3_printf("AT+VTS=%c\r\n",call_btnstr_tbl[i][0]); 
								pdis->inbuf[pdis->inlen]=call_btnstr_tbl[i][0];	//����ַ�
								pdis->inlen++;
								pdis->inbuf[pdis->inlen]=0;//��ӽ�����
							}
						}
					}
					if(i==12&&pdis->plen)//�����к����ʱ����в���
					{
						if(gsmdev.mode==0)
						{
							if((gsmdev.status&0XC0)!=0XC0)//GSMģ��/SIM�� ����,��ʾ������Ϣ
							{
 								if((gsmdev.status&0X80)==0)window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)phone_remind_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);
								else window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)phone_remind_tbl[3][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);
 								break;
							} 
							gsmdev.mode=1;//���벦����ģʽ
							p_btn[12]->bcfdcolor=0XFBEF;;	//����ʱ�ı���ɫ
							p_btn[12]->bcfucolor=0XFA69;	//�ɿ�ʱ����ɫ 
							p_btn[12]->picbtnpathu=(u8*)PHONE_HANGUP_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_HANGUP_PIC[lcdtype];
							phone_show_clear(pdis);			//���ԭ������ʾ
							phone_show_phone(pdis,&gsmdev);//��ʾ�绰����
							phone_show_calling(pdis,&gsmdev);//������
							u3_printf("ATD%s;\r\n",pdis->phonebuf);	//����
						}else 
						{
							gsmdev.mode=0;//�ָ�����ģʽ
							p_btn[12]->bcfdcolor=0X76B2;;	//����ʱ�ı���ɫ
							p_btn[12]->bcfucolor=0X3DEA;	//�ɿ�ʱ����ɫ 
							p_btn[12]->picbtnpathu=(u8*)PHONE_CALL_PIC[lcdtype];
							p_btn[12]->picbtnpathd=(u8*)PHONE_CALL_PIC[lcdtype];
							phone_show_clear(pdis);			//���ԭ������ʾ
							pdis->inlen=0;
							pdis->inbuf[pdis->inlen]=0;		//������
							gsm_send_cmd("ATH","OK",200);//�һ�
							gsm_cmd_over();//�������
						}
						btn_draw(p_btn[12]);
					}
					if(i==13)//�˸�
					{
						if(gsmdev.mode==0)//��������ģʽ
						{
							if(pdis->plen)pdis->plen--; 
							pdis->phonebuf[pdis->plen]=0;	//������
						}else if(gsmdev.mode==2)
						{
							if(pdis->inlen)pdis->inlen--; 
							pdis->inbuf[pdis->inlen]=0;		//������
						}
					}
					if(gsmdev.mode==0)phone_show_phone(pdis,&gsmdev);//��ʾ�绰����
					if(gsmdev.mode==2)phone_show_input(pdis,&gsmdev);//��ʾ�������� 
				}
			} 
		}
	}
	if(gsmdev.mode)
	{
		gsm_send_cmd("ATH","OK",200);//�һ�
		gsm_cmd_over();//�������
	}
	gui_memin_free(f_phone);		//�ͷ��ڴ�
	gui_memin_free(pdis->inbuf);	//�ͷ��ڴ�
	gui_memin_free(pdis->phonebuf);	//�ͷ��ڴ�
	gui_memin_free(pdis);			//�ͷ��ڴ�
	if(lcddev.width<=272)
	{
		gui_memex_free(asc2_2814);
		asc2_2814=0;
	}else if(lcddev.width==320)
	{
		gui_memex_free(asc2_3618);
		asc2_3618=0;
	}else if(lcddev.width>=480)
	{
		gui_memex_free(asc2_5427);
		asc2_5427=0;
	} 	
	for(i=0;i<14;i++)btn_delete(p_btn[i]);//ɾ����ť
	return 0;
}








































