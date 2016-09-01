#include "videoplay.h"	 
#include "audioplay.h"
#include "settings.h"
#include "ucos_ii.h"
#include "wm8978.h"
#include "sai.h"
#include "wavplay.h"   
#include "mjpeg.h" 
#include "timer.h" 
#include "avi.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-��Ƶ������ ����	   
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
 
__videodev videodev;		//��Ƶ���ſ�����
__videoui *vui;				//��Ƶ���Ž��������


extern vu8 framecnt;		//ͳһ��֡������
extern vu8 framecntout;		//ͳһ��֡�������������
vu8 aviframeup;				//��Ƶ����ʱ϶���Ʊ���,������1��ʱ��,���Ը�����һ֡��Ƶ

//��Ƶ����SAI DMA����ص�����
void audio_sai_dma_callback(void) 
{      
	videodev.saiplaybuf++;
	if(videodev.saiplaybuf>3)videodev.saiplaybuf=0;
	if(DMA2_Stream3->CR&(1<<19))
	{	 
		DMA2_Stream3->M0AR=(u32)videodev.saibuf[videodev.saiplaybuf];//ָ����һ��buf
	}
	else 
	{   		
		DMA2_Stream3->M1AR=(u32)videodev.saibuf[videodev.saiplaybuf];//ָ����һ��buf
	} 
}  
/////////////////////////////////////////////////////////////////////////
//��ʾaudio����ʱ��
//sx,sy:��ʼ����
//sec:ʱ��
void video_time_show(u16 sx,u16 sy,u16 sec)
{
	u16 min;
	u8 *buf;
	buf=gui_memin_malloc(100);
	if(buf==0)return;
	min=sec/60;//�õ������� 
	sec=sec%60;//�õ������� 
	sprintf((char*)buf,"%03d:%02d",min,sec);
	gui_fill_rectangle(sx,sy,vui->msgfsize*3,vui->msgfsize,AUDIO_MAIN_BKCOLOR);	//����ɫ 
	gui_show_string(buf,sx,sy,vui->msgfsize*3,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);	
	gui_memin_free(buf);
} 



