#include "spblcd.h"
#include "spb.h"
#include "dma.h"
#include "lcd.h"
#include "ltdc.h"
#include "delay.h"
#include "malloc.h"
#include "usart.h"
#include "stm32f4xx_hal_dma.h"


//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//SPBLCD ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/20
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved				
//********************************************************************************
//�޸�˵�� 
//V1.1  20160320 
//�޸�Ϊ֧��3ҳ����.
//////////////////////////////////////////////////////////////////////////////////	  


u16 *sramlcdbuf;							//SRAM LCD BUFFER,����ͼƬ�Դ��� 
DMA_HandleTypeDef SPIDMA_Handler;

//��ָ��λ�û���.
//x,y:����
//color:��ɫ.
void slcd_draw_point(u16 x,u16 y,u16 color)
{	 
	if(lcdltdc.pwidth)x=spbdev.spbwidth*SPB_PAGE_NUM+spbdev.spbahwidth*2-x-1;
	sramlcdbuf[y+x*spbdev.spbheight]=color;
}
//��ȡָ��λ�õ����ɫֵ
//x,y:����
//����ֵ:��ɫ
u16 slcd_read_point(u16 x,u16 y)
{
	if(lcdltdc.pwidth)x=spbdev.spbwidth*SPB_PAGE_NUM+spbdev.spbahwidth*2-x-1;
	return sramlcdbuf[y+x*spbdev.spbheight];
} 
//�����ɫ
//x,y:��ʼ����
//width��height����Ⱥ͸߶�
//*color����ɫ����
void slcd_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color)
{   
	u16 i,j; 
 	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			slcd_draw_point(x+j,y+i,*color++);
		}	
	}	
} 
//SRAM --> LCD_RAM dma����
//16λ,�ⲿSRAM���䵽LCD_RAM. 
void slcd_dma_init(void)
{  
//	RCC->AHB1ENR|=1<<22;		//DMA2ʱ��ʹ��    
//	while(DMA2_Stream0->CR&0X01);//�ȴ�DMA2_Stream0������ 
//	DMA2->LIFCR|=0X3D<<6*0;		//���ͨ��0�������жϱ�־
//	DMA2_Stream0->FCR=0X0000021;//����ΪĬ��ֵ	
//	
//	DMA2_Stream0->PAR=0;		//�ݲ�����
//	DMA2_Stream0->M0AR=(u32)&LCD->LCD_RAM;	//Ŀ���ַΪLCD_RAM
//	DMA2_Stream0->M1AR=0;		//��������
//	DMA2_Stream0->NDTR=0;		//��ʱ���ó���Ϊ0
//	DMA2_Stream0->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
//	DMA2_Stream0->CR|=2<<6;		//�洢�����洢��ģʽ 
//	DMA2_Stream0->CR|=0<<8;		//��ͨģʽ
//	DMA2_Stream0->CR|=1<<9;		//��������ģʽ
//	DMA2_Stream0->CR|=0<<10;	//�洢��������ģʽ
//	DMA2_Stream0->CR|=1<<11;	//�������ݳ���:16λ
//	DMA2_Stream0->CR|=1<<13;	//�洢�����ݳ���:16λ
//	DMA2_Stream0->CR|=0<<16;	//�����ȼ�
//	DMA2_Stream0->CR|=0<<18;	//������ģʽ
//	DMA2_Stream0->CR|=0<<21;	//����ͻ�����δ���
//	DMA2_Stream0->CR|=0<<23;	//�洢��ͻ�����δ���
//	DMA2_Stream0->CR|=0<<25;	//ѡ��ͨ��0 
//	
//	DMA2_Stream0->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
//	DMA2_Stream0->FCR&=~(3<<0);	//��FIFO ����  
	

	__HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ�� 
	while(__HAL_DMA_GET_IT_SOURCE(&SPIDMA_Handler,HAL_DMA_STATE_READY));		//�ȴ�DMA2_Stream0������
	SPIDMA_Handler.Instance = DMA2_Stream0;//ѡ��������0
	
	HAL_DMA_DeInit(&SPIDMA_Handler);//��ȫ����λdma�Ĵ���ֵ  
	__HAL_DMA_Stream_SET_M0AR(&SPIDMA_Handler,(u32)&LCD->LCD_RAM);//Ŀ���ַΪLCD_RAM
	SPIDMA_Handler.Init.Channel = DMA_CHANNEL_0;//ѡ��ͨ��0 
	SPIDMA_Handler.Init.Direction = DMA_MEMORY_TO_MEMORY;//�洢�����洢��ģʽ 
	SPIDMA_Handler.Init.Mode = DMA_NORMAL;//��ͨģʽ
	SPIDMA_Handler.Init.PeriphInc = DMA_PINC_ENABLE;//��������ģʽ
	SPIDMA_Handler.Init.MemInc = DMA_MINC_DISABLE;//�洢��������ģʽ
	SPIDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;//�������ݳ���:16λ
	SPIDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;//�洢�����ݳ���:16λ
	SPIDMA_Handler.Init.Priority = DMA_PRIORITY_LOW;//�����ȼ�
	SPIDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;//��ʹ��FIFOģʽ
	SPIDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;//��FIFO ����  
	SPIDMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;//�洢��ͻ�����δ���
	SPIDMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;//����ͻ�����δ���
	
	HAL_DMA_Init(&SPIDMA_Handler);
} 

