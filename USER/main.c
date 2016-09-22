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
{//软件复位
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
	Stm32_Clock_Init(384,25,2,8);   //设置时钟,192Mhz   
	delay_init(192);                //初始化延时函数
	uart_init(115200);              //初始化USART
	LED_Init();                     //初始化LED 
	SDRAM_Init();                   //SDRAM初始化
 	my_mem_init(SRAMIN);		    //初始化内部内存池
	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//检测W25Q128错误
	MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);			//申请内存SRAMIN
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);		    
	while(1)；
}

#else
//加载主界面   
void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//清屏
 	POINT_COLOR=RED;	//设置字体为红色 
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
		    
 	LCD_ShowString(5,125,240,16,16,"QUEUE MSG");//队列消息
	LCD_ShowString(5,150,240,16,16,"Message:");	 
	LCD_ShowString(5+130,125,240,16,16,"FLAGS");//信号量集
	LCD_ShowString(5,225,240,16,16,"TOUCH");	//触摸屏
	LCD_ShowString(5+130,225,240,16,16,"TMR2");	//队列消息
	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(170,10,200,16,16,"CPU:   %");	
	LCD_ShowString(170,30,200,16,16,"MEM:   %");	
	LCD_ShowString(170,50,200,16,16," Q :000");	
	delay_ms(300);
}
//主线程
void main_thread(void *pdata);
//LED线程
void led_task(void *pdata);
void usb_Task(void *pdata);
void key_thread(void *pdata);
int main(void)
{
   HAL_Init();                     //初始化HAL库  
#if 1	
    Stm32_Clock_Init(480,25,2,10);   //设置时钟,240Mhz
    delay_init(240);                //初始化延时函数
#else	
    Stm32_Clock_Init(384,25,2,8);   //设置时钟,180Mhz
    delay_init(192);                //初始化延时函数
#endif	
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
  uart1_init(115200);              //初始化USART
	usmart_dev.init(96);
	LED_Init();                     //初始化LED
	SDRAM_Init();                   //初始化SDRAM
	LCD_Init();                     //初始化LCD
	KEY_Init();
	AT24CXX_Init();				    //初始化IIC
	W25QXX_Init();				    //W25QXX初始化	
	Adc_Init();					//初始化内部温度传感器  
	
	tp_dev.init();				    //初始化触摸屏
	my_mem_init(SRAMIN);     //初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	

	printf("mpu init res:%d\r\n",MPU9250_Init());
	gui_init();
	piclib_init();				//piclib初始化
	slcd_dma_init();
	exfuns_init();				//FATFS 申请内存
	usbapp_init();
	
	printf("RTC Check...\r\n");			   
 	if(RTC_Init())
	{
		printf("RTC ERROR!\r\n"); //RTC检测
	}
	else 
	{
		calendar_get_time(&calendar);//得到当前时间
		calendar_get_date(&calendar);//得到当前日期
		printf("RTC OK!\r\n");;			   
	}


	font_init(0);//初始化字库	 0检查  1强制更新
	POINT_COLOR = RED;
	Show_Str(30,50+32*4,300,32,		"F429 Develop Kit.",24,0);				    	 
	Show_Str(30,50+32*4+24*1,300,32,"综合测试试验...",24,0);				    	 				    	 
	Show_Str(30,50+32*4+24*2,300,32,"www.sunsheen.cn",24,0);				    	 
	Show_Str(30,50+32*4+24*3,300,32,"2016年8月25日",24,0);

	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//检测NandFlash错误
	
//	MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);			//申请内存SRAMIN
//	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);		    
//	Sleep(2000);
	
	f_mount(fs[0],"0:",1); 		//挂载SD卡  
	f_mount(fs[1],"1:",1); 		//挂载SPI FLASH. 
	f_mount(fs[2],"2:",1); 		//挂载NAND FLASH. 
	
	gui_phy.memdevflag|=1<<2;	//设置NAND FLASH在位.
	gui_phy.memdevflag|=1<<1;	//设置SPI FLASH在位.
	
	OSInit();                       //UCOS初始化
	thread_create(usb_Task, 0, TASK_USB_PRIO, 0, TASK_USB_STACK_SIZE, "usb_task");
	thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");
	thread_create(key_thread, 0, TASK_KEY_PRIO, 0, TASK_KEY_STACK_SIZE, "key_thread");
	OSStart(); //开始任务
	while(1);					
}

//USER TASK

//LED任务
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
			LED1_OFF();	//LED0灭
			LED0_STA_TURN();//提示系统在运;
		}			
		if(t==8*5)		//LED0亮
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
//			printf("in:%3.1f, ex:%3.1f, ccm:%3.1f, CPU:%d\r\n",psin/10,psex/10,psccm/10,OSCPUUsage);//打印内存占用率
		} 
	} 	
}
void picture_task(void *pdata)
{
	pdata=pdata;
//	ShowPicture();	//显示图片
	while(1)
	{
//		if(usbx.mode==USBH_MSC_MODE)			//U盘模式,才处理
//		{
//			u_printf(DBG,"USB USBH_MSC_MODE\r\n");
//			while((usbx.bDeviceState&0XC0)==0X40)//USB设备插入了,但是还没连接成功,猛查询.
//			{
//				usbapp_pulling();	//轮询处理USB事务
//				delay_us(1000);		//不能像HID那么猛...,U盘比较慢
//			}
//			u_printf(DBG,"USB After query\r\n");
//			usbapp_pulling();//处理USB事务
//		}
		Sleep(1000);
	}
}

