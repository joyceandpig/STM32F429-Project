#include "sys.h"
#include "dcmi.h" 
#include "lcd.h" 
#include "led.h" 
#include "ov5640.h" 
#include "ltdc.h"  
#include "ucos_ii.h" 
#include "facereco.h" 
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//DCMI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//升级说明
//V1.1  20160302
//修改DCMI_DMA_Init和DCMI_Init函数对CR寄存器的操作方式,防止意外死机.
//V1.2  20160515
//修改DMA2_Stream1_IRQn的优先级，防止数据覆盖，导致图片末尾错位.
////////////////////////////////////////////////////////////////////////////////// 	 

DCMI_HandleTypeDef  DCMI_Handler;           //DCMI句柄
DMA_HandleTypeDef   DMADMCI_Handler;        //DMA句柄

u8 ov_frame=0;  						//帧率
extern void jpeg_data_process(void);	//JPEG数据处理函数 
 
//DCMI中断服务函数
void DCMI_IRQHandler(void)
{    
	OSIntEnter();   
//	if(DCMI->MISR&0X01)		//捕获到一帧图像
//	{  
//		jpeg_data_process();//jpeg数据处理
//		DCMI->ICR|=1<<0;	//清除帧中断
//		//LED1=!LED1;
//		LED1_STA_TURN();
//		ov_frame++; 
//	}
	HAL_DCMI_IRQHandler(&DCMI_Handler);
	OSIntExit(); 				 
} 

