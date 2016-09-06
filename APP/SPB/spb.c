#include "spb.h" 
#include "piclib.h"
#include "touch.h"
#include "delay.h"
#include "string.h"
#include "malloc.h"
#include "text.h"
#include "common.h"
#include "calendar.h"
#include "gsm.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//SPB效果实现 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved							  
//*******************************************************************************
//V1.1 20160627
//新增三页滑动支持							  
////////////////////////////////////////////////////////////////////////////////// 	

//SPB 控制器
m_spb_dev spbdev; 
 
//背景图路径,根据不同的lcd选择不同的路径
u8*const spb_bkpic_path_tbl[5]=
{ 
	"2:/SYSTEM/SPB/BACKPIC/840224.jpg", 
	"2:/SYSTEM/SPB/BACKPIC/952384.jpg", 
	"2:/SYSTEM/SPB/BACKPIC/1120364.jpg", 
	"2:/SYSTEM/SPB/BACKPIC/1680610.jpg", 
	"2:/SYSTEM/SPB/BACKPIC/2100834.jpg", 
};	
//界面图标的路径表
u8*const spb_icos_path_tbl[3][SPB_ICOS_NUM]=
{
{ 
	"2:/SYSTEM/SPB/ICOS/ebook_56.bmp",
	"2:/SYSTEM/SPB/ICOS/picture_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/music_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/video_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/clock_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/set_56.bmp",	    		    
	"2:/SYSTEM/SPB/ICOS/notepad_56.bmp",   
	"2:/SYSTEM/SPB/ICOS/setup_56.bmp",	   
	"2:/SYSTEM/SPB/ICOS/paint_56.bmp",
	"2:/SYSTEM/SPB/ICOS/camera_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/recorder_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/usb_56.bmp",	    
	"2:/SYSTEM/SPB/ICOS/earthnet_56.bmp",	
	"2:/SYSTEM/SPB/ICOS/calc_56.bmp",	
	"2:/SYSTEM/SPB/ICOS/qrcode_56.bmp",
	"2:/SYSTEM/SPB/ICOS/webcam_56.bmp",
	"2:/SYSTEM/SPB/ICOS/facerec_56.bmp", 
	"2:/SYSTEM/SPB/ICOS/9dtest_56.bmp",	
	"2:/SYSTEM/SPB/ICOS/grad_56.bmp",
	"2:/SYSTEM/SPB/ICOS/keytest_56.bmp",
	"2:/SYSTEM/SPB/ICOS/ledtest_56.bmp",	
},
{ 
	"2:/SYSTEM/SPB/ICOS/ebook_70.bmp",
	"2:/SYSTEM/SPB/ICOS/picture_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/music_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/video_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/clock_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/set_70.bmp",	    		    
	"2:/SYSTEM/SPB/ICOS/notepad_70.bmp",   
	"2:/SYSTEM/SPB/ICOS/setup_70.bmp",	   
	"2:/SYSTEM/SPB/ICOS/paint_70.bmp",
	"2:/SYSTEM/SPB/ICOS/camera_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/recorder_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/usb_70.bmp",	    
	"2:/SYSTEM/SPB/ICOS/earthnet_70.bmp",
	"2:/SYSTEM/SPB/ICOS/calc_70.bmp",	 
	"2:/SYSTEM/SPB/ICOS/qrcode_70.bmp",
	"2:/SYSTEM/SPB/ICOS/webcam_70.bmp",
	"2:/SYSTEM/SPB/ICOS/facerec_70.bmp", 
	"2:/SYSTEM/SPB/ICOS/9dtest_70.bmp",	
	"2:/SYSTEM/SPB/ICOS/grad_70.bmp",	
	"2:/SYSTEM/SPB/ICOS/keytest_70.bmp",
	"2:/SYSTEM/SPB/ICOS/ledtest_70.bmp",	
},
{ 
	"2:/SYSTEM/SPB/ICOS/ebook_110.bmp",
	"2:/SYSTEM/SPB/ICOS/picture_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/music_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/video_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/clock_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/set_110.bmp",	    	    
	"2:/SYSTEM/SPB/ICOS/notepad_110.bmp",   
	"2:/SYSTEM/SPB/ICOS/setup_110.bmp",	   
	"2:/SYSTEM/SPB/ICOS/paint_110.bmp",
	"2:/SYSTEM/SPB/ICOS/camera_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/recorder_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/usb_110.bmp",	    
	"2:/SYSTEM/SPB/ICOS/earthnet_110.bmp",
	"2:/SYSTEM/SPB/ICOS/calc_110.bmp",	 
	"2:/SYSTEM/SPB/ICOS/qrcode_110.bmp",
	"2:/SYSTEM/SPB/ICOS/webcam_110.bmp",
	"2:/SYSTEM/SPB/ICOS/facerec_110.bmp", 
	"2:/SYSTEM/SPB/ICOS/9dtest_110.bmp",
	"2:/SYSTEM/SPB/ICOS/grad_110.bmp",		
	"2:/SYSTEM/SPB/ICOS/keytest_110.bmp",
	"2:/SYSTEM/SPB/ICOS/ledtest_110.bmp",		  
},
};
//三个主图标的路径表
u8*const spb_micos_path_tbl[3][3]=
{ 
{
	"2:/SYSTEM/SPB/icos/phone_56.bmp",
	"2:/SYSTEM/SPB/icos/app_56.bmp",	    
	"2:/SYSTEM/SPB/icos/sms_56.bmp",	   
},	    
{ 
	"2:/SYSTEM/SPB/icos/phone_70.bmp",
	"2:/SYSTEM/SPB/icos/app_70.bmp",	    
	"2:/SYSTEM/SPB/icos/sms_70.bmp",  
},	    
{ 
	"2:/SYSTEM/SPB/icos/phone_110.bmp",
	"2:/SYSTEM/SPB/icos/app_110.bmp",	    
	"2:/SYSTEM/SPB/icos/sms_110.bmp",
},	 
};

