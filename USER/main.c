#include "includes.h" 
#include "user_config.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "sdram.h"
#include "malloc.h"
#include "touch.h"
#include "lcd.h"
#include "usmart.h"
#include "debug.h"
#include "24cxx.h"
#include "w25qxx.h"

#include "text.h"

#include "nand.h"
#include "ftl.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbd_msc_bot.h"

#include "spblcd.h"
#include "spb.h"
#include "piclib.h"
#include "gui.h"
#include "RTC.h"
#include "key.h"
#include "mpu9250.h"

#include "common.h"
#include "calendar.h" 
#include "settings.h"
#include "ebook.h"
#include "ledplay.h"
#include "gradienter.h"
#include "audioplay.h"
#include "picviewer.h"
#include "videoplay.h"
#include "notepad.h"
#include "exeplay.h"
#include "paint.h"
#include "camera.h"
#include "recorder.h"
#include "calculator.h"
#include "qrplay.h"
#include "webcamera.h"
#include "gyroscope.h"
#include "facereco.h"
#include "keyplay.h"
#include "appplay.h"
#include "smsplay.h"
#include "phoneplay.h"


#include "usb_app.h"
#include "usbplay.h"

enum 
{
	ebook_app = 0,
	picviewer_app,
	audio_app,
	video_app,
	calendar_app,
	sysset_app,
	notepad_app,
	exe_app,
	paint_app,
	camera_app,
	recorder_app,
	usb_app,
	net_app,
	calc_app,
	qr_app,
	webcam_app,
	frec_app,
	gyro_app,
	grad_app,
	key_app,
	led_app,
	
	phone_app = SPB_ICOS_NUM,
	AppCenter = SPB_ICOS_NUM + 1,
	msm_app   = SPB_ICOS_NUM + 2,
};

USB_OTG_CORE_HANDLE USB_OTG_dev;


void sys_reset(void)
{//�����λ
	u_printf(INFO,"System will Reset.\r\n");
	NVIC_SystemReset();
  __DSB();  
	while (1);
}

unsigned long int os_time_get(void)
{
	return 0;
}

#if 0
int main(void)
{	
	Stm32_Clock_Init(384,25,2,8);   //����ʱ��,192Mhz   
	delay_init(192);                //��ʼ����ʱ����
	uart_init(115200);              //��ʼ��USART
	LED_Init();                     //��ʼ��LED 
	SDRAM_Init();                   //SDRAM��ʼ��
 	my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//���W25Q128����
	MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);			//�����ڴ�SRAMIN
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);		    
	while(1)��
}

#else
//����������   
void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//����
 	POINT_COLOR=RED;	//��������Ϊ��ɫ 
	LCD_ShowString(10,10,200,16,16,"Apollo STM32F4/F7");	
	LCD_ShowString(10,30,200,16,16,"UCOSII TEST3");	
	LCD_ShowString(10,50,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(10,75,240,16,16,"TPAD:TMR2 SW   KEY_UP:ADJUST");	
	LCD_ShowString(10,95,240,16,16,"KEY0:DS0 KEY1:Q SW KEY2:CLR");	
 	LCD_DrawLine(0,70,lcddev.width-1,70);
	LCD_DrawLine(150,0,150,70);

 	LCD_DrawLine(0,120,lcddev.width-1,120);
 	LCD_DrawLine(0,220,lcddev.width-1,220);
	LCD_DrawLine(130,120,130,lcddev.height-1);
		    
 	LCD_ShowString(5,125,240,16,16,"QUEUE MSG");//������Ϣ
	LCD_ShowString(5,150,240,16,16,"Message:");	 
	LCD_ShowString(5+130,125,240,16,16,"FLAGS");//�ź�����
	LCD_ShowString(5,225,240,16,16,"TOUCH");	//������
	LCD_ShowString(5+130,225,240,16,16,"TMR2");	//������Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(170,10,200,16,16,"CPU:   %");	
	LCD_ShowString(170,30,200,16,16,"MEM:   %");	
	LCD_ShowString(170,50,200,16,16," Q :000");	
	delay_ms(300);
}
//���߳�
void main_thread(void *pdata);
//LED�߳�
void led_task(void *pdata);
void usb_Task(void *pdata);
void key_thread(void *pdata);
int main(void)
{
   HAL_Init();                     //��ʼ��HAL��  
#if 1	
    Stm32_Clock_Init(480,25,2,10);   //����ʱ��,240Mhz
    delay_init(240);                //��ʼ����ʱ����
#else	
    Stm32_Clock_Init(384,25,2,8);   //����ʱ��,180Mhz
    delay_init(192);                //��ʼ����ʱ����
#endif	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
  uart1_init(115200);              //��ʼ��USART
	usmart_dev.init(96);
	LED_Init();                     //��ʼ��LED
	SDRAM_Init();                   //��ʼ��SDRAM
	LCD_Init();                     //��ʼ��LCD
	KEY_Init();
	AT24CXX_Init();				    //��ʼ��IIC
	W25QXX_Init();				    //W25QXX��ʼ��	
	Adc_Init();					//��ʼ���ڲ��¶ȴ�����  
	
	tp_dev.init();				    //��ʼ��������
	my_mem_init(SRAMIN);     //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	

	printf("mpu init res:%d\r\n",MPU9250_Init());
	gui_init();
	piclib_init();				//piclib��ʼ��
	slcd_dma_init();
	exfuns_init();				//FATFS �����ڴ�
	usbapp_init();
	
	printf("RTC Check...\r\n");			   
 	if(RTC_Init())
	{
		printf("RTC ERROR!\r\n"); //RTC���
	}
	else 
	{
		calendar_get_time(&calendar);//�õ���ǰʱ��
		calendar_get_date(&calendar);//�õ���ǰ����
		printf("RTC OK!\r\n");;			   
	}


	font_init(0);//��ʼ���ֿ�	 0���  1ǿ�Ƹ���
	POINT_COLOR = RED;
	Show_Str(30,50+32*4,300,32,		"F429 Develop Kit.",24,0);				    	 
	Show_Str(30,50+32*4+24*1,300,32,"�ۺϲ�������...",24,0);				    	 				    	 
	Show_Str(30,50+32*4+24*2,300,32,"www.sunsheen.cn",24,0);				    	 
	Show_Str(30,50+32*4+24*3,300,32,"2016��8��25��",24,0);

	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//���NandFlash����
	
//	MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);			//�����ڴ�SRAMIN
//	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);		    
//	Sleep(2000);
	
	f_mount(fs[0],"0:",1); 		//����SD��  
	f_mount(fs[1],"1:",1); 		//����SPI FLASH. 
	f_mount(fs[2],"2:",1); 		//����NAND FLASH. 
	
	gui_phy.memdevflag|=1<<2;	//����NAND FLASH��λ.
	gui_phy.memdevflag|=1<<1;	//����SPI FLASH��λ.
	
	OSInit();                       //UCOS��ʼ��
	thread_create(usb_Task, 0, TASK_USB_PRIO, 0, TASK_USB_STACK_SIZE, "usb_task");
	thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");
	thread_create(key_thread, 0, TASK_KEY_PRIO, 0, TASK_KEY_STACK_SIZE, "key_thread");
	OSStart(); //��ʼ����
	while(1);					
}

