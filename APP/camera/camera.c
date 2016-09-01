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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-照相机 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

extern u8 framecnt;						//统一的帧计数器,在timer.c里面定义
vu8 hsync_int=0;						//帧中断标志
u8 ov_flash=0;							//闪光灯
u8 bmp_request=0;						//bmp拍照请求:0,无bmp拍照请求;1,有bmp拍照请求,需要在帧中断里面,关闭DCMI接口.
u8 ovx_mode=0;							//bit0:0,RGB565模式;1,JPEG模式 
u16 curline=0;							//摄像头输出数据,当前行编号
u16 yoffset=0;							//y方向的偏移量

#define jpeg_buf_size   4*1024*1024		//定义JPEG数据缓存jpeg_buf的大小(4M字节)
#define jpeg_line_size	2*1024			//定义DMA接收数据时,一行数据的最大值

u32 *dcmi_line_buf[2];					//RGB屏时,摄像头采用一行一行读取,定义行缓存  
u32 *jpeg_data_buf;						//JPEG数据缓存buf 

volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
		
///////////////////////////////////////////////////////////////////////////////////
//摄像头提示
u8*const camera_remind_tbl[4][GUI_LANGUAGE_NUM]=
{
{"初始化OV5640,请稍侯...","初始化OV5640,請稍後...","OV5640 Init,Please wait...",},	 
{"未检测到OV5640,请检查...","未檢測到OV5640,請檢查...","No OV5640 find,Please check...",},	 
{"保存为:","保存為:","SAVE AS:",}, 
{"分辨率","分辨率","Resolution",},	 	 
};
//縮放提示
u8*const camera_scalemsg_tbl[2][GUI_LANGUAGE_NUM]=
{
"1:1显示(无缩放)！","1:1顯示(無縮放)！","1:1 Display(No Scale)！",
"全尺寸缩放！","全尺寸縮放！","Full Scale！",
};
u8*const camera_jpegimgsize_tbl[12]=	//JPEG图片 12种尺寸 
{"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};	 
//拍照成功提示框标题
u8*const camera_saveok_caption[GUI_LANGUAGE_NUM]=
{
"拍照成功！","拍照成功！","Take Photo OK！",
}; 		
//拍照失败提示信息
u8*const camera_failmsg_tbl[3][GUI_LANGUAGE_NUM]=
{	                      						 
"创建文件失败,请检查!","創建文件失敗,請檢查!","Creat File Failed,Please check!",
"内存不足!","內存不足!","Out of memory!",
"数据错误(图片尺寸太大)!","數據錯誤(圖片尺寸太大)!","Data Error(Picture is too big)!",
}; 
//00级功能选项表标题
u8*const camera_l00fun_caption[GUI_LANGUAGE_NUM]=
{
"相机设置","相機設置","Camera Set",
};
//00级功能选项表
u8*const camera_l00fun_table[GUI_LANGUAGE_NUM][6]=
{
{"场景设置","特效设置","亮度设置","色度设置","对比度设置","闪光灯设置",},
{"場景設置","特效設置","亮度設置","色度設置","對比度設置","閃光燈設置",},
{"Scene","Effects","Brightness","Saturation","Contrast","Flashlight",},
};	
//////////////////////////////////////////////////////////////////////////	   
//10级功能选项表
//场景模式
u8*const camera_l10fun_table[GUI_LANGUAGE_NUM][5]=
{
{"自动","晴天","办公室","阴天","家庭",},
{"自動","晴天","辦公室","陰天","家庭",},
{"Auto","Sunny","Office","Cloudy","Home"},
};	
//11级功能选项表
//特效设置
u8*const camera_l11fun_table[GUI_LANGUAGE_NUM][7]=
{
{"正常","冷色","暖色","黑白","偏黄","反色","偏绿"},
{"正常","冷色","暖色","黑白","偏黃","反色","偏綠"},
{"Normal","Cool","Warm","W&B","Yellowish","Invert","Greenish",},
};	
//12~14级功能选项表
//都是-3~3的7個值
u8*const camera_l124fun_table[GUI_LANGUAGE_NUM][7]=
{
{"-3","-2","-1","0","+1","+2","+3",},					   
{"-3","-2","-1","0","+1","+2","+3",},					   
{"-3","-2","-1","0","+1","+2","+3",},					   
};	
//15级功能选项表
//闪光灯设置
u8*const camera_l15fun_table[GUI_LANGUAGE_NUM][2]=
{
{"关闭","打开",},
{"關閉","打開",},
{"OFF","ON",},
};
//////////////////////////////////////////////////////////////////////////	  


//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;			//剩余数据长度
	u32 *pbuf;
	if(qr_dcmi_rgbbuf_sta&0X02)			//QR识别模式
	{						
		qr_dcmi_curline=0;				//行数清零   
		qr_dcmi_rgbbuf_sta|=1<<0;		//标记数据准备好了 
		return ;						//直接返回,无需执行剩下的代码		
	}
	curline=yoffset;	//行数清零
	framecnt++;	
	if(ovx_mode&0X01)	//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{
			DMA2_Stream1->CR&=~(1<<0);		//停止当前传输
			while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置 
			rlen=jpeg_line_size-DMA2_Stream1->NDTR;//得到剩余数据长度	
			pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾,继续添加
			if(DMA2_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[1][i];//读取buf1里面的剩余数据
			else for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[0][i];//读取buf0里面的剩余数据 
			jpeg_data_len+=rlen;			//加上剩余长度
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_line_size;//传输长度为jpeg_buf_size*4字节
			DMA2_Stream1->CR|=1<<0;			//重新传输
			jpeg_data_ok=0;					//标记数据未采集
			jpeg_data_len=0;				//数据重新开始
		}
	}else
	{  
		if(bmp_request==1)	//有bmp拍照请求,关闭DCMI
		{
			DCMI_Stop();	//停止DCMI
			bmp_request=0;	//标记请求处理完成.
		} 
		LCD_SetCursor(frec_dev.xoff,frec_dev.yoff);   
		frec_curline=frec_dev.yoff;
		LCD_WriteRAM_Prepare();				//开始写入GRAM   
		hsync_int=1;
	}  
} 
//jpeg数据接收回调函数
void jpeg_dcmi_rx_callback(void)
{  
	u16 i;
	u32 *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾
	if(DMA2_Stream1->CR&(1<<19))//buf0已满,正常处理buf1
	{ 
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[0][i];//读取buf0里面的数据
		jpeg_data_len+=jpeg_line_size;//偏移
	}else //buf1已满,正常处理buf0
	{
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[1][i];//读取buf1里面的数据
		jpeg_data_len+=jpeg_line_size;//偏移 
	} 
} 

