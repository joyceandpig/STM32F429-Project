#include "qrplay.h"
#include "ov5640.h" 
#include "dcmi.h"  
//#include "pcf8574.h"
#include "qr_encode.h"
#include "atk_qrdecode.h"
#include "t9input.h"
#include "audioplay.h" 
#include "camera.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-��ά��ʶ��&���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
extern u8*const camera_remind_tbl[4][GUI_LANGUAGE_NUM];

extern u32 *dcmi_line_buf[2];		//����ͷ����һ��һ�ж�ȡ,�����л���  
u16 *rgb_data_buf;					//RGB565֡����buf 

vu8 qr_dcmi_rgbbuf_sta=0;			//RGB BUF״̬
									//bit:7~2,����
									//bit1:0,��QRʶ��ģʽ;1,QRʶ��ģʽ;
									//bit0:0,����δ׼����;1,����׼������
									
extern u32 *ltdc_framebuf[2];		//LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ����� 
u32 qr_parm1,qr_parm2,qr_parm3;		//��LTDC���,����ٶȵ��������� 
			
u16 qr_dcmi_curline=0;				//����ͷ�������,��ǰ�б��
u16 qr_lcd_width=0;					//qrʶ��,��ʾ��LCD�����ͼ��Ŀ��(�߶ȵ��ڿ��)
u16 qr_lcd_xoff;					//qrʶ��,��ʾ��LCD��ʱ,X�����ƫ����
u16 qr_lcd_yoff;					//qrʶ��,��ʾ��LCD��ʱ,Y�����ƫ����

//ģʽѡ��
u8*const qr_mode_tbl[GUI_LANGUAGE_NUM][2]=
{
{"ʶ���ά��","���ɶ�ά��",},
{"�R�e���S�a","���ɶ��S�a",},		 
{"Decode QR Code","Encode QR code",},
}; 

//qr������ʾ��Ϣ
u8*const qr_remind_msg_tbl[3][GUI_LANGUAGE_NUM]=
{ 
{"��������:","ݔ�����:","Input Text:",},
{"����������!","Ոݔ�����!","Pls Input Text!",}, 
{"����̫��!","����̫��!","Too Many Text",},  
};
//qr���밴ť����
u8*const qr_enc_tbl[GUI_LANGUAGE_NUM]= 
{"����","���a","Encode",};
//qr���������
u8*const qr_result_tbl[2][GUI_LANGUAGE_NUM]= 
{
{"ʶ����","�R�e�Y��","Decode Result",},
{"������","���a�Y��","Encode Result",},
};

//����ͷ���ݽ��ջص�����
void qr_dcmi_rx_callback(void)
{  
	u32 *pbuf;
	u32 *qr_plcd=ltdc_framebuf[lcdltdc.activelayer];	//ָ���Դ��ַ�����������,����ٶ�
	u16 *disbuf;
	u16 i;
	u32 temp;
	if(lcdltdc.pwidth!=0)	//RGB��,ֱ����ʾ
	{
//		__HAL_DMA_GET_IT_SOURCE(DMA_Handler, DMA_FLAG_TEIF2_6);
		if(DMA2_Stream1->CR&(1<<19))//DMAʹ��buf1,��ȡbuf0
		{ 
			disbuf=(u16*)dcmi_line_buf[0];
		}else 						//DMAʹ��buf0,��ȡbuf1
		{
			disbuf=(u16*)dcmi_line_buf[1];
		}
		temp=qr_parm1+qr_parm3*qr_dcmi_curline; 
		for(i=0;i<qr_lcd_width;i++)//���,�����RGB LCD��,ÿ�����һ������
		{  
			*(u16*)((u32)qr_plcd+temp-qr_parm2*i)=*(disbuf+i);
		}
	}else					//MCU��,��Ҫ��ÿһ�����ݽ��д洢
	{
		pbuf=(u32*)(rgb_data_buf+qr_dcmi_curline*qr_lcd_width);
		if(DMA2_Stream1->CR&(1<<19))//DMAʹ��buf1,��ȡbuf0
		{ 
			for(i=0;i<qr_lcd_width/2;i++)
			{
				pbuf[i]=dcmi_line_buf[0][i];
			} 
		}else 						//DMAʹ��buf0,��ȡbuf1
		{
			for(i=0;i<qr_lcd_width/2;i++)
			{
				pbuf[i]=dcmi_line_buf[1][i];
			} 
		} 
		LCD_Color_Fill(qr_lcd_xoff,qr_lcd_yoff+qr_dcmi_curline,qr_lcd_xoff+qr_lcd_width-1,qr_lcd_yoff+qr_dcmi_curline,(u16*)pbuf);
	}
	if(qr_dcmi_curline<(qr_lcd_width-1))qr_dcmi_curline++;
}  

