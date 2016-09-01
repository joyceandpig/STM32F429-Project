#include "sai.h"  
#include "includes.h"	 	//ucos ʹ��	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//SAI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/11
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved	
//********************************************************************************
//V1.2 20160111
//����SAIB_Init��SAIA_RX_DMA_Init��SAI_Rec_Start��SAI_Rec_Stop�Ⱥ���		
//V1.1 20160602
//SAIA_RX_DMA_Init��ΪSAIB_RX_DMA_Init								   
////////////////////////////////////////////////////////////////////////////////// 	

//SAI Block A��ʼ��,I2S,�����ֱ�׼
//mode:����ģʽ,00,��������;01,��������;10,�ӷ�����;11,�ӽ�����
//cpol:0,ʱ���½���ѡͨ;1,ʱ��������ѡͨ
//datalen:���ݴ�С,0/1,δ�õ�,2,8λ;3,10λ;4,16λ;5,20λ;6,24λ;7,32λ.
void SAIA_Init(u8 mode,u8 cpol,u8 datalen)
{ 
	u32 tempreg=0;
	RCC->APB2ENR|=1<<22;		//ʹ��SAI1ʱ��
	RCC->APB2RSTR|=1<<22;		//��λSAI1
	RCC->APB2RSTR&=~(1<<22);	//������λ  
	tempreg|=mode<<0;			//����SAI1����ģʽ
	tempreg|=0<<2;				//����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
	tempreg|=datalen<<5;		//�������ݴ�С
	tempreg|=0<<8;				//����MSBλ����
	tempreg|=(u16)cpol<<9;		//������ʱ�ӵ�����/�½���ѡͨ
	tempreg|=0<<10;				//��Ƶģ���첽
	tempreg|=0<<12;				//������ģʽ
	tempreg|=1<<13;				//����������Ƶģ�����
	tempreg|=0<<19;				//ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
	SAI1_Block_A->CR1=tempreg;	//����CR1�Ĵ���
	
	tempreg=(64-1)<<0;			//����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
	tempreg|=(32-1)<<8;			//����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
	tempreg|=1<<16;				///FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
	tempreg|=0<<17;				///FS�͵�ƽ��Ч(�½���)
	tempreg|=1<<18;				//��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	 
	SAI1_Block_A->FRCR=tempreg;
	
	tempreg=0<<0;				//slotƫ��(FBOFF)Ϊ0
	tempreg|=2<<6;				//slot��СΪ32λ
	tempreg|=(2-1)<<8;			//slot��Ϊ2��
	tempreg|=(1<<17)|(1<<16);	//ʹ��slot0��slot1
	SAI1_Block_A->SLOTR=tempreg;//����slot 
	
	SAI1_Block_A->CR2=1<<0;		//����FIFO��ֵ:1/4 FIFO  
	SAI1_Block_A->CR2|=1<<3;	//FIFOˢ�� 
} 
//SAI Block B��ʼ��,I2S,�����ֱ�׼,ͬ��ģʽ
//mode:����ģʽ,00,��������;01,��������;10,�ӷ�����;11,�ӽ�����
//cpol:0,ʱ���½���ѡͨ;1,ʱ��������ѡͨ
//datalen:���ݴ�С,0/1,δ�õ�,2,8λ;3,10λ;4,16λ;5,20λ;6,24λ;7,32λ.
void SAIB_Init(u8 mode,u8 cpol,u8 datalen)
{ 
	u32 tempreg=0;  
	tempreg|=mode<<0;			//����SAI1����ģʽ
	tempreg|=0<<2;				//����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
	tempreg|=datalen<<5;		//�������ݴ�С
	tempreg|=0<<8;				//����MSBλ����
	tempreg|=(u16)cpol<<9;		//������ʱ�ӵ�����/�½���ѡͨ
	tempreg|=1<<10;				//ʹ��ͬ��ģʽ
	tempreg|=0<<12;				//������ģʽ
	tempreg|=1<<13;				//����������Ƶģ�����
	SAI1_Block_B->CR1=tempreg;	//����CR1�Ĵ���
	
	tempreg=(64-1)<<0;			//����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
	tempreg|=(32-1)<<8;			//����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
	tempreg|=1<<16;				///FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
	tempreg|=0<<17;				///FS�͵�ƽ��Ч(�½���)
	tempreg|=1<<18;				//��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	 
	SAI1_Block_B->FRCR=tempreg;
	
	tempreg=0<<0;				//slotƫ��(FBOFF)Ϊ0
	tempreg|=2<<6;				//slot��СΪ32λ
	tempreg|=(2-1)<<8;			//slot��Ϊ2��
	tempreg|=(1<<17)|(1<<16);	//ʹ��slot0��slot1
	SAI1_Block_B->SLOTR=tempreg;//����slot 
	
	SAI1_Block_B->CR2=1<<0;		//����FIFO��ֵ:1/4 FIFO  
	SAI1_Block_B->CR2|=1<<3;	//FIFOˢ��  
	SAI1_Block_B->CR1|=1<<17;	//ʹ��DMA
	SAI1_Block_B->CR1|=1<<16;	//ʹ��SAI1 Block B
} 
//SAI Block A����������
//�����ʼ��㹫ʽ:
//MCKDIV!=0: Fs=SAI_CK_x/[512*MCKDIV]
//MCKDIV==0: Fs=SAI_CK_x/256
//SAI_CK_x=(HSE/pllm)*PLLI2SN/PLLI2SQ/(PLLI2SDIVQ+1)
//һ��HSE=25Mhz
//pllm:��Stm32_Clock_Init���õ�ʱ��ȷ����һ����25
//PLLI2SN:һ����192~432 
//PLLI2SQ:2~15 
//PLLI2SDIVQ:0~31
//MCKDIV:0~15 
//SAI A��Ƶϵ����@pllm=8,HSE=25Mhz,��vco����Ƶ��Ϊ1Mhz 
const u16 SAI_PSC_TBL[][5]=
{
	{800 ,344,7,0,12},	//8Khz������
	{1102,429,2,18,2},	//11.025Khz������ 
	{1600,344,7, 0,6},	//16Khz������
	{2205,429,2,18,1},	//22.05Khz������
	{3200,344,7, 0,3},	//32Khz������
	{4410,429,2,18,0},	//44.1Khz������
	{4800,344,7, 0,2},	//48Khz������
	{8820,271,2, 2,1},	//88.2Khz������
	{9600,344,7, 0,1},	//96Khz������
	{17640,271,2,2,0},	//176.4Khz������ 
	{19200,344,7,0,0},	//192Khz������
};  
//����SAIA�Ĳ�����(@MCKEN)
//samplerate:������,��λ:Hz
//����ֵ:0,���óɹ�;1,�޷�����.
u8 SAIA_SampleRate_Set(u32 samplerate)
{ 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//��С10��   
	for(i=0;i<(sizeof(SAI_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if(samplerate==SAI_PSC_TBL[i][0])break;
	}
	RCC->CR&=~(1<<26);						//�ȹر�PLLI2S  
	if(i==(sizeof(SAI_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
	tempreg|=(u32)SAI_PSC_TBL[i][1]<<6;		//����PLLI2SN
	tempreg|=(u32)SAI_PSC_TBL[i][2]<<24;	//����PLLI2SQ 
	RCC->PLLI2SCFGR=tempreg;				//����I2SxCLK��Ƶ�� 
	tempreg=RCC->DCKCFGR;			
	tempreg&=~(0X1F);						//���PLLI2SDIVQ����.
	tempreg&=~(0X03<<20);					//���SAI1ASRC����.
	tempreg|=SAI_PSC_TBL[i][3]<<0;			//����PLLI2SDIVQ 
	tempreg|=1<<20;							//����SAI1Aʱ����ԴΪPLLI2SQ
	RCC->DCKCFGR=tempreg;					//����DCKCFGR�Ĵ��� 
	RCC->CR|=1<<26;							//����I2Sʱ��
	while((RCC->CR&1<<27)==0);				//�ȴ�I2Sʱ�ӿ����ɹ�. 
	tempreg=SAI1_Block_A->CR1;			
	tempreg&=~(0X0F<<20);					//���MCKDIV����
	tempreg|=(u32)SAI_PSC_TBL[i][4]<<20;	//����MCKDIV
	tempreg|=1<<16;							//ʹ��SAI1 Block A
	tempreg|=1<<17;							//ʹ��DMA
	SAI1_Block_A->CR1=tempreg;				//����MCKDIV,ͬʱʹ��SAI1 Block A 
	return 0;
}  
//SAIA TX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
//width:λ��(�洢��������,ͬʱ����),0,8λ;1,16λ;2,32λ;
void SAIA_TX_DMA_Init(u8* buf0,u8 *buf1,u16 num,u8 width)
{  
	RCC->AHB1ENR|=1<<22;		//DMA2ʱ��ʹ��   
	while(DMA2_Stream3->CR&0X01);//�ȴ�DMA2_Stream3������ 
	DMA2->LIFCR|=0X3D<<6*4;		//���ͨ��3�������жϱ�־
	DMA2_Stream3->FCR=0X0000021;//����ΪĬ��ֵ	
	
	DMA2_Stream3->PAR=(u32)&SAI1_Block_A->DR;//�����ַΪ:SAI1_Block_A->DR
	DMA2_Stream3->M0AR=(u32)buf0;//�ڴ�1��ַ
	DMA2_Stream3->M1AR=(u32)buf1;//�ڴ�2��ַ
	DMA2_Stream3->NDTR=num;		//��ʱ���ó���Ϊ1
	DMA2_Stream3->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
	DMA2_Stream3->CR|=1<<6;		//�洢��������ģʽ 
	DMA2_Stream3->CR|=1<<8;		//ѭ��ģʽ
	DMA2_Stream3->CR|=0<<9;		//���������ģʽ
	DMA2_Stream3->CR|=1<<10;	//�洢������ģʽ
	DMA2_Stream3->CR|=(u16)width<<11;//�������ݳ���:16λ/32λ
	DMA2_Stream3->CR|=(u16)width<<13;//�洢�����ݳ���:16λ/32λ
	DMA2_Stream3->CR|=2<<16;	//�����ȼ�
	DMA2_Stream3->CR|=1<<18;	//˫����ģʽ
	DMA2_Stream3->CR|=0<<21;	//����ͻ�����δ���
	DMA2_Stream3->CR|=0<<23;	//�洢��ͻ�����δ���
	DMA2_Stream3->CR|=0<<25;	//ѡ��ͨ��0 SAI1_Aͨ�� 

	DMA2_Stream3->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
	DMA2_Stream3->FCR&=~(3<<0);	//��FIFO ����
	
	DMA2_Stream3->CR|=1<<4;		//������������ж�
//	MY_NVIC_Init(0,0,DMA2_Stream3_IRQn,2);	//��ռ1�������ȼ�0����2  
}  
//SAIB RX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
//width:λ��(�洢��������,ͬʱ����),0,8λ;1,16λ;2,32λ;
void SAIB_RX_DMA_Init(u8* buf0,u8 *buf1,u16 num,u8 width)
{  
	RCC->AHB1ENR|=1<<22;		//DMA2ʱ��ʹ��   
	while(DMA2_Stream5->CR&0X01);//�ȴ�DMA2_Stream5������ 
	DMA2->HIFCR|=0X3D<<6*1;		//���ͨ��5�������жϱ�־
	DMA2_Stream5->FCR=0X0000021;//����ΪĬ��ֵ	
	
	DMA2_Stream5->PAR=(u32)&SAI1_Block_B->DR;//�����ַΪ:SAI1_Block_B->DR
	DMA2_Stream5->M0AR=(u32)buf0;//�ڴ�1��ַ
	DMA2_Stream5->M1AR=(u32)buf1;//�ڴ�2��ַ
	DMA2_Stream5->NDTR=num;		//��ʱ���ó���Ϊ1
	DMA2_Stream5->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
	DMA2_Stream5->CR|=0<<6;		//���赽�洢��ģʽ 
	DMA2_Stream5->CR|=1<<8;		//ѭ��ģʽ
	DMA2_Stream5->CR|=0<<9;		//���������ģʽ
	DMA2_Stream5->CR|=1<<10;	//�洢������ģʽ
	DMA2_Stream5->CR|=(u16)width<<11;//�������ݳ���:16λ/32λ
	DMA2_Stream5->CR|=(u16)width<<13;//�洢�����ݳ���:16λ/32λ
	DMA2_Stream5->CR|=1<<16;	//�е����ȼ�
	DMA2_Stream5->CR|=1<<18;	//˫����ģʽ
	DMA2_Stream5->CR|=0<<21;	//����ͻ�����δ���
	DMA2_Stream5->CR|=0<<23;	//�洢��ͻ�����δ���
	DMA2_Stream5->CR|=0<<25;	//ѡ��ͨ��0 SAI1_Bͨ�� 

	DMA2_Stream5->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
	DMA2_Stream5->FCR&=~(3<<0);	//��FIFO ����
	
	DMA2_Stream5->CR|=1<<4;		//������������ж�
//	MY_NVIC_Init(0,1,DMA2_Stream5_IRQn,2);	//��ռ1�������ȼ�1����2  
} 
//SAI DMA�ص�����ָ��
void (*sai_tx_callback)(void);	//TX�ص����� 
void (*sai_rx_callback)(void);	//RX�ص�����

//DMA2_Stream3�жϷ�����
void DMA2_Stream3_IRQHandler(void)
{      
	OSIntEnter();   
	if(DMA2->LISR&(1<<27))	//DMA2_Steam3,������ɱ�־
	{ 
		DMA2->LIFCR|=1<<27;	//�����������ж�
      	sai_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}   		   
	OSIntExit(); 	    									 
}  
//DMA2_Stream5�жϷ�����
void DMA2_Stream5_IRQHandler(void)
{       			    
	OSIntEnter();   
	if(DMA2->HISR&(1<<11))	//DMA2_Steam5,������ɱ�־
	{ 
		DMA2->HIFCR|=1<<11;	//�����������ж�
      	sai_rx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
	}   		   
	OSIntExit(); 	    									 
}  
//SAI��ʼ����
void SAI_Play_Start(void)
{   	  
	DMA2_Stream3->CR|=1<<0;		//����DMA TX����  		
}
//�ر�I2S����
void SAI_Play_Stop(void)
{   	 
	DMA2_Stream3->CR&=~(1<<0);	//��������	 	 
} 
//SAI��ʼ¼��
void SAI_Rec_Start(void)
{   	    
	DMA2_Stream5->CR|=1<<0;		//����DMA RX���� 		
}
//�ر�SAI¼��
void SAI_Rec_Stop(void)
{   	  
	DMA2_Stream5->CR&=~(1<<0);	//����¼��		 
}









