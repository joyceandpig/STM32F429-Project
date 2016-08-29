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
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况

void sys_reset(void)
{//软件复位
	u_printf(INFO,"System well Reset.\r\n");
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
	MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);			//申请内存
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
int main(void)
{
   HAL_Init();                     //初始化HAL库  
#if 1	
    Stm32_Clock_Init(480,25,2,10);   //设置时钟,180Mhz
    delay_init(240);                //初始化延时函数
#else	
    Stm32_Clock_Init(384,25,2,8);   //设置时钟,180Mhz
    delay_init(192);                //初始化延时函数
#endif	
  uart_init(115200);              //初始化USART
	usmart_dev.init(96);
	LED_Init();                     //初始化LED
	SDRAM_Init();                   //初始化SDRAM
	LCD_Init();                     //初始化LCD
	AT24CXX_Init();				    //初始化IIC
	W25QXX_Init();				    //W25QXX初始化	
	tp_dev.init();				    //初始化触摸屏
	my_mem_init(SRAMIN);            //初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	my_mem_init(SRAMCCM);		//初始化CCM内存池 

	font_init(0);//初始化字库	
	POINT_COLOR = RED;
	Show_Str(30,50+32*4,300,32,		"F429 Develop Kit.",24,0);				    	 
	Show_Str(30,50+32*4+24*1,300,32,"综合测试试验...",24,0);				    	 				    	 
	Show_Str(30,50+32*4+24*2,300,32,"www.sunsheen.cn",24,0);				    	 
	Show_Str(30,50+32*4+24*3,300,32,"2016年8月25日",24,0);

	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//检测W25Q128错误
	MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);			//申请内存
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);		    
	Sleep(5000);
	
	OSInit();                       //UCOS初始化
	thread_create(usb_Task, 0, TASK_USB_PRIO, 0, TASK_USB_STACK_SIZE, "usb_task");
	thread_create(main_thread, 0, TASK_MAIN_PRIO, 0, TASK_MAIN_STACK_SIZE, "main_thread");	
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
		Sleep(5);
		if(t==8)LED1=1;	//LED0灭
		if(t==100)		//LED0亮
		{
			t=0;
			LED1=0;
		}
	}									 
}
void ShowPicture(void);
void MPU_Test(void);
void usb_Task(void *pdata)
{
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;

	pdata=pdata;

 	u_printf(DBG,"USB Connecting...");	//提示正在建立连接 	
	MPU_Test();		    
	while(1)
	{
        Sleep(100);				  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   		  	   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LED1=0;
				u_printf(DBG,"USB Writing...");//提示USB正在写入数据    
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LED1=0;
				u_printf(DBG,"USB Reading...");//提示USB正在读出数据		 
			}	 										  
			if(USB_STATUS_REG&0x04)
				u_printf(DBG,"USB Write Err ");//提示写入错误  
			if(USB_STATUS_REG&0x08)
				u_printf(DBG,"USB Read  Err ");//提示读出错误   
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)
				u_printf(DBG,"USB Connected    ");//提示USB连接已经建立
			else	
				u_printf(DBG,"USB DisConnected ");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED1=1;
			LED0=!LED0;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;
			}else//没有得到轮询 
			{
				offline_cnt++;  				
				if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了 
			}
			USB_STATUS_REG=0;
		} 
	} 	
}
//主线程
void main_thread(void *pdata)
{

	pdata=pdata;

	OSStatInit();  //开启统计任务
	thread_create(led_task, 0, TASK_LED_PRIO, 0, TASK_LED_STACK_SIZE, "led_task");
	ShowPicture();	//显示图片
	while(1)
	{
		Sleep(100);
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
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//申请内存
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
	myfree(SRAMIN,tfileinfo);//释放内存
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
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//申请内存
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//为带路径的文件名分配内存
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);					//申请4*totpicnum个字节的内存,用于存放图片索引
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
			if((t%20)==0)LED0=!LED0;//LED0闪烁,提示程序正在运行.
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
	myfree(SRAMIN,picfileinfo);			//释放内存						   		    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picoffsettbl);			//释放内存	
}
#endif