//��ʾʶ����
void qr_decode_show_result(u8* result)
{
 	_window_obj* twin=0;	//����
	_memo_obj * tmemo=0;	//memo�ؼ�
	u8*p;
	u8 linecnt=1;			//����
	u16 wwidth,wheight;
	u16 wy;
	u16 temp,linecharcnt;
	u8 fsize=16;  
	u16 winoff,wxoff;		//memo�ڴ����ڵ�ƫ��/����x����ƫ��
	
	LCD_Clear(BLACK); 
	if(lcddev.width==240)
	{
		winoff=10;wxoff=10;
	}else if(lcddev.width==272)
	{
		winoff=12;wxoff=15;
	}else if(lcddev.width==320)
	{
		winoff=15;wxoff=20;
	}else if(lcddev.width>=480)
	{
		winoff=20;wxoff=30;
		fsize=24;
	}
	wwidth=lcddev.width-2*wxoff;
	linecharcnt=(wwidth-2*winoff-MEMO_SCB_WIDTH)*2/fsize;//ÿһ�п�����ʾ���ַ�����
	temp=strlen((char*)result);				//�õ��ַ������峤��
	temp/=linecharcnt;						//�õ���Ҫ������
	p=result;
	while(1)
	{
		p=(u8*)strstr((char*)p,"\n");		//���һ���,ͳ������
		if(p)
		{
			linecnt++;						//������1
			p+=1;							//����ƫ��һ����ַ
		}else break;		
	}
	if(linecnt<temp)linecnt=temp;			//�������Ӷ���
	if(linecnt<3)linecnt=3;					//����3������
	wheight=2*winoff+(2+linecnt)*fsize;		//����߶� 
	if(wheight>(lcddev.height-4*fsize))wheight=lcddev.height-4*fsize;//������2���ָߵļ�϶  
 	wy=(lcddev.height-wheight)/2; 
	tmemo=memo_creat(wxoff+winoff,wy+fsize*2+winoff,wwidth-winoff*2,wheight-(fsize+winoff)*2,0,1,fsize,1024);//����1K�ڴ�.	 
  	twin=window_creat(wxoff,wy,wwidth,wheight,0,1|(1<<5),fsize);//��������
	if(twin&&tmemo)
	{
		twin->captionheight=2*fsize;		//Ĭ�ϸ߶�
		twin->caption=qr_result_tbl[0][gui_phy.language];	//��ʾ����
		twin->font=fsize;					//���������С
		strcpy((char*)tmemo->text,(const char*)result);		//�����ַ���
 		window_draw(twin);					//��������
		memo_draw_memo(tmemo,0);			//����memo
		system_task_return=0;
		while(1)
		{
			tp_dev.scan(0);
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			memo_check(tmemo,&in_obj);
			if(system_task_return)break; 	//����
			delay_ms(5);			//��ʱ 
		}
	}
	window_delete(twin);
	memo_delete(tmemo);				//�ͷ��ڴ�	 
	system_task_return=0;
}
//��ά��ʶ��
void qr_decode_play(void)
{
	u8 rval=0;
	u16 i=0,j=0;
 	float fac; 
	
	static u8 bartype=0; 			//��ѯ���б���
	
	u16 qr_input_width=0;			//����ʶ������ͼ����,��󲻳���240!
	u8 *qr_gray_buf;				//8λ�Ҷ�ͼƬ����
	u8 qr_img_scale=1;				//�������� 
	u8 *qr_result=NULL;
	u16 color; 	
	u8 qr_data_ready=0;				//qrʶ���õ�����׼���ñ�־:0,δ׼����;��0,׼������

	
	
	if(audiodev.status&(1<<7))		//��ǰ�ڷŸ�??����ֹͣ
	{
		audio_stop_req(&audiodev);	//ֹͣ��Ƶ����
		audio_task_delete();		//ɾ�����ֲ�������.
	}
 	//��ʾ��ʼ���OV5640
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(OV5640_Init())//��ʼ��OV5640
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);  
		rval=1;
	} 
	if(lcddev.width<=240)
	{
		qr_lcd_width=lcddev.width;
		qr_input_width=qr_lcd_width;
	}else if(lcddev.width<480)
	{
		qr_lcd_width=240;
		qr_input_width=qr_lcd_width; 
	}else 
	{ 
		qr_lcd_width=480; 
		qr_input_width=240;
		qr_img_scale=2;
	}	
	qr_result=mymalloc(SRAMEX,2048);	//SRAMIN
	qr_gray_buf=mymalloc(SRAMCCM,qr_input_width*qr_input_width);	//�����ڴ�
	dcmi_line_buf[0]=gui_memin_malloc(qr_lcd_width*2);				//Ϊ�л�����������ڴ�	
	dcmi_line_buf[1]=gui_memin_malloc(qr_lcd_width*2);				//Ϊ�л�����������ڴ�
	rgb_data_buf=gui_memex_malloc(qr_lcd_width*qr_lcd_width*2);		//Ϊrgb֡���������ڴ�
	if(!qr_gray_buf||!dcmi_line_buf[1]||!rgb_data_buf[1])rval=1;
	if(rval==0)
	{ 
		//�Զ��Խ���ʼ��
		OV5640_RGB565_Mode();		//RGB565ģʽ 
		OV5640_Focus_Init(); 
		OV5640_Light_Mode(0);		//�Զ�ģʽ
		OV5640_Color_Saturation(3);	//ɫ�ʱ��Ͷ�0
		OV5640_Brightness(4);		//����0
		OV5640_Contrast(3);			//�Աȶ�0
		OV5640_Sharpness(33);		//�Զ����
		OV5640_Focus_Constant();	//���������Խ� 
		if(lcdltdc.pwidth!=0)OV5640_WR_Reg(0X3821,0x00);//������  
		DCMI_Init();				//DCMI����  
		qr_dcmi_rgbbuf_sta|=1<<1;	//���qrģʽ
		dcmi_rx_callback=qr_dcmi_rx_callback;//���ݽ��ջص�����
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],qr_lcd_width/2,1,1);//DCMI DMA����   
		//����RGB��GRAMƫ�Ʋ���
		qr_lcd_xoff=(lcddev.width-qr_lcd_width)/2;
		qr_lcd_yoff=(lcddev.height-qr_lcd_width)/2;
		qr_parm1=lcdltdc.pixsize*lcdltdc.pwidth*(lcdltdc.pheight-qr_lcd_xoff-1)+lcdltdc.pixsize*qr_lcd_yoff;
		qr_parm2=lcdltdc.pixsize*lcdltdc.pwidth;
		qr_parm3=lcdltdc.pixsize;
		rval=atk_qr_init();	//��ʼ��ʶ���
		if(rval==0)
		{
			fac=800/qr_lcd_width;		//�õ���������
			OV5640_OutSize_Set((1280-fac*qr_lcd_width)/2,(800-fac*qr_lcd_width)/2,qr_lcd_width,qr_lcd_width); 
			DCMI_Start(); 				//��������
			LCD_Clear(BLACK);
			while(1)
			{
				tp_dev.scan(0);
				if(tp_dev.sta&TP_PRES_DOWN)
				{    	 
					OV5640_Focus_Single();//ִ���Զ��Խ�
					tp_dev.scan(0);
					tp_dev.sta=0;
				}
				if((qr_dcmi_rgbbuf_sta&0X01)&&qr_data_ready==0)//��ʶ���þ�δ׼����,��������ͷ�ɼ������Ѿ�OK.
				{ 
					for(i=0;i<qr_input_width;i++)
					{
						for(j=0;j<qr_input_width;j++)
						{
							if(lcdltdc.pwidth)color=LTDC_Read_Point(j*qr_img_scale+qr_lcd_xoff,i*qr_img_scale+qr_lcd_yoff);
							else color=*(rgb_data_buf+i*qr_img_scale*qr_lcd_width+j*qr_img_scale);
							*(qr_gray_buf+i*qr_input_width+j)=(((color&0xF800)>> 8)*76+((color&0x7E0)>>3)*150+((color&0x001F)<<3)*30)>>8;//ת��Ϊ�Ҷ�
						}
					} 
					qr_dcmi_rgbbuf_sta&=~(1<<0);	//�������׼���ñ��
					qr_data_ready=5;				//���ʶ��������׼������
				}
				if(qr_data_ready)					//����׼������? 
				{ 		
					atk_qr_decode(qr_input_width,qr_input_width,qr_gray_buf,bartype,qr_result);//ʶ��Ҷ�ͼƬ�����κ�ʱԼ0.2S��
					if(qr_result[0]==0)//û��ʶ�����
					{
						bartype++;
						if(qr_data_ready)qr_data_ready--;
						if(bartype>=5)bartype=0; 	//���֧��5�ֱ����ʽ 
					}else if(qr_result[0]!=0)
					{	
//						PCF8574_WriteBit(BEEP_IO,0);
						delay_ms(100);
//						PCF8574_WriteBit(BEEP_IO,1);
						qr_data_ready=0;			//ʶ�����,����һ������
						DCMI_Stop();
						sw_sdcard_mode();			//�л�ΪSD��ģʽ 
						printf("qr decode result:%s\r\n",qr_result); 
						qr_decode_show_result(qr_result);
						LCD_Clear(BLACK);
						sw_ov5640_mode();			//�л�ΪOV5640ģʽ   
						DCMI_Start();	 
						delay_ms(500);
						qr_dcmi_rgbbuf_sta&=~(1<<0);//�������׼���ñ��
						qr_data_ready=0;			//���ʶ��������׼������
					}   
				}
				i++;
				if(system_task_return)break;		//TPAD����,�ٴ�ȷ��,�ų�����	
				delay_ms(1000/OS_TICKS_PER_SEC);	//��ʱһ��ʱ�ӽ���  					
			}
			atk_qr_destroy();
		}
	}
	DCMI_Stop();						//ֹͣ����ͷ����
	myfree(SRAMEX,qr_result);//SRAMIN
	myfree(SRAMCCM,qr_gray_buf);	//
	gui_memin_free(dcmi_line_buf[0]);
	gui_memin_free(dcmi_line_buf[1]);
	gui_memex_free(rgb_data_buf);
	qr_dcmi_rgbbuf_sta=0;
	sw_sdcard_mode();				//�л�ΪSD��ģʽ 
}
//��ά������
void qr_encode_play(void)
{
 	_window_obj* twin=0;//����
	u8 res; 
	u8 rval=0; 
 	
	u16 i,j;
	u16 pixsize;		//���С
	u16 wwidth,wheight;	//QR������ڴ�С
	u16 wpitch;			//��϶

	u16 textlen;		//�����ַ�������
	u16 h1,h2;			//������
	u16 w1;				//������
	u16 t9height=0;		//T9���뷨�߶�
	u16 btnw,btnh;		//��ť���/�߶�
	u16 memow,memoh;	//�ı�����/�߶� 
	
	u8 cbtnfsize=0;		//��ť�����С
	u16 tempy;  
	
 	_btn_obj* encbtn=0;	//���밴ť
	_memo_obj * tmemo=0;//memo�ؼ�
	_t9_obj * t9=0;		//���뷨  
	u16 *bkcolorbuf;	//����ɫ������
	 
	 
	if(lcddev.width==240)
	{
		btnw=65;btnh=30;
		memow=160,memoh=132;
		cbtnfsize=16;t9height=134;  
	}else if(lcddev.width==272)
	{ 
		btnw=75;btnh=38;
		memow=180,memoh=242; 	 
		cbtnfsize=16;t9height=176;
	}else if(lcddev.width==320)
	{ 
		btnw=90;btnh=40;
		memow=200,memoh=242; 	 
		cbtnfsize=16;t9height=176;
	}else if(lcddev.width==480)
	{ 	
		btnw=120;btnh=40; 
		memow=300,memoh=434; 
		cbtnfsize=24;t9height=266;
	}else if(lcddev.width==600)
	{ 	
		btnw=160;btnh=60; 
		memow=400,memoh=582; 
		cbtnfsize=24;t9height=368;
	}
	h1=(lcddev.height-gui_phy.tbheight-memoh-t9height-cbtnfsize)*4/9;
	h2=h1/4;
	w1=(lcddev.width-memow-btnw)/3;
	
	LCD_Clear(QR_BACK_COLOR);//������ɫ
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[16][gui_phy.language],0X05);	//��ʾ����  
	POINT_COLOR=WHITE;  
	gui_show_string(qr_remind_msg_tbl[0][gui_phy.language],w1,gui_phy.tbheight+h1,lcddev.width,gui_phy.tbfsize,gui_phy.tbfsize,BLACK);//��ʾ��ʾ	
 	tempy=gui_phy.tbheight+h1+h2+gui_phy.tbfsize;
 	tmemo=memo_creat(w1,tempy,memow,memoh,0,1,gui_phy.tbfsize,2000);		//����2000���ֽڵ��ڴ�. 
	t9=t9_creat((lcddev.width%5)/2,lcddev.height-t9height,lcddev.width-(lcddev.width%5),t9height,0); 
	tempy=gui_phy.tbheight+h1+h2+gui_phy.tbfsize+(memoh-btnh)/2;
 	encbtn=btn_creat(w1*2+memow,tempy,btnw,btnh,0,0); 
	qrx=gui_memin_malloc(sizeof(_qr_encode));	
	if(!t9||!encbtn||!tmemo||!qrx)rval=1;//�ڴ�����ʧ��  
 	if(rval==0)
	{
		encbtn->caption=qr_enc_tbl[gui_phy.language];
		encbtn->font=cbtnfsize;
		t9_draw(t9);				//��T9���뷨
		btn_draw(encbtn);			//����ť
		memo_draw_memo(tmemo,0);	//������memo��
		while(1)
		{					  
 			if(system_task_return)break;			//TPAD����
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ    
			memo_check(tmemo,&in_obj);   
			t9_check(t9,&in_obj);		   
			if(t9->outstr[0]!=NULL)//����ַ�
			{  
				memo_add_text(tmemo,t9->outstr);  
				t9->outstr[0]=NULL;//�������ַ� 
			} 		
			res=btn_check(encbtn,&in_obj);   
			if(res&&((encbtn->sta&(1<<7))==0)&&(encbtn->sta&(1<<6)))//������,�а����������ɿ�,����TP�ɿ���
			{   
				textlen=strlen((char*)tmemo->text);
				rval=0;
				if(textlen)
				{
					EncodeData((char *)tmemo->text);
					if(qrx->m_nSymbleSize*2>(lcddev.width-40))rval=2;			//��ʾ��Ϣ2
					else
					{
						for(wpitch=5;wpitch>1;wpitch--)
						{
							if(qrx->m_nSymbleSize*2<(lcddev.width-10*4*wpitch))break;
						}
						wpitch*=10;//�õ���϶��С
						pixsize=(lcddev.width-4*wpitch)/qrx->m_nSymbleSize;		//���С
						if(pixsize>8)pixsize=8;									//��󲻳���8������
						wwidth=pixsize*qrx->m_nSymbleSize+2*wpitch;
						wheight=pixsize*qrx->m_nSymbleSize+2*wpitch+2*cbtnfsize;   
						twin=window_creat((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,0,1|(1<<5),cbtnfsize);//��������
						twin->captionheight=2*cbtnfsize;						//Ĭ�ϸ߶�
						twin->caption=qr_result_tbl[1][gui_phy.language];		//��ʾ����
						twin->font=cbtnfsize;									//���������С
						bkcolorbuf=gui_memex_malloc(wwidth*wheight*2);			//�����ڴ�
						app_read_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//��ȡ����ɫ 
						window_draw(twin);										//��������
 						for(i=0;i<qrx->m_nSymbleSize;i++)
						{
							for(j=0;j<qrx->m_nSymbleSize;j++)
							{ 
								if(qrx->m_byModuleData[i][j]==1)
								{
									gui_fill_rectangle((lcddev.width-wwidth)/2+wpitch+pixsize*i,(lcddev.height-wheight)/2+twin->captionheight+wpitch+pixsize*j,pixsize,pixsize,BLACK); 
								}
							} 
						}
						window_delete(twin);
						while(1)
						{
							if(system_task_return)break;	//TPAD����
							delay_ms(10);
						} 
						app_recover_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//�ָ�����ɫ  				  
						gui_memex_free(bkcolorbuf);			//�ͷ��ڴ� 
						system_task_return=0;
					}						
				}else rval=1;	//��ʾ��Ϣ1
				if(rval)
				{
					bkcolorbuf=gui_memex_malloc(220*100*2);	//�����ڴ�
					app_read_bkcolor((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,bkcolorbuf);//��ȡ����ɫ
					window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)qr_remind_msg_tbl[rval][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
					delay_ms(1000);
					app_recover_bkcolor((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,bkcolorbuf);//�ָ�����ɫ  				  
					gui_memex_free(bkcolorbuf);				//�ͷ��ڴ�
				}
			}
			delay_ms(5);			
		} 
	}		  
	t9_delete(t9);
 	btn_delete(encbtn);
 	memo_delete(tmemo); 
	gui_memin_free(qrx);
}
//��ά�����Ӧ��
void qr_play(void)
{
	u8 rval;
	u8 mode=0;
 	//��ѡ��ģʽ 
	rval=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)qr_mode_tbl[gui_phy.language],2,(u8*)&mode,0X90,(u8*)APP_MODESEL_CAPTION_TBL[gui_phy.language]);//2��ѡ��
	if(rval==0)
	{
		if(mode==0)//ʶ���ά��
		{
			qr_decode_play();
		}else qr_encode_play();//���ɶ�ά��
	}
}





