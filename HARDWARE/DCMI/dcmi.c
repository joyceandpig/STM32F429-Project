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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//DCMI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//����˵��
//V1.1  20160302
//�޸�DCMI_DMA_Init��DCMI_Init������CR�Ĵ����Ĳ�����ʽ,��ֹ��������.
//V1.2  20160515
//�޸�DMA2_Stream1_IRQn�����ȼ�����ֹ���ݸ��ǣ�����ͼƬĩβ��λ.
////////////////////////////////////////////////////////////////////////////////// 	 

DCMI_HandleTypeDef  DCMI_Handler;           //DCMI���
DMA_HandleTypeDef   DMADMCI_Handler;        //DMA���

u8 ov_frame=0;  						//֡��
extern void jpeg_data_process(void);	//JPEG���ݴ����� 
 
//DCMI�жϷ�����
void DCMI_IRQHandler(void)
{    
	OSIntEnter();   
//	if(DCMI->MISR&0X01)		//����һ֡ͼ��
//	{  
//		jpeg_data_process();//jpeg���ݴ���
//		DCMI->ICR|=1<<0;	//���֡�ж�
//		//LED1=!LED1;
//		LED1_STA_TURN();
//		ov_frame++; 
//	}
	HAL_DCMI_IRQHandler(&DCMI_Handler);
	OSIntExit(); 				 
} 