//video����������
void video_load_ui(void)
{	 
 	if(lcddev.width==240)
	{
		vui->tpbar_height=20;
		vui->capfsize=12;
		vui->msgfsize=12;		//���ܴ���16
		
		vui->msgbar_height=46;
		vui->nfsize=12;
		vui->xygap=4;
		vui->msgdis=4;			//����3��dis
		 
		vui->prgbar_height=30;
		vui->pbarwidth=140;		//������������  14*vui->msgfsize/2
		
		vui->btnbar_height=60; 
	}else if(lcddev.width==272)
	{
		vui->tpbar_height=24;
		vui->capfsize=12;
		vui->msgfsize=12;		//���ܴ���16
		
		vui->msgbar_height=50;
		vui->nfsize=12;
		vui->xygap=4;
		vui->msgdis=6;			//����3��dis
		
		vui->prgbar_height=30;
		vui->pbarwidth=170;		//������������  14*vui->msgfsize/2
		
		vui->btnbar_height=80; 
	}else if(lcddev.width==320)
	{
		vui->tpbar_height=24;
		vui->capfsize=12;
		vui->msgfsize=12;		//���ܴ���16 
		
		vui->msgbar_height=50;
		vui->nfsize=12;
		vui->xygap=4;
		vui->msgdis=16;			//����3��dis
		
		vui->prgbar_height=30;
		vui->pbarwidth=210;		//������������  14*vui->msgfsize/2
		
		vui->btnbar_height=80; 
	}else if(lcddev.width==480)
	{
		vui->tpbar_height=30;
		vui->capfsize=16;
		vui->msgfsize=12;		//���ܴ���16 
		
		vui->msgbar_height=60;
		vui->nfsize=12;
		vui->xygap=6;
		vui->msgdis=24;			//����3��dis
		
		vui->prgbar_height=40;
		vui->pbarwidth=340;		//������������  14*vui->msgfsize/2
		
		vui->btnbar_height=120; 
	}else if(lcddev.width==600)
	{
		vui->tpbar_height=40;
		vui->capfsize=24;
		vui->msgfsize=16;		//���ܴ���16 
		
		vui->msgbar_height=100;
		vui->nfsize=16;
		vui->xygap=10;
		vui->msgdis=40;			//����3��dis
		
		vui->prgbar_height=60;
		vui->pbarwidth=400;		//������������  12*vui->msgfsize/2
		
		vui->btnbar_height=150; 
	} 
	vui->vbarheight=vui->msgfsize;//����vui->msgfsize�Ĵ�С
	vui->pbarheight=vui->msgfsize;//����vui->msgfsize�Ĵ�С
	vui->vbarwidth=lcddev.width-16-2*vui->xygap-3*vui->msgdis-14*vui->msgfsize/2;
	vui->vbarx=vui->msgdis+16+vui->xygap;
	vui->vbary=vui->tpbar_height+vui->xygap*2+vui->msgfsize+(vui->msgbar_height-(vui->msgfsize+vui->xygap*2+vui->xygap/2+vui->msgfsize+vui->vbarheight))/2;
	vui->pbarx=(lcddev.width-vui->pbarwidth-12*vui->msgfsize/2)/2+vui->msgfsize*6/2;
	vui->pbary=lcddev.height-vui->btnbar_height-vui->prgbar_height+(vui->prgbar_height-vui->pbarheight)/2;
	

	 
	gui_fill_rectangle(0,0,lcddev.width,vui->tpbar_height,AUDIO_TITLE_BKCOLOR);	//����������ɫ 
	gui_show_strmid(0,0,lcddev.width,vui->tpbar_height,AUDIO_TITLE_COLOR,vui->capfsize,(u8*)APP_MFUNS_CAPTION_TBL[3][gui_phy.language]);	//��ʾ����
	gui_fill_rectangle(0,vui->tpbar_height,lcddev.width,vui->msgbar_height,AUDIO_MAIN_BKCOLOR);									//�����Ϣ������ɫ   
 	minibmp_decode((u8*)APP_VOL_PIC,vui->msgdis,vui->vbary-(16-vui->msgfsize)/2,16,16,0,0);				//��������ͼ��
 	gui_show_string("00%",vui->vbarx,vui->vbary+vui->vbarheight+vui->xygap/2,3*vui->msgfsize/2,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);//��ʾ����
  	gui_fill_rectangle(0,lcddev.height-vui->btnbar_height-vui->prgbar_height,lcddev.width,vui->prgbar_height,AUDIO_MAIN_BKCOLOR);	//��������������ɫ
	gui_fill_rectangle(0,lcddev.height-vui->btnbar_height,lcddev.width,vui->btnbar_height,AUDIO_BTN_BKCOLOR);						//��䰴ť������ɫ   
	gui_fill_rectangle(0,vui->tpbar_height+vui->msgbar_height,lcddev.width,lcddev.height-vui->tpbar_height-vui->msgbar_height-vui->prgbar_height-vui->btnbar_height,AUDIO_MAIN_BKCOLOR);//����ɫ
}
//��ʾ�����ٷֱ�  
//pctx:�ٷֱ�ֵ
void video_show_vol(u8 pctx)
{
	u8 *buf; 
	u8 sy=vui->vbary+vui->vbarheight+vui->xygap/2;
	gui_phy.back_color=AUDIO_MAIN_BKCOLOR;//���ñ���ɫΪ��ɫ
 	gui_fill_rectangle(vui->vbarx,sy,4*vui->msgfsize/2,vui->msgfsize,AUDIO_MAIN_BKCOLOR);//��䱳��ɫ 
	buf=gui_memin_malloc(32);
	sprintf((char*)buf,"%d%%",pctx);
 	gui_show_string(buf,vui->vbarx,sy,4*vui->msgfsize/2,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);//��ʾ����
	gui_memin_free(buf);	 
} 
//videodevx:audio������
//videoprgbx:������
//aviinfo:��Ƶ��Ϣ 
void video_info_upd(__videodev *videodevx,_progressbar_obj* videoprgbx,_progressbar_obj* volprgbx,AVI_INFO *aviinfo,u8 flag)
{
	static u32 oldsec; 
	u16 tempx,tempy;
	u8 *buf;
	float ftemp;
	u32 totsec=0;		//video�ļ���ʱ�� 
	u32 cursec; 		//��ǰ����ʱ�� 
	if(flag==1)//��Ҫ�������ֵ���Ϣ
	{ 
		buf=gui_memin_malloc(100);	//����100�ֽ��ڴ�
		if(buf==NULL)return;		//game over  
		gui_fill_rectangle(0,vui->tpbar_height+vui->xygap-1,lcddev.width,vui->msgfsize+2,AUDIO_MAIN_BKCOLOR);//���¸������һ��,���֮ǰ����ʾ 
		gui_show_ptstrwhiterim(vui->xygap,vui->tpbar_height+vui->xygap,lcddev.width-vui->xygap,lcddev.height,0,0X0000,0XFFFF,vui->msgfsize,videodevx->name);	//��ʾ�µ�����		
 		gui_phy.back_color=AUDIO_MAIN_BKCOLOR;//���ñ���ɫΪ��ɫ 
		//��ʾ�����ٷֱ�
		video_show_vol((volprgbx->curpos*100)/volprgbx->totallen);//��ʾ�����ٷֱ� 
		//��ʾ��Ŀ���
		sprintf((char*)buf,"%03d/%03d",videodevx->curindex+1,videodevx->mfilenum);
		tempx=vui->vbarx+vui->vbarwidth-7*(vui->msgfsize)/2;
		tempy=vui->vbary+vui->xygap/2+vui->vbarheight;
		gui_fill_rectangle(tempx,tempy,7*(vui->msgfsize)/2,vui->msgfsize,AUDIO_MAIN_BKCOLOR);		//���֮ǰ����ʾ  
		gui_show_string(buf,tempx,tempy,7*(vui->msgfsize)/2,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);	
  		//��ʾxxxKhz	
		tempx=vui->vbarx+vui->vbarwidth+vui->msgdis;  
		gui_fill_rectangle(tempx,vui->vbary,9*vui->msgfsize/2,vui->msgfsize,AUDIO_MAIN_BKCOLOR);		//���֮ǰ����ʾ  
		ftemp=(float)aviinfo->SampleRate/1000;//xxx.xKhz 
		sprintf((char*)buf,"%2.3fKhz",ftemp);
		gui_show_string(buf,tempx,vui->vbary,9*vui->msgfsize/2,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);	 	 
		//��ʾ�ֱ���  
		tempy=vui->vbary+vui->xygap/2+vui->vbarheight;  
		gui_fill_rectangle(tempx,tempy,11*(vui->msgfsize)/2,vui->msgfsize,AUDIO_MAIN_BKCOLOR);	//���֮ǰ����ʾ   
		sprintf((char*)buf,"Pix:%dX%d",aviinfo->Width,aviinfo->Height);  
		gui_show_string(buf,tempx,tempy,11*(vui->msgfsize)/2,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);	//��ʾ�ֱ���
 		//��������
 	 	oldsec=0;
 		videoprgbx->totallen=videodevx->file->obj.objsize;	//�����ܳ���	
		videoprgbx->curpos=0;	  
		gui_memin_free(buf);//�ͷ��ڴ�
	}  
	totsec=(aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;	//�����ܳ���(��λ:ms) 
	totsec/=1000;		//������. 
  	cursec=((double)videodevx->file->fptr/videodevx->file->obj.objsize)*totsec;//��ǰ���ŵ���������?   
	videoprgbx->curpos=f_tell(videodevx->file);	//�õ���ǰ�Ĳ���λ��
	progressbar_draw_progressbar(videoprgbx);	//���½�����λ�� 
	if(oldsec!=cursec)
	{
		oldsec=cursec;  
		//��ʾ֡�� 
		buf=gui_memin_malloc(100);	//����100�ֽ��ڴ�
		if(buf==NULL)return;
		tempx=vui->vbarx+vui->vbarwidth+vui->msgdis+9*(vui->msgfsize)/2+vui->xygap;   
		gui_fill_rectangle(tempx,vui->vbary,5*(vui->msgfsize)/2,vui->msgfsize,AUDIO_MAIN_BKCOLOR);		//���֮ǰ����ʾ
		sprintf((char*)buf,"%02dfps",framecntout); 
		gui_show_string(buf,tempx,vui->vbary,5*(vui->msgfsize)/2,vui->msgfsize,vui->msgfsize,AUDIO_INFO_COLOR);	   	
		gui_memin_free(buf);//�ͷ��ڴ�
		//��ʾʱ��		
		tempx=vui->pbarx-7*vui->msgfsize/2;
		video_time_show(tempx,vui->pbary,cursec);			//��ʾ����ʱ��  
		tempx=vui->pbarx+vui->pbarwidth+vui->msgfsize/2;
		video_time_show(tempx,vui->pbary,totsec);			//��ʾ��ʱ��  
	}
}
//��Ƶ������ʾ��
u8*const video_msg_tbl[3][GUI_LANGUAGE_NUM]=
{
{"��ȡ�ļ�����!!","�xȡ�ļ��e�`!!","File Read Error!",},
{"�ڴ治��!!","�ȴ治��!!","Out of memory!",},	 	
{"�ֱ��ʲ�֧��","�ֱ��ʲ�֧��","Image Size Error!",},
}; 
//��Ƶ����
u8 video_play(void)
{				 	 
	u8 rval=0;			//����ֵ	  
	u8 *pname=0; 
	u16 i;
	u8 key; 
	
 	_btn_obj* rbtn;		//���ذ�ť�ؼ�
   	_filelistbox_obj * flistbox;
	_filelistbox_list * filelistx; 	//�ļ�  
	memset(&videodev,0,sizeof(__videodev));//videodev�ṹ������.
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[3][gui_phy.language],0X07);//ѡ��Ŀ���ļ�,���õ�Ŀ������

   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//����һ��filelistbox
 	if(flistbox==NULL)rval=1;							//�����ڴ�ʧ��.
	else  
	{
		flistbox->fliter=FLBOX_FLT_VIDEO;		//ͼƬ�ļ�
		filelistbox_add_disk(flistbox);			//��Ӵ���·��
 		filelistbox_draw_listbox(flistbox);
	} 	   
	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
 	if(rbtn==NULL)rval=1;	//û���㹻�ڴ湻����
	else
	{
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];	//����
	 	rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
		rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(rbtn);//����ť
	} 
	while(rval==0)//��ѭ��
	{
		tp_dev.scan(0);    							//ɨ�败����
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);		//�õ�������ֵ   
		delay_ms(5);
		if(system_task_return)break;				//TPAD����   	 
		filelistbox_check(flistbox,&in_obj);		//ɨ���ļ�
		
		if(flistbox->dbclick==0X81)					//˫���ļ���
		{	
			gui_memin_free(videodev.path);			//�ͷ��ڴ�
			gui_memex_free(videodev.mfindextbl);	//�ͷ��ڴ�
			videodev.path=(u8*)gui_memin_malloc(strlen((const char*)flistbox->path)+1);//Ϊ�µ�·�������ڴ�
			if(videodev.path==NULL){rval=1;break;}	//�ڴ����ʧ��
			videodev.path[0]='\0';					//���ʼ���������.
			strcpy((char *)videodev.path,(char *)flistbox->path);
			videodev.mfindextbl=(u32*)gui_memex_malloc(flistbox->filecnt*4);//Ϊ�µ�tbl�����ڴ�
			if(videodev.mfindextbl==NULL){rval=1;break;}//�ڴ����ʧ��
			for(i=0;i<flistbox->filecnt;i++)videodev.mfindextbl[i]=flistbox->findextbl[i];//����
			videodev.mfilenum=flistbox->filecnt;		//��¼�ļ�����	 
			videodev.curindex=flistbox->selindex-flistbox->foldercnt;//��ǰӦ�ò��ŵ��ļ����� 
			printf("play:%d/%d\r\n",videodev.curindex,videodev.mfilenum);
			rval=video_play_mjpeg();
			if(rval==0XFF)break;
			else
			{
				if(rval)//���ڴ���
				{ 
					if(rval<20)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)video_msg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);	//�ļ���ȡ����
					if(rval==0X21)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)video_msg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//�ڴ����
					if(rval==0X22)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)video_msg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//�ֱ��ʴ���			
					delay_ms(2000);
					rval=0;
				}
				flistbox->dbclick=0;	//���÷��ļ����״̬
				app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[3][gui_phy.language],0X07);//ѡ��Ŀ���ļ�,���õ�Ŀ������
				btn_draw(rbtn);			//����ť
				flistbox->selindex=flistbox->foldercnt+videodev.curindex;//ѡ����ĿΪ��ǰ���ڲ��ŵ���Ŀ			
				filelistbox_rebuild_filelist(flistbox);//�ؽ�flistbox
 				system_task_return=0;	//�������˳���Ƶ����			
			} 
		} 
		key=btn_check(rbtn,&in_obj);
  		if(key&&((rbtn->sta&0X80)==0))
		{		  
			if(flistbox->dbclick!=0X81)//���ļ������ʱ���˷��ذ�ť,�򷵻���һ��Ŀ¼
			{
				filelistx=filelist_search(flistbox->list,flistbox->selindex);//�õ���ʱѡ�е�list����Ϣ
				if(filelistx->type==FICO_DISK)//�Ѿ�������������,���˳���Ƶ���
				{				 
					break;
				}else filelistbox_back(flistbox);//�˻���һ��Ŀ¼	 
			} 
		}  
	}	
	filelistbox_delete(flistbox);		//ɾ��filelist
	btn_delete(rbtn);					//ɾ����ť	 	  
 	gui_memin_free(pname);				//�ͷ��ڴ�	
	gui_memin_free(videodev.path);		//�ͷ��ڴ�
	gui_memex_free(videodev.mfindextbl);//�ͷ��ڴ� 
 	return rval; 
} 

 
//����mjpeg�ļ�
//����ֵ:
//0,�������Ž���,�����ļ����״̬.
//0XFF,���˳���ť��,ֱ�ӽ�����Ƶ����
//����,�������
u8 video_play_mjpeg(void)
{    
	u8 *pname=0;		//��·��������
	u8* framebuf=0;		//��Ƶ����buf	 
	u8* pbuf=0;			//bufָ��   
	DIR videodir;		//videoĿ¼	  
 	FILINFO *videoinfo;	//video�ļ���Ϣ	
	_progressbar_obj* videoprgb=0,*volprgb=0;
	_btn_obj* tbtn[5];	
	u16 lastvolpos; 
	u8 btnsize=0;		//��ť�ߴ�
	u8 btnxpit=0;		//��ť��x�����ϵļ�϶
	u8 i;
	u8 tcnt=0;

	u8  res=0;
	u16 offset=0; 
	u32	nr;  
    u8 saisavebuf; 
	u16 videoheight;	//��Ƶ��ʾ����߶�
	u16 yoff;
	u8 key=0;	
	
	for(i=0;i<5;i++)tbtn[i]=0;
	if(audiodev.status&(1<<7))//��ǰ�ڷŸ�??
	{
		audio_stop_req(&audiodev);	//ֹͣ��Ƶ����
		audio_task_delete();		//ɾ�����ֲ�������.
	}
	app_wm8978_volset(wm8978set.mvol);	 
	WM8978_ADDA_Cfg(1,0);	//����DAC
	WM8978_Input_Cfg(0,0,0);//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);	//����DAC��� 
	videoinfo=(FILINFO*)gui_memin_malloc(sizeof(FILINFO));		//����FILENFO�ڴ�
	framebuf=gui_memin_malloc(AVI_VIDEO_BUF_SIZE);				//������Ƶbuf
	videodev.saibuf[0]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ�
	videodev.saibuf[1]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ�
	videodev.saibuf[2]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ�
	videodev.saibuf[3]=gui_memin_malloc(AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ� 
	videodev.file=(FIL*)gui_memin_malloc(sizeof(FIL)); 			//����videodev.file�ڴ� 
	vui=(__videoui*)gui_memin_malloc(sizeof(__videoui));
	if(!videodev.saibuf[3]||!framebuf||!videodev.file)
	{ 
		res=0X21;
	}else 
	{	
		memset(videodev.saibuf[0],0,AVI_AUDIO_BUF_SIZE);
		memset(videodev.saibuf[1],0,AVI_AUDIO_BUF_SIZE); 
		memset(videodev.saibuf[2],0,AVI_AUDIO_BUF_SIZE);
		memset(videodev.saibuf[3],0,AVI_AUDIO_BUF_SIZE);  
		video_load_ui();						//����������
		videoprgb=progressbar_creat(vui->pbarx,vui->pbary,vui->pbarwidth,vui->pbarheight,0X20);	//audio���Ž�����
		if(videoprgb==NULL)res=0X21;
		volprgb=progressbar_creat(vui->vbarx,vui->vbary,vui->vbarwidth,vui->vbarheight,0X20);	//������С������
		if(volprgb==NULL)res=0X21;	   
		volprgb->totallen=63;
		videoheight=lcddev.height-(vui->tpbar_height+vui->msgbar_height+vui->prgbar_height+vui->btnbar_height);
		yoff=vui->tpbar_height+vui->msgbar_height; 
		if(wm8978set.mvol<=63)volprgb->curpos=wm8978set.mvol;
		else//��������� 
		{
			wm8978set.mvol=0;
			volprgb->curpos=0;
		}	  
		lastvolpos=volprgb->curpos;//�趨�����λ��
		switch(lcddev.width)
		{
			case 240:
				btnsize=48;	
				break;
			case 272:
				btnsize=50;	
				break;
			case 320:
				btnsize=60;	
				break;
			case 480:
				btnsize=80;	
				break;
			case 600:
				btnsize=100;	
				break;
			
		}
		btnxpit=(lcddev.width-5*btnsize)/5;
		for(i=0;i<5;i++)//ѭ������5����ť
		{
			tbtn[i]=btn_creat(btnxpit/2+i*(btnsize+btnxpit),lcddev.height-btnsize-(vui->btnbar_height-btnsize)/2,btnsize,btnsize,0,1);//����ͼƬ��ť
			if(tbtn[i]==NULL){res=0X21;break;}		//����ʧ��.
			tbtn[i]->bcfdcolor=0X2CFF;				//����ʱ�ı���ɫ
			tbtn[i]->bcfucolor=AUDIO_BTN_BKCOLOR;	//�ɿ�ʱ����ɫ 
			tbtn[i]->picbtnpathu=(u8*)AUDIO_BTN_PIC_TBL[0][i];
			tbtn[i]->picbtnpathd=(u8*)AUDIO_BTN_PIC_TBL[1][i];
			tbtn[i]->sta=0;	 
		} 
		if(res==0)
		{
			res=f_opendir(&videodir,(const TCHAR*)videodev.path);//��ѡ�е�Ŀ¼ 
		} 
	}  		
	if(res==0)
	{ 
		videoprgb->inbkcolora=0x738E;			//Ĭ��ɫ
		videoprgb->inbkcolorb=AUDIO_INFO_COLOR;	//Ĭ��ɫ 
		videoprgb->infcolora=0X75D;				//Ĭ��ɫ
		videoprgb->infcolorb=0X596;				//Ĭ��ɫ  
		volprgb->inbkcolora=AUDIO_INFO_COLOR;	//Ĭ��ɫ
		volprgb->inbkcolorb=AUDIO_INFO_COLOR;	//Ĭ��ɫ 
		volprgb->infcolora=0X75D;				//Ĭ��ɫ
		volprgb->infcolorb=0X596;				//Ĭ��ɫ   
		for(i=0;i<5;i++)btn_draw(tbtn[i]);		//����ť
		tbtn[2]->picbtnpathu=(u8*)AUDIO_PLAYR_PIC;//����һ��֮���Ϊ�����ɿ�״̬
		progressbar_draw_progressbar(videoprgb);//��������	 
		progressbar_draw_progressbar(volprgb);	//��������
	}
	while(res==0)
	{ 
		ff_enter(videodir.obj.fs);//����fatfs,��ֹ�����.
		dir_sdi(&videodir,videodev.mfindextbl[videodev.curindex]);
		ff_leave(videodir.obj.fs);//�˳�fatfs,��������os��
		res=f_readdir(&videodir,videoinfo);//��ȡ�ļ���Ϣ
		if(res)break;//��ʧ��   
		videodev.name=(u8*)(videoinfo->fname);
		pname=gui_memin_malloc(strlen((const char*)videodev.name)+strlen((const char*)videodev.path)+2);//�����ڴ�
		if(pname==NULL){res=0X21;break;}	//����ʧ��	    
		pname=gui_path_name(pname,videodev.path,videodev.name);	//�ļ�������·��    
		res=f_open(videodev.file,(char *)pname,FA_READ);
		gui_memin_free(pname);			//�ͷ��ڴ� 
		if(res==0)
		{
			pbuf=framebuf;			
			res=f_read(videodev.file,pbuf,AVI_VIDEO_BUF_SIZE,&nr);//��ʼ��ȡ	
			if(res)//�ļ�ϵͳ����,ֱ���˳����ٲ���
			{ 
				break;
			} 	 
			//��ʼavi����
			res=avi_init(pbuf,AVI_VIDEO_BUF_SIZE);	//avi����
			if(res)//һ������,�����Լ�������
			{
				printf("avi err:%d\r\n",res);
				break;
			} 	 
			if(avix.Height>videoheight||avix.Width>lcddev.width)//ͼƬ�ߴ�������,ֱ���˳����ٲ���
			{
				res=0X22;
				printf("avi size error\r\n");
				break;
			}
			TIM6_Int_Init(avix.SecPerFrame/100-1,9600-1);	//10Khz����Ƶ��,��1��100us,������Ƶ֡�������
 			TIM8_Int_Init(10000-1,19200-1);					//10Khz����,1�����ж�һ��,����֡����	
			offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi");//Ѱ��movi ID	 
			avi_get_streaminfo(pbuf+offset+4);			//��ȡ����Ϣ 
			f_lseek(videodev.file,offset+12);			//������־ID,����ַƫ�Ƶ������ݿ�ʼ��	 
			res=mjpegdec_init((lcddev.width-avix.Width)/2,yoff+(videoheight-avix.Height)/2);//JPG�����ʼ�� 
			if(avix.SampleRate)							//����Ƶ��Ϣ,�ų�ʼ��
			{
				WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
				SAIA_Init(0,1,4);		//����SAI,������,16λ����
				SAIA_SampleRate_Set(avix.SampleRate);//���ò����� 
 				SAIA_TX_DMA_Init(videodev.saibuf[1],videodev.saibuf[2],avix.AudioBufSize/2,1); //����DMA
				sai_tx_callback=audio_sai_dma_callback;	//�ص�����ָ��sai_DMA_Callback
				videodev.saiplaybuf=0;
				saisavebuf=0; 
				SAI_Play_Start(); //����sai���� 
			}
			gui_fill_rectangle(0,yoff,lcddev.width,videoheight,BLACK);	//�����Ƶ����
			video_info_upd(&videodev,videoprgb,volprgb,&avix,1);		//����������Ϣ		
			tcnt=0;
			key=0;
			videodev.status=3;//����ͣ,�ǿ������
 			while(key==0)//����ѭ��
			{		
				if(videodev.status&(1<<0)&&videodev.status&(1<<1))
				{
					if(avix.StreamID==AVI_VIDS_FLAG)//��Ƶ��
					{
						pbuf=framebuf;
						f_read(videodev.file,pbuf,avix.StreamSize+8,&nr);//������֡+��һ������ID��Ϣ  
						res=mjpegdec_decode(pbuf,avix.StreamSize);
						if(res)
						{
							printf("decode error!\r\n");
						} 
						while(aviframeup==0)delay_ms(1000/OS_TICKS_PER_SEC);//�ȴ�ʱ�䵽��(��TIM6���ж���������Ϊ1)
						aviframeup=0;	//��־���� 
						framecnt++;
					}else 	//��Ƶ��
					{		  
						//video_time_show(videodev.file,&avix); 	//��ʾ��ǰ����ʱ��
						saisavebuf++;
						if(saisavebuf>3)saisavebuf=0;
						do
						{
							nr=videodev.saiplaybuf;
							if(nr)nr--;
							else nr=3; 
						}while(saisavebuf==nr);//��ײ�ȴ�. 
						f_read(videodev.file,videodev.saibuf[saisavebuf],avix.StreamSize+8,&nr);//���videodev.saibuf	 
						pbuf=videodev.saibuf[saisavebuf];  
					}  
				}else	//��ͣ״̬
				{
					delay_ms(10000/OS_TICKS_PER_SEC);
				}
				
				tp_dev.scan(0);    
				in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
				for(i=0;i<5;i++)
				{
					res=btn_check(tbtn[i],&in_obj);	 
					if((res&&((tbtn[i]->sta&(1<<7))==0)&&(tbtn[i]->sta&(1<<6)))||system_task_return)//�а����������ɿ�,����TP�ɿ��˻���TPAD����
					{ 
						if(system_task_return){key=0XFF;break;};//TPAD���أ�����Ŀ¼
						switch(i)
						{
							case 0://file list
								key=0xff; 
								break;
							case 1://��һ����Ƶ 
								if(systemset.videomode!=1)//���������
								{
									if(videodev.curindex)videodev.curindex--;
									else videodev.curindex=videodev.mfilenum-1;
								}else	//�������
								{
									videodev.curindex=app_get_rand(videodev.mfilenum);//�õ���һ�׸���������
								}
								key=1;
								break;
							case 3://��һ����Ƶ 
								if(systemset.videomode!=1)//���������
								{
									if(videodev.curindex<(videodev.mfilenum-1))videodev.curindex++;
									else videodev.curindex=0;	
								}else	//�������
								{
									videodev.curindex=app_get_rand(videodev.mfilenum);//�õ���һ�׸���������
								}						
								key=3;
								break;
							case 2://����/��ͣ
								if(videodev.status&(1<<0))//����ͣ
								{ 
									videodev.status&=~(1<<0);	//�����ͣ 
									SAI_Play_Stop();			//�ر���Ƶ
									tbtn[2]->picbtnpathd=(u8*)AUDIO_PLAYP_PIC; 
									tbtn[2]->picbtnpathu=(u8*)AUDIO_PAUSER_PIC;
								}else//��ͣ״̬
								{
									videodev.status|=1<<0;		//ȡ����ͣ
									SAI_Play_Start();			//����DMA���� 
									tbtn[2]->picbtnpathd=(u8*)AUDIO_PAUSEP_PIC; 
									tbtn[2]->picbtnpathu=(u8*)AUDIO_PLAYR_PIC;
								}
								break;
							case 4://ֹͣ����,ֱ���˳��������� 
								key=4;  
								break; 
						}
					}  
				}				
				res=progressbar_check(volprgb,&in_obj);//�������������
				if(res&&lastvolpos!=volprgb->curpos)//��������,��λ�ñ仯��.ִ����������
				{		  
					lastvolpos=volprgb->curpos;
					if(volprgb->curpos)wm8978set.mvol=volprgb->curpos;//��������
					else wm8978set.mvol=0;	  
					app_wm8978_volset(wm8978set.mvol);	 
					video_show_vol((volprgb->curpos*100)/volprgb->totallen);	//��ʾ�����ٷֱ�   
				}	  
				res=progressbar_check(videoprgb,&in_obj);
				if(res)//��������,�����ɿ���,ִ�п������
				{
					videodev.status&=~(1<<1);//��־����������״̬
					if(videodev.status&1<<0)SAI_Play_Stop();//����ͣʱ,��ʼ�������,�ر���Ƶ
 					video_seek(&videodev,&avix,framebuf,videoprgb->curpos);
					pbuf=framebuf;
					video_info_upd(&videodev,videoprgb,volprgb,&avix,0);//������ʾ��Ϣ,ÿ100msִ��һ��	
				}else if((videodev.status&(1<<1))==0)//���ڿ��/����״̬?
				{
					if(videodev.status&1<<0)SAI_Play_Start();//����ͣ״̬,������˽���,����Ƶ
					videodev.status|=1<<1;	//ȡ���������״̬
				}
				if(videodev.status&(1<<0)&&videodev.status&(1<<1))//����ͣ״̬�Ϳ������״̬
				{
					if(avi_get_streaminfo(pbuf+avix.StreamSize))//��ȡ��һ֡ ����־,�������,�����ǵ�Ӱ������
					{
						printf("frame error \r\n"); 
						break; 
					}
				}
				tcnt++;
				if((tcnt%20)==0)video_info_upd(&videodev,videoprgb,volprgb,&avix,0);//������ʾ��Ϣ,ÿ100msִ��һ��			
			}
			SAI_Play_Stop();	//�ر���Ƶ
			TIM6->CR1&=~(1<<0); //�رն�ʱ��6 
			TIM8->CR1&=~(1<<0); //�رն�ʱ��3  
			mjpegdec_free();	//�ͷ��ڴ�
			f_close(videodev.file); 
			if(key==0)
			{
				if(systemset.videomode==0)//˳�򲥷�
				{
					if(videodev.curindex<(videodev.mfilenum-1))videodev.curindex++;
					else videodev.curindex=0;
				}else if(systemset.videomode==1)//�������
				{						    
					videodev.curindex=app_get_rand(videodev.mfilenum);//�õ���һ�׸���������	  
				}else videodev.curindex=videodev.curindex;//����ѭ��		 
			}else if(key==0XFF){res=0;break;}
			else if(key==4){res=0XFF;break;}
		}
	}  
	gui_memin_free(videodev.saibuf[0]);
	gui_memin_free(videodev.saibuf[1]);
	gui_memin_free(videodev.saibuf[2]);
	gui_memin_free(videodev.saibuf[3]);
	gui_memin_free(framebuf);
	gui_memin_free(videoinfo);
	gui_memin_free(videodev.file);
 	gui_memin_free(vui);
	for(i=0;i<5;i++)if(tbtn[i])btn_delete(tbtn[i]);//ɾ����ť	
	if(videoprgb)progressbar_delete(videoprgb);
	if(volprgb)progressbar_delete(volprgb);
	WM8978_ADDA_Cfg(0,0);				//�ر�DAC&ADC
	WM8978_Input_Cfg(0,0,0);			//�ر�����ͨ��
	WM8978_Output_Cfg(0,0);				//�ر�DAC���   	
	app_wm8978_volset(0);				//�ر�WM8978�������
	return res;
}
//avi�ļ�����
u8 video_seek(__videodev *videodevx,AVI_INFO *aviinfo,u8 *mbuf,u32 dstpos)
{
	u32 fpos=dstpos;
	u8 *pbuf;
	u16 offset;
	u32 br;  
	
	f_lseek(videodevx->file,fpos);///ƫ�Ƶ�Ŀ��λ��
	f_read(videodevx->file,mbuf,AVI_VIDEO_BUF_SIZE,&br);	//������֡+��һ������ID��Ϣ 
	pbuf=mbuf; 
	if(fpos==0) //��0��ʼ,����Ѱ��movi ID
	{
		offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi"); 
	}else offset=0;
	offset+=avi_srarch_id(pbuf+offset,AVI_VIDEO_BUF_SIZE,aviinfo->VideoFLAG);	//Ѱ����Ƶ֡	
	avi_get_streaminfo(pbuf+offset);			//��ȡ����Ϣ 
	f_lseek(videodevx->file,fpos+offset+8);		//������־ID,����ַƫ�Ƶ������ݿ�ʼ��	 
	if(aviinfo->StreamID==AVI_VIDS_FLAG)
	{
		f_read(videodevx->file,mbuf,aviinfo->StreamSize+8,&br);	//������֡ 
		mjpegdec_decode(mbuf,aviinfo->StreamSize); 		//��ʾ��Ƶ֡
	} 
	return 0;
}






















