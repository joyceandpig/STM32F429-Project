#include "qrcodeplay.h"
#include "qr_encode.h"
#include "t9input.h"


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
u8*const qr_result_tbl[GUI_LANGUAGE_NUM]= 
{"������","���a�Y��","Result",};
 
//��ά�����Ӧ��
void qrencode_play(void)
{
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
						wheight=pixsize*qrx->m_nSymbleSize+2*wpitch+WIN_CAPTION_HEIGHT;   
						bkcolorbuf=gui_memex_malloc(wwidth*wheight*2);			//�����ڴ�
						app_read_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//��ȡ����ɫ 
						window_msg_box((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,(u8*)"",(u8*)qr_result_tbl[gui_phy.language],0,0,1<<5,0);
						for(i=0;i<qrx->m_nSymbleSize;i++)
						{
							for(j=0;j<qrx->m_nSymbleSize;j++)
							{ 
								if(qrx->m_byModuleData[i][j]==1)
								{
									gui_fill_rectangle((lcddev.width-wwidth)/2+wpitch+pixsize*i,(lcddev.height-wheight)/2+WIN_CAPTION_HEIGHT+wpitch+pixsize*j,pixsize,pixsize,BLACK); 
								}
							} 
						}
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