//����һ֡ͼ������
//hdcmi:DCMI���
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	jpeg_data_process();//jpeg���ݴ���
	ov_frame++; 
    //����ʹ��֡�ж�,��ΪHAL_DCMI_IRQHandler()������ر�֡�ж�
  __HAL_DCMI_ENABLE_IT(&DCMI_Handler,DCMI_IT_FRAME);
}
//DCMI DMA����
//mem0addr:�洢����ַ0  ��Ҫ�洢����ͷ���ݵ��ڴ��ַ(Ҳ�����������ַ)
//mem1addr:�洢����ַ1  ��ֻʹ��mem0addr��ʱ��,��ֵ����Ϊ0
//memsize:�洢������    0~65535
//memblen:�洢��λ��    0,8λ,1,16λ,2,32λ
//meminc:�洢��������ʽ,0,������;1,����
void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u8 memblen,u8 meminc)
{ 
//	u32 tempreg=0;
//	RCC->AHB1ENR|=1<<22;			//DMA2ʱ��ʹ�� 
// 	while(DMA2_Stream1->CR&0X01);	//�ȴ�DMA2_Stream1������ 
//	DMA2->LIFCR|=0X3D<<6*1;			//���ͨ��1�������жϱ�־
//	DMA2_Stream1->FCR=0X0000021;	//����ΪĬ��ֵ	
//	
//	DMA2_Stream1->PAR=(u32)&DCMI->DR;//�����ַΪ:DCMI->DR
//	DMA2_Stream1->M0AR=mem0addr;	//mem0addr��ΪĿ���ַ0
//	DMA2_Stream1->M1AR=mem1addr;	//mem1addr��ΪĿ���ַ1
//	DMA2_Stream1->NDTR=memsize;		//���䳤��Ϊmemsize  
//	
//	tempreg|=0<<6;					//���赽�洢��ģʽ 
//	tempreg|=1<<8;					//ѭ��ģʽ
//	tempreg|=0<<9;					//���������ģʽ
//	tempreg|=meminc<<10;			//�洢������ģʽ
//	tempreg|=2<<11;					//�������ݳ���:32λ
//	tempreg|=memblen<<13;			//�洢��λ��,8/16/32bit
//	tempreg|=2<<16;					//�����ȼ� 
//	tempreg|=0<<21;					//����ͻ�����δ���
//	tempreg|=0<<23;					//�洢��ͻ�����δ���
//	tempreg|=1<<25;					//ͨ��1 DCMIͨ��  
//	if(mem1addr)					//˫�����ʱ��,����Ҫ����
//	{
//		tempreg|=1<<18;				//˫����ģʽ
//		tempreg|=1<<4;				//������������ж�
////		MY_NVIC_Init(2,3,DMA2_Stream1_IRQn,2);//��ռ1�������ȼ�3����2  
//		HAL_NVIC_SetPriority(DMA2_Stream1_IRQn,1,3);
//		
//	}
//	DMA2_Stream1->CR=tempreg;		//����CR�Ĵ���

		__HAL_RCC_DMA2_CLK_ENABLE();                                    //ʹ��DMA2ʱ��
    __HAL_LINKDMA(&DCMI_Handler,DMA_Handle,DMADMCI_Handler);        //��DMA��DCMI��ϵ����
	
    DMADMCI_Handler.Instance=DMA2_Stream1;                          //DMA2������1                     
    DMADMCI_Handler.Init.Channel=DMA_CHANNEL_1;                     //ͨ��1
    DMADMCI_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;            //���赽�洢��
    DMADMCI_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                //���������ģʽ
    DMADMCI_Handler.Init.MemInc=meminc;                             //�洢������ģʽ
    DMADMCI_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;   //�������ݳ���:32λ
    DMADMCI_Handler.Init.MemDataAlignment=memblen;                  //�洢�����ݳ���:8/16/32λ
    DMADMCI_Handler.Init.Mode=DMA_CIRCULAR;                         //ʹ��ѭ��ģʽ 
    DMADMCI_Handler.Init.Priority=DMA_PRIORITY_HIGH;                //�����ȼ�
    DMADMCI_Handler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;              //ʹ��FIFO
    DMADMCI_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_HALFFULL; //ʹ��1/2��FIFO 
    DMADMCI_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                //�洢��ͻ������
    DMADMCI_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;             //����ͻ�����δ��� 
    HAL_DMA_DeInit(&DMADMCI_Handler);                               //�������ǰ������
    HAL_DMA_Init(&DMADMCI_Handler);	                                //��ʼ��DMA
    
    //�ڿ���DMA֮ǰ��ʹ��__HAL_UNLOCK()����һ��DMA,��ΪHAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
    //����������һ��ʼҪ��ʹ��__HAL_LOCK()����DMA,������__HAL_LOCK()���жϵ�ǰ��DMA״̬�Ƿ�Ϊ����״̬�������
    //����״̬�Ļ���ֱ�ӷ���HAL_BUSY�������ᵼ�º���HAL_DMA_Statrt()��HAL_DMAEx_MultiBufferStart()������DMA����
    //����ֱ�ӱ�������DMAҲ�Ͳ�������������Ϊ�˱���������������������DMA֮ǰ�ȵ���__HAL_UNLOC()�Ƚ���һ��DMA��
    __HAL_UNLOCK(&DMADMCI_Handler);
    if(mem1addr==0)    //����DMA����ʹ��˫����
    {
        HAL_DMA_Start(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,memsize);
    }
    else                //ʹ��˫����
    {
        HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,mem1addr,memsize);//����˫����
        __HAL_DMA_ENABLE_IT(&DMADMCI_Handler,DMA_IT_TC);    //������������ж�
        HAL_NVIC_SetPriority(DMA2_Stream1_IRQn,0,0);        //DMA�ж����ȼ�
        HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    }
} 
void (*dcmi_rx_callback)(void);//DCMI DMA���ջص�����
//DMA2_Stream1�жϷ�����(��˫����ģʽ���õ�)
void DMA2_Stream1_IRQHandler(void)
{              
	OSIntEnter();    
//	if(DMA2->LISR&(1<<11))	//DMA2_Steam1,������ɱ�־
//	{ 
//		DMA2->LIFCR|=1<<11;	//�����������ж�
//      	dcmi_rx_callback();	//ִ������ͷ���ջص�����,��ȡ���ݵȲ����������洦��  
//	}     

	   if(__HAL_DMA_GET_FLAG(&DMADMCI_Handler,DMA_FLAG_TCIF1_5)!=RESET)//DMA�������
    {
        __HAL_DMA_CLEAR_FLAG(&DMADMCI_Handler,DMA_FLAG_TCIF1_5);//���DMA��������жϱ�־λ
        dcmi_rx_callback();	//ִ������ͷ���ջص�����,��ȡ���ݵȲ����������洦��
    } 
	OSIntExit(); 				 	 											 
}  
//DCMI��ʼ��
//����ͷģ�� ------------ STM32������
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
	//����IO 
