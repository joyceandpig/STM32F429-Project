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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-二维码识别&编码 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
extern u8*const camera_remind_tbl[4][GUI_LANGUAGE_NUM];

extern u32 *dcmi_line_buf[2];		//摄像头采用一行一行读取,定义行缓存  
u16 *rgb_data_buf;					//RGB565帧缓存buf 

vu8 qr_dcmi_rgbbuf_sta=0;			//RGB BUF状态
									//bit:7~2,保留
									//bit1:0,非QR识别模式;1,QR识别模式;
									//bit0:0,数据未准备好;1,数据准备好了
									
extern u32 *ltdc_framebuf[2];		//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域 
u32 qr_parm1,qr_parm2,qr_parm3;		//简化LTDC打点,提高速度的三个参数 
			
u16 qr_dcmi_curline=0;				//摄像头输出数据,当前行编号
u16 qr_lcd_width=0;					//qr识别,显示在LCD上面的图像的宽度(高度等于宽度)
u16 qr_lcd_xoff;					//qr识别,显示在LCD上时,X方向的偏移量
u16 qr_lcd_yoff;					//qr识别,显示在LCD上时,Y方向的偏移量

//模式选择
u8*const qr_mode_tbl[GUI_LANGUAGE_NUM][2]=
{
{"识别二维码","生成二维码",},
{"識別二維碼","生成二維碼",},		 
{"Decode QR Code","Encode QR code",},
}; 

//qr编码提示信息
u8*const qr_remind_msg_tbl[3][GUI_LANGUAGE_NUM]=
{ 
{"输入内容:","輸入內容:","Input Text:",},
{"请输入内容!","請輸入內容!","Pls Input Text!",}, 
{"内容太多!","內容太多!","Too Many Text",},  
};
//qr编码按钮标题
u8*const qr_enc_tbl[GUI_LANGUAGE_NUM]= 
{"编码","編碼","Encode",};
//qr结果窗标题
u8*const qr_result_tbl[2][GUI_LANGUAGE_NUM]= 
{
{"识别结果","識別結果","Decode Result",},
{"编码结果","編碼結果","Encode Result",},
};

