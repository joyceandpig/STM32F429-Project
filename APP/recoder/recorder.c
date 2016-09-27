#include "recorder.h"
#include "calendar.h"
#include "audioplay.h"
#include "wm8978.h"
#include "sai.h"
#include "settings.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-¼���� ����	   
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

u8 *sairecbuf1;
u8 *sairecbuf2; 

//REC¼��FIFO�������.
//����FATFS�ļ�д��ʱ��Ĳ�ȷ����,���ֱ���ڽ����ж�����д�ļ�,���ܵ���ĳ��д��ʱ�����
//�Ӷ��������ݶ�ʧ,�ʼ���FIFO����,�Խ��������.
vu8 sairecfifordpos = 0;	//FIFO��λ��
vu8 sairecfifowrpos = 0;	//FIFOдλ��
u8 *sairecfifobuf[SAI_RX_FIFO_SIZE];//����10��¼������FIFO

FIL* f_rec = 0;		//¼���ļ�	
u32 wavsize;		//wav���ݴ�С(�ֽ���,�������ļ�ͷ!!)
u8 rec_sta = 0;		//¼��״̬
					//[7]:0,û�п���¼��;1,�Ѿ�����¼��;
					//[6:1]:����
					//[0]:0,����¼��;1,��ͣ¼��;
u8 *vubuf;			//vu��buf,ֱ��ָ��sairecbuf1/sairecbuf2
 


#define RECORDER_TITLE_COLOR   	    0XFFFF		//¼����������ɫ	
#define RECORDER_TITLE_BKCOLOR	    0X0000		//¼�������ⱳ��ɫ	

#define RECORDER_VU_BKCOLOR    	    0X39C7		//VU Meter����ɫ	
#define RECORDER_VU_L1COLOR    	    0X07FF		//VU Meter L1ɫ	
#define RECORDER_VU_L2COLOR       	0xFFE0		//VU Meter L2ɫ	
#define RECORDER_VU_L3COLOR    	    0xF800		//VU Meter L3ɫ	

#define RECORDER_TIME_COLOR    			0X07FF		//ʱ����ɫ
#define RECORDER_MAIN_BKCOLOR				0X18E3		//������ɫ

//������Ƕ����ɫ
#define RECORDER_INWIN_FONT_COLOR		0X736C		//0XAD53		


u8*const RECORDER_DEMO_PIC   = "1:/SYSTEM/APP/RECORDER/Demo.bmp";		//demoͼƬ·�� 	      
u8*const RECORDER_RECR_PIC   = "1:/SYSTEM/APP/RECORDER/RecR.bmp";		//¼�� �ɿ�
u8*const RECORDER_RECP_PIC   = "1:/SYSTEM/APP/RECORDER/RecP.bmp";		//¼�� ����
u8*const RECORDER_PAUSER_PIC = "1:/SYSTEM/APP/RECORDER/PauseR.bmp";	//��ͣ �ɿ�
u8*const RECORDER_PAUSEP_PIC = "1:/SYSTEM/APP/RECORDER/PauseP.bmp";	//��ͣ ����
u8*const RECORDER_STOPR_PIC  = "1:/SYSTEM/APP/RECORDER/StopR.bmp";		//ֹͣ �ɿ�
u8*const RECORDER_STOPP_PIC  = "1:/SYSTEM/APP/RECORDER/StopP.bmp";		//ֹͣ ����    

//¼������ѡ���б�
u8*const recorder_setsel_tbl[GUI_LANGUAGE_NUM][2] =
{
{"����������","MIC��������",},
{"������O��","MIC�����O��",},		 
{"Samplerate set","MIC gain set",},
};

//¼����������ʾ��Ϣ��
u8*const recorder_sampleratemsg_tbl[5] = {"8KHz","16Khz","32Khz","44.1Khz","48Khz",};

//¼�������ʱ�
const u16 recorder_samplerate_tbl[5] = {8000,16000,32000,44100,48000};

//¼����ʾ��Ϣ
u8*const recorder_remind_tbl[3][GUI_LANGUAGE_NUM] =
{
{"�Ƿ񱣴��¼���ļ�?","�Ƿ񱣴�ԓ����ļ�?","Do you want to save?"}, 
{"����ֹͣ¼��!","Ո��ֹͣ���!","Please stop REC first!",},	  
{"�ڴ治��!!","�ȴ治��!!","Out of memory!",},	 	 
};	
//00������ѡ������
u8*const recorder_modesel_tbl[GUI_LANGUAGE_NUM] =
{
"¼������","����O��","Recorder Set",
};	