//USER TASK

//LED����
void led_task(void *pdata)
{
	u8 t;
	
	while(1)
	{
		usmart_scan();
		t++;
		Sleep(125);
		if(t==4)
		{	
			LED1_OFF();	//LED0��
			LED0_STA_TURN();//��ʾϵͳ����;
		}			
		if(t==8*5)		//LED0��
		{
			t=0;
			LED1_ON();
		}
	}									 
}
void key_thread(void *pdata)
{
	u8 t;
	
	while(1)
	{
		t = KEY_Scan(0);
		if(t == 1)
		{
			system_task_return = 1;
		}
	Sleep(100);
	}									 
}
void ShowPicture(void);
void MPU_Test(void);
void usb_Task(void *pdata)
{	
	u8 tct=0;

	float psin,psex,psccm;
	pdata=pdata;

//	MPU_Test();	
	while(1)
	{
//    Sleep(500);		
OSTimeDlyHMSM(0,0,0,500);		
		tct++;
		if(tct==5)
		{
			tct=0;

//			psin=my_mem_perused(SRAMIN);
//			psex=my_mem_perused(SRAMEX);
//			psccm=my_mem_perused(SRAMCCM);
//			printf("in:%3.1f, ex:%3.1f, ccm:%3.1f, CPU:%d\r\n",psin/10,psex/10,psccm/10,OSCPUUsage);//��ӡ�ڴ�ռ����
		} 
	} 	
}
void picture_task(void *pdata)
{
	pdata=pdata;
//	ShowPicture();	//��ʾͼƬ
	while(1)
	{
//		if(usbx.mode==USBH_MSC_MODE)			//U��ģʽ,�Ŵ���
//		{
//			u_printf(DBG,"USB USBH_MSC_MODE\r\n");
//			while((usbx.bDeviceState&0XC0)==0X40)//USB�豸������,���ǻ�û���ӳɹ�,�Ͳ�ѯ.
//			{
//				usbapp_pulling();	//��ѯ����USB����
//				delay_us(1000);		//������HID��ô��...,U�̱Ƚ���
//			}
//			u_printf(DBG,"USB After query\r\n");
//			usbapp_pulling();//����USB����
//		}
		Sleep(1000);
	}
}