//界面图标icos的对应功能名字
u8*const icos_name_tbl[GUI_LANGUAGE_NUM][SPB_ICOS_NUM]=
{
{ 
	"电子图书","数码相框","音乐播放","视频播放",
	"时钟","系统设置","记事本",	 
	"运行器","手写画笔","照相机","录音机",
	"USB 连接","网络通信","计算器", 
	"二维码","IP摄像头","人脸识别","9D测试",
	"水平仪","按键测试","LED测试",
},
{ 
	"電子圖書","數碼相框","音樂播放","視頻播放",
	"時鐘","系統設置","記事本",	 
	"運行器","手寫畫筆","照相机","錄音機 ",
	"USB 連接","網絡通信","計算器", 
	"二維碼","IP攝像頭","人臉識別","9D測試",
	"水平儀","按鍵測試","LED測試",
},
{ 
	"EBOOK","PHOTOS","MUSIC","VIDEO",
	"TIME","SETTINGS","NOTEPAD",	  
	"EXE","PAINT","CAMERA","RECODER",
	"USB","ETHERNET","CALC", 
	"QR CODE","WEB CAM","FACE REC","9D TEST",
	"LEVEL","KEY TEST","LED TEST",
},
};						  
//主图标对应的名字
u8*const micos_name_tbl[GUI_LANGUAGE_NUM][3]=
{ 
{
	"拨号","应用中心","短信",    
},
{
	"撥號","應用中心","短信", 
}, 
{
	"PHONE","APPS","SMS", 
}, 
};  