//��ȡ¼��FIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,û�����ݿɶ�;
//      1,������1�����ݿ�
u8 rec_sai_fifo_read(u8 **buf)
{
	if(sairecfifordpos == sairecfifowrpos){
		return 0;
	}
	sairecfifordpos++;		//��λ�ü�1
	if(sairecfifordpos >= SAI_RX_FIFO_SIZE){
		sairecfifordpos=0;//���� 
	}
	*buf = sairecfifobuf[sairecfifordpos];
	return 1;
}
//дһ��¼��FIFO
//buf:���ݻ������׵�ַ
//����ֵ:0,д��ɹ�;
//      1,д��ʧ��
u8 rec_sai_fifo_write(u8 *buf)
{
	u16 i;
	u8 temp = sairecfifowrpos;//��¼��ǰдλ��
	sairecfifowrpos++;		//дλ�ü�1
	if(sairecfifowrpos >= SAI_RX_FIFO_SIZE){
		sairecfifowrpos = 0;//����  
	}
	if(sairecfifordpos == sairecfifowrpos){
		sairecfifowrpos = temp;//��ԭԭ����дλ��,�˴�д��ʧ��
		return 1;	
	}
	for(i = 0; i < SAI_RX_DMA_BUF_SIZE; i++){
		sairecfifobuf[sairecfifowrpos][i] = buf[i];//��������
	}
	return 0;
} 
//¼��SAI DMA RX�ص�����
void rec_sai_dma_rx_callback(void) 
{     
	if(DMA2_Stream5->CR&(1<<19))
	{
		vubuf=sairecbuf1;	//vubufָ��sairecbuf1
		if(rec_sta==0X80)	//¼��ģʽ,����ͣ,��д�ļ�
		{
			rec_sai_fifo_write(sairecbuf1);	//sairecbuf1д��FIFO
		}
	}else 
	{
		vubuf=sairecbuf2;	//vubufָ��sairecbuf2
		if(rec_sta==0X80)	//¼��ģʽ,����ͣ,��д�ļ�
		{
			rec_sai_fifo_write(sairecbuf2);	//sairecbuf2д��FIFO 
		}
	}
		
}  
const u16 saiplaybuf[2]={0X0000,0X0000};//2��16λ����,����¼��ʱSAI Master����.ѭ������0.
//����PCM¼��ģʽ 		  
void recorder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//����ADC
	WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//����BYPASS��� 
	WM8978_MIC_Gain(46);		//MIC�������� 
	app_wm8978_volset(wm8978set.mvol);//����WM8978������� 
	
	WM8978_SPKvol_Set(0);		//�ر�������� 
	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
	
	SAIA_Init(0,1,4);			//SAI1 Block A,������,16λ����
	SAIB_Init(3,1,4);			//SAI1 Block B��ģʽ����,16λ 
	SAIA_TX_DMA_Init((u8*)&saiplaybuf[0],(u8*)&saiplaybuf[1],1,1);	//����TX DMA,16λ
 	DMA2_Stream3->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
	SAIB_RX_DMA_Init(sairecbuf1,sairecbuf2,SAI_RX_DMA_BUF_SIZE/2,1);//����RX DMA

	//����I2S DMA�жϵ����ȼ�,��USB���ȼ����Դ��DMA�ж����ȼ�
//	MY_NVIC_Init(1,0,DMA1_Stream4_IRQn,2);	//��ռ1�������ȼ�0����2 
//	MY_NVIC_Init(1,1,DMA1_Stream3_IRQn,2);	//��ռ1�������ȼ�1����2 
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn,1,0);
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,1,1);
	
  sai_rx_callback=rec_sai_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
 	SAI_Play_Start();	//��ʼSAI���ݷ���(����)
	SAI_Rec_Start(); 	//��ʼSAI���ݽ���(�ӻ�)
}
//ֹͣ¼��ģʽ
void recorder_stop_rec_mode(void)
{
	SAI_Play_Stop();			//ֹͣSAI
	SAI_Rec_Stop(); 
	WM8978_ADDA_Cfg(0,0);		//�ر�DAC&ADC
	WM8978_Input_Cfg(0,0,0);	//�ر�����ͨ��
	WM8978_Output_Cfg(0,0);		//�ر�DAC���   	
	app_wm8978_volset(0);		//�ر�WM8978������� 
} 
//����¼������Ƶ��
//wavhead:wav�ļ�ͷ�ṹ��
//samplerate:8K~192K.
void recoder_set_samplerate(__WaveHeader* wavhead,u16 samplerate)
{
	SAI_Play_Stop(); 
	SAI_Rec_Stop();
	SAIA_SampleRate_Set(samplerate);
 	wavhead->fmt.SampleRate=samplerate;	//������,��λ:Hz
 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*4;//�ֽ�����=������*ͨ����*(ADCλ��/8)
 	SAI_Play_Start();	//��ʼSAI���ݷ���(����)
	SAI_Rec_Start(); 	//��ʼSAI���ݽ���(�ӻ�)
}
//��ʼ��WAVͷ.
//wavhead:wav�ļ�ͷ�ṹ��
void recorder_wav_init(__WaveHeader* wavhead) 	   
{
	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
	wavhead->riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
	wavhead->riff.Format=0X45564157; 	//"WAVE"
	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
	wavhead->fmt.ChunkSize=16; 			//��СΪ16���ֽ�
	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
 	wavhead->fmt.NumOfChannels=2;		//˫����
 	wavhead->fmt.SampleRate=0;			//������,��λ:Hz,������ȷ��
 	wavhead->fmt.ByteRate=0;			//�ֽ�����=������*ͨ����*(ADCλ��/8),����ȷ��
 	wavhead->fmt.BlockAlign=4;			//���С=ͨ����*(ADCλ��/8)
 	wavhead->fmt.BitsPerSample=16;		//16λPCM
  wavhead->data.ChunkID=0X61746164;	//"data"
 	wavhead->data.ChunkSize=0;			//���ݴ�С,����Ҫ����  
} 
//��ƽ��ֵ��
const u16 vu_val_tbl[10]={1200,2400,3600,4800,6000,8000,11000,16000,21000,28000};
//���źŵ�ƽ�õ�vu����ֵ
//signallevel:�źŵ�ƽ
//����ֵ:vuֵ
u8 recorder_vu_get(u16 signallevel)
{
	u8 i;
	for(i=10;i>0;i--)
	{
		if(signallevel>=vu_val_tbl[i-1])break;
	}
	return i;

}
//��ʾVU Meter
//level:0~10;
//x,y:����
void recorder_vu_meter(u16 x,u16 y,u8 level)
{
	u8 i;
	u16 vucolor=RECORDER_VU_L1COLOR;
 	if(level>10)return ;
	if(level==0)
	{
		gui_fill_rectangle(x,y,218,10,RECORDER_VU_BKCOLOR);	//��䱳��ɫ
		return;
	}   
	for(i=0;i<level;i++)
	{
		if(i==9)vucolor=RECORDER_VU_L3COLOR;
		else if(i>5)vucolor=RECORDER_VU_L2COLOR;
		gui_fill_rectangle(x+22*i,y,20,10,vucolor);	//��䱳��ɫ
	}
	if(level<10)gui_fill_rectangle(x+level*22,y,218-level*22,10,RECORDER_VU_BKCOLOR);	//��䱳��ɫ	 
}   
//��ʾ¼��ʱ��
//��ʾ�ߴ�Ϊ:150*60
//x,y:��ַ
//tsec:������.
void recorder_show_time(u16 x,u16 y,u32 tsec)
{
	u8 min;
	if(tsec>=60*100)min=99;
	else min=tsec/60;	 
	gui_phy.back_color=RECORDER_MAIN_BKCOLOR;
	gui_show_num(x,y,2,RECORDER_TIME_COLOR,60,min,0X80);	//XX					   
	gui_show_ptchar(x+60,y,lcddev.width,lcddev.height,0,RECORDER_TIME_COLOR,60,':',0);	//":" 
	gui_show_num(x+90,y,2,RECORDER_TIME_COLOR,60,tsec%60,0X80);	//XX	    	    
} 
//��ʾ����
//x,y:����(��Ҫ��0��ʼ)
//name:����
void recorder_show_name(u16 x,u16 y,u8 *name)
{
 	gui_fill_rectangle(x-1,y-1,lcddev.width,13,RECORDER_MAIN_BKCOLOR);	//��䱳��ɫ
 	gui_show_ptstrwhiterim(x,y,lcddev.width,y+12,0,BLACK,WHITE,12,name); 	  
}
//��ʾ������
//x,y:����(��Ҫ��0��ʼ)
//samplerate:������
void recorder_show_samplerate(u16 x,u16 y,u16 samplerate)
{
	u8 *buf=0;
	float temp;
	temp=(float)samplerate/1000; 
	buf=gui_memin_malloc(60);//�����ڴ�
	if(buf==0)return;
	if(samplerate%1000)sprintf((char*)buf,"%.1fKHz",temp);//��С����
	else sprintf((char*)buf,"%dKHz",samplerate/1000);  
 	gui_fill_rectangle(x,y,42,12,RECORDER_MAIN_BKCOLOR);//��䱳��ɫ
	gui_show_string(buf,x,y,42,12,12,RECORDER_INWIN_FONT_COLOR);//��ʾagc  	  
	gui_memin_free(buf);//�ͷ��ڴ� 
}
//����¼����������UI	  
void recorder_load_ui(void)
{	
 	gui_fill_rectangle(0,0,lcddev.width,gui_phy.tbheight,RECORDER_TITLE_BKCOLOR);				//��䱳��ɫ
	gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,RECORDER_TITLE_COLOR,gui_phy.tbfsize,(u8*)APP_MFUNS_CAPTION_TBL[11][gui_phy.language]);//��ʾ����
 	gui_fill_rectangle(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight,RECORDER_MAIN_BKCOLOR);//����ɫ 
 	minibmp_decode((u8*)RECORDER_DEMO_PIC,(lcddev.width-100)/2,100+(lcddev.height-320)/2,100,100,0,0);//����100*100��ͼƬDEMO
	recorder_vu_meter((lcddev.width-218)/2,(lcddev.height-320)/2+200+5,10);				//��ʾvu meter;
	app_gui_tcbar(0,lcddev.height-gui_phy.tbheight,lcddev.width,gui_phy.tbheight,0x01);	//�Ϸֽ���
}