//RGB屏数据接收回调函数
void rgblcd_dcmi_rx_callback(void)
{  
	u16 *pbuf;
	if(DMA2_Stream1->CR&(1<<19))//DMA使用buf1,读取buf0
	{ 
		pbuf=(u16*)dcmi_line_buf[0]; 
	}else 						//DMA使用buf0,读取buf1
	{
		pbuf=(u16*)dcmi_line_buf[1]; 
	} 	
	LTDC_Color_Fill(0,curline,lcddev.width-1,curline,pbuf);//DM2D填充 
	if(curline<lcddev.height)curline++;
}
//切换为OV5640模式
void sw_ov5640_mode(void)
{ 
 	OV5640_WR_Reg(0X3017,0XFF);	//开启OV5650输出(可以正常显示)
	OV5640_WR_Reg(0X3018,0XFF); 
	//GPIOC8/9/11切换为 DCMI接口
 	GPIO_AF_Set(GPIOC,8,13);	//PC8,AF13  DCMI_D2
 	GPIO_AF_Set(GPIOC,9,13);	//PC9,AF13  DCMI_D3
 	GPIO_AF_Set(GPIOC,11,13);	//PC11,AF13 DCMI_D4  
} 
//切换为SD卡模式
void sw_sdcard_mode(void)
{
	OV5640_WR_Reg(0X3017,0X00);	//关闭OV5640全部输出(不影响SD卡通信)
	OV5640_WR_Reg(0X3018,0X00);  
 	//GPIOC8/9/11切换为 SDIO接口
  	GPIO_AF_Set(GPIOC,8,12);	//PC8,AF12
 	GPIO_AF_Set(GPIOC,9,12);	//PC9,AF12 
 	GPIO_AF_Set(GPIOC,11,12);	//PC11,AF12  
} 
//得到文件名,按文日期时间命名
//mode:0,创建.bmp文件;1,创建.jpg文件.
//bmp组合成:形如"0:PHOTO/PIC20120321210633.bmp"/"3:PHOTO/PIC20120321210633.bmp"的文件名
//jpg组合成:形如"0:PHOTO/PIC20120321210633.jpg"/"3:PHOTO/PIC20120321210633.jpg"的文件名
void camera_new_pathname(u8 *pname,u8 mode)
{	  
	calendar_get_time(&calendar);
	calendar_get_date(&calendar); 
	if(mode==0)
	{
		if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//首选保存在SD卡
		else if(gui_phy.memdevflag&(1<<3))sprintf((char*)pname,"3:PHOTO/PIC%04d%02d%02d%02d%02d%02d.bmp",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD卡不存在,则保存在U盘	
	}else 
	{
		if(gui_phy.memdevflag&(1<<0))sprintf((char*)pname,"0:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);		//首选保存在SD卡
		else if(gui_phy.memdevflag&(1<<3))sprintf((char*)pname,"3:PHOTO/PIC%04d%02d%02d%02d%02d%02d.jpg",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	//SD卡不存在,则保存在U盘	
	}
} 
//OV5640拍照jpg图片
//返回值:0,成功
//    其他,错误代码
u8 ov5640_jpg_photo(u8 *pname)
{
	FIL* f_jpg; 
	u8 res=0,headok=0;
	u32 bwr;
	u32 i,jpgstart,jpglen;
	u8* pbuf;
	f_jpg=(FIL *)mymalloc(SRAMIN,sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_jpg==NULL)return 0XFF;				//内存申请失败.
	ovx_mode=1;
	jpeg_data_ok=0;
	sw_ov5640_mode();						//切换为OV5640模式 
	OV5640_JPEG_Mode();						//JPEG模式  
	OV5640_OutSize_Set(16,4,2592,1944);		//设置输出尺寸(500W) 
	dcmi_rx_callback=jpeg_dcmi_rx_callback;	//JPEG接收数据回调函数
	DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],jpeg_line_size,2,1);//DCMI DMA配置    
	DCMI_Start(); 			//启动传输 
	while(jpeg_data_ok!=1);	//等待第一帧图片采集完
	jpeg_data_ok=2;			//忽略本帧图片,启动下一帧采集 
	if(ov_flash)
	{
		while(jpeg_data_ok==2);
		OV5640_Flash_Ctrl(1);//打开闪光灯
	}
	while(jpeg_data_ok!=1);	//等待第二帧图片采集完,第二帧,才保存到SD卡去. 
	if(ov_flash)OV5640_Flash_Ctrl(0);//关闭闪光灯
	DCMI_Stop(); 			//停止DMA搬运
	ovx_mode=0; 
	sw_sdcard_mode();		//切换为SD卡模式
	res=f_open(f_jpg,(const TCHAR*)pname,FA_WRITE|FA_CREATE_NEW);//模式0,或者尝试打开失败,则创建新文件	 
	if(res==0)
	{
		printf("jpeg data size:%d\r\n",jpeg_data_len*4);//串口打印JPEG文件大小
		pbuf=(u8*)jpeg_data_buf;
		jpglen=0;	//设置jpg文件大小为0
		headok=0;	//清除jpg头标记
		for(i=0;i<jpeg_data_len*4;i++)//查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小
		{
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))//找到FF D8
			{
				jpgstart=i;
				headok=1;	//标记找到jpg头(FF D8)
			}
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD9)&&headok)//找到头以后,再找FF D9
			{
				jpglen=i-jpgstart+2;
				break;
			}
		}
		if(jpglen)			//正常的jpeg数据 
		{
			pbuf+=jpgstart;	//偏移到0XFF,0XD8处
			res=f_write(f_jpg,pbuf,jpglen,&bwr);
			if(bwr!=jpglen)res=0XFE; 
			
		}else res=0XFD; 
	}
	jpeg_data_len=0;
	f_close(f_jpg); 
	sw_ov5640_mode();		//切换为OV5640模式
	OV5640_RGB565_Mode();	//RGB565模式  
	if(lcdltdc.pwidth!=0)	//RGB屏
	{
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB屏接收数据回调函数
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA配置  
	}else					//MCU 屏
	{
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA配置,MCU屏,竖屏
	}
	myfree(SRAMIN,f_jpg); 
	return res;
}   

