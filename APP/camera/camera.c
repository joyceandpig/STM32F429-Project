#include "camera.h"
#include "common.h"	
#include "calendar.h"  
#include "audioplay.h"  
#include "ov5640.h"  
#include "ltdc.h"   
#include "facereco.h" 
#include "qrplay.h"
//#include "pcf8574.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-����� ����	   
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

extern u8 framecnt;						//ͳһ��֡������,��timer.c���涨��
vu8 hsync_int=0;						//֡�жϱ�־
u8 ov_flash=0;							//�����
u8 bmp_request=0;						//bmp��������:0,��bmp��������;1,��bmp��������,��Ҫ��֡�ж�����,�ر�DCMI�ӿ�.
u8 ovx_mode=0;							//bit0:0,RGB565ģʽ;1,JPEGģʽ 
u16 curline=0;							//����ͷ�������,��ǰ�б��
u16 yoffset=0;							//y�����ƫ����

#define jpeg_buf_size   4*1024*1024		//����JPEG���ݻ���jpeg_buf�Ĵ�С(4M�ֽ�)
#define jpeg_line_size	2*1024			//����DMA��������ʱ,һ�����ݵ����ֵ

u32 *dcmi_line_buf[2];					//RGB��ʱ,����ͷ����һ��һ�ж�ȡ,�����л���  
u32 *jpeg_data_buf;						//JPEG���ݻ���buf 

volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
		