//ͨ��ʱ���ȡ�ļ���
//������SD��/U�̱���,��֧��FLASH DISK����
//��ϳ�:����"0:RECORDER/REC20120321210633.wav"/"2:RECORDER/REC20120321210633.wav"���ļ���
void recorder_new_pathname(u8 *pname)
{	 
	calendar_get_time(&calendar);
	calendar_get_date(&calendar); 
	if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:RECORDER/REC%04d%02d%02d%02d%02d%02d.wav",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//��ѡ������SD��
	else if(gui_phy.memdevflag&(1<<3))sprintf((char*)pname,"3:RECORDER/REC%04d%02d%02d%02d%02d%02d.wav",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD��������,�򱣴���U��	
}
//��ʾAGC��С
//x,y:����
//agc:����ֵ 0~63,��Ӧ-12dB~35.25dB,0.75dB/Step
void recorder_show_agc(u16 x,u16 y,u8 agc)
{
	u8 *buf;
	float temp;
	buf=gui_memin_malloc(60);//�����ڴ�
	if(buf==0)return;
	temp=agc*0.75;
	temp-=12;
	sprintf((char*)buf,"%.2fdB",temp); 
	gui_phy.back_color=APP_WIN_BACK_COLOR;					//���ñ���ɫΪ��ɫ
	gui_fill_rectangle(x,y,48,12,APP_WIN_BACK_COLOR);		//��䱳��ɫ  
	gui_show_string(buf,x,y,48,12,12,RECORDER_INWIN_FONT_COLOR);//��ʾagc 
	gui_memin_free(buf);//�ͷ��ڴ�
}	
//agc���ý���.�̶��ߴ�:180*122    
//x,y:���Ͻ�����
//agc:�Զ�����ָ��,��Χ:0~63,��Ӧ-12dB~35.25dB,0.75dB/Step
//caption:��������				  
//����ֵ:0,�ɹ�����;
//    ����,����������
u8 recorder_agc_set(u16 x,u16 y,u8 *agc,u8*caption)
{
 	u8 rval=0,res;
   	_window_obj* twin=0;			//����
 	_btn_obj * rbtn=0;				//ȡ����ť		  
 	_btn_obj * okbtn=0;				//ȷ����ť		  
	_progressbar_obj * agcprgb;		//AGC���ý�����
 	u8 tempagc=*agc;

  	twin=window_creat(x,y,180,122,0,1|1<<5,16);			//�������� ��
 	agcprgb=progressbar_creat(x+10,y+52,160,15,0X20);	//����������
	if(agcprgb==NULL)rval=1;
  	okbtn=btn_creat(x+20,y+82,60,30,0,0x02);		//������ť
 	rbtn=btn_creat(x+20+60+20,y+82,60,30,0,0x02);	//������ť
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
		agcprgb->totallen=63;		//���AGCΪ63
	   	agcprgb->curpos=tempagc;	//��ǰ�ߴ� 
		rbtn->caption=(u8*)GUI_CANCEL_CAPTION_TBL[gui_phy.language];	//����Ϊȡ��
 		okbtn->caption=(u8*)GUI_OK_CAPTION_TBL[gui_phy.language];		//����Ϊȷ��
  		window_draw(twin);				//��������
		btn_draw(rbtn);					//����ť
		btn_draw(okbtn);				//����ť
		progressbar_draw_progressbar(agcprgb);
   		gui_show_string("AGC:",x+10,y+38,24,12,12,RECORDER_INWIN_FONT_COLOR);//��ʾSIZE
		recorder_show_agc(x+10+24,y+38,tempagc);
 		while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
			if(system_task_return)break;			//TPAD����
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
			res=progressbar_check(agcprgb,&in_obj);
 			if(res&&(tempagc!=agcprgb->curpos))	//�������Ķ���
			{
				tempagc=agcprgb->curpos;		//�������µĽ��  
 				recorder_show_agc(x+10+24,y+38,tempagc);
				WM8978_MIC_Gain(tempagc); 		//��������
			} 		
 		}
	}
	window_delete(twin);							//ɾ������
	btn_delete(rbtn);								//ɾ����ť	
	progressbar_delete(agcprgb);//ɾ��������  
	system_task_return=0;		 
	if(rval==0XFF)
	{
		*agc=tempagc;
		return 0;
 	}
	return rval;
}