//摄像头数据接收回调函数
void qr_dcmi_rx_callback(void)
{  
	u32 *pbuf;
	u32 *qr_plcd=ltdc_framebuf[lcdltdc.activelayer];	//指向显存地址减少数组访问,提高速度
	u16 *disbuf;
	u16 i;
	u32 temp;
	if(lcdltdc.pwidth!=0)	//RGB屏,直接显示
	{
//		__HAL_DMA_GET_IT_SOURCE(DMA_Handler, DMA_FLAG_TEIF2_6);
		if(DMA2_Stream1->CR&(1<<19))//DMA使用buf1,读取buf0
		{ 
			disbuf=(u16*)dcmi_line_buf[0];
		}else 						//DMA使用buf0,读取buf1
		{
			disbuf=(u16*)dcmi_line_buf[1];
		}
		temp=qr_parm1+qr_parm3*qr_dcmi_curline; 
		for(i=0;i<qr_lcd_width;i++)//打点,输出给RGB LCD屏,每次输出一行数据
		{  
			*(u16*)((u32)qr_plcd+temp-qr_parm2*i)=*(disbuf+i);
		}
	}else					//MCU屏,需要对每一行数据进行存储
	{
		pbuf=(u32*)(rgb_data_buf+qr_dcmi_curline*qr_lcd_width);
		if(DMA2_Stream1->CR&(1<<19))//DMA使用buf1,读取buf0
		{ 
			for(i=0;i<qr_lcd_width/2;i++)
			{
				pbuf[i]=dcmi_line_buf[0][i];
			} 
		}else 						//DMA使用buf0,读取buf1
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

//显示识别结果
void qr_decode_show_result(u8* result)
{
 	_window_obj* twin=0;	//窗体
	_memo_obj * tmemo=0;	//memo控件
	u8*p;
	u8 linecnt=1;			//行数
	u16 wwidth,wheight;
	u16 wy;
	u16 temp,linecharcnt;
	u8 fsize=16;  
	u16 winoff,wxoff;		//memo在窗体内的偏移/窗体x方向偏移
	
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
	linecharcnt=(wwidth-2*winoff-MEMO_SCB_WIDTH)*2/fsize;//每一行可以显示的字符个数
	temp=strlen((char*)result);				//得到字符串整体长度
	temp/=linecharcnt;						//得到需要的行数
	p=result;
	while(1)
	{
		p=(u8*)strstr((char*)p,"\n");		//查找换行,统计行数
		if(p)
		{
			linecnt++;						//行数加1
			p+=1;							//往后偏移一个地址
		}else break;		
	}
	if(linecnt<temp)linecnt=temp;			//少数服从多数
	if(linecnt<3)linecnt=3;					//至少3行数据
	wheight=2*winoff+(2+linecnt)*fsize;		//整体高度 
	if(wheight>(lcddev.height-4*fsize))wheight=lcddev.height-4*fsize;//最少留2个字高的间隙  
 	wy=(lcddev.height-wheight)/2; 
	tmemo=memo_creat(wxoff+winoff,wy+fsize*2+winoff,wwidth-winoff*2,wheight-(fsize+winoff)*2,0,1,fsize,1024);//申请1K内存.	 
  	twin=window_creat(wxoff,wy,wwidth,wheight,0,1|(1<<5),fsize);//创建窗口
	if(twin&&tmemo)
	{
		twin->captionheight=2*fsize;		//默认高度
		twin->caption=qr_result_tbl[0][gui_phy.language];	//显示标题
		twin->font=fsize;					//设置字体大小
		strcpy((char*)tmemo->text,(const char*)result);		//复制字符串
 		window_draw(twin);					//画出窗体
		memo_draw_memo(tmemo,0);			//画出memo
		system_task_return=0;
		while(1)
		{
			tp_dev.scan(0);
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值   
			memo_check(tmemo,&in_obj);
			if(system_task_return)break; 	//返回
			delay_ms(5);			//延时 
		}
	}
	window_delete(twin);
	memo_delete(tmemo);				//释放内存	 
	system_task_return=0;
}
//二维码识别
void qr_decode_play(void)
{
	u8 rval=0;
	u16 i=0,j=0;
 	float fac; 
	
	static u8 bartype=0; 			//轮询所有编码
	
	u16 qr_input_width=0;			//输入识别器的图像宽度,最大不超过240!
	u8 *qr_gray_buf;				//8位灰度图片缓存
	u8 qr_img_scale=1;				//比例因子 
	u8 *qr_result=NULL;
	u16 color; 	
	u8 qr_data_ready=0;				//qr识别用的数据准备好标志:0,未准备好;非0,准备好了

	
	
	if(audiodev.status&(1<<7))		//当前在放歌??必须停止
	{
		audio_stop_req(&audiodev);	//停止音频播放
		audio_task_delete();		//删除音乐播放任务.
	}
 	//提示开始检测OV5640
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(OV5640_Init())//初始化OV5640
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
	qr_gray_buf=mymalloc(SRAMCCM,qr_input_width*qr_input_width);	//申请内存
	dcmi_line_buf[0]=gui_memin_malloc(qr_lcd_width*2);				//为行缓存接收申请内存	
	dcmi_line_buf[1]=gui_memin_malloc(qr_lcd_width*2);				//为行缓存接收申请内存
	rgb_data_buf=gui_memex_malloc(qr_lcd_width*qr_lcd_width*2);		//为rgb帧缓存申请内存
	if(!qr_gray_buf||!dcmi_line_buf[1]||!rgb_data_buf[1])rval=1;
	if(rval==0)
	{ 
		//自动对焦初始化
		OV5640_RGB565_Mode();		//RGB565模式 
		OV5640_Focus_Init(); 
		OV5640_Light_Mode(0);		//自动模式
		OV5640_Color_Saturation(3);	//色彩饱和度0
		OV5640_Brightness(4);		//亮度0
		OV5640_Contrast(3);			//对比度0
		OV5640_Sharpness(33);		//自动锐度
		OV5640_Focus_Constant();	//启动持续对焦 
		if(lcdltdc.pwidth!=0)OV5640_WR_Reg(0X3821,0x00);//不镜像  
		DCMI_Init();				//DCMI配置  
		qr_dcmi_rgbbuf_sta|=1<<1;	//标记qr模式
		dcmi_rx_callback=qr_dcmi_rx_callback;//数据接收回调函数
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],qr_lcd_width/2,1,1);//DCMI DMA配置   
		//计算RGB屏GRAM偏移参数
		qr_lcd_xoff=(lcddev.width-qr_lcd_width)/2;
		qr_lcd_yoff=(lcddev.height-qr_lcd_width)/2;
		qr_parm1=lcdltdc.pixsize*lcdltdc.pwidth*(lcdltdc.pheight-qr_lcd_xoff-1)+lcdltdc.pixsize*qr_lcd_yoff;
		qr_parm2=lcdltdc.pixsize*lcdltdc.pwidth;
		qr_parm3=lcdltdc.pixsize;
		rval=atk_qr_init();	//初始化识别库
		if(rval==0)
		{
			fac=800/qr_lcd_width;		//得到比例因子
			OV5640_OutSize_Set((1280-fac*qr_lcd_width)/2,(800-fac*qr_lcd_width)/2,qr_lcd_width,qr_lcd_width); 
			DCMI_Start(); 				//启动传输
			LCD_Clear(BLACK);
			while(1)
			{
				tp_dev.scan(0);
				if(tp_dev.sta&TP_PRES_DOWN)
				{    	 
					OV5640_Focus_Single();//执行自动对焦
					tp_dev.scan(0);
					tp_dev.sta=0;
				}
				if((qr_dcmi_rgbbuf_sta&0X01)&&qr_data_ready==0)//数识别用据未准备好,但是摄像头采集数据已经OK.
				{ 
					for(i=0;i<qr_input_width;i++)
					{
						for(j=0;j<qr_input_width;j++)
						{
							if(lcdltdc.pwidth)color=LTDC_Read_Point(j*qr_img_scale+qr_lcd_xoff,i*qr_img_scale+qr_lcd_yoff);
							else color=*(rgb_data_buf+i*qr_img_scale*qr_lcd_width+j*qr_img_scale);
							*(qr_gray_buf+i*qr_input_width+j)=(((color&0xF800)>> 8)*76+((color&0x7E0)>>3)*150+((color&0x001F)<<3)*30)>>8;//转换为灰度
						}
					} 
					qr_dcmi_rgbbuf_sta&=~(1<<0);	//清除数据准备好标记
					qr_data_ready=5;				//标记识别用数据准备好了
				}
				if(qr_data_ready)					//数据准备好了? 
				{ 		
					atk_qr_decode(qr_input_width,qr_input_width,qr_gray_buf,bartype,qr_result);//识别灰度图片（单次耗时约0.2S）
					if(qr_result[0]==0)//没有识别出来
					{
						bartype++;
						if(qr_data_ready)qr_data_ready--;
						if(bartype>=5)bartype=0; 	//最多支持5种编码格式 
					}else if(qr_result[0]!=0)
					{	
//						PCF8574_WriteBit(BEEP_IO,0);
						delay_ms(100);
//						PCF8574_WriteBit(BEEP_IO,1);
						qr_data_ready=0;			//识别完成,换下一批数据
						DCMI_Stop();
						sw_sdcard_mode();			//切换为SD卡模式 
						printf("qr decode result:%s\r\n",qr_result); 
						qr_decode_show_result(qr_result);
						LCD_Clear(BLACK);
						sw_ov5640_mode();			//切换为OV5640模式   
						DCMI_Start();	 
						delay_ms(500);
						qr_dcmi_rgbbuf_sta&=~(1<<0);//清除数据准备好标记
						qr_data_ready=0;			//标记识别用数据准备好了
					}   
				}
				i++;
				if(system_task_return)break;		//TPAD返回,再次确认,排除干扰	
				delay_ms(1000/OS_TICKS_PER_SEC);	//延时一个时钟节拍  					
			}
			atk_qr_destroy();
		}
	}
	DCMI_Stop();						//停止摄像头工作
	myfree(SRAMEX,qr_result);//SRAMIN
	myfree(SRAMCCM,qr_gray_buf);	//
	gui_memin_free(dcmi_line_buf[0]);
	gui_memin_free(dcmi_line_buf[1]);
	gui_memex_free(rgb_data_buf);
	qr_dcmi_rgbbuf_sta=0;
	sw_sdcard_mode();				//切换为SD卡模式 
}
//二维码生成
void qr_encode_play(void)
{
 	_window_obj* twin=0;//窗体
	u8 res; 
	u8 rval=0; 
 	
	u16 i,j;
	u16 pixsize;		//点大小
	u16 wwidth,wheight;	//QR结果窗口大小
	u16 wpitch;			//间隙

	u16 textlen;		//输入字符串长度
	u16 h1,h2;			//纵向间隔
	u16 w1;				//横向间隔
	u16 t9height=0;		//T9输入法高度
	u16 btnw,btnh;		//按钮宽度/高度
	u16 memow,memoh;	//文本框宽度/高度 
	
	u8 cbtnfsize=0;		//按钮字体大小
	u16 tempy;  
	
 	_btn_obj* encbtn=0;	//编码按钮
	_memo_obj * tmemo=0;//memo控件
	_t9_obj * t9=0;		//输入法  
	u16 *bkcolorbuf;	//背景色缓存区
	 
	 
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
	
	LCD_Clear(QR_BACK_COLOR);//画背景色
	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[16][gui_phy.language],0X05);	//显示标题  
	POINT_COLOR=WHITE;  
	gui_show_string(qr_remind_msg_tbl[0][gui_phy.language],w1,gui_phy.tbheight+h1,lcddev.width,gui_phy.tbfsize,gui_phy.tbfsize,BLACK);//显示提示	
 	tempy=gui_phy.tbheight+h1+h2+gui_phy.tbfsize;
 	tmemo=memo_creat(w1,tempy,memow,memoh,0,1,gui_phy.tbfsize,2000);		//申请2000个字节的内存. 
	t9=t9_creat((lcddev.width%5)/2,lcddev.height-t9height,lcddev.width-(lcddev.width%5),t9height,0); 
	tempy=gui_phy.tbheight+h1+h2+gui_phy.tbfsize+(memoh-btnh)/2;
 	encbtn=btn_creat(w1*2+memow,tempy,btnw,btnh,0,0); 
	qrx=gui_memin_malloc(sizeof(_qr_encode));	
	if(!t9||!encbtn||!tmemo||!qrx)rval=1;//内存申请失败  
 	if(rval==0)
	{
		encbtn->caption=qr_enc_tbl[gui_phy.language];
		encbtn->font=cbtnfsize;
		t9_draw(t9);				//画T9输入法
		btn_draw(encbtn);			//画按钮
		memo_draw_memo(tmemo,0);	//画输入memo框
		while(1)
		{					  
 			if(system_task_return)break;			//TPAD返回
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//得到按键键值    
			memo_check(tmemo,&in_obj);   
			t9_check(t9,&in_obj);		   
			if(t9->outstr[0]!=NULL)//添加字符
			{  
				memo_add_text(tmemo,t9->outstr);  
				t9->outstr[0]=NULL;//清空输出字符 
			} 		
			res=btn_check(encbtn,&in_obj);   
			if(res&&((encbtn->sta&(1<<7))==0)&&(encbtn->sta&(1<<6)))//有输入,有按键按下且松开,并且TP松开了
			{   
				textlen=strlen((char*)tmemo->text);
				rval=0;
				if(textlen)
				{
					EncodeData((char *)tmemo->text);
					if(qrx->m_nSymbleSize*2>(lcddev.width-40))rval=2;			//提示信息2
					else
					{
						for(wpitch=5;wpitch>1;wpitch--)
						{
							if(qrx->m_nSymbleSize*2<(lcddev.width-10*4*wpitch))break;
						}
						wpitch*=10;//得到间隙大小
						pixsize=(lcddev.width-4*wpitch)/qrx->m_nSymbleSize;		//点大小
						if(pixsize>8)pixsize=8;									//最大不超过8个像素
						wwidth=pixsize*qrx->m_nSymbleSize+2*wpitch;
						wheight=pixsize*qrx->m_nSymbleSize+2*wpitch+2*cbtnfsize;   
						twin=window_creat((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,0,1|(1<<5),cbtnfsize);//创建窗口
						twin->captionheight=2*cbtnfsize;						//默认高度
						twin->caption=qr_result_tbl[1][gui_phy.language];		//显示标题
						twin->font=cbtnfsize;									//设置字体大小
						bkcolorbuf=gui_memex_malloc(wwidth*wheight*2);			//申请内存
						app_read_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//读取背景色 
						window_draw(twin);										//画出窗体
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
							if(system_task_return)break;	//TPAD返回
							delay_ms(10);
						} 
						app_recover_bkcolor((lcddev.width-wwidth)/2,(lcddev.height-wheight)/2,wwidth,wheight,bkcolorbuf);//恢复背景色  				  
						gui_memex_free(bkcolorbuf);			//释放内存 
						system_task_return=0;
					}						
				}else rval=1;	//提示信息1
				if(rval)
				{
					bkcolorbuf=gui_memex_malloc(220*100*2);	//申请内存
					app_read_bkcolor((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,bkcolorbuf);//读取背景色
					window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)qr_remind_msg_tbl[rval][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
					delay_ms(1000);
					app_recover_bkcolor((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,bkcolorbuf);//恢复背景色  				  
					gui_memex_free(bkcolorbuf);				//释放内存
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
//二维码编码应用
void qr_play(void)
{
	u8 rval;
	u8 mode=0;
 	//先选择模式 
	rval=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)qr_mode_tbl[gui_phy.language],2,(u8*)&mode,0X90,(u8*)APP_MODESEL_CAPTION_TBL[gui_phy.language]);//2个选择
	if(rval==0)
	{
		if(mode==0)//识别二维码
		{
			qr_decode_play();
		}else qr_encode_play();//生成二维码
	}
}





