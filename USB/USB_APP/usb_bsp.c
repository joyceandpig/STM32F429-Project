#include "usb_bsp.h"
#include "sys.h"  
#include "delay.h" 
#include "usart.h" 
//#include "pcf8574.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//USB-BSP 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/21
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   


  
//USB OTG 底层IO初始化
//pdev:USB OTG内核结构体指针
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
//	RCC->AHB1ENR|=1<<0;    		//使能PORTA时钟	    
// 	RCC->AHB2ENR|=1<<7;    		//使能USB OTG时钟	
//	GPIO_Set(GPIOA,3<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);	//PA11/12复用功能输出	
//	GPIO_Set(GPIOA,PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);	//PA15推挽输出	
////  	PCF8574_WriteBit(USB_PWR_IO,1);//开启USB HOST电源供电
//   	GPIO_AF_Set(GPIOA,11,10);	//PA11,AF10(USB)
// 	GPIO_AF_Set(GPIOA,12,10);	//PA12,AF10(USB)
	
	GPIO_InitTypeDef USB_PORT_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();       //使能PORTA时钟	  
	__HAL_RCC_USB_OTG_FS_CLK_ENABLE();  //使能USB OTG时钟	

	USB_PORT_Initure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
	USB_PORT_Initure.Mode = GPIO_MODE_AF_PP;
	USB_PORT_Initure.Pull = GPIO_NOPULL;
	USB_PORT_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	USB_PORT_Initure.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOA, &USB_PORT_Initure);//PA10、11复用功能输出

	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_15);
	USB_PORT_Initure.Pin = GPIO_PIN_15;
	USB_PORT_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	USB_PORT_Initure.Pull = GPIO_NOPULL;
	USB_PORT_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &USB_PORT_Initure);//PA10、11复用功能输出
}

//USB OTG 中断设置,开启USB FS中断
//pdev:USB OTG内核结构体指针
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
//   	MY_NVIC_Init(0,3,OTG_FS_IRQn,2);	//优先级设置为抢占0,子优先级3，组2	 
	HAL_NVIC_SetPriority(OTG_FS_IRQn,0,3);        //抢占优先级0，子优先级3
}

//USB OTG 中断设置,开启USB FS中断
//pdev:USB OTG内核结构体指针
void USB_OTG_BSP_DisableInterrupt(void)
{ 
}
//USB OTG 端口供电设置(本例程未用到)
//pdev:USB OTG内核结构体指针
//state:0,断电;1,上电
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{ 
}
//USB_OTG 端口供电IO配置(本例程未用到)
//pdev:USB OTG内核结构体指针
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{ 
} 
//USB_OTG us级延时函数
//本例程采用SYSTEM文件夹的delay.c里面的delay_us函数实现
//官方例程采用的是定时器2来实现的.
//usec:要延时的us数.
void USB_OTG_BSP_uDelay (const uint32_t usec)
{ 
   	delay_us(usec);
}
//USB_OTG ms级延时函数
//本例程采用SYSTEM文件夹的delay.c里面的delay_ms函数实现
//官方例程采用的是定时器2来实现的.
//msec:要延时的ms数.
void USB_OTG_BSP_mDelay (const uint32_t msec)
{  
	delay_ms(msec);
}






