//捕获到一帧图像处理函数
//hdcmi:DCMI句柄
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	jpeg_data_process();//jpeg数据处理
	ov_frame++; 
    //重新使能帧中断,因为HAL_DCMI_IRQHandler()函数会关闭帧中断
  __HAL_DCMI_ENABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);
}
//DCMI DMA配置
//mem0addr:存储器地址0  将要存储摄像头数据的内存地址(也可以是外设地址)
//mem1addr:存储器地址1  当只使用mem0addr的时候,该值必须为0
//memsize:存储器长度    0~65535
//memblen:存储器位宽    0,8位,1,16位,2,32位
//meminc:存储器增长方式,0,不增长;1,增长
void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u8 memblen,u8 meminc)
{ 
//	u32 tempreg=0;
//	RCC->AHB1ENR|=1<<22;			//DMA2时钟使能 
// 	while(DMA2_Stream1->CR&0X01);	//等待DMA2_Stream1可配置 
//	DMA2->LIFCR|=0X3D<<6*1;			//清空通道1上所有中断标志
//	DMA2_Stream1->FCR=0X0000021;	//设置为默认值	
//	
//	DMA2_Stream1->PAR=(u32)&DCMI->DR;//外设地址为:DCMI->DR
//	DMA2_Stream1->M0AR=mem0addr;	//mem0addr作为目标地址0
//	DMA2_Stream1->M1AR=mem1addr;	//mem1addr作为目标地址1
//	DMA2_Stream1->NDTR=memsize;		//传输长度为memsize  
//	
//	tempreg|=0<<6;					//外设到存储器模式 
//	tempreg|=1<<8;					//循环模式
//	tempreg|=0<<9;					//外设非增量模式
//	tempreg|=meminc<<10;			//存储器增量模式
//	tempreg|=2<<11;					//外设数据长度:32位
//	tempreg|=memblen<<13;			//存储器位宽,8/16/32bit
//	tempreg|=2<<16;					//高优先级 
//	tempreg|=0<<21;					//外设突发单次传输
//	tempreg|=0<<23;					//存储器突发单次传输
//	tempreg|=1<<25;					//通道1 DCMI通道  
//	if(mem1addr)					//双缓冲的时候,才需要开启
//	{
//		tempreg|=1<<18;				//双缓冲模式
//		tempreg|=1<<4;				//开启传输完成中断
////		MY_NVIC_Init(2,3,DMA2_Stream1_IRQn,2);//抢占1，子优先级3，组2  
//		HAL_NVIC_SetPriority(DMA2_Stream1_IRQn,1,3);
//		
//	}
//	DMA2_Stream1->CR=tempreg;		//设置CR寄存器

		__HAL_RCC_DMA2_CLK_ENABLE();                                    //使能DMA2时钟
    __HAL_LINKDMA(&DCMI_Handler,DMA_Handle,DMADMCI_Handler);        //将DMA与DCMI联系起来
	
    DMADMCI_Handler.Instance=DMA2_Stream1;                          //DMA2数据流1                     
    DMADMCI_Handler.Init.Channel=DMA_CHANNEL_1;                     //通道1
    DMADMCI_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;            //外设到存储器
    DMADMCI_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //外设非增量模式
    DMADMCI_Handler.Init.MemInc=meminc;                             //存储器增量模式
    DMADMCI_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;   //外设数据长度:32位
    DMADMCI_Handler.Init.MemDataAlignment=memblen;                  //存储器数据长度:8/16/32位
    DMADMCI_Handler.Init.Mode=DMA_CIRCULAR;                         //使用循环模式 
    DMADMCI_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //高优先级
    DMADMCI_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //使能FIFO
    DMADMCI_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //使用1/2的FIFO 
    DMADMCI_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //存储器突发传输
    DMADMCI_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //外设突发单次传输 
    HAL_DMA_DeInit(&DMADMCI_Handler);                               //先清除以前的设置
    HAL_DMA_Init(&DMADMCI_Handler);	                                //初始化DMA
    
    //在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
    //这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
    //锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
    //程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOC()先解锁一次DMA。
    __HAL_UNLOCK(&DMADMCI_Handler);
    if(mem1addr==0)    //开启DMA，不使用双缓冲
    {
        HAL_DMA_Start(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,memsize);
    }
    else                //使用双缓冲
    {
        HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,mem1addr,memsize);//开启双缓冲
        __HAL_DMA_ENABLE_IT(&DMADMCI_Handler,DMA_IT_TC);    //开启传输完成中断
        HAL_NVIC_SetPriority(DMA2_Stream1_IRQn,0,0);        //DMA中断优先级
        HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    }
} 
void (*dcmi_rx_callback)(void);//DCMI DMA接收回调函数
//DMA2_Stream1中断服务函数(仅双缓冲模式会用到)
void DMA2_Stream1_IRQHandler(void)
{              
	OSIntEnter();    
//	if(DMA2->LISR&(1<<11))	//DMA2_Steam1,传输完成标志
//	{ 
//		DMA2->LIFCR|=1<<11;	//清除传输完成中断
//      	dcmi_rx_callback();	//执行摄像头接收回调函数,读取数据等操作在这里面处理  
//	}     

	   if(__HAL_DMA_GET_FLAG(&DMADMCI_Handler,DMA_FLAG_TCIF1_5)!=RESET)//DMA传输完成
    {
        __HAL_DMA_CLEAR_FLAG(&DMADMCI_Handler,DMA_FLAG_TCIF1_5);//清除DMA传输完成中断标志位
        dcmi_rx_callback();	//执行摄像头接收回调函数,读取数据等操作在这里面处理
    } 
	OSIntExit(); 				 	 											 
}  
//DCMI初始化
//摄像头模块 ------------ STM32开发板
// OV_D0~D7  ------------  PB8/PB9/PD3/PC11/PC9/PC8/PC7/PC6
// OV_SCL    ------------  PB4
// OV_SDA    ------------  PB3
// OV_VSYNC  ------------  PB7
// OV_HREF   ------------  PH8
// OV_RESET  ------------  PA15
// OV_PCLK   ------------  PA6
// OV_PWDN   ------------  PCF8574_P2
void DCMI_Init(void)
{
//	u32 tempreg=0;
	//设置IO 
//	RCC->AHB1ENR|=1<<0;		//使能外设PORTA时钟
//	RCC->AHB1ENR|=1<<1;		//使能外设PORTB时钟
// 	RCC->AHB1ENR|=1<<2;		//使能外设PORTC时钟
// 	RCC->AHB1ENR|=1<<3;		//使能外设PORTD时钟
//  RCC->AHB1ENR|=1<<7;		//使能外设PORTH时钟     
//	RCC->AHB2ENR|=1<<0;		//能DCMI时钟 

//	GPIO_Set(GPIOA,PIN6,                     GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PA6复用功能输出
//	GPIO_Set(GPIOB,PIN7|PIN8|PIN9,           GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB7/8/9复用功能输出
//	GPIO_Set(GPIOC,PIN6|PIN7|PIN8|PIN9|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PC6/7/8/9/11 复用功能输出
//	GPIO_Set(GPIOD,PIN3,                     GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PD3复用功能输出
//	GPIO_Set(GPIOH,PIN8,                     GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PH8复用功能输出 
		
//	GPIO_AF_Set(GPIOH,8,13);	//PH8,AF13  DCMI_HSYNC
//	GPIO_AF_Set(GPIOA,6,13);	//PA6,AF13  DCMI_PCLK  
//	GPIO_AF_Set(GPIOB,7,13);	//PB7,AF13  DCMI_VSYNC  
// 	GPIO_AF_Set(GPIOC,6,13);	//PC6,AF13  DCMI_D0
// 	GPIO_AF_Set(GPIOC,7,13);	//PC7,AF13  DCMI_D1
// 	GPIO_AF_Set(GPIOC,8,13);	//PC8,AF13  DCMI_D2
// 	GPIO_AF_Set(GPIOC,9,13);	//PC9,AF13  DCMI_D3
// 	GPIO_AF_Set(GPIOC,11,13);	//PC11,AF13 DCMI_D4 
//	GPIO_AF_Set(GPIOD,3,13);	//PD3,AF13  DCMI_D5
//	GPIO_AF_Set(GPIOB,8,13);	//PB8,AF13  DCMI_D6
//	GPIO_AF_Set(GPIOB,9,13);	//PB9,AF13  DCMI_D7

//	//清除原来的设置 
//	DCMI->IER=0x0;
//	DCMI->ICR=0x1F;
//	DCMI->ESCR=0x0;
//	DCMI->ESUR=0x0;
//	DCMI->CWSTRTR=0x0;
//	DCMI->CWSIZER=0x0; 
//	tempreg|=0<<1;		//连续模式
//	tempreg|=0<<2;		//全帧捕获
//	tempreg|=0<<4;		//硬件同步HSYNC,VSYNC
//	tempreg|=1<<5;		//PCLK 上升沿有效
//	tempreg|=0<<6;		//HSYNC 低电平有效
//	tempreg|=0<<7;		//VSYNC 低电平有效
//	tempreg|=0<<8;		//捕获所有的帧
//	tempreg|=0<<10; 	//8位数据格式  
//	DCMI->IER|=1<<0; 	//开启帧中断  
//	tempreg|=1<<14; 	//DCMI使能
//	DCMI->CR=tempreg;	//设置CR寄存器
////	MY_NVIC_Init(2,2,DCMI_IRQn,2);	//抢占1，子优先级2，组2 
//	HAL_NVIC_SetPriority(DCMI_IRQn,1,2);

	  DCMI_Handler.Instance=DCMI;
    DCMI_Handler.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;    //硬件同步HSYNC,VSYNC
    DCMI_Handler.Init.PCKPolarity=DCMI_PCKPOLARITY_RISING;  //PCLK 上升沿有效
    DCMI_Handler.Init.VSPolarity=DCMI_VSPOLARITY_LOW;       //VSYNC 低电平有效
    DCMI_Handler.Init.HSPolarity=DCMI_HSPOLARITY_LOW;       //HSYNC 低电平有效
    DCMI_Handler.Init.CaptureRate=DCMI_CR_ALL_FRAME;        //全帧捕获
    DCMI_Handler.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B; //8位数据格式 
    HAL_DCMI_Init(&DCMI_Handler);                           //初始化DCMI，此函数会开启帧中断 
} 
//DCMI底层驱动，引脚配置，时钟使能，中断配置
//此函数会被HAL_DCMI_Init()调用
//hdcmi:DCMI句柄
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{   
	GPIO_InitTypeDef DCMI_GPIO_Handler;
	
    __HAL_RCC_DCMI_CLK_ENABLE();                //使能DCMI时钟

    __HAL_RCC_GPIOA_CLK_ENABLE();               //使能GPIOA时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();               //使能GPIOB时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();               //使能GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();               //使能GPIOD时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();               //使能GPIOH时钟

	DCMI_GPIO_Handler.Pin = DCMI_PCLK_GPIO_PIN;
	DCMI_GPIO_Handler.Mode = GPIO_MODE_AF_PP;
	DCMI_GPIO_Handler.Pull = GPIO_PULLUP;
	DCMI_GPIO_Handler.Speed = GPIO_SPEED_FREQ_HIGH;
	DCMI_GPIO_Handler.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(DCMI_PCLK_GPIO_PORT, &DCMI_GPIO_Handler);//PA6复用功能输出
	
	DCMI_GPIO_Handler.Pin = DCMI_VSYNC_GPIO_PIN | DCMI_D6_GPIO_PIN | DCMI_D7_GPIO_PIN;
	HAL_GPIO_Init(DCMI_VSYNC_GPIO_PORT, &DCMI_GPIO_Handler);//PB7/8/9复用功能输出
	
	DCMI_GPIO_Handler.Pin = DCMI_D0_GPIO_PIN | DCMI_D1_GPIO_PIN | DCMI_D2_GPIO_PIN \
												 |DCMI_D3_GPIO_PIN | DCMI_D4_GPIO_PIN;
	HAL_GPIO_Init(DCMI_D0_GPIO_PORT, &DCMI_GPIO_Handler);//PC6/7/8/9/11 复用功能输出
	
	DCMI_GPIO_Handler.Pin = DCMI_D5_GPIO_PIN;
	HAL_GPIO_Init(DCMI_D6_GPIO_PORT, &DCMI_GPIO_Handler);//PD3复用功能输出
	
	DCMI_GPIO_Handler.Pin = DCMI_HSYNC_GPIO_PIN;
	HAL_GPIO_Init(DCMI_HSYNC_GPIO_PORT, &DCMI_GPIO_Handler);//PH8复用功能输出 
	
    
	HAL_NVIC_SetPriority(DCMI_IRQn,0,0);        //抢占优先级1，子优先级2
	HAL_NVIC_EnableIRQ(DCMI_IRQn);              //使能DCMI中断
}


