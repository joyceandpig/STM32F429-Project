#include "dma.h"																	   	  
#include "delay.h"		 
#include "stm32f4xx_hal_dma.h"


//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//DMA 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
DMA_HandleTypeDef DMA_Handler;

//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,范围:0~7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{ 
//	DMA_TypeDef *DMAx;
//	u8 streamx;
//	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
//	{
//		DMAx=DMA2;
//		RCC->AHB1ENR|=1<<22;//DMA2时钟使能 
//	}else 
//	{
//		DMAx=DMA1; 
// 		RCC->AHB1ENR|=1<<21;//DMA1时钟使能 
//	}
//	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
//	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//得到stream通道号
// 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//清空之前该stream上的所有中断标志
//	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //清空之前该stream上的所有中断标志
//	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//清空之前该stream上的所有中断标志
//	else DMAx->LIFCR|=0X3D<<6*streamx;						//清空之前该stream上的所有中断标志
//	
//	DMA_Streamx->PAR=par;		//DMA外设地址
//	DMA_Streamx->M0AR=mar;		//DMA 存储器0地址
//	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址
//	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
//	
//	DMA_Streamx->CR|=1<<6;		//存储器到外设模式
//	DMA_Streamx->CR|=0<<8;		//非循环模式(即使用普通模式)
//	DMA_Streamx->CR|=0<<9;		//外设非增量模式
//	DMA_Streamx->CR|=1<<10;		//存储器增量模式
//	DMA_Streamx->CR|=0<<11;		//外设数据长度:8位
//	DMA_Streamx->CR|=0<<13;		//存储器数据长度:8位
//	DMA_Streamx->CR|=1<<16;		//中等优先级
//	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
//	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
//	DMA_Streamx->CR|=(u32)chx<<25;//通道选择
//	//DMA_Streamx->FCR=0X21;	//FIFO控制寄存器

	DMA_TypeDef *DMAx;
	
	DMA_Handler.Instance = DMA_Streamx;
	
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx = DMA2;
		__HAL_RCC_DMA2_CLK_ENABLE();//DMA2时钟使能 
	}else{ 
		DMAx = DMA1;
 		__HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
	}
	while(__HAL_DMA_GET_IT_SOURCE(&DMA_Handler,HAL_DMA_STATE_READY));		//等待DMA2_Stream0可配置
		HAL_DMA_DeInit(&DMA_Handler);//先全部复位dma寄存器值   

	DMA_Handler.Init.Channel = (((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//得到stream通道号
	
 	if(DMA_Handler.Init.Channel>=6){
		__HAL_DMA_TypeDef_SET_HIFCR(DMAx, 0X3D<<(6*(DMA_Handler.Init.Channel-6)+16));//清空之前该stream上的所有中断标志
	}else if(DMA_Handler.Init.Channel>=4){
		__HAL_DMA_TypeDef_SET_HIFCR(DMAx,0X3D<<6*(DMA_Handler.Init.Channel-4));    //清空之前该stream上的所有中断标志
	}else if(DMA_Handler.Init.Channel>=2){
		__HAL_DMA_TypeDef_SET_LIFCR(DMAx,0X3D<<(6*(DMA_Handler.Init.Channel-2)+16));//清空之前该stream上的所有中断标志
	}else {
		__HAL_DMA_TypeDef_SET_LIFCR(DMAx,0X3D<<6*DMA_Handler.Init.Channel);						//清空之前该stream上的所有中断标志
	}
	DMA_Handler.Instance->PAR = par;	//DMA外设地址
	DMA_Handler.Instance->M0AR = mar;	//DMA 存储器0地址
	__HAL_DMA_SET_COUNTER(&DMA_Handler,ndtr);	//DMA 存储器0地址
	
	DMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;		//存储器到外设模式
	DMA_Handler.Init.Mode = DMA_NORMAL;		//非循环模式(即使用普通模式)
	DMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;		//外设非增量模式
	DMA_Handler.Init.MemInc = DMA_MINC_ENABLE;		//存储器增量模式
	DMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		//外设数据长度:8位
	DMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;		//存储器数据长度:8位
	DMA_Handler.Init.Priority = DMA_PRIORITY_MEDIUM;		//中等优先级
	DMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;//存储器突发单次传输
	DMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;//外设突发单次传输
	HAL_DMA_Init(&DMA_Handler);
} 
//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
//	DMA_Streamx->CR&=~(1<<0); 	//关闭DMA传输 
//	while(DMA_Streamx->CR&0X1);	//确保DMA可以被设置  
//	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址 
//	DMA_Streamx->CR|=1<<0;		//开启DMA传输
	
	__HAL_DMA_DISABLE(&DMA_Handler);//关闭DMA传输 
	while(__HAL_DMA_GET_IT_SOURCE(&DMA_Handler,HAL_DMA_STATE_READY));		//等待DMA_Stream可配置 	
	__HAL_DMA_SET_COUNTER(&DMA_Handler,ndtr);	//设置传输长度
	__HAL_DMA_ENABLE(&DMA_Handler);				//开启DMA传输 
}	  

 

