vu8 system_task_return;		//任务强制返回标志.
vu8 ledplay_ds0_sta=0;		//ledplay任务,DS0的控制状态
extern OS_EVENT * audiombox;	//音频播放任务邮箱
//主线程
void main_thread(void *pdata)
{

	pdata=pdata;
	u8 selx; 
	u16 tcnt=0;
	
	OSStatInit();  //开启统计任务
	audiombox=OSMboxCreate((void*) 0);	//创建邮箱
	thread_create(led_task, 0, TASK_LED_PRIO, 0, TASK_LED_STACK_SIZE, "led_task");
	thread_create(picture_task, 0, TASK_PICTURE_PRIO, 0, TASK_PICTURE_STACK_SIZE, "picture_task");
	
	spb_init();			//初始化SPB界面
	spb_load_mui();		//加载SPB主界面
	slcd_frame_show(spbdev.spbahwidth);	//显示界面
	while(1)
	{
		selx=spb_move_chk(); 
		system_task_return=0;//清退出标志 
		switch(selx)//发生了双击事件
		{    
			case ebook_app		:ebook_play();		break;//电子图书 
 			case picviewer_app	:picviewer_play();	break;//数码相框  
 			case audio_app		:audio_play();		break;//音乐播放 
 			case video_app		:video_play();		break;//视频播放
			case calendar_app	:calendar_play();	break;//时钟 
 			case sysset_app		:sysset_play();		break;//系统设置
			case notepad_app	:notepad_play();	break;//记事本	
			case exe_app			:exe_play();		break;//运行器
			case paint_app		:paint_play();		break;//手写画笔
 			case camera_app		:camera_play();		break;//摄像头
			case recorder_app	:recorder_play();	break;//录音机
 			case usb_app		:usb_play();		break;//USB连接
 	    case net_app		:net_play();		break;//网络测试
			case calc_app		:calc_play();		break;//计算器   
			case qr_app			:qr_play();			break;//二维码
 			case webcam_app		:webcam_play();		break;//网络摄像头
			case frec_app		:frec_play();		break;//人脸识别
			case gyro_app		:gyro_play();		break;//9轴传感器
			case grad_app		:grad_play();		break;//水平仪
			case key_app		:key_play();		break;//按键测试
			case led_app		:led_play();		break;//led测试

			case phone_app:phone_play();break;	//电话功能
			case AppCenter:app_play();break;	//APP 
 			case msm_app:sms_play();break;	//短信功能
		} 
		
		if(selx!=0XFF)spb_load_mui();//显示主界面
		delay_ms(10000/OS_TICKS_PER_SEC);//延时一个时钟节拍
		tcnt++;
		if(tcnt==50)	//500个节拍为1秒钟
		{
			tcnt=0;
			spb_stabar_msg_show(0);//更新状态栏信息
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

//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO *tfileinfo;	//临时文件信息	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMEX,sizeof(FILINFO));//申请内存SRAMIN
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,tfileinfo);       		//读取目录下的一个文件  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//错误了/到末尾了,退出	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	}  
	myfree(SRAMEX,tfileinfo);//释放内存SRAMIN
	return rval;
}

void ShowPicture(void)
{
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO *picfileinfo;//文件信息 
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//图片文件offset索引表 

	exfuns_init();				//为fatfs相关变量申请内存  
// 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
 	f_mount(fs[2],"2:",1); 		//挂载NAND FLASH.
	
	font_init(0);
	
	
	POINT_COLOR=RED;      
 	while(f_opendir(&picdir,"2:/PICTURE"))//打开图片文件夹
 	{	    
		u_printf(DBG,"DIR is Error!");			  	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("2:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		u_printf(DBG,"0 Picture!");
		delay_ms(200);			  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMEX,sizeof(FILINFO));	//申请内存SRAMIN
 	pname=mymalloc(SRAMEX,_MAX_LFN*2+1);					//为带路径的文件名分配内存SRAMIN
 	picoffsettbl=mymalloc(SRAMEX,4*totpicnum);					//申请4*totpicnum个字节的内存,用于存放图片索引SRAMIN
 	while(!picfileinfo||!pname||!picoffsettbl)					//内存分配出错
 	{	    	
		u_printf(DBG,"mem is malloc error!");     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"2:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"Start...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"2:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		strcpy((char*)pname,"2:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
#if 0			
 		LCD_Clear(BLACK); //清除全屏		
		POINT_COLOR=RED;		
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//全屏显示图片 
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//显示图片名字
#else 			
		LCD_Fill(0,103,lcddev.width-1,lcddev.height-1,BLACK);
		POINT_COLOR=RED;		
		Show_Str(2,110,lcddev.width,16,pname,16,1); 				//显示图片名字
 		ai_load_picfile(pname,0,150,lcddev.width,lcddev.height-150-30,1);//区域显示图片    		
#endif		
		t=0;
		while(1) 
		{
			key=0;		//扫描按键
			if(t>250)key=1;			//模拟一次按下KEY0    
//			if((t%20)==0)LED0=!LED0;//LED0闪烁,提示程序正在运行.
			if(key==3)		//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==1)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}
			t++;
			delay_ms(20); 
		}				
		res=0; 	
	} 							    
	myfree(SRAMEX,picfileinfo);			//释放内存SRAMIN						   		    
	myfree(SRAMEX,pname);				//释放内存	SRAMIN		    
	myfree(SRAMEX,picoffsettbl);			//释放内存	SRAMIN
}
#endif