//综合实验的bug,必须单独搞一个函数,来启动DMA传输,否则黑屏!!!
void DCMI_DMA_ENABLE(void)
{
	DMA2_Stream1->CR|=1<<0;		//开启DMA2,Stream1  
}

//MDK诡异的问题，编写多了代码莫名其妙摄像头黑屏。
//如果摄像头黑屏，就将DCMI_CAM_BLACK_SCREEN的值取反即可。
#define DCMI_CAM_BLACK_SCREEN	1

//DCMI,启动传输
void DCMI_Start(void)
{  
  	LCD_SetCursor(frec_dev.xoff,frec_dev.yoff); 
	LCD_WriteRAM_Prepare();		//开始写入GRAM  
#if	DCMI_CAM_BLACK_SCREEN==1
//	DCMI_DMA_ENABLE();			//开启DMA2 Stream1
	__HAL_DMA_ENABLE(&DMADMCI_Handler); //使能DMA
#else
	DMA2_Stream1->CR|=1<<0;		//开启DMA2,Stream1  
#endif
//	DCMI->CR|=1<<0; 			//DCMI捕获使能 
	DCMI->CR|=DCMI_CR_CAPTURE;          //DCMI捕获使能
}
//DCMI,关闭传输
void DCMI_Stop(void)
{
//	DCMI->CR&=~(1<<0); 			//DCMI捕获关闭 
//	while(DCMI->CR&0X01);		//等待传输结束 
//	DMA2_Stream1->CR&=~(1<<0);	//关闭DMA2,Stream1 
	
    DCMI->CR&=~(DCMI_CR_CAPTURE);       //关闭捕获
    while(DCMI->CR&0X01);               //等待传输完成
    __HAL_DMA_DISABLE(&DMADMCI_Handler);//关闭DMA	
} 
////////////////////////////////////////////////////////////////////////////////
//以下两个函数,供usmart调用,用于调试代码

