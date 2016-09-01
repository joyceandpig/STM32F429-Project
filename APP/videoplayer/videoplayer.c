#include "videoplayer.h" 
#include "string.h"  
#include "key.h" 
#include "usart.h"   
#include "delay.h"
#include "timer.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "touch.h" 
#include "malloc.h"
#include "sai.h" 
#include "wm8978.h" 
#include "mjpeg.h" 
#include "avi.h"
#include "exfuns.h"
#include "text.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//��Ƶ������ Ӧ�ô���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
 
    
extern u16 frame;
extern vu8 frameup;//��Ƶ����ʱ϶���Ʊ���,������1��ʱ��,���Ը�����һ֡��Ƶ


volatile u8 i2splaybuf;	//�������ŵ���Ƶ֡������
u8* i2sbuf[4]; 			//��Ƶ����֡,��4֡,4*5K=20K
  
//��Ƶ����I2S DMA����ص�����
void audio_i2s_dma_callback(void) 
{      
	i2splaybuf++;
	if(i2splaybuf>3)i2splaybuf=0;
	if(DMA2_Stream3->CR&(1<<19))
	{	 
		DMA2_Stream3->M0AR=(u32)i2sbuf[i2splaybuf];//ָ����һ��buf
	}
	else 
	{   		
		DMA2_Stream3->M1AR=(u32)i2sbuf[i2splaybuf];//ָ����һ��buf
	} 
} 
//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 video_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ		
	u8 *fn; 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X60)//ȡ����λ,�����ǲ�����Ƶ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
} 
//��ʾ��ǰ����ʱ��
//favi:��ǰ���ŵ���Ƶ�ļ�
//aviinfo;avi���ƽṹ��
void video_time_show(FIL *favi,AVI_INFO *aviinfo)
{	 
	static u32 oldsec;	//��һ�εĲ���ʱ��
	u8* buf;
	u32 totsec=0;		//video�ļ���ʱ�� 
	u32 cursec; 		//��ǰ����ʱ�� 
	totsec=(aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;	//�����ܳ���(��λ:ms) 
	totsec/=1000;		//������. 
  	cursec=((double)favi->fptr/favi->fsize)*totsec;//��ǰ���ŵ���������?  
	if(oldsec!=cursec)	//��Ҫ������ʾʱ��
	{
		buf=mymalloc(SRAMIN,100);//����100�ֽ��ڴ�
		oldsec=cursec; 
		POINT_COLOR=BLUE; 
		sprintf((char*)buf,"����ʱ��:%02d:%02d:%02d/%02d:%02d:%02d",cursec/3600,(cursec%3600)/60,cursec%60,totsec/3600,(totsec%3600)/60,totsec%60);
 		Show_Str(10,90,lcddev.width-10,16,buf,16,0);	//��ʾ��������
		myfree(SRAMIN,buf);		
	} 		 
}
//��ʾ��ǰ��Ƶ�ļ��������Ϣ 
//aviinfo;avi���ƽṹ��
void video_info_show(AVI_INFO *aviinfo)
{	  
	u8 *buf;
	buf=mymalloc(SRAMIN,100);//����100�ֽ��ڴ� 
	POINT_COLOR=RED; 
	sprintf((char*)buf,"������:%d,������:%d",aviinfo->Channels,aviinfo->SampleRate*10); 
 	Show_Str(10,50,lcddev.width-10,16,buf,16,0);	//��ʾ��������
	sprintf((char*)buf,"֡��:%d֡",1000/(aviinfo->SecPerFrame/1000)); 
 	Show_Str(10,70,lcddev.width-10,16,buf,16,0);	//��ʾ��������
	myfree(SRAMIN,buf);	  
}
//��Ƶ������Ϣ��ʾ
//name:��Ƶ����
//index:��ǰ����
//total:���ļ���
void video_bmsg_show(u8* name,u16 index,u16 total)
{		
	u8* buf;
	buf=mymalloc(SRAMIN,100);//����100�ֽ��ڴ�
	POINT_COLOR=RED;
	sprintf((char*)buf,"�ļ���:%s",name);
	Show_Str(10,10,lcddev.width-10,16,buf,16,0);//��ʾ�ļ���
	sprintf((char*)buf,"����:%d/%d",index,total);	
	Show_Str(10,30,lcddev.width-10,16,buf,16,0);//��ʾ���� 		  	  
	myfree(SRAMIN,buf);		
}

//������Ƶ
void video_play(void)
{
	u8 res;
 	DIR vdir;	 		//Ŀ¼
	FILINFO vfileinfo;	//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totavinum; 		//��Ƶ�ļ�����
	u16 curindex;		//��Ƶ�ļ���ǰ����
	u8 key;				//��ֵ		  
 	u16 temp;
	u16 *vindextbl;		//��Ƶ�ļ������� 
	
 	while(f_opendir(&vdir,"0:/VIDEO"))//����Ƶ�ļ���
 	{	    
		Show_Str(60,190,240,16,"VIDEO�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 									  
	totavinum=video_get_tnum("0:/VIDEO"); //�õ�����Ч�ļ���
  	while(totavinum==NULL)//��Ƶ�ļ�����Ϊ0		
 	{	    
		Show_Str(60,190,240,16,"û����Ƶ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}										   
  	vfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	vfileinfo.lfname=mymalloc(SRAMIN,vfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,vfileinfo.lfsize);			//Ϊ��·�����ļ��������ڴ�
 	vindextbl=mymalloc(SRAMIN,2*totavinum);				//����2*totavinum���ֽڵ��ڴ�,���ڴ����Ƶ�ļ�����
 	while(vfileinfo.lfname==NULL||pname==NULL||vindextbl==NULL)//�ڴ�������
 	{	    
		Show_Str(60,190,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,146,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
 	//��¼����
    res=f_opendir(&vdir,"0:/VIDEO"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=vdir.index;							//��¼��ǰindex
	        res=f_readdir(&vdir,&vfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||vfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*vfileinfo.lfname?vfileinfo.lfname:vfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X60)//ȡ����λ,�����ǲ��������ļ�	
			{
				vindextbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
   	curindex=0;										//��0��ʼ��ʾ
   	res=f_opendir(&vdir,(const TCHAR*)"0:/VIDEO"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&vdir,vindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&vdir,&vfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||vfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
     	fn=(u8*)(*vfileinfo.lfname?vfileinfo.lfname:vfileinfo.fname);			 
		strcpy((char*)pname,"0:/VIDEO/");			//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  		//���ļ������ں��� 
		LCD_Clear(WHITE);							//������
		video_bmsg_show(fn,curindex+1,totavinum);	//��ʾ����,��������Ϣ		
		key=video_play_mjpeg(pname); 			 	//���������Ƶ�ļ�
		if(key==KEY2_PRES)		//��һ��
		{
			if(curindex)curindex--;
			else curindex=totavinum-1;
 		}else if(key==KEY0_PRES)//��һ��
		{
			curindex++;		   	
			if(curindex>=totavinum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
 		}else break;	//�����˴��� 	 
	} 											  
	myfree(SRAMIN,vfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,vindextbl);			//�ͷ��ڴ�	 
}
//����һ��mjpeg�ļ�
//pname:�ļ���
//����ֵ:
//KEY0_PRES:��һ��
//KEY1_PRES:��һ��
//����:����
u8 video_play_mjpeg(u8 *pname)
{   
	u8* framebuf;	//��Ƶ����buf	 
	u8* pbuf;		//bufָ��  
	FIL *favi;
	u8  res=0;
	u16 offset=0; 
	u32	nr; 
	u8 key;   
    u8 i2ssavebuf;  
	i2sbuf[0]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ�
	i2sbuf[1]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ�
	i2sbuf[2]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ�
	i2sbuf[3]=mymalloc(SRAMIN,AVI_AUDIO_BUF_SIZE);	//������Ƶ�ڴ� 
	framebuf=mymalloc(SRAMIN,AVI_VIDEO_BUF_SIZE);	//������Ƶbuf
	favi=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//����favi�ڴ� 
	memset(i2sbuf[0],0,AVI_AUDIO_BUF_SIZE);
	memset(i2sbuf[1],0,AVI_AUDIO_BUF_SIZE); 
	memset(i2sbuf[2],0,AVI_AUDIO_BUF_SIZE);
	memset(i2sbuf[3],0,AVI_AUDIO_BUF_SIZE); 
	if(i2sbuf[3]==NULL||framebuf==NULL||favi==NULL)
	{
		printf("memory error!\r\n");
		res=0XFF;
	}   
	while(res==0)
	{ 
		res=f_open(favi,(char *)pname,FA_READ);
		if(res==0)
		{
			pbuf=framebuf;			
			res=f_read(favi,pbuf,AVI_VIDEO_BUF_SIZE,&nr);//��ʼ��ȡ	
			if(res)
			{
				printf("fread error:%d\r\n",res);
				break;
			} 	 
			//��ʼavi����
			res=avi_init(pbuf,AVI_VIDEO_BUF_SIZE);	//avi����
			if(res)
			{
				printf("avi err:%d\r\n",res);
				break;
			} 	
			video_info_show(&avix); 
			TIM6_Int_Init(avix.SecPerFrame/100-1,9600-1);//10Khz����Ƶ��,��1��100us 
			offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi");//Ѱ��movi ID	 
			avi_get_streaminfo(pbuf+offset+4);			//��ȡ����Ϣ 
			f_lseek(favi,offset+12);					//������־ID,����ַƫ�Ƶ������ݿ�ʼ��	 
			res=mjpegdec_init((lcddev.width-avix.Width)/2,110+(lcddev.height-110-avix.Height)/2);//JPG�����ʼ�� 
			if(avix.SampleRate)							//����Ƶ��Ϣ,�ų�ʼ��
			{
				WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
				SAIA_Init(0,1,4);		//����SAI,������,16λ���� 
				SAIA_SampleRate_Set(avix.SampleRate);	//���ò�����
				SAIA_TX_DMA_Init(i2sbuf[1],i2sbuf[2],avix.AudioBufSize/2,1);//����DMA
				sai_tx_callback=audio_i2s_dma_callback;	//�ص�����ָ��I2S_DMA_Callback
				i2splaybuf=0;
				i2ssavebuf=0; 
				SAI_Play_Start(); //����I2S���� 
			}
 			while(1)//����ѭ��
			{					
				if(avix.StreamID==AVI_VIDS_FLAG)	//��Ƶ��
				{
					pbuf=framebuf;
					f_read(favi,pbuf,avix.StreamSize+8,&nr);		//������֡+��һ������ID��Ϣ  
					res=mjpegdec_decode(pbuf,avix.StreamSize);
					if(res)
					{
						printf("decode error!\r\n");
					} 
					while(frameup==0);	//�ȴ�ʱ�䵽��(��TIM6���ж���������Ϊ1)
					frameup=0;			//��־����
					frame++; 
				}else 	//��Ƶ��
				{		  
					video_time_show(favi,&avix); 	//��ʾ��ǰ����ʱ��
					i2ssavebuf++;
					if(i2ssavebuf>3)i2ssavebuf=0;
					do
					{
						nr=i2splaybuf;
						if(nr)nr--;
						else nr=3; 
					}while(i2ssavebuf==nr);//��ײ�ȴ�. 
					f_read(favi,i2sbuf[i2ssavebuf],avix.StreamSize+8,&nr);//���i2sbuf	 
					pbuf=i2sbuf[i2ssavebuf];  
				} 
				key=KEY_Scan(0);
				if(key==KEY0_PRES||key==KEY2_PRES)//KEY0/KEY2����,������һ��/��һ����Ƶ
				{
					res=key;
					break; 
				}else if(key==KEY1_PRES||key==WKUP_PRES)
				{
					SAI_Play_Stop();//�ر���Ƶ
					video_seek(favi,&avix,framebuf);
					pbuf=framebuf;
					SAI_Play_Start();//����DMA���� 
				}
				if(avi_get_streaminfo(pbuf+avix.StreamSize))//��ȡ��һ֡ ����־
				{
					printf("frame error \r\n"); 
					res=KEY0_PRES;
					break; 
				} 					   	
			}
			SAI_Play_Stop();	//�ر���Ƶ
			TIM6->CR1&=~(1<<0); //�رն�ʱ��6
			LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ�����
			mjpegdec_free();	//�ͷ��ڴ�
			f_close(favi); 
		}
	}
	myfree(SRAMIN,i2sbuf[0]);
	myfree(SRAMIN,i2sbuf[1]);
	myfree(SRAMIN,i2sbuf[2]);
	myfree(SRAMIN,i2sbuf[3]);
	myfree(SRAMIN,framebuf);
	myfree(SRAMIN,favi);
	return res;
}
//avi�ļ�����
u8 video_seek(FIL *favi,AVI_INFO *aviinfo,u8 *mbuf)
{
	u32 fpos=favi->fptr;
	u8 *pbuf;
	u16 offset;
	u32 br;
	u32 delta;
	u32 totsec;
	u8 key; 
	totsec=(aviinfo->SecPerFrame/1000)*aviinfo->TotalFrame;	
	totsec/=1000;//������.
	delta=(favi->fsize/totsec)*5;		//ÿ��ǰ��5���ӵ������� 
	while(1)
	{
		key=KEY_Scan(1); 
		if(key==WKUP_PRES)//���
		{
			if(fpos<favi->fsize)fpos+=delta; 
			if(fpos>(favi->fsize-AVI_VIDEO_BUF_SIZE))
			{
				fpos=favi->fsize-AVI_VIDEO_BUF_SIZE;
			}
		}else if(key==KEY1_PRES)//����
		{
			if(fpos>delta)fpos-=delta;
			else fpos=0; 
		}else break;
		f_lseek(favi,fpos);
		f_read(favi,mbuf,AVI_VIDEO_BUF_SIZE,&br);	//������֡+��һ������ID��Ϣ 
		pbuf=mbuf; 
		if(fpos==0) //��0��ʼ,����Ѱ��movi ID
		{
			offset=avi_srarch_id(pbuf,AVI_VIDEO_BUF_SIZE,"movi"); 
		}else offset=0;
		offset+=avi_srarch_id(pbuf+offset,AVI_VIDEO_BUF_SIZE,avix.VideoFLAG);	//Ѱ����Ƶ֡	
		avi_get_streaminfo(pbuf+offset);			//��ȡ����Ϣ 
		f_lseek(favi,fpos+offset+8);				//������־ID,����ַƫ�Ƶ������ݿ�ʼ��	 
		if(avix.StreamID==AVI_VIDS_FLAG)
		{
			f_read(favi,mbuf,avix.StreamSize+8,&br);	//������֡ 
			mjpegdec_decode(mbuf,avix.StreamSize); 		//��ʾ��Ƶ֡
		}else 
		{
			printf("error flag");
		}
		video_time_show(favi,&avix);  
	}
	return 0;
}






