//¼����
//����¼���ļ�,��������SD��RECORDER�ļ�����.
u8 recorder_play(void)
{
	u8 res;
	u8 rval=0;
	__WaveHeader *wavhead=0;
	u8 *pname=0; 
	u8 *pdatabuf;
 	_btn_obj * rbtn=0;				//ȡ����ť		  
 	_btn_obj * mbtn=0;		//ѡ�ť	
 	_btn_obj * recbtn=0;	//¼����ť		  
 	_btn_obj * stopbtn=0;	//ֹͣ¼����ť  

 	u16 *pset_bkctbl=0;		//����ʱ����ɫָ��
	u32 recsec=0;			//¼��ʱ��
	
	u8 timecnt=0;
	u8 vulevel=0;
	short*tempvubuf;
	short tempval;
	u16 maxval=0;
	u16 i;
   
	u8 recspd=1;			//¼��������ѡ��,Ĭ��ѡ��16Khz¼��
	u8 recagc=42;			//Ĭ������Ϊ46*0.75-12dB 
	
	sairecbuf1=gui_memin_malloc(SAI_RX_DMA_BUF_SIZE);	//SAI¼���ڴ�1����
	sairecbuf2=gui_memin_malloc(SAI_RX_DMA_BUF_SIZE);	//SAI¼���ڴ�2���� 
	for(i=0;i<SAI_RX_FIFO_SIZE;i++)
	{
		sairecfifobuf[i]=mymalloc(SRAMIN,SAI_RX_DMA_BUF_SIZE);//SAI¼��FIFO�ڴ�����
		if(sairecfifobuf[i]==NULL)break;				//����ʧ��
	}	
  	f_rec=(FIL *)gui_memin_malloc(sizeof(FIL));			//����FIL�ֽڵ��ڴ�����  
 	wavhead=(__WaveHeader*)gui_memin_malloc(sizeof(__WaveHeader));//����__WaveHeader�ֽڵ��ڴ����� 
	pname=gui_memin_malloc(60);							//����30���ֽ��ڴ�,����"0:RECORDER/REC20120321210633.wav" 
	pset_bkctbl=gui_memex_malloc(180*272*2);			//Ϊ����ʱ�ı���ɫ�������ڴ�
	if(!sairecbuf1|!sairecbuf2||!f_rec||!wavhead||!pname||!pset_bkctbl||i!=SAI_RX_FIFO_SIZE)rval=1;
 	else
	{
		//��������
		res=f_open(f_rec,(const TCHAR*)APP_ASCII_S6030,FA_READ);//���ļ���
		if(res==FR_OK)
		{
			asc2_s6030=(u8*)gui_memex_malloc(f_rec->obj.objsize);	//Ϊ�����忪�ٻ����ַ
			if(asc2_s6030==0)rval=1;
			else 
			{
				res=f_read(f_rec,asc2_s6030,f_rec->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
 			}
		} 
		if(res)rval=1; 
		recorder_load_ui();						//װ��������
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
		mbtn=btn_creat(0,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
		recbtn=btn_creat((lcddev.width-96)/3,(lcddev.height-320)/2+215+18,48,48,0,1);	//����ͼƬ��ť
		stopbtn=btn_creat((lcddev.width-96)*2/3+48,(lcddev.height-320)/2+215+18,48,48,0,1);//����ͼƬ��ť
		if(!rbtn||!mbtn||!recbtn||!stopbtn)rval=1;//û���㹻�ڴ湻����	
		else
		{																				
			rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//���� 
			rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
			rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
			rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
			
			mbtn->caption=(u8*)GUI_OPTION_CAPTION_TBL[gui_phy.language];//���� 
			mbtn->font=gui_phy.tbfsize;//�����µ������С		 
			mbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
			mbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
			
			recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
			recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;
			recbtn->bcfucolor=0X0001;//����䱳��
			recbtn->bcfdcolor=0X0001;//����䱳��
			recbtn->sta=0;	 
			
			stopbtn->picbtnpathu=(u8*)RECORDER_STOPR_PIC;
			stopbtn->picbtnpathd=(u8*)RECORDER_STOPP_PIC;
			stopbtn->bcfucolor=0X0001;//����䱳��
			stopbtn->bcfdcolor=0X0001;//����䱳��
			recbtn->sta=0;	 
		} 
	}
	if(rval==0)
	{    
		if(gui_phy.memdevflag&(1<<0))f_mkdir("0:RECORDER");	//ǿ�ƴ����ļ���,��¼������
		if(gui_phy.memdevflag&(1<<3))f_mkdir("3:RECORDER");	//ǿ�ƴ����ļ���,��¼������
		btn_draw(rbtn);	
		btn_draw(mbtn);	
		btn_draw(recbtn);
		recbtn->picbtnpathu=(u8*)RECORDER_PAUSER_PIC;
		recbtn->picbtnpathd=(u8*)RECORDER_RECP_PIC;	  
		btn_draw(stopbtn);
		if(audiodev.status&(1<<7))		//��ǰ�ڷŸ�??����ֹͣ
		{
			audio_stop_req(&audiodev);	//ֹͣ��Ƶ����
			audio_task_delete();		//ɾ�����ֲ�������.
		}
		tempvubuf=0;
		rec_sta=0;
		recsec=0;
		wavsize=0;
		recorder_enter_rec_mode();		//����¼��ģʽ,������AGC
		recoder_set_samplerate(wavhead,recorder_samplerate_tbl[recspd]);//���ò����� 
		recorder_show_samplerate((lcddev.width-218)/2,(lcddev.height-320)/2+200+5-15,recorder_samplerate_tbl[recspd]);//��ʾ������
		WM8978_MIC_Gain(recagc); 		//��������
  		recorder_show_time((lcddev.width-150)/2,40+(lcddev.height-320)/2,recsec);//��ʾʱ��
 	   	while(rval==0)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);			//�õ�������ֵ   
			if(rec_sai_fifo_read(&pdatabuf)&&(rec_sta&0X80))//��ȡһ������,����������,д���ļ�
			{
				res=f_write(f_rec,pdatabuf,SAI_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
				if(res)
				{
					printf("write error:%d\r\n",res);
				}
				wavsize+=SAI_RX_DMA_BUF_SIZE;				
			}else 
			{
				timecnt++;
				if((timecnt%20)==0)
				{	
					tempvubuf=(short*)vubuf;
					for(i=0;i<512;i++)//ȡǰ512��������������ֵ
					{
						tempval=tempvubuf[i];
						if(tempval<0)tempval=-tempval;
						if(maxval<tempval)maxval=tempval;//ȡ���ֵ	  
					}
					tempval=recorder_vu_get(maxval);
					if(tempval>vulevel)vulevel=tempval;
					else if(vulevel)vulevel--;
					recorder_vu_meter((lcddev.width-218)/2,(lcddev.height-320)/2+200+5,vulevel);//��ʾvu meter;
					maxval=0;			
				} 
				delay_ms(1000/OS_TICKS_PER_SEC);	//��ʱһ��ʱ�ӽ���
				if(recsec!=(wavsize/wavhead->fmt.ByteRate))	//¼��ʱ����ʾ
				{
					recsec=wavsize/wavhead->fmt.ByteRate;	//¼��ʱ��
					recorder_show_time((lcddev.width-150)/2,40+(lcddev.height-320)/2,recsec);//��ʾʱ��
				}
 			}
			if(system_task_return)break;			//TPAD����
			res=btn_check(rbtn,&in_obj); 			//��鷵�ذ�ť
			if(res&&((rbtn->sta&(1<<7))==0)&&(rbtn->sta&(1<<6)))break;//���ذ�ť
  			res=btn_check(mbtn,&in_obj); 			//������ð�ť
			if(res&&((mbtn->sta&(1<<7))==0)&&(mbtn->sta&(1<<6)))
			{   
				app_read_bkcolor((lcddev.width-180)/2,(lcddev.height-272)/2,180,272,pset_bkctbl);//��ȡ����ɫ
				res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)recorder_setsel_tbl[gui_phy.language],2,(u8*)&rval,0X90,(u8*)recorder_modesel_tbl[gui_phy.language]);//2��ѡ��				
				if(res==0)
				{
					app_recover_bkcolor((lcddev.width-180)/2,(lcddev.height-272)/2,180,272,pset_bkctbl);//�ָ�����ɫ  				  
					if(rval==0)	//���ò�����
					{
						if(rec_sta&0X80)//����¼��?��ʾ�û������ֹ¼�������ò�����
						{
							window_msg_box((lcddev.width-180)/2,(lcddev.height-80)/2,180,80,(u8*)recorder_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
							delay_ms(1500);
						}else
						{
							res=app_items_sel((lcddev.width-180)/2,(lcddev.height-272)/2,180,72+40*5,(u8**)recorder_sampleratemsg_tbl,5,(u8*)&recspd,0X90,(u8*)recorder_setsel_tbl[gui_phy.language][0]);//��ѡ
							if(res==0)recoder_set_samplerate(wavhead,recorder_samplerate_tbl[recspd]);//���ò����� 
						}
					}else		//����AGC����
					{
						res=recorder_agc_set((lcddev.width-180)/2,(lcddev.height-122)/2,&recagc,(u8*)recorder_setsel_tbl[gui_phy.language][1]);//����AGC
						WM8978_MIC_Gain(recagc); //��������			
					} 
				}
				rval=0;//�ָ�rval��ֵ
				app_recover_bkcolor((lcddev.width-180)/2,(lcddev.height-272)/2,180,272,pset_bkctbl);//�ָ�����ɫ  				  
				recorder_show_samplerate((lcddev.width-218)/2,(lcddev.height-320)/2+200+5-15,recorder_samplerate_tbl[recspd]);//��ʾ������
			}
  			res=btn_check(recbtn,&in_obj); 			//���¼����ť
			if(res&&((recbtn->sta&(1<<7))==0)&&(recbtn->sta&(1<<6)))
			{  
				if(rec_sta&0X01)//ԭ������ͣ,����¼��
				{
					rec_sta&=0XFE;//ȡ����ͣ
					recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;
				}else if(rec_sta&0X80)//�Ѿ���¼����,��ͣ
				{
					rec_sta|=0X01;//��ͣ
					recbtn->picbtnpathu=(u8*)RECORDER_PAUSER_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_RECP_PIC;
				}else	//��û��ʼ¼�� 
				{
 					rec_sta|=0X80;	//��ʼ¼��
					wavsize=0;		//�ļ���С����Ϊ0
					recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;	 	 
			 		pname[0]='\0';					//��ӽ�����
					recorder_new_pathname(pname);	//�õ��µ�����
					recorder_show_name(2,gui_phy.tbheight+4,pname);//��ʾ����
			 		recorder_wav_init(wavhead);		//��ʼ��wav����	
					recoder_set_samplerate(wavhead,recorder_samplerate_tbl[recspd]);//���ò����� 
					res=f_open(f_rec,(const TCHAR*)pname, FA_CREATE_ALWAYS|FA_WRITE); 
					if(res)//�ļ�����ʧ��
					{
						rec_sta=0;//�����ļ�ʧ��,����¼��
						rval=0XFE;//��ʾ�Ƿ����SD��
					}else res=f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
				}								  
			}
  			res=btn_check(stopbtn,&in_obj); 			//���ֹͣ��ť
			if(res&&((recbtn->sta&(1<<7))==0)&&(recbtn->sta&(1<<6)))
			{
				if(rec_sta&0X80)//��¼��
				{ 
					wavhead->riff.ChunkSize=wavsize+36;	//�����ļ��Ĵ�С-8;
			   		wavhead->data.ChunkSize=wavsize;	//���ݴ�С
					f_lseek(f_rec,0);							//ƫ�Ƶ��ļ�ͷ.
			  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
					f_close(f_rec);
					wavsize=0;

					recbtn->picbtnpathu=(u8*)RECORDER_RECR_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_PAUSEP_PIC;
					btn_draw(recbtn);	
					recbtn->picbtnpathu=(u8*)RECORDER_PAUSER_PIC;
					recbtn->picbtnpathd=(u8*)RECORDER_RECP_PIC;	  
				}
				rec_sta=0;
				recsec=0;
				recorder_show_name(2,gui_phy.tbheight+4,"");				//��ʾ����
				recorder_show_time((lcddev.width-150)/2,40+(lcddev.height-320)/2,recsec);//��ʾʱ��
  			} 
		}	
		recorder_stop_rec_mode();//ֹͣ¼��		
	}else
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)recorder_remind_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//�ڴ����
		delay_ms(2000);
	}	
	if(rval==0XFE)				//�����ļ�ʧ����,��Ҫ��ʾ�Ƿ����SD��
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)APP_CREAT_ERR_MSG_TBL[gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//��ʾSD���Ƿ����
		delay_ms(2000);			//�ȴ�2����
 	}
	if(rec_sta&0X80)			//�������¼��,����ʾ�������¼���ļ�
	{
 		res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)recorder_remind_tbl[0][gui_phy.language],12,0,0X03,0);
		if(res==1)//��Ҫ����
		{ 
			wavhead->riff.ChunkSize=wavsize+36;	//�����ļ��Ĵ�С-8;
	   		wavhead->data.ChunkSize=wavsize;	//���ݴ�С
			f_lseek(f_rec,0);					//ƫ�Ƶ��ļ�ͷ.
	  		f_write(f_rec,(const void*)wavhead,sizeof(__WaveHeader),&bw);//д��ͷ����
			f_close(f_rec);
		}
	} 
	//�ͷ��ڴ�
	gui_memin_free(sairecbuf1);	  
	gui_memin_free(sairecbuf2);	
	for(i=0;i<SAI_RX_FIFO_SIZE;i++)myfree(SRAMIN,sairecfifobuf[i]);//SAI¼��FIFO�ڴ��ͷ�
 	gui_memin_free(f_rec);
	gui_memin_free(wavhead);  
	gui_memin_free(pname);  
	gui_memex_free(pset_bkctbl);
	gui_memex_free(asc2_s6030);
	asc2_s6030=0;				//����
	btn_delete(rbtn);	 
	btn_delete(mbtn);	 
	btn_delete(recbtn);	 
	btn_delete(stopbtn);	 
	return rval;
}
			   			   


