////DCMI设置显示窗口
////sx,sy;LCD的起始坐标
////width,height:LCD显示范围.
//void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
//{
//	DCMI_Stop(); 
//	LCD_Clear(WHITE);
//	LCD_Set_Window(sx,sy,width,height);
//	OV5640_OutSize_Set(0,0,width,height);
//  	LCD_SetCursor(0,0);  
//	LCD_WriteRAM_Prepare();		//开始写入GRAM
//	DMA2_Stream1->CR|=1<<0;		//开启DMA2,Stream1 
//	DCMI->CR|=1<<0; 			//DCMI捕获使能  
//}
//   
////通过usmart调试,辅助测试用.
////pclk/hsync/vsync:三个信号的有限电平设置
//void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
//{
//	DCMI->CR=0;
//	DCMI->CR|=pclk<<5;		//PCLK 有效边沿设置
//	DCMI->CR|=hsync<<6;		//HSYNC 有效电平设置
//	DCMI->CR|=vsync<<7;		//VSYNC 有效电平设置	
//	DCMI->CR|=1<<14; 		//DCMI使能
//	DCMI->CR|=1<<0; 		//DCMI捕获使能   
//}

//以下两个函数,供usmart调用,用于调试代码

//DCMI设置显示窗口
//sx,sy;LCD的起始坐标
//width,height:LCD显示范围.
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	DCMI_Stop(); 
	LCD_Clear(WHITE);
	LCD_Set_Window(sx,sy,width,height);
	OV5640_OutSize_Set(0,0,width,height);
	LCD_SetCursor(0,0);  
	LCD_WriteRAM_Prepare();		        //开始写入GRAM  
	__HAL_DMA_ENABLE(&DMADMCI_Handler); //开启DMA2,Stream1 
	DCMI->CR|=DCMI_CR_CAPTURE;          //DCMI捕获使能	
}
   
//通过usmart调试,辅助测试用.
//pclk/hsync/vsync:三个信号的有限电平设置
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
{
    HAL_DCMI_DeInit(&DCMI_Handler);//清除原来的设置
    DCMI_Handler.Instance=DCMI;
    DCMI_Handler.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;//硬件同步HSYNC,VSYNC
    DCMI_Handler.Init.PCKPolarity=pclk<<5;              //PCLK 上升沿有效
    DCMI_Handler.Init.VSPolarity=vsync<<7;              //VSYNC 低电平有效
    DCMI_Handler.Init.HSPolarity=hsync<<6;              //HSYNC 低电平有效
    DCMI_Handler.Init.CaptureRate=DCMI_CR_ALL_FRAME;    //全帧捕获
    DCMI_Handler.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B;//8位数据格式 
    HAL_DCMI_Init(&DCMI_Handler);                       //初始化DCMI
    DCMI_Handler.Instance->CR|=DCMI_MODE_CONTINUOUS;    //持续模式
}