//	RCC->AHB1ENR|=1<<0;		//ʹ������PORTAʱ��
//	RCC->AHB1ENR|=1<<1;		//ʹ������PORTBʱ��
// 	RCC->AHB1ENR|=1<<2;		//ʹ������PORTCʱ��
// 	RCC->AHB1ENR|=1<<3;		//ʹ������PORTDʱ��
//  RCC->AHB1ENR|=1<<7;		//ʹ������PORTHʱ��     
//	RCC->AHB2ENR|=1<<0;		//��DCMIʱ�� 

//	GPIO_Set(GPIOA,PIN6,                     GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PA6���ù������
//	GPIO_Set(GPIOB,PIN7|PIN8|PIN9,           GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB7/8/9���ù������
//	GPIO_Set(GPIOC,PIN6|PIN7|PIN8|PIN9|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PC6/7/8/9/11 ���ù������
//	GPIO_Set(GPIOD,PIN3,                     GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PD3���ù������
//	GPIO_Set(GPIOH,PIN8,                     GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);			//PH8���ù������ 
		
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

//	//���ԭ�������� 
//	DCMI->IER=0x0;
//	DCMI->ICR=0x1F;
//	DCMI->ESCR=0x0;
//	DCMI->ESUR=0x0;
//	DCMI->CWSTRTR=0x0;
//	DCMI->CWSIZER=0x0; 
//	tempreg|=0<<1;		//����ģʽ
//	tempreg|=0<<2;		//ȫ֡����
//	tempreg|=0<<4;		//Ӳ��ͬ��HSYNC,VSYNC
//	tempreg|=1<<5;		//PCLK ��������Ч
//	tempreg|=0<<6;		//HSYNC �͵�ƽ��Ч
//	tempreg|=0<<7;		//VSYNC �͵�ƽ��Ч
//	tempreg|=0<<8;		//�������е�֡
//	tempreg|=0<<10; 	//8λ���ݸ�ʽ  
//	DCMI->IER|=1<<0; 	//����֡�ж�  
//	tempreg|=1<<14; 	//DCMIʹ��
//	DCMI->CR=tempreg;	//����CR�Ĵ���
////	MY_NVIC_Init(2,2,DCMI_IRQn,2);	//��ռ1�������ȼ�2����2 
//	HAL_NVIC_SetPriority(DCMI_IRQn,1,2);

	  DCMI_Handler.Instance=DCMI;
    DCMI_Handler.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;    //Ӳ��ͬ��HSYNC,VSYNC
    DCMI_Handler.Init.PCKPolarity=DCMI_PCKPOLARITY_RISING;  //PCLK ��������Ч
    DCMI_Handler.Init.VSPolarity=DCMI_VSPOLARITY_LOW;       //VSYNC �͵�ƽ��Ч
    DCMI_Handler.Init.HSPolarity=DCMI_HSPOLARITY_LOW;       //HSYNC �͵�ƽ��Ч
    DCMI_Handler.Init.CaptureRate=DCMI_CR_ALL_FRAME;        //ȫ֡����
    DCMI_Handler.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B; //8λ���ݸ�ʽ 
    HAL_DCMI_Init(&DCMI_Handler);                           //��ʼ��DCMI���˺����Ὺ��֡�ж� 
} 
//DCMI�ײ��������������ã�ʱ��ʹ�ܣ��ж�����
//�˺����ᱻHAL_DCMI_Init()����
//hdcmi:DCMI���
void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{   
	GPIO_InitTypeDef DCMI_GPIO_Handler;
	
    __HAL_RCC_DCMI_CLK_ENABLE();                //ʹ��DCMIʱ��

    __HAL_RCC_GPIOA_CLK_ENABLE();               //ʹ��GPIOAʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();               //ʹ��GPIOBʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();               //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();               //ʹ��GPIODʱ��
    __HAL_RCC_GPIOH_CLK_ENABLE();               //ʹ��GPIOHʱ��

	DCMI_GPIO_Handler.Pin = DCMI_PCLK_GPIO_PIN;
	DCMI_GPIO_Handler.Mode = GPIO_MODE_AF_PP;
	DCMI_GPIO_Handler.Pull = GPIO_PULLUP;
	DCMI_GPIO_Handler.Speed = GPIO_SPEED_FREQ_HIGH;
	DCMI_GPIO_Handler.Alternate = GPIO_AF13_DCMI;
	HAL_GPIO_Init(DCMI_PCLK_GPIO_PORT, &DCMI_GPIO_Handler);//PA6���ù������
	
	DCMI_GPIO_Handler.Pin = DCMI_VSYNC_GPIO_PIN | DCMI_D6_GPIO_PIN | DCMI_D7_GPIO_PIN;
	HAL_GPIO_Init(DCMI_VSYNC_GPIO_PORT, &DCMI_GPIO_Handler);//PB7/8/9���ù������
	
	DCMI_GPIO_Handler.Pin = DCMI_D0_GPIO_PIN | DCMI_D1_GPIO_PIN | DCMI_D2_GPIO_PIN \
												 |DCMI_D3_GPIO_PIN | DCMI_D4_GPIO_PIN;
	HAL_GPIO_Init(DCMI_D0_GPIO_PORT, &DCMI_GPIO_Handler);//PC6/7/8/9/11 ���ù������
	
	DCMI_GPIO_Handler.Pin = DCMI_D5_GPIO_PIN;
	HAL_GPIO_Init(DCMI_D6_GPIO_PORT, &DCMI_GPIO_Handler);//PD3���ù������
	
	DCMI_GPIO_Handler.Pin = DCMI_HSYNC_GPIO_PIN;
	HAL_GPIO_Init(DCMI_HSYNC_GPIO_PORT, &DCMI_GPIO_Handler);//PH8���ù������ 
	
    
	HAL_NVIC_SetPriority(DCMI_IRQn,0,0);        //��ռ���ȼ�1�������ȼ�2
	HAL_NVIC_EnableIRQ(DCMI_IRQn);              //ʹ��DCMI�ж�
}


