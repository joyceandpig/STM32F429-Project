#include "dma.h"																	   	  
#include "delay.h"		 
#include "stm32f4xx_hal_dma.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//DMA ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/27
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
DMA_HandleTypeDef DMA_Handler;

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:0~7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{ 
//	DMA_TypeDef *DMAx;
//	u8 streamx;
//	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
//	{
//		DMAx=DMA2;
//		RCC->AHB1ENR|=1<<22;//DMA2ʱ��ʹ�� 
//	}else 
//	{
//		DMAx=DMA1; 
// 		RCC->AHB1ENR|=1<<21;//DMA1ʱ��ʹ�� 
//	}
//	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
//	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//�õ�streamͨ����
// 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//���֮ǰ��stream�ϵ������жϱ�־
//	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //���֮ǰ��stream�ϵ������жϱ�־
//	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//���֮ǰ��stream�ϵ������жϱ�־
//	else DMAx->LIFCR|=0X3D<<6*streamx;						//���֮ǰ��stream�ϵ������жϱ�־
//	
//	DMA_Streamx->PAR=par;		//DMA�����ַ
//	DMA_Streamx->M0AR=mar;		//DMA �洢��0��ַ
//	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ
//	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
//	
//	DMA_Streamx->CR|=1<<6;		//�洢��������ģʽ
//	DMA_Streamx->CR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
//	DMA_Streamx->CR|=0<<9;		//���������ģʽ
//	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
//	DMA_Streamx->CR|=0<<11;		//�������ݳ���:8λ
//	DMA_Streamx->CR|=0<<13;		//�洢�����ݳ���:8λ
//	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
//	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
//	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
//	DMA_Streamx->CR|=(u32)chx<<25;//ͨ��ѡ��
//	//DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���

	DMA_TypeDef *DMAx;
	
	DMA_Handler.Instance = DMA_Streamx;
	
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx = DMA2;
		__HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ�� 
	}else{ 
		DMAx = DMA1;
 		__HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
	}
	while(__HAL_DMA_GET_IT_SOURCE(&DMA_Handler,HAL_DMA_STATE_READY));		//�ȴ�DMA2_Stream0������
		HAL_DMA_DeInit(&DMA_Handler);//��ȫ����λdma�Ĵ���ֵ   

	DMA_Handler.Init.Channel = (((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//�õ�streamͨ����
	
 	if(DMA_Handler.Init.Channel>=6){
		__HAL_DMA_TypeDef_SET_HIFCR(DMAx, 0X3D<<(6*(DMA_Handler.Init.Channel-6)+16));//���֮ǰ��stream�ϵ������жϱ�־
	}else if(DMA_Handler.Init.Channel>=4){
		__HAL_DMA_TypeDef_SET_HIFCR(DMAx,0X3D<<6*(DMA_Handler.Init.Channel-4));    //���֮ǰ��stream�ϵ������жϱ�־
	}else if(DMA_Handler.Init.Channel>=2){
		__HAL_DMA_TypeDef_SET_LIFCR(DMAx,0X3D<<(6*(DMA_Handler.Init.Channel-2)+16));//���֮ǰ��stream�ϵ������жϱ�־
	}else {
		__HAL_DMA_TypeDef_SET_LIFCR(DMAx,0X3D<<6*DMA_Handler.Init.Channel);						//���֮ǰ��stream�ϵ������жϱ�־
	}
	DMA_Handler.Instance->PAR = par;	//DMA�����ַ
	DMA_Handler.Instance->M0AR = mar;	//DMA �洢��0��ַ
	__HAL_DMA_SET_COUNTER(&DMA_Handler,ndtr);	//DMA �洢��0��ַ
	
	DMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;		//�洢��������ģʽ
	DMA_Handler.Init.Mode = DMA_NORMAL;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	DMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;		//���������ģʽ
	DMA_Handler.Init.MemInc = DMA_MINC_ENABLE;		//�洢������ģʽ
	DMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		//�������ݳ���:8λ
	DMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;		//�洢�����ݳ���:8λ
	DMA_Handler.Init.Priority = DMA_PRIORITY_MEDIUM;		//�е����ȼ�
	DMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;//�洢��ͻ�����δ���
	DMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;//����ͻ�����δ���
	HAL_DMA_Init(&DMA_Handler);
} 
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
//	DMA_Streamx->CR&=~(1<<0); 	//�ر�DMA���� 
//	while(DMA_Streamx->CR&0X1);	//ȷ��DMA���Ա�����  
//	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ 
//	DMA_Streamx->CR|=1<<0;		//����DMA����
	
	__HAL_DMA_DISABLE(&DMA_Handler);//�ر�DMA���� 
	while(__HAL_DMA_GET_IT_SOURCE(&DMA_Handler,HAL_DMA_STATE_READY));		//�ȴ�DMA_Stream������ 	
	__HAL_DMA_SET_COUNTER(&DMA_Handler,ndtr);	//���ô��䳤��
	__HAL_DMA_ENABLE(&DMA_Handler);				//����DMA���� 
}	  

 

