vu8 system_task_return;		//����ǿ�Ʒ��ر�־.
vu8 ledplay_ds0_sta=0;		//ledplay����,DS0�Ŀ���״̬
extern OS_EVENT * audiombox;	//��Ƶ������������
//���߳�
void main_thread(void *pdata)
{

	pdata=pdata;
	u8 selx; 
	u16 tcnt=0;
	
	OSStatInit();  //����ͳ������
	audiombox=OSMboxCreate((void*) 0);	//��������
	thread_create(led_task, 0, TASK_LED_PRIO, 0, TASK_LED_STACK_SIZE, "led_task");
	thread_create(picture_task, 0, TASK_PICTURE_PRIO, 0, TASK_PICTURE_STACK_SIZE, "picture_task");
	
	spb_init();			//��ʼ��SPB����
	spb_load_mui();		//����SPB������
	slcd_frame_show(spbdev.spbahwidth);	//��ʾ����
	while(1)
	{
		selx=spb_move_chk(); 
		system_task_return=0;//���˳���־ 
		switch(selx)//������˫���¼�
		{    
			case ebook_app		:ebook_play();		break;//����ͼ�� 
 			case picviewer_app	:picviewer_play();	break;//�������  
 			case audio_app		:audio_play();		break;//���ֲ��� 
 			case video_app		:video_play();		break;//��Ƶ����
			case calendar_app	:calendar_play();	break;//ʱ�� 
 			case sysset_app		:sysset_play();		break;//ϵͳ����
			case notepad_app	:notepad_play();	break;//���±�	
			case exe_app			:exe_play();		break;//������
			case paint_app		:paint_play();		break;//��д����
 			case camera_app		:camera_play();		break;//����ͷ
			case recorder_app	:recorder_play();	break;//¼����
 			case usb_app		:usb_play();		break;//USB����
 	    case net_app		:net_play();		break;//�������
			case calc_app		:calc_play();		break;//������   
			case qr_app			:qr_play();			break;//��ά��
 			case webcam_app		:webcam_play();		break;//��������ͷ
			case frec_app		:frec_play();		break;//����ʶ��
			case gyro_app		:gyro_play();		break;//9�ᴫ����
			case grad_app		:grad_play();		break;//ˮƽ��
			case key_app		:key_play();		break;//��������
			case led_app		:led_play();		break;//led����

			case phone_app:phone_play();break;	//�绰����
			case AppCenter:app_play();break;	//APP 
 			case msm_app:sms_play();break;	//���Ź���
		} 
		
		if(selx!=0XFF)spb_load_mui();//��ʾ������
		delay_ms(10000/OS_TICKS_PER_SEC);//��ʱһ��ʱ�ӽ���
		tcnt++;
		if(tcnt==50)	//500������Ϊ1����
		{
			tcnt=0;
			spb_stabar_msg_show(0);//����״̬����Ϣ
		}
	}
}


#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"

//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO *tfileinfo;	//��ʱ�ļ���Ϣ	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMEX,sizeof(FILINFO));//�����ڴ�SRAMIN
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼ 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//������/��ĩβ��,�˳�	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	}  
	myfree(SRAMEX,tfileinfo);//�ͷ��ڴ�SRAMIN
	return rval;
}

void ShowPicture(void)
{
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO *picfileinfo;//�ļ���Ϣ 
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//ͼƬ�ļ�offset������ 

	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
// 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.
 	f_mount(fs[2],"2:",1); 		//����NAND FLASH.
	
	font_init(0);
	
	
	POINT_COLOR=RED;      
 	while(f_opendir(&picdir,"2:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		u_printf(DBG,"DIR is Error!");			  	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("2:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		u_printf(DBG,"0 Picture!");
		delay_ms(200);			  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMEX,sizeof(FILINFO));	//�����ڴ�SRAMIN
 	pname=mymalloc(SRAMEX,_MAX_LFN*2+1);					//Ϊ��·�����ļ��������ڴ�SRAMIN
 	picoffsettbl=mymalloc(SRAMEX,4*totpicnum);					//����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����SRAMIN
 	while(!picfileinfo||!pname||!picoffsettbl)					//�ڴ�������
 	{	    	
		u_printf(DBG,"mem is malloc error!");     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"2:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.dptr;								//��¼��ǰdptrƫ��
	        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picoffsettbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"Start...",16,0); 
	delay_ms(1500);
	piclib_init();										//��ʼ����ͼ	   	   
	curindex=0;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"2:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�
		strcpy((char*)pname,"2:/PICTURE/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)picfileinfo->fname);//���ļ������ں���
#if 0			
 		LCD_Clear(BLACK); //���ȫ��		
		POINT_COLOR=RED;		
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//ȫ����ʾͼƬ 
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//��ʾͼƬ����
#else 			
		LCD_Fill(0,103,lcddev.width-1,lcddev.height-1,BLACK);
		POINT_COLOR=RED;		
		Show_Str(2,110,lcddev.width,16,pname,16,1); 				//��ʾͼƬ����
 		ai_load_picfile(pname,0,150,lcddev.width,lcddev.height-150-30,1);//������ʾͼƬ    		
#endif		
		t=0;
		while(1) 
		{
			key=0;		//ɨ�谴��
			if(t>250)key=1;			//ģ��һ�ΰ���KEY0    
//			if((t%20)==0)LED0=!LED0;//LED0��˸,��ʾ������������.
			if(key==3)		//��һ��
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==1)//��һ��
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
				break;
			}
			t++;
			delay_ms(20); 
		}				
		res=0; 	
	} 							    
	myfree(SRAMEX,picfileinfo);			//�ͷ��ڴ�SRAMIN						   		    
	myfree(SRAMEX,pname);				//�ͷ��ڴ�	SRAMIN		    
	myfree(SRAMEX,picoffsettbl);			//�ͷ��ڴ�	SRAMIN
}
#endif