//初始化spb各个参数	
//返回值:0,操作成功
//    其他,错误代码
u8 spb_init(void)
{
	
	u16 i;
	u8 res=0;
	u8 lcdtype=0;	//LCD类型
	
	u8 icoindex=0;	//ico索引编号	
	u16 icowidth;	//图标的宽度
	u16 icoxpit;	//x方向图标之间的间距
	u16 micoyoff;
	
	memset((void*)&spbdev,0,sizeof(spbdev)); 
	spbdev.selico=0xff; 
	
	if(lcddev.width==240)	//对于240*320的LCD屏幕
	{ 
		icowidth=56; 
		micoyoff=4;
		lcdtype=0;
		icoindex=0;
		spbdev.stabarheight=20;
		spbdev.spbheight=224;
		spbdev.spbwidth =240; 
		spbdev.spbfontsize=12; 
	}if(lcddev.width==272)	//对于272*480的LCD屏幕
	{ 
		icowidth=56; 
		micoyoff=4;
		lcdtype=1;
		icoindex=0;
		spbdev.stabarheight=20;
		spbdev.spbheight=384;
		spbdev.spbwidth =272; 
		spbdev.spbfontsize=12; 
	}else if(lcddev.width==320)	//对于320*480的LCD屏幕
	{
		icowidth=70; 
		micoyoff=6;
		lcdtype=2;
		icoindex=1;
		spbdev.stabarheight=24;
		spbdev.spbheight=364;
		spbdev.spbwidth =320;  
		spbdev.spbfontsize=12; 
	}else if(lcddev.width==480)	//对于480*800的LCD屏幕
	{ 
		icowidth=110; 
		micoyoff=18;
		lcdtype=3;
		icoindex=2;
		spbdev.stabarheight=30;
		spbdev.spbheight=610;
		spbdev.spbwidth =480; 
		spbdev.spbfontsize=16; 
	}else if(lcddev.width==600)	//对于600*1024的LCD屏幕
	{ 
		icowidth=110; 
		micoyoff=18;
		lcdtype=4;
		icoindex=2;
		spbdev.stabarheight=30;
		spbdev.spbheight=834;
		spbdev.spbwidth =600; 
		spbdev.spbfontsize=16; 
	}
	
	icoxpit=(lcddev.width-icowidth*4)/4;
	spbdev.spbahwidth=spbdev.spbwidth/4;

	for(i=0;i<SPB_ICOS_NUM;i++)
	{
		spbdev.icos[i].width=icowidth;	//必须 等于图片的宽度尺寸
		spbdev.icos[i].height=icowidth+spbdev.spbfontsize+spbdev.spbfontsize/4;
		spbdev.icos[i].x=icoxpit/2+(i%4)*(icowidth+icoxpit)+(i/8)*spbdev.spbwidth;
		spbdev.icos[i].y=spbdev.stabarheight+4+((i%8)/4)*(spbdev.icos[i].height+icoxpit);
		spbdev.icos[i].path=(u8*)spb_icos_path_tbl[icoindex][i];
		spbdev.icos[i].name=(u8*)icos_name_tbl[gui_phy.language][i];
	}  
	for(i=0;i<3;i++)
	{
		spbdev.micos[i].x=(spbdev.spbwidth-3*icowidth-2*icoxpit)/2+i*(icowidth+icoxpit);
		spbdev.micos[i].y=spbdev.stabarheight+spbdev.spbheight+micoyoff;
		spbdev.micos[i].width=icowidth;//必须 等于图片的宽度尺寸
		spbdev.micos[i].height=icowidth+spbdev.spbfontsize+spbdev.spbfontsize/4;
		spbdev.micos[i].path=(u8*)spb_micos_path_tbl[icoindex][i];
		spbdev.micos[i].name=(u8*)micos_name_tbl[gui_phy.language][i]; 
	} 
	//指向EX SRAM LCD BUF  
	pic_phy.read_point=(u32(*)(u16,u16))slcd_read_point;
	pic_phy.draw_point=(void(*)(u16,u16,u32))slcd_draw_point;	 
	pic_phy.fillcolor=slcd_fill_color; 
	picinfo.lcdwidth=spbdev.spbwidth*SPB_PAGE_NUM+spbdev.spbahwidth*2;  
	gui_phy.read_point=(u32(*)(u16,u16))slcd_read_point;
	gui_phy.draw_point=(void(*)(u16,u16,u32))slcd_draw_point;	 
	sramlcdbuf=gui_memex_malloc(spbdev.spbheight*spbdev.spbwidth*7);	//申请3.5帧缓存大小
	if(sramlcdbuf==NULL)return 1;//错误	 
	res=ai_load_picfile(spb_bkpic_path_tbl[lcdtype],0,0,spbdev.spbwidth*SPB_PAGE_NUM+spbdev.spbahwidth*2,spbdev.spbheight,0);//加载背景图片
 	if(res==0)res=spb_load_icos();	 
 	//指向LCD
	pic_phy.read_point=LCD_ReadPoint;
	pic_phy.draw_point=LCD_Fast_DrawPoint;	 
	pic_phy.fillcolor=piclib_fill_color;  
	picinfo.lcdwidth=lcddev.width;
	gui_phy.read_point=LCD_ReadPoint;
	gui_phy.draw_point=LCD_Fast_DrawPoint;
	spbdev.pos=spbdev.spbahwidth;//默认是第1页开始位置
	return 0;
} 
//删除SPB
void spb_delete(void)
{
	memset((void*)&spbdev,0,sizeof(spbdev));
	gui_memex_free(sramlcdbuf);
}
//装载主界面icos
//返回值:0,操作成功
//    其他,错误代码
u8 spb_load_icos(void)
{
	u8 j;
	u8 res=0;  
	for(j=0;j<SPB_ICOS_NUM;j++)
	{
		res=minibmp_decode(spbdev.icos[j].path,spbdev.icos[j].x+spbdev.spbahwidth,spbdev.icos[j].y-spbdev.stabarheight,spbdev.icos[j].width,spbdev.icos[j].width,0,0);
 		if(res)return 1; 
		gui_show_strmid(spbdev.icos[j].x+spbdev.spbahwidth,spbdev.icos[j].y+spbdev.icos[j].width-spbdev.stabarheight,spbdev.icos[j].width,spbdev.spbfontsize,SPB_FONT_COLOR,spbdev.spbfontsize,spbdev.icos[j].name);//显示名字  
 	} 
	return 0;
} 
//装载主界面icos
//frame:帧编号 
//返回值:0,操作成功
//    其他,错误代码
u8 spb_load_micos(void)
{
	u8 j;
	u8 res=0;   
	gui_fill_rectangle(0,spbdev.stabarheight+spbdev.spbheight,lcddev.width,lcddev.height-spbdev.stabarheight-spbdev.spbheight,SPB_MICO_BKCOLOR);
	for(j=0;j<3;j++)
	{
		res=minibmp_decode(spbdev.micos[j].path,spbdev.micos[j].x,spbdev.micos[j].y,spbdev.micos[j].width,spbdev.micos[j].width,0,0);
		if(res)return 1; 
		gui_show_strmid(spbdev.micos[j].x,spbdev.micos[j].y+spbdev.micos[j].width,spbdev.micos[j].width,spbdev.spbfontsize,SPB_FONT_COLOR,spbdev.spbfontsize,spbdev.micos[j].name);//显示名字  
	} 
	return 0;
}
//SD卡图标
//PCtoLCD2002取模方式:阴码,逐行式,顺向
const u8 SPB_SD_ICO[60]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,
0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFC,
0x00,0x0F,0xFE,0x00,0x0F,0xFF,0x00,0x0F,0xFF,0x00,0x0F,0xFC,0x00,0x0A,0xAC,0x00,
0x0A,0xAC,0x00,0x0A,0xAC,0x00,0x0F,0xFC,0x00,0x00,0x00,0x00,
};
//USB 图标 
//PCtoLCD2002取模方式:阴码,逐行式,顺向
const u8 SPB_USB_ICO[60]=
{
0x00,0x00,0x00,0x00,0x60,0x00,0x00,0xF0,0x00,0x01,0xF8,0x00,0x00,0x60,0x00,0x00,
0x67,0x00,0x04,0x67,0x00,0x0E,0x62,0x00,0x0E,0x62,0x00,0x04,0x62,0x00,0x04,0x7C,
0x00,0x06,0x60,0x00,0x03,0xE0,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x60,0x00,
0x00,0xF0,0x00,0x00,0xF0,0x00,0x00,0x60,0x00,0x00,0x00,0x00,
}; 
//显示gsm信号强度,占用20*20像素大小
//x,y:起始坐标
//signal:信号强度,范围:0~30
void spb_gsm_signal_show(u16 x,u16 y,u8 signal)
{ 
	u8 t;
	u16 color;
	signal/=5;
	if(signal>5)signal=5;
	for(t=0;t<5;t++)
	{
		if(signal)//有信号
		{
			signal--;
			color=WHITE;
		}else color=0X6B4D;//无信号
		gui_fill_rectangle(x+1+t*4,y+20-6-t*3,3,(t+1)*3,color); 
	}
}
//GSM模塊提示信息 
u8* const spb_gsma_msg[GUI_LANGUAGE_NUM][3]=
{
"无移动网","中国移动","中国联通",
"無移動網","中國移動","中國聯通",
" NO NET ","CHN Mobi","CHN Unic",
}; 
//更新顶部信息条信息数据
//clr:0,不清除背景
//    1,清除背景
void spb_stabar_msg_show(u8 clr)
{ 
	u8 temp;
	if(clr)
	{
		gui_fill_rectangle(0,0,lcddev.width,spbdev.stabarheight,BLACK);
		gui_show_string("CPU:  %",24+64+20+2+20+2,(spbdev.stabarheight-16)/2,64,16,16,WHITE);//显示CPU数据
		gui_show_string(":",lcddev.width-42+16,(spbdev.stabarheight-16)/2,8,16,16,WHITE);		//显示时后的':'
		
		gui_show_string(":",lcddev.width-68+16,(spbdev.stabarheight-16)/2,8,16,16,WHITE);		//显示分后的':'
		
		gui_show_num(lcddev.width-250,(spbdev.stabarheight-16)/2,2,WHITE,16,20,0X80); //显示201x年中的20
		
		gui_show_string("年",lcddev.width-100-118,(spbdev.stabarheight-16)/2,16,16,16,WHITE);		//显示'年'
		gui_show_string("月",lcddev.width-100-86,(spbdev.stabarheight-16)/2,16,16,16,WHITE);		//显示'月'
		gui_show_string("日",lcddev.width-100-56,(spbdev.stabarheight-16)/2,16,16,16,WHITE);		//显示'日'
		gui_show_string("周",lcddev.width-100-32,(spbdev.stabarheight-16)/2,16,16,16,WHITE);		//显示'周'
 	}
	//GSM信息显示
	spb_gsm_signal_show(2,(spbdev.stabarheight-20)/2,gsmdev.csq);//显示信号质量
	gui_fill_rectangle(2+20+2,(spbdev.stabarheight-16)/2,64,16,BLACK);
	if(gsmdev.status&(1<<5))
	{
		if(gsmdev.status&(1<<4))temp=2;
		else temp=1;
		gui_show_string(spb_gsma_msg[gui_phy.language][temp],2+20+2,(spbdev.stabarheight-16)/2,64,16,16,WHITE);//显示运营商名字
	}else gui_show_string(spb_gsma_msg[gui_phy.language][0],2+20+2,(spbdev.stabarheight-16)/2,64,16,16,WHITE);//显示运营商名字	
	//更新SD卡信息 
	if(gui_phy.memdevflag&(1<<0))app_show_mono_icos(24+64+2,(spbdev.stabarheight-20)/2,20,20,(u8*)SPB_SD_ICO,WHITE,BLACK);
	else gui_fill_rectangle(24+64+2,0,20,spbdev.stabarheight,BLACK);
	//更新U盘信息 
	if(gui_phy.memdevflag&(1<<3))app_show_mono_icos(24+64+2+20,(spbdev.stabarheight-20)/2,20,20,(u8*)SPB_USB_ICO,WHITE,BLACK);
	else gui_fill_rectangle(24+64+2+20,0,20,spbdev.stabarheight,BLACK);
	//显示CPU使用率
	gui_phy.back_color=BLACK;
	temp=OSCPUUsage;
	if(OSCPUUsage>99)temp=99;//最多显示到99%
	gui_show_num(24+64+2+20+20+2+32,(spbdev.stabarheight-16)/2,2,WHITE,16,temp,0);//显示CPU使用率
	//显示时间
	calendar_get_time(&calendar);
	calendar_get_date(&calendar);
//	printf("sec:%d\r\n",calendar.sec);
	gui_show_num(lcddev.width-68,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.hour,0X80);	//显示时钟
	gui_show_num(lcddev.width-42,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.min,0X80);//显示分钟	
	gui_show_num(lcddev.width-2-16,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.sec,0X80);	//显示秒
	
	gui_show_num(lcddev.width-100-134,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.w_year,0X80);//显示年
	gui_show_num(lcddev.width-100-102,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.w_month,0X80);	//显示月
	gui_show_num(lcddev.width-100-72,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.w_date,0X80);//显示日
	gui_show_num(lcddev.width-100-16,(spbdev.stabarheight-16)/2,2,WHITE,16,calendar.week,0X80);//显示星期
}
extern u8*const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM];
//加载SPB主界面UI
//返回值:0,操作成功
//    其他,错误代码
u8 spb_load_mui(void)
{
	u8 res=0;  
	spb_stabar_msg_show(1);			//显示状态栏信息,清除所有后显示
	slcd_frame_show(spbdev.pos);	//显示主界面icos
	res=spb_load_micos();			//加载主图标 
	return res;
}
//移动屏幕
//dir:方向,0:左移;1,右移
//skips:每次跳跃列数
//pos:起始位置
void spb_frame_move(u8 dir,u8 skips,u16 pos)
{
	u8 i;
	u16 x;
	u16 endpos=spbdev.spbahwidth; 
	for(i=0;i<SPB_PAGE_NUM;i++)//得到终点位置
	{
		if(dir==0)	//左移
		{
			if(pos<=(spbdev.spbwidth*i+spbdev.spbahwidth))
			{
				endpos=spbdev.spbwidth*i+spbdev.spbahwidth;
				break;
			}
		}else		//右移
		{			
			if(pos>=(spbdev.spbwidth*(SPB_PAGE_NUM-i-1)+spbdev.spbahwidth))
			{
				endpos=spbdev.spbwidth*(SPB_PAGE_NUM-i-1)+spbdev.spbahwidth;
				break;
			}  
		}
	}
	if(dir)//屏幕右移
	{
		for(x=pos;x>endpos;)
		{
			if((x-endpos)>skips)x-=skips;
			else x=endpos;
			slcd_frame_show(x);
 	 	} 	
	}else //屏幕左移
	{
		for(x=pos;x<endpos;)
		{
			x+=skips;
			if(x>endpos)x=endpos;
			slcd_frame_show(x);
 	 	} 
	} 
	spbdev.pos=endpos;	
} 
//清除某个mico图标的选中状态
//selx:SPB_ICOS_NUM~SPB_ICOS_NUM+2,表示将要清除选中状态的mico编号.
void spb_unsel_micos(u8 selx)
{  
	if(selx>=SPB_ICOS_NUM&&selx<(SPB_ICOS_NUM+3))	//合法的编号
	{
		selx-=SPB_ICOS_NUM;
 		gui_fill_rectangle(spbdev.micos[selx].x,spbdev.micos[selx].y,spbdev.micos[selx].width,spbdev.micos[selx].height,SPB_MICO_BKCOLOR); 
		minibmp_decode(spbdev.micos[selx].path,spbdev.micos[selx].x,spbdev.micos[selx].y,spbdev.micos[selx].width,spbdev.micos[selx].width,0,0);
 		gui_show_strmid(spbdev.micos[selx].x,spbdev.micos[selx].y+spbdev.micos[selx].width,spbdev.micos[selx].width,spbdev.spbfontsize,SPB_FONT_COLOR,spbdev.spbfontsize,spbdev.micos[selx].name);//显示名字
	}	
}
//设置选中哪个图标
//sel:0~18代表当前页的选中ico 
void spb_set_sel(u8 sel)
{  
	u16 temp=0;
	slcd_frame_show(spbdev.pos);	//刷新背景  
	spb_unsel_micos(spbdev.selico);	//清除主图标选中状态
	spbdev.selico=sel; 
	if(sel<SPB_ICOS_NUM)
	{ 
		temp=spbdev.icos[sel].x-spbdev.spbwidth*(sel/8); 
 		gui_alphablend_area(temp,spbdev.icos[sel].y,spbdev.icos[sel].width,spbdev.icos[sel].height,SPB_ALPHA_COLOR,SPB_ALPHA_VAL);
		minibmp_decode(spbdev.icos[sel].path,temp,spbdev.icos[sel].y,spbdev.icos[sel].width,spbdev.icos[sel].width,0,0);
 		gui_show_strmid(temp,spbdev.icos[sel].y+spbdev.icos[sel].width,spbdev.icos[sel].width,spbdev.spbfontsize,SPB_FONT_COLOR,spbdev.spbfontsize,spbdev.icos[sel].name);//显示名字
	}else
	{
		sel-=SPB_ICOS_NUM;
 		gui_alphablend_area(spbdev.micos[sel].x,spbdev.micos[sel].y,spbdev.micos[sel].width,spbdev.micos[sel].height,SPB_ALPHA_COLOR,SPB_ALPHA_VAL); 
		minibmp_decode(spbdev.micos[sel].path,spbdev.micos[sel].x,spbdev.micos[sel].y,spbdev.micos[sel].width,spbdev.micos[sel].width,0,0);
		gui_show_strmid(spbdev.micos[sel].x,spbdev.micos[sel].y+spbdev.micos[sel].width,spbdev.micos[sel].width,spbdev.spbfontsize,SPB_FONT_COLOR,spbdev.spbfontsize,spbdev.micos[sel].name);//显示名字
	}
}  