///////////////////////////////////////////////////////////////////////////////////
//����ͷ��ʾ
u8*const camera_remind_tbl[4][GUI_LANGUAGE_NUM]=
{
{"��ʼ��OV5640,���Ժ�...","��ʼ��OV5640,Ո����...","OV5640 Init,Please wait...",},	 
{"δ��⵽OV5640,����...","δ�z�y��OV5640,Ո�z��...","No OV5640 find,Please check...",},	 
{"����Ϊ:","�����:","SAVE AS:",}, 
{"�ֱ���","�ֱ���","Resolution",},	 	 
};
//�s����ʾ
u8*const camera_scalemsg_tbl[2][GUI_LANGUAGE_NUM]=
{
"1:1��ʾ(������)��","1:1�@ʾ(�o�s��)��","1:1 Display(No Scale)��",
"ȫ�ߴ����ţ�","ȫ�ߴ�s�ţ�","Full Scale��",
};
u8*const camera_jpegimgsize_tbl[12]=	//JPEGͼƬ 12�ֳߴ� 
{"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};	 
//���ճɹ���ʾ�����
u8*const camera_saveok_caption[GUI_LANGUAGE_NUM]=
{
"���ճɹ���","���ճɹ���","Take Photo OK��",
}; 		
//����ʧ����ʾ��Ϣ
u8*const camera_failmsg_tbl[3][GUI_LANGUAGE_NUM]=
{	                      						 
"�����ļ�ʧ��,����!","�����ļ�ʧ��,Ո�z��!","Creat File Failed,Please check!",
"�ڴ治��!","�ȴ治��!","Out of memory!",
"���ݴ���(ͼƬ�ߴ�̫��)!","�����e�`(�DƬ�ߴ�̫��)!","Data Error(Picture is too big)!",
}; 
//00������ѡ������
u8*const camera_l00fun_caption[GUI_LANGUAGE_NUM]=
{
"�������","���C�O��","Camera Set",
};
//00������ѡ���
u8*const camera_l00fun_table[GUI_LANGUAGE_NUM][6]=
{
{"��������","��Ч����","��������","ɫ������","�Աȶ�����","���������",},
{"�����O��","��Ч�O��","�����O��","ɫ���O��","���ȶ��O��","�W����O��",},
{"Scene","Effects","Brightness","Saturation","Contrast","Flashlight",},
};	
//////////////////////////////////////////////////////////////////////////	   
//10������ѡ���
//����ģʽ
u8*const camera_l10fun_table[GUI_LANGUAGE_NUM][5]=
{
{"�Զ�","����","�칫��","����","��ͥ",},
{"�Ԅ�","����","�k����","���","��ͥ",},
{"Auto","Sunny","Office","Cloudy","Home"},
};	
//11������ѡ���
//��Ч����
u8*const camera_l11fun_table[GUI_LANGUAGE_NUM][7]=
{
{"����","��ɫ","ůɫ","�ڰ�","ƫ��","��ɫ","ƫ��"},
{"����","��ɫ","ůɫ","�ڰ�","ƫ�S","��ɫ","ƫ�G"},
{"Normal","Cool","Warm","W&B","Yellowish","Invert","Greenish",},
};	
//12~14������ѡ���
//����-3~3��7��ֵ
u8*const camera_l124fun_table[GUI_LANGUAGE_NUM][7]=
{
{"-3","-2","-1","0","+1","+2","+3",},					   
{"-3","-2","-1","0","+1","+2","+3",},					   
{"-3","-2","-1","0","+1","+2","+3",},					   
};	
//15������ѡ���
//���������
u8*const camera_l15fun_table[GUI_LANGUAGE_NUM][2]=
{
{"�ر�","��",},
{"�P�]","���_",},
{"OFF","ON",},
};
//////////////////////////////////////////////////////////////////////////	  


//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;			//ʣ�����ݳ���
	u32 *pbuf;
	if(qr_dcmi_rgbbuf_sta&0X02)			//QRʶ��ģʽ
	{						
		qr_dcmi_curline=0;				//��������   
		qr_dcmi_rgbbuf_sta|=1<<0;		//�������׼������ 
		return ;						//ֱ�ӷ���,����ִ��ʣ�µĴ���		
	}
	curline=yoffset;	//��������
	framecnt++;	
	if(ovx_mode&0X01)	//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{
			DMA2_Stream1->CR&=~(1<<0);		//ֹͣ��ǰ����
			while(DMA2_Stream1->CR&0X01);	//�ȴ�DMA2_Stream1������ 
			rlen=jpeg_line_size-DMA2_Stream1->NDTR;//�õ�ʣ�����ݳ���	
			pbuf=jpeg_data_buf+jpeg_data_len;//ƫ�Ƶ���Ч����ĩβ,�������
			if(DMA2_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[1][i];//��ȡbuf1�����ʣ������
			else for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[0][i];//��ȡbuf0�����ʣ������ 
			jpeg_data_len+=rlen;			//����ʣ�೤��
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_line_size;//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA2_Stream1->CR|=1<<0;			//���´���
			jpeg_data_ok=0;					//�������δ�ɼ�
			jpeg_data_len=0;				//�������¿�ʼ
		}
	}else
	{  
		if(bmp_request==1)	//��bmp��������,�ر�DCMI
		{
			DCMI_Stop();	//ֹͣDCMI
			bmp_request=0;	//������������.
		} 
		LCD_SetCursor(frec_dev.xoff,frec_dev.yoff);   
		frec_curline=frec_dev.yoff;
		LCD_WriteRAM_Prepare();				//��ʼд��GRAM   
		hsync_int=1;
	}  
} 
//jpeg���ݽ��ջص�����
void jpeg_dcmi_rx_callback(void)
{  
	u16 i;
	u32 *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;//ƫ�Ƶ���Ч����ĩβ
	if(DMA2_Stream1->CR&(1<<19))//buf0����,��������buf1
	{ 
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[0][i];//��ȡbuf0���������
		jpeg_data_len+=jpeg_line_size;//ƫ��
	}else //buf1����,��������buf0
	{
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[1][i];//��ȡbuf1���������
		jpeg_data_len+=jpeg_line_size;//ƫ�� 
	} 
} 