//�ۺ�ʵ���bug,���뵥����һ������,������DMA����,�������!!!
void DCMI_DMA_ENABLE(void)
{
	DMA2_Stream1->CR|=1<<0;		//����DMA2,Stream1  
}

//MDK��������⣬��д���˴���Ī����������ͷ������
//�������ͷ�������ͽ�DCMI_CAM_BLACK_SCREEN��ֵȡ�����ɡ�
#define DCMI_CAM_BLACK_SCREEN	1

//DCMI,��������
void DCMI_Start(void)
{  
  	LCD_SetCursor(frec_dev.xoff,frec_dev.yoff); 
	LCD_WriteRAM_Prepare();		//��ʼд��GRAM  
#if	DCMI_CAM_BLACK_SCREEN==1
//	DCMI_DMA_ENABLE();			//����DMA2 Stream1
	__HAL_DMA_ENABLE(&DMADMCI_Handler); //ʹ��DMA
#else
	DMA2_Stream1->CR|=1<<0;		//����DMA2,Stream1  
#endif
//	DCMI->CR|=1<<0; 			//DCMI����ʹ�� 
	DCMI->CR|=DCMI_CR_CAPTURE;          //DCMI����ʹ��
}
//DCMI,�رմ���
void DCMI_Stop(void)
{
//	DCMI->CR&=~(1<<0); 			//DCMI����ر� 
//	while(DCMI->CR&0X01);		//�ȴ�������� 
//	DMA2_Stream1->CR&=~(1<<0);	//�ر�DMA2,Stream1 
	
    DCMI->CR&=~(DCMI_CR_CAPTURE);       //�رղ���
    while(DCMI->CR&0X01);               //�ȴ��������
    __HAL_DMA_DISABLE(&DMADMCI_Handler);//�ر�DMA	
} 
////////////////////////////////////////////////////////////////////////////////
//������������,��usmart����,���ڵ��Դ���

////DCMI������ʾ����
////sx,sy;LCD����ʼ����
////width,height:LCD��ʾ��Χ.
//void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
//{
//	DCMI_Stop(); 
//	LCD_Clear(WHITE);
//	LCD_Set_Window(sx,sy,width,height);
//	OV5640_OutSize_Set(0,0,width,height);
//  	LCD_SetCursor(0,0);  
//	LCD_WriteRAM_Prepare();		//��ʼд��GRAM
//	DMA2_Stream1->CR|=1<<0;		//����DMA2,Stream1 
//	DCMI->CR|=1<<0; 			//DCMI����ʹ��  
//}
//   
////ͨ��usmart����,����������.
////pclk/hsync/vsync:�����źŵ����޵�ƽ����
//void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
//{
//	DCMI->CR=0;
//	DCMI->CR|=pclk<<5;		//PCLK ��Ч��������
//	DCMI->CR|=hsync<<6;		//HSYNC ��Ч��ƽ����
//	DCMI->CR|=vsync<<7;		//VSYNC ��Ч��ƽ����	
//	DCMI->CR|=1<<14; 		//DCMIʹ��
//	DCMI->CR|=1<<0; 		//DCMI����ʹ��   
//}

//������������,��usmart����,���ڵ��Դ���

//DCMI������ʾ����
//sx,sy;LCD����ʼ����
//width,height:LCD��ʾ��Χ.
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	DCMI_Stop(); 
	LCD_Clear(WHITE);
	LCD_Set_Window(sx,sy,width,height);
	OV5640_OutSize_Set(0,0,width,height);
	LCD_SetCursor(0,0);  
	LCD_WriteRAM_Prepare();		        //��ʼд��GRAM  
	__HAL_DMA_ENABLE(&DMADMCI_Handler); //����DMA2,Stream1 
	DCMI->CR|=DCMI_CR_CAPTURE;          //DCMI����ʹ��	
}
   
//ͨ��usmart����,����������.
//pclk/hsync/vsync:�����źŵ����޵�ƽ����
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
{
    HAL_DCMI_DeInit(&DCMI_Handler);//���ԭ��������
    DCMI_Handler.Instance=DCMI;
    DCMI_Handler.Init.SynchroMode=DCMI_SYNCHRO_HARDWARE;//Ӳ��ͬ��HSYNC,VSYNC
    DCMI_Handler.Init.PCKPolarity=pclk<<5;              //PCLK ��������Ч
    DCMI_Handler.Init.VSPolarity=vsync<<7;              //VSYNC �͵�ƽ��Ч
    DCMI_Handler.Init.HSPolarity=hsync<<6;              //HSYNC �͵�ƽ��Ч
    DCMI_Handler.Init.CaptureRate=DCMI_CR_ALL_FRAME;    //ȫ֡����
    DCMI_Handler.Init.ExtendedDataMode=DCMI_EXTEND_DATA_8B;//8λ���ݸ�ʽ 
    HAL_DCMI_Init(&DCMI_Handler);                       //��ʼ��DCMI
    DCMI_Handler.Instance->CR|=DCMI_MODE_CONTINUOUS;    //����ģʽ
}