//屏幕滑动及按键检测
//返回值:0~17,被双击的图标编号.		    
//       0xff,没有任何图标被双击或者按下
u8 spb_move_chk(void)
{		 
	u8 i=0xff;
	u16 temp=0;
	u16 movewidth=spbdev.spbwidth*(SPB_PAGE_NUM-1)+2*spbdev.spbahwidth;//可以移动宽度
	u16 movecnt=0;	//得到滑动点数
	u8 skips=5;		//默认每次跳5个像素  
	switch(lcddev.width)
	{
		case 240:
			skips=5;		 
			break;
		case 272:
			skips=2;
			break; 
		case 320:
			skips=10;
			break;
		case 480:
			if(lcdltdc.pwidth)skips=8;	//RGB屏,跳8个像素
			else skips=20;				//MCU屏,跳20个像素
			break;
		case 600:
			skips=100;
			break; 
	} 
	tp_dev.scan(0);	//扫描		
	if(tp_dev.sta&TP_PRES_DOWN)//有按键被按下
	{
		if(spbdev.spbsta&0X8000)//已经被标记了
		{
			movecnt=spbdev.spbsta&0X3FFF;//得到滑动点数
			if(gui_disabs(spbdev.curxpos,tp_dev.x[0])>=SPB_MOVE_WIN)//移动大于等于SPB_MOVE_WIN个点
			{
				if(movecnt<SPB_MOVE_MIN/SPB_MOVE_WIN)spbdev.spbsta++;//点数增加1	
			}
		}			  		    
		spbdev.curxpos=tp_dev.x[0];				//记录当前坐标
		spbdev.curypos=tp_dev.y[0];				//记录当前坐标
		if((spbdev.spbsta&0X8000)==0)			//按键第一次被按下
		{
			if(spbdev.curxpos>4096||spbdev.curypos>4096)return 0XFF;//非法的数据
			spbdev.spbsta=0;				 	//状态清零
			spbdev.spbsta|=1<<15;				//标记按下	    
			spbdev.oldxpos=tp_dev.x[0];			//记录按下时的坐标  
			spbdev.oldpos=spbdev.pos;			//记录按下时的帧位置
		}else if(spbdev.spbsta&0X4000)			//有滑动
		{
			if(spbdev.oldxpos>tp_dev.x[0]) 		//x左移,屏幕pos右移
			{
				if(spbdev.pos<movewidth)spbdev.pos+=spbdev.oldxpos-tp_dev.x[0]; 
				if(spbdev.pos>movewidth)spbdev.pos=movewidth;	//溢出了. 
			}else 												//右移,屏幕pos左移
			{			    
				if(spbdev.pos>0)spbdev.pos-=tp_dev.x[0]-spbdev.oldxpos;
				if(spbdev.pos>movewidth)spbdev.pos=0;			//溢出了.	   		  
			}	    
			spbdev.oldxpos=tp_dev.x[0];
			slcd_frame_show(spbdev.pos);
		}else if((gui_disabs(spbdev.curxpos,spbdev.oldxpos)>=SPB_MOVE_MIN)&&(movecnt>=SPB_MOVE_MIN/SPB_MOVE_WIN))//滑动距离超过SPB_MOVE_MIN,并且检测到的有效滑动数不少于SPB_MOVE_MIN/SPB_MOVE_WIN.
		{ 
			spbdev.spbsta|=1<<14;//标记滑动	
		}   
	}else //按键松开了
	{ 
		if(spbdev.spbsta&0x8000)//之前有按下
		{	  
			if(spbdev.spbsta&0X4000)//有滑动
			{	 
				if(spbdev.pos>spbdev.oldpos)//左移
				{
					if(((spbdev.pos-spbdev.oldpos)>SPB_MOVE_ACT)&&(spbdev.oldpos<(movewidth-spbdev.spbahwidth)))
					{
						spb_frame_move(0,skips,spbdev.pos); 
					}else 
					{	
						spb_frame_move(1,skips,spbdev.pos); 
					}	 
				}else//右移
				{
					if(((spbdev.oldpos-spbdev.pos)>SPB_MOVE_ACT)&&(spbdev.pos>spbdev.spbahwidth))
					{
						spb_frame_move(1,skips,spbdev.pos); 
					}else 
					{	
						spb_frame_move(0,skips,spbdev.pos); 
					}					
				} 
				spb_unsel_micos(spbdev.selico);	//清除主图标选中状态
				spbdev.selico=0xff;				//取消spbdev.selico原先的设置  
			}else	//属于点按.
			{  
				for(i=0;i<SPB_ICOS_NUM+3;i++)	//检查按下的图标编号
				{
					if(i<SPB_ICOS_NUM)
					{
						temp=spbdev.curxpos+spbdev.pos-spbdev.spbahwidth;
						if((temp>spbdev.icos[i].x)&&(temp<spbdev.icos[i].x+spbdev.icos[i].width)&&(temp>spbdev.icos[i].x)&&
						   (spbdev.curypos<spbdev.icos[i].y+spbdev.icos[i].height))
						{
							break;//得到选中的编号	
						}
					}else
					{
						if((spbdev.curxpos>spbdev.micos[i-SPB_ICOS_NUM].x)&&(spbdev.curxpos<spbdev.micos[i-SPB_ICOS_NUM].x+spbdev.micos[i-SPB_ICOS_NUM].width)&&
						   (spbdev.curypos>spbdev.micos[i-SPB_ICOS_NUM].y)&&(spbdev.curypos<spbdev.micos[i-SPB_ICOS_NUM].y+spbdev.micos[i-SPB_ICOS_NUM].height))
						{
							break;//得到选中的编号	
						} 
					}
				}
				if(i<(SPB_ICOS_NUM+3))//有效
				{ 
					if(i!=spbdev.selico)//选中了不同的图标,切换图标
					{
						spb_set_sel(i);
						i=0xff;
					}else
					{
						//if(spbdev.frame==1&&i<8)i+=8;			//第二页内容,编号偏移到8~15
						spbdev.selico=0XFF;						//发生了双击,重新复位selico.
 					}
				}else i=0XFF;//无效的点按. 
			}
		} 							
		spbdev.spbsta=0;//清空标志
	} 
	return i;

}