//RGB�����ݽ��ջص�����
void rgblcd_dcmi_rx_callback(void)
{  
	u16 *pbuf;
	if(DMA2_Stream1->CR&(1<<19))//DMAʹ��buf1,��ȡbuf0
	{ 
		pbuf=(u16*)dcmi_line_buf[0]; 
	}else 						//DMAʹ��buf0,��ȡbuf1
	{
		pbuf=(u16*)dcmi_line_buf[1]; 
	} 	
	LTDC_Color_Fill(0,curline,lcddev.width-1,curline,pbuf);//DM2D��� 
	if(curline<lcddev.height)curline++;
}
//�л�ΪOV5640ģʽ
void sw_ov5640_mode(void)
{ 
 	OV5640_WR_Reg(0X3017,0XFF);	//����OV5650���(����������ʾ)
	OV5640_WR_Reg(0X3018,0XFF); 
	//GPIOC8/9/11�л�Ϊ DCMI�ӿ�
 	GPIO_AF_Set(GPIOC,8,13);	//PC8,AF13  DCMI_D2
 	GPIO_AF_Set(GPIOC,9,13);	//PC9,AF13  DCMI_D3
 	GPIO_AF_Set(GPIOC,11,13);	//PC11,AF13 DCMI_D4  
} 
//�л�ΪSD��ģʽ
void sw_sdcard_mode(void)
{
	OV5640_WR_Reg(0X3017,0X00);	//�ر�OV5640ȫ�����(��Ӱ��SD��ͨ��)
	OV5640_WR_Reg(0X3018,0X00);  
 	//GPIOC8/9/11�л�Ϊ SDIO�ӿ�
  	GPIO_AF_Set(GPIOC,8,12);	//PC8,AF12
 	GPIO_AF_Set(GPIOC,9,12);	//PC9,AF12 
 	GPIO_AF_Set(GPIOC,11,12);	//PC11,AF12  
} 
//�õ��ļ���,��������ʱ������
//mode:0,����.bmp�ļ�;1,����.jpg�ļ�.
//bmp��ϳ�:����"0:PHOTO/PIC20120321210633.bmp"/"3:PHOTO/PIC20120321210633.bmp"���ļ���
//jpg��ϳ�:����"0:PHOTO/PIC20120321210633.jpg"/"3:PHOTO/PIC20120321210633.jpg"���ļ���
void camera_new_pathname(u8 *pname,u8 mode)
{	  
	calendar_get_time(&calendar);
	calendar_get_date(&calendar); 
	if(mode==0)
	{
		if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//��ѡ������SD��
		else if(gui_phy.memdevflag&(1<<3))sprintf((char*)pname,"3:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD��������,�򱣴���U��	
	}else 
	{
		if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//��ѡ������SD��
		else if(gui_phy.memdevflag&(1<<3))sprintf((char*)pname,"3:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD��������,�򱣴���U��	
	}
} 
//OV5640����jpgͼƬ
//����ֵ:0,�ɹ�
//    ����,�������
u8 ov5640_jpg_photo(u8 *pname)
{
	FIL* f_jpg; 
	u8 res=0,headok=0;
	u32 bwr;
	u32 i,jpgstart,jpglen;
	u8* pbuf;
	f_jpg=(FIL *)mymalloc(SRAMIN,sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_jpg==NULL)return 0XFF;				//�ڴ�����ʧ��.
	ovx_mode=1;
	jpeg_data_ok=0;
	sw_ov5640_mode();						//�л�ΪOV5640ģʽ 
	OV5640_JPEG_Mode();						//JPEGģʽ  
	OV5640_OutSize_Set(16,4,2592,1944);		//��������ߴ�(500W) 
	dcmi_rx_callback=jpeg_dcmi_rx_callback;	//JPEG�������ݻص�����
	DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],jpeg_line_size,2,1);//DCMI DMA����    
	DCMI_Start(); 			//�������� 
	while(jpeg_data_ok!=1);	//�ȴ���һ֡ͼƬ�ɼ���
	jpeg_data_ok=2;			//���Ա�֡ͼƬ,������һ֡�ɼ� 
	if(ov_flash)
	{
		while(jpeg_data_ok==2);
		OV5640_Flash_Ctrl(1);//�������
	}
	while(jpeg_data_ok!=1);	//�ȴ��ڶ�֡ͼƬ�ɼ���,�ڶ�֡,�ű��浽SD��ȥ. 
	if(ov_flash)OV5640_Flash_Ctrl(0);//�ر������
	DCMI_Stop(); 			//ֹͣDMA����
	ovx_mode=0; 
	sw_sdcard_mode();		//�л�ΪSD��ģʽ
	res=f_open(f_jpg,(const TCHAR*)pname,FA_WRITE|FA_CREATE_NEW);//ģʽ0,���߳��Դ�ʧ��,�򴴽����ļ�	 
	if(res==0)
	{
		printf("jpeg data size:%d\r\n",jpeg_data_len*4);//���ڴ�ӡJPEG�ļ���С
		pbuf=(u8*)jpeg_data_buf;
		jpglen=0;	//����jpg�ļ���СΪ0
		headok=0;	//���jpgͷ���
		for(i=0;i<jpeg_data_len*4;i++)//����0XFF,0XD8��0XFF,0XD9,��ȡjpg�ļ���С
		{
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))//�ҵ�FF D8
			{
				jpgstart=i;
				headok=1;	//����ҵ�jpgͷ(FF D8)
			}
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD9)&&headok)//�ҵ�ͷ�Ժ�,����FF D9
			{
				jpglen=i-jpgstart+2;
				break;
			}
		}
		if(jpglen)			//������jpeg���� 
		{
			pbuf+=jpgstart;	//ƫ�Ƶ�0XFF,0XD8��
			res=f_write(f_jpg,pbuf,jpglen,&bwr);
			if(bwr!=jpglen)res=0XFE; 
			
		}else res=0XFD; 
	}
	jpeg_data_len=0;
	f_close(f_jpg); 
	sw_ov5640_mode();		//�л�ΪOV5640ģʽ
	OV5640_RGB565_Mode();	//RGB565ģʽ  
	if(lcdltdc.pwidth!=0)	//RGB��
	{
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB���������ݻص�����
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA����  
	}else					//MCU ��
	{
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA����,MCU��,����
	}
	myfree(SRAMIN,f_jpg); 
	return res;
}   