//摄像头功能
//所有照片文件,均保存在SD卡PHOTO文件夹内.
u8 camera_play(void)
{												   
	u8 rval=0;
	u8 res,fac;	
 	u8 *caption=0;
	u8 *pname;
	u8 selx=0;
	u8 l00sel=0,l10sel=0,l11sel=0;//默认选择项
	u8 l2345sel[3];
	u8 *psn;
	u8 key;
 	u8 scale=1;				//默认是全尺寸缩放
	u8 tcnt=0;
	u16 outputheight=0;		//RGB 图像输出的宽度
	
 	if(audiodev.status&(1<<7))		//当前在放歌??必须停止
	{
		audio_stop_req(&audiodev);	//停止音频播放
		audio_task_delete();		//删除音乐播放任务.
	}
 	//提示开始检测OV5640
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(gui_phy.memdevflag&(1<<0))f_mkdir("0:PHOTO");//强制创建文件夹,给照相机用
	if(gui_phy.memdevflag&(1<<3))f_mkdir("3:PHOTO");//强制创建文件夹,给照相机用
	if(OV5640_Init())//初始化OV5640
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);  
		rval=1;
	} 
	dcmi_line_buf[0]=gui_memin_malloc(jpeg_line_size*4);//为jpeg dma接收申请内存	
	dcmi_line_buf[1]=gui_memin_malloc(jpeg_line_size*4);//为jpeg dma接收申请内存	
	jpeg_data_buf=gui_memex_malloc(jpeg_buf_size);		//为jpeg文件申请内存(最大4MB)
	pname=gui_memin_malloc(40);	//申请40个字节内存,用于存放文件名
	psn=gui_memin_malloc(50);	//申请50个字节内存,用于存放类似：“保存为:0:PHOTO/PIC20120321210633.bmp"”的提示语
	if(!dcmi_line_buf[1]||!jpeg_data_buf||!pname||!psn)rval=1;//申请失败	  
	if(rval==0)//OV5640正常
	{ 
		l2345sel[0]=3;			//亮度默认为3,实际值0
		l2345sel[1]=3;			//色度默认为3,实际值0
		l2345sel[2]=3;			//对比度默认为3,实际值0  
 		OV5640_RGB565_Mode();	//RGB565模式
		OV5640_Focus_Init();  
		OV5640_Focus_Constant();//启动持续对焦
		OV5640_Light_Mode(0);	//自动模式
		OV5640_Brightness(l2345sel[0]+1);		//亮度设置
		OV5640_Color_Saturation(l2345sel[1]);	//色度设置
		OV5640_Contrast(l2345sel[2]);			//对比度设置 
		OV5640_Exposure(3);		//曝光等级
 		OV5640_Sharpness(33);	//自动锐度
		DCMI_Init();			//DCMI配置 
		if(lcdltdc.pwidth!=0)	//RGB屏
		{
			dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB屏接收数据回调函数
			DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA配置  
		}else					//MCU 屏
		{
			DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA配置,MCU屏,竖屏
		} 	
		if(lcddev.height>800)
		{
			yoffset=(lcddev.height-800)/2;
			outputheight=800;
			TIM3->CR1&=~(0x01);	//关闭定时器3,
		}else 
		{
			yoffset=0;
			outputheight=lcddev.height;
		}
		curline=yoffset;		//行数复位
		OV5640_OutSize_Set(16,4,lcddev.width,outputheight);//设置摄像头输出尺寸为LCD的尺寸大小		
		DCMI_Start(); 			//启动传输  
		LCD_Clear(BLACK);
   		system_task_return=0;	//清除TPAD	 
 		while(1)	 
		{		 	
			tp_dev.scan(0);
			if(tp_dev.sta&TP_PRES_DOWN)
			{    	 
				OV5640_Focus_Single();//执行自动对焦
				tp_dev.scan(0);
				tp_dev.sta=0;
 			}
			key=KEY_Scan(0);//按键扫描
			if(key)
			{
				if(key==KEY2_PRES)//如果是BMP拍照,则等待300ms,去抖动,以获得稳定的bmp照片	
				{ 
					delay_ms(300);
					hsync_int=0;
					while(hsync_int==0);				//等待帧中断
					hsync_int=0;
					if(ov_flash)OV5640_Flash_Ctrl(1);	//打开闪光灯 
					bmp_request=1;						//请求关闭DCMI
					while(hsync_int==0);
					if(ov_flash)OV5640_Flash_Ctrl(0);	//关闭闪光灯 
					while(bmp_request);					//等带请求处理完成 
				}else DCMI_Stop();
				while(KEY_Scan(1));//等待按键松开
				tcnt=0;
				switch(key)
				{
					case KEY0_PRES:	//KEY0按下,JPEG拍照
					case KEY2_PRES:	//KEY2按下,BMP拍照
						LED1=0;		//DS1亮,提示拍照中
						sw_sdcard_mode();	//切换为SD卡模式
						if(key==KEY0_PRES)	//JPEG拍照
						{
							camera_new_pathname(pname,1);//得到jpg文件名	
							res=ov5640_jpg_photo(pname);
						}else//BMP拍照
						{
							camera_new_pathname(pname,0);//得到bmp文件名	
							res=bmp_encode(pname,0,yoffset,lcddev.width,outputheight,0);//bmp拍照
						}	
 						if(res)//拍照失败了
						{
							if(res==0XFF)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//内存错误
							else if(res==0XFD)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[2][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//数据错误
							else window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)camera_failmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);//提示SD卡是否存在
						}else
						{
							strcpy((char*)psn,(const char *)camera_remind_tbl[2][gui_phy.language]);
							strcat((char*)psn,(const char *)pname);
							window_msg_box((lcddev.width-180)/2,(lcddev.height-80)/2,180,80,psn,(u8*)camera_saveok_caption[gui_phy.language],12,0,0,0);
// 							PCF8574_WriteBit(BEEP_IO,0);	//蜂鸣器短叫，提示拍照完成
							delay_ms(100); 
						}
						LED1=1;						//DS1灭,提示保存完成
//						PCF8574_WriteBit(BEEP_IO,1);//关闭蜂鸣器
						delay_ms(2000); 
						sw_ov5640_mode();			//切换为OV5640模式   
						if(scale==0)
						{
							fac=800/outputheight;//得到比例因子
							OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						}else 
						{
							OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
						} 	
						system_task_return=0;//清除TPAD
						break;
					case KEY1_PRES:	//KEY1按下,缩放/1:1显示(不缩放)
						scale=!scale;  
						if(scale==0)
						{
							OSTaskSuspend(7); 		//挂起usart_task
							fac=800/outputheight;	//得到比例因子
							OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						}else
						{
							OSTaskResume(7); 		//恢复usart_task
							OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
						}	  
						window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_scalemsg_tbl[scale][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
 						while(KEY_Scan(1)==0&&tcnt<80)//等待800ms,如果没有按键按下的话.
						{
							delay_ms(10);
							tcnt++;
						}	
						break;
					case WKUP_PRES:			//设置摄像头参数 
						sw_sdcard_mode();	//切换为SD卡模式
						caption=(u8*)camera_l00fun_caption[gui_phy.language];
						res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*6)/2,160,72+32*6,(u8**)camera_l00fun_table[gui_phy.language],6,(u8*)&l00sel,0X90,caption);//单选
						sw_ov5640_mode();	//切换为OV5640模式
						if(res==0)
						{ 
							DCMI_Start();
							delay_ms(200);
							DCMI_Stop();
							sw_sdcard_mode();	//切换为SD卡模式 
							caption=(u8*)camera_l00fun_table[gui_phy.language][l00sel]; 
							switch(l00sel)
							{
								case 0://场景设置	  
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*5)/2,160,72+32*5,(u8**)camera_l10fun_table[gui_phy.language],5,(u8*)&l10sel,0X90,caption);//单选
									sw_ov5640_mode();//切换为OV5640模式
									if(res==0)
									{
										OV5640_Light_Mode(l10sel);
									}
									break;
								case 1://特效设置		 
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*7)/2,160,72+32*7,(u8**)camera_l11fun_table[gui_phy.language],7,(u8*)&l11sel,0X90,caption);//单选
									sw_ov5640_mode();//切换为OV5640模式
									if(res==0)
									{
										OV5640_Special_Effects(l11sel);  
									}
									break;  
								case 2://亮度设置    
								case 3://色度设置
								case 4://对比度设置
									selx=l2345sel[l00sel-2];//得到之前的选择
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*7)/2,160,72+32*7,(u8**)camera_l124fun_table[gui_phy.language],7,(u8*)&selx,0X90,caption);//单选
									sw_ov5640_mode();//切换为OV5640模式
									if(res==0)
									{
										l2345sel[l00sel-2]=selx;//记录新值 
										if(l00sel==2)OV5640_Brightness(selx+1);	   	//亮度设置
										if(l00sel==3)OV5640_Color_Saturation(selx);	//色度设置
										if(l00sel==4)OV5640_Contrast(selx);			//对比度设置
									}
									break; 
								case 5://闪光灯设置
									selx=ov_flash;
									res=app_items_sel((lcddev.width-160)/2,(lcddev.height-72-32*2)/2,160,72+32*2,(u8**)camera_l15fun_table[gui_phy.language],2,(u8*)&selx,0X90,caption);//单选
									if(res==0)ov_flash=selx; 
									break;
							}
						}      
						break;
						
				}
				sw_ov5640_mode();	//切换为OV5640模式   
				DCMI_Start();		//这里先使能dcmi,然后立即关闭DCMI,后面再开启DCMI,可以防止RGB屏的侧移问题.
				DCMI_Stop();	
				DCMI_Start();		
			} 
			if(system_task_return)
			{
				delay_ms(15);
				if(TPAD_Scan(1))break;//TPAD返回,再次确认,排除干扰	
				else system_task_return=0;
 			}
			if(hsync_int)//刚刚产生帧中断,可以延时
			{
				delay_ms(10);
				hsync_int=0;
			}  
 		}
	}
	DCMI_Stop();		//停止摄像头工作
	sw_sdcard_mode();	//切换为SD卡模式 
	gui_memin_free(dcmi_line_buf[0]);
	gui_memin_free(dcmi_line_buf[1]);
	gui_memex_free(jpeg_data_buf);
	gui_memin_free(pname);				
	gui_memin_free(psn);  
	OSTaskResume(7);	//恢复usart_task
 	return 0;	 
}