//����һ��SPI��LCD��DMA�Ĵ���
//x:��ʼ�����ַ���
void slcd_dma_enable(u32 x)
{	  
	u32 lcdsize=spbdev.spbwidth*spbdev.spbheight;
	u32 dmatransfered=0;
//	while(lcdsize)
//	{ 
//		DMA2_Stream0->CR&=~(1<<0);			//�ر�DMA���� 
//		while(DMA2_Stream0->CR&0X01);		//�ȴ�DMA2_Stream0������ 
//		DMA2->LIFCR|=1<<5;					//����ϴεĴ�����ɱ��
//		if(lcdsize>SLCD_DMA_MAX_TRANS)
//		{
//			lcdsize-=SLCD_DMA_MAX_TRANS;
//			DMA2_Stream0->NDTR=SLCD_DMA_MAX_TRANS;	//���ô��䳤��
//		}else
//		{
//			DMA2_Stream0->NDTR=lcdsize;	//���ô��䳤��
//			lcdsize=0;
//		}	
//		DMA2_Stream0->PAR=(u32)(sramlcdbuf+x*spbdev.spbheight+dmatransfered);	
//		dmatransfered+=SLCD_DMA_MAX_TRANS;		
//		DMA2_Stream0->CR|=1<<0;				//����DMA���� 		
//		while((DMA2->LISR&(1<<5))==0);		//�ȴ�������� 
//	} 
//	DMA2_Stream0->CR&=~(1<<0);				//�ر�DMA���� 

	while(lcdsize)
	{
		__HAL_DMA_DISABLE(&SPIDMA_Handler);//�ر�DMA���� 
		while(__HAL_DMA_GET_IT_SOURCE(&SPIDMA_Handler,HAL_DMA_STATE_READY));		//�ȴ�DMA2_Stream0������ 	
		__HAL_DMA_CLEAR_FLAG(&SPIDMA_Handler,DMA2D_IFSR_CCEIF);	//����ϴεĴ�����ɱ��
		if(lcdsize>SLCD_DMA_MAX_TRANS)
		{
			lcdsize-=SLCD_DMA_MAX_TRANS;
			__HAL_DMA_SET_COUNTER(&SPIDMA_Handler,SLCD_DMA_MAX_TRANS);	//���ô��䳤��
		}else
		{
			__HAL_DMA_SET_COUNTER(&SPIDMA_Handler,lcdsize);	//���ô��䳤��
			lcdsize=0;
		}	
		__HAL_DMA_Stream_SET_PAR(&SPIDMA_Handler,(u32)(sramlcdbuf+x*spbdev.spbheight+dmatransfered));
		dmatransfered+=SLCD_DMA_MAX_TRANS;		
		__HAL_DMA_ENABLE(&SPIDMA_Handler);				//����DMA���� 		
		while(__HAL_DMA_GET_FLAG(&SPIDMA_Handler,DMA2D_ISR_CEIF)==0);		//�ȴ��������
	}
	__HAL_DMA_DISABLE(&SPIDMA_Handler);//�ر�DMA���� 
}
//��ʾһ֡,������һ��spi��lcd����ʾ.
//x:����ƫ����
void slcd_frame_show(u32 x)
{  
	if(lcdltdc.pwidth)
	{
		x=spbdev.spbwidth*(SPB_PAGE_NUM-1)+spbdev.spbahwidth*2-x;
		LTDC_Color_Fill(0,spbdev.stabarheight,spbdev.spbwidth-1,spbdev.stabarheight+spbdev.spbheight-1,sramlcdbuf+x*spbdev.spbheight);
	}else
	{
		LCD_Scan_Dir(U2D_L2R);		//����ɨ�跽��  
		if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X6804)
		{
			LCD_Set_Window(spbdev.stabarheight,0,spbdev.spbheight,spbdev.spbwidth);
			LCD_SetCursor(spbdev.stabarheight,0);//���ù��λ�� 
		}else
		{
			LCD_Set_Window(0,spbdev.stabarheight,spbdev.spbwidth,spbdev.spbheight);
			if(lcddev.id!=0X1963)LCD_SetCursor(0,spbdev.stabarheight);//���ù��λ�� 		
		}
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		slcd_dma_enable(x);
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ�Ϸ���
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ�Ĭ�ϴ��ڴ�С
	} 
}
 