//����ͷ����
//������Ƭ�ļ�,��������SD��PHOTO�ļ�����.
u8 camera_play(void)
{												   
	u8 rval=0;
	u8 res,fac;	
 	u8 *caption=0;
	u8 *pname;
	u8 selx=0;
	u8 l00sel=0,l10sel=0,l11sel=0;//Ĭ��ѡ����
	u8 l2345sel[3];
	u8 *psn;
	u8 key;
 	u8 scale=1;				//Ĭ����ȫ�ߴ�����
	u8 tcnt=0;
	u16 outputheight=0;		//RGB ͼ������Ŀ��
	
 	if(audiodev.status&(1<<7))		//��ǰ�ڷŸ�??����ֹͣ
	{
		audio_stop_req(&audiodev);	//ֹͣ��Ƶ����
		audio_task_delete();		//ɾ�����ֲ�������.
	}
 	//��ʾ��ʼ���OV5640
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(gui_phy.memdevflag&(1<<0))f_mkdir("0:PHOTO");//ǿ�ƴ����ļ���,���������
	if(gui_phy.memdevflag&(1<<3))f_mkdir("3:PHOTO");//ǿ�ƴ����ļ���,���������
	if(OV5640_Init())//��ʼ��OV5640
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);  
		rval=1;
	} 
	dcmi_line_buf[0]=gui_memin_malloc(jpeg_line_size*4);//Ϊjpeg dma���������ڴ�	
	dcmi_line_buf[1]=gui_memin_malloc(jpeg_line_size*4);//Ϊjpeg dma���������ڴ�	
	jpeg_data_buf=gui_memex_malloc(jpeg_buf_size);		//Ϊjpeg�ļ������ڴ�(���4MB)
	pname=gui_memin_malloc(40);	//����40���ֽ��ڴ�,���ڴ���ļ���
	psn=gui_memin_malloc(50);	//����50���ֽ��ڴ�,���ڴ�����ƣ�������Ϊ:0:PHOTO/PIC20120321210633.bmp"������ʾ��
	if(!dcmi_line_buf[1]||!jpeg_data_buf||!pname||!psn)rval=1;//����ʧ��	  
	if(rval==0)//OV5640����
	{ 
		l2345sel[0]=3;			//����Ĭ��Ϊ3,ʵ��ֵ0
		l2345sel[1]=3;			//ɫ��Ĭ��Ϊ3,ʵ��ֵ0
		l2345sel[2]=3;			//�Աȶ�Ĭ��Ϊ3,ʵ��ֵ0  
 		OV5640_RGB565_Mode();	//RGB565ģʽ
		OV5640_Focus_Init();  
		OV5640_Focus_Constant();//���������Խ�
		OV5640_Light_Mode(0);	//�Զ�ģʽ
		OV5640_Brightness(l2345sel[0]+1);		//��������
		OV5640_Color_Saturation(l2345sel[1]);	//ɫ������
		OV5640_Contrast(l2345sel[2]);			//�Աȶ����� 
		OV5640_Exposure(3);		//�ع�ȼ�
 		OV5640_Sharpness(33);	//�Զ����
		DCMI_Init();			//DCMI���� 
		if(lcdltdc.pwidth!=0)	//RGB��
		{
			dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB���������ݻص�����
			DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA����  
		}else					//MCU ��
		{
			DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA����,MCU��,����
		} 	
		if(lcddev.height>800)
		{
			yoffset=(lcddev.height-800)/2;
			outputheight=800;
			TIM3->CR1&=~(0x01);	//�رն�ʱ��3,
		}else 
		{
			yoffset=0;
			outputheight=lcddev.height;
		}
		curline=yoffset;		//������λ
		OV5640_OutSize_Set(16,4,lcddev.width,outputheight);//��������ͷ����ߴ�ΪLCD�ĳߴ��С		
		DCMI_Start(); 			//��������  
		LCD_Clear(BLACK);
   		system_task_return=0;	//���TPAD	 
 		while(1)	 
		{		 	
			tp_dev.scan(0);
			if(tp_dev.sta&TP_PRES_DOWN)
			{    	 
				OV5640_Focus_Single();//ִ���Զ��Խ�
				tp_dev.scan(0);
				tp_dev.sta=0;
 			}
			key=KEY_Scan(0);//����ɨ��
			if(key)
			{
				if(key==KEY2_PRES)//�����BMP����,��ȴ�300ms,ȥ����,�Ի���ȶ���bmp��Ƭ	
				{ 
					delay_ms(300);
					hsync_int=0;
					while(hsync_int==0);				//�ȴ�֡�ж�
					hsync_int=0;
					if(ov_flash)OV5640_Flash_Ctrl(1);	//������� 
					bmp_request=1;						//����ر�DCMI
					while(hsync_int==0);
					if(ov_flash)OV5640_Flash_Ctrl(0);	//�ر������ 
					while(bmp_request);					//�ȴ���������� 
				}else DCMI_Stop();
				while(KEY_Scan(1));//�ȴ������ɿ�
				tcnt=0;
				switch(key)
				{
					case KEY0_PRES:	//KEY0����,JPEG����
					case KEY2_PRES:	//KEY2����,BMP����
						LED1=0;		//DS1��,��ʾ������
						sw_sdcard_mode();	//�л�ΪSD��ģʽ
						if(key==KEY0_PRES)	//JPEG����
						{
							camera_new_pathname(pname,1);//�õ�jpg�ļ���	
							res=ov5640_jpg_photo(pname);
						}else//BMP����
						{
							camera_new_pathname(pname,0);//�õ�bmp�ļ���	
							res=bmp_encode(pname,0,yoffset,lcddev.width,outputheight,0);//bmp����
						}	
 						if(res)//����ʧ����
						{
							if(res==0XFF)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//�ڴ����
							else if(res==0XFD)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//���ݴ���
							else window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//��ʾSD���Ƿ����
						}else
						{
							strcpy((char*)psn,(const char *)camera_remind_tbl[2][gui_phy.language]);
							strcat((char*)psn,(const char *)pname);
							window_msg_box((lcddev.width-180)/2,(lcddev.height-80)/2,180,80,psn,(u8*)camera_saveok_caption[gui_phy.language],12,0,0,0);
// 							PCF8574_WriteBit(BEEP_IO,0);	//�������̽У���ʾ�������
							delay_ms(100); 
						}
						LED1=1;						//DS1��,��ʾ�������
//						PCF8574_WriteBit(BEEP_IO,1);//�رշ�����
						delay_ms(2000); 
						sw_ov5640_mode();			//�л�ΪOV5640ģʽ   
						if(scale==0)
						{
							fac=800/outputheight;//�õ���������
							OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						}else 
						{
							OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
						} 	
						system_task_return=0;//���TPAD
						break;
					case KEY1_PRES:	//KEY1����,����/1:1��ʾ(������)
						scale=!scale;  
						if(scale==0)
						{
							OSTaskSuspend(7); 		//����usart_task
							fac=800/outputheight;	//�õ���������
							OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						}else
						{
							OSTaskResume(7); 		//�ָ�usart_task
							OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
						}	  
						window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_scalemsg_tbl[scale][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 						while(KEY_Scan(1)==0&&tcnt<80)//�ȴ�800ms,���û�а������µĻ�.
						{
							delay_ms(10);
							tcnt++;
						}	
						break;
					case WKUP_PRES:			//��������ͷ���� 
						sw_sdcard_mode();	//�л�ΪSD��ģʽ
						caption=(u8*)camera_l00fun_caption[gui_phy.language];
						res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*6)/2,160,72+32*6,(u8**)camera_l00fun_table[gui_phy.language],6,(u8*)&l00sel,0X90,caption);//��ѡ
						sw_ov5640_mode();	//�л�ΪOV5640ģʽ
						if(res==0)
						{ 
							DCMI_Start();
							delay_ms(200);
							DCMI_Stop();
							sw_sdcard_mode();	//�л�ΪSD��ģʽ 
							caption=(u8*)camera_l00fun_table[gui_phy.language][l00sel]; 
							switch(l00sel)
							{
								case 0://��������	  
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*5)/2,160,72+32*5,(u8**)camera_l10fun_table[gui_phy.language],5,(u8*)&l10sel,0X90,caption);//��ѡ
									sw_ov5640_mode();//�л�ΪOV5640ģʽ
									if(res==0)
									{
										OV5640_Light_Mode(l10sel);
									}
									break;
								case 1://��Ч����		 
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*7)/2,160,72+32*7,(u8**)camera_l11fun_table[gui_phy.language],7,(u8*)&l11sel,0X90,caption);//��ѡ
									sw_ov5640_mode();//�л�ΪOV5640ģʽ
									if(res==0)
									{
										OV5640_Special_Effects(l11sel);  
									}
									break;  
								case 2://��������    
								case 3://ɫ������
								case 4://�Աȶ�����
									selx=l2345sel[l00sel-2];//�õ�֮ǰ��ѡ��
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*7)/2,160,72+32*7,(u8**)camera_l124fun_table[gui_phy.language],7,(u8*)&selx,0X90,caption);//��ѡ
									sw_ov5640_mode();//�л�ΪOV5640ģʽ
									if(res==0)
									{
										l2345sel[l00sel-2]=selx;//��¼��ֵ 
										if(l00sel==2)OV5640_Brightness(selx+1);	   	//��������
										if(l00sel==3)OV5640_Color_Saturation(selx);	//ɫ������
										if(l00sel==4)OV5640_Contrast(selx);			//�Աȶ�����
									}
									break; 
								case 5://���������
									selx=ov_flash;
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*2)/2,160,72+32*2,(u8**)camera_l15fun_table[gui_phy.language],2,(u8*)&selx,0X90,caption);//��ѡ
									if(res==0)ov_flash=selx; 
									break;
							}
						}      
						break;
						
				}
				sw_ov5640_mode();	//�л�ΪOV5640ģʽ   
				DCMI_Start();		//������ʹ��dcmi,Ȼ�������ر�DCMI,�����ٿ���DCMI,���Է�ֹRGB���Ĳ�������.
				DCMI_Stop();	
				DCMI_Start();		
			} 
			if(system_task_return)
			{
				delay_ms(15);
				if(TPAD_Scan(1))break;//TPAD����,�ٴ�ȷ��,�ų�����	
				else system_task_return=0;
 			}
			if(hsync_int)//�ող���֡�ж�,������ʱ
			{
				delay_ms(10);
				hsync_int=0;
			}  
 		}
	}
	DCMI_Stop();		//ֹͣ����ͷ����
	sw_sdcard_mode();	//�л�ΪSD��ģʽ 
	gui_memin_free(dcmi_line_buf[0]);
	gui_memin_free(dcmi_line_buf[1]);
	gui_memex_free(jpeg_data_buf);
	gui_memin_free(pname);				
	gui_memin_free(psn);  
	OSTaskResume(7);	//�ָ�usart_task
 	return 0;	 
}















