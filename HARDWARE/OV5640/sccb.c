#include "sys.h"
#include "sccb.h"
#include "delay.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//OV系列摄像头 SCCB 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void SCCB_Delay(void)
{
	delay_us(5);
}
//初始化SCCB接口 
void SCCB_Init(void)
{											      	 
//	RCC->AHB1ENR|=1<<1;    	//使能外设PORTB时钟	
////	GPIO_Set(GPIOB,PIN3|PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PB3/4推挽输出
//	GPIOB->ODR|=3<<3;			//PB3/4输出1  
	
	   GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4; //PB3,4
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	
	SCCB_SDA_OUT();	   
}			 

//SCCB起始信号
//当时钟为高的时候,数据线的高到低,为SCCB起始信号
//在激活状态下,SDA和SCL均为低电平
void SCCB_Start(void)
{
    SCCB_SDA_SET();     //数据线高电平	   
    SCCB_SCL_SET();	    //在时钟线高的时候数据线由高至低
    SCCB_Delay();  
    SCCB_SDA_CLEAR();
    SCCB_Delay();	 
    SCCB_SCL_CLEAR();	    //数据线恢复低电平，单操作函数必要	  
}

//SCCB停止信号
//当时钟为高的时候,数据线的低到高,为SCCB停止信号
//空闲状况下,SDA,SCL均为高电平
void SCCB_Stop(void)
{
    SCCB_SDA_CLEAR();
    SCCB_Delay();	 
    SCCB_SCL_SET();	    
    SCCB_Delay();
    SCCB_SDA_SET();    
    SCCB_Delay();
}  
//产生NA信号
void SCCB_No_Ack(void)
{
	SCCB_Delay();
  SCCB_SDA_SET();    
    SCCB_SCL_SET();    
	SCCB_Delay();
    SCCB_SCL_CLEAR();    
	SCCB_Delay();
    SCCB_SDA_CLEAR();    
	SCCB_Delay();
}
//SCCB,写入一个字节
//返回值:0,成功;1,失败. 
u8 SCCB_WR_Byte(u8 dat)
{
	u8 j,res;	 
	for(j=0;j<8;j++) //循环8次发送数据
	{
		if(dat&0x80)SCCB_SDA_SET();
		else SCCB_SDA_CLEAR();
		dat<<=1;
		SCCB_Delay();
		SCCB_SCL_SET();
		SCCB_Delay();
		SCCB_SCL_CLEAR();		   
	}			 
	SCCB_SDA_IN();		//设置SDA为输入 
	SCCB_Delay();
	SCCB_SCL_SET();			//接收第九位,以判断是否发送成功
	SCCB_Delay();
	if(SCCB_READ_SDA_STA())res=1;  //SDA=1发送失败，返回1
	else res=0;         //SDA=0发送成功，返回0
	SCCB_SCL_CLEAR();		 
	SCCB_SDA_OUT();		//设置SDA为输出    
	return res;  
}	 
//SCCB 读取一个字节
//在SCL的上升沿,数据锁存
//返回值:读到的数据
u8 SCCB_RD_Byte(void)
{
	u8 temp=0,j;    
	SCCB_SDA_IN();		//设置SDA为输入  
	for(j=8;j>0;j--) 	//循环8次接收数据
	{		     	  
		SCCB_Delay();
		SCCB_SCL_SET();
		temp=temp<<1;
		if(SCCB_READ_SDA_STA())temp++;   
		SCCB_Delay();
		SCCB_SCL_CLEAR();
	}	
	SCCB_SDA_OUT();		//设置SDA为输出    
	return temp;
} 							    
 
\












