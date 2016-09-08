#include "sai.h"  
#include "includes.h"	 	//ucos ʹ��	  
#include "stm32f4xx_hal_dma.h"
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

SAI_HandleTypeDef SAI1A_Handler;        //SAI1 Block A���
SAI_HandleTypeDef SAI1B_Handler;        //SAI1 Block B���
DMA_HandleTypeDef SAI1_TXDMA_Handler;   //DMA���;��
DMA_HandleTypeDef SAI1_RXDMA_Handler;   //DMA���;��


//SAI Block A��ʼ��,I2S,�����ֱ�׼
//mode:����ģʽ,00,��������;01,��������;10,�ӷ�����;11,�ӽ�����
//cpol:0,ʱ���½���ѡͨ;1,ʱ��������ѡͨ
//datalen:���ݴ�С,0/1,δ�õ�,2,8λ;3,10λ;4,16λ;5,20λ;6,24λ;7,32λ.
void SAIA_Init(u8 mode,u8 cpol,u8 datalen)
{ 
//	u32 tempreg=0;
//	RCC->APB2ENR|=1<<22;		//ʹ��SAI1ʱ��
//	RCC->APB2RSTR|=1<<22;		//��λSAI1
//	RCC->APB2RSTR&=~(1<<22);	//������λ  
//	tempreg|=mode<<0;			//����SAI1����ģʽ
//	tempreg|=0<<2;				//����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
//	tempreg|=datalen<<5;		//�������ݴ�С
//	tempreg|=0<<8;				//����MSBλ����
//	tempreg|=(u16)cpol<<9;		//������ʱ�ӵ�����/�½���ѡͨ
//	tempreg|=0<<10;				//��Ƶģ���첽
//	tempreg|=0<<12;				//������ģʽ
//	tempreg|=1<<13;				//����������Ƶģ�����
//	tempreg|=0<<19;				//ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
//	SAI1_Block_A->CR1=tempreg;	//����CR1�Ĵ���
//	
//	tempreg=(64-1)<<0;			//����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
//	tempreg|=(32-1)<<8;			//����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
//	tempreg|=1<<16;				///FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
//	tempreg|=0<<17;				///FS�͵�ƽ��Ч(�½���)
//	tempreg|=1<<18;				//��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	 
//	SAI1_Block_A->FRCR=tempreg;
//	
//	tempreg=0<<0;				//slotƫ��(FBOFF)Ϊ0
//	tempreg|=2<<6;				//slot��СΪ32λ
//	tempreg|=(2-1)<<8;			//slot��Ϊ2��
//	tempreg|=(1<<17)|(1<<16);	//ʹ��slot0��slot1
//	SAI1_Block_A->SLOTR=tempreg;//����slot 
//	
//	SAI1_Block_A->CR2=1<<0;		//����FIFO��ֵ:1/4 FIFO  
//	SAI1_Block_A->CR2|=1<<3;	//FIFOˢ�� 
	
	  HAL_SAI_DeInit(&SAI1A_Handler);                          //�����ǰ������
    SAI1A_Handler.Instance=SAI1_Block_A;                     //SAI1 Bock A
    SAI1A_Handler.Init.AudioMode=mode;                       //����SAI1����ģʽ
    SAI1A_Handler.Init.Synchro=SAI_ASYNCHRONOUS;             //��Ƶģ���첽
    SAI1A_Handler.Init.OutputDrive=SAI_OUTPUTDRIVE_ENABLE;   //����������Ƶģ�����
    SAI1A_Handler.Init.NoDivider=SAI_MASTERDIVIDER_ENABLE;   //ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
    SAI1A_Handler.Init.FIFOThreshold=SAI_FIFOTHRESHOLD_1QF;  //����FIFO��ֵ,1/4 FIFO
    SAI1A_Handler.Init.ClockSource=SAI_CLKSOURCE_PLLI2S;     //SIAʱ��ԴΪPLL2S
    SAI1A_Handler.Init.MonoStereoMode=SAI_STEREOMODE;        //������ģʽ
    SAI1A_Handler.Init.Protocol=SAI_FREE_PROTOCOL;           //����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
    SAI1A_Handler.Init.DataSize=datalen;                     //�������ݴ�С
    SAI1A_Handler.Init.FirstBit=SAI_FIRSTBIT_MSB;            //����MSBλ����
    SAI1A_Handler.Init.ClockStrobing=cpol;                   //������ʱ�ӵ�����/�½���ѡͨ
    
    //֡����
    SAI1A_Handler.FrameInit.FrameLength=64;                  //����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
    SAI1A_Handler.FrameInit.ActiveFrameLength=32;            //����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
    SAI1A_Handler.FrameInit.FSDefinition=SAI_FS_CHANNEL_IDENTIFICATION;//FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
    SAI1A_Handler.FrameInit.FSPolarity=SAI_FS_ACTIVE_LOW;    //FS�͵�ƽ��Ч(�½���)
    SAI1A_Handler.FrameInit.FSOffset=SAI_FS_BEFOREFIRSTBIT;  //��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	

    //SLOT����
    SAI1A_Handler.SlotInit.FirstBitOffset=0;                 //slotƫ��(FBOFF)Ϊ0
    SAI1A_Handler.SlotInit.SlotSize=SAI_SLOTSIZE_32B;        //slot��СΪ32λ
    SAI1A_Handler.SlotInit.SlotNumber=2;                     //slot��Ϊ2��    
    SAI1A_Handler.SlotInit.SlotActive=SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;//ʹ��slot0��slot1
    
    HAL_SAI_Init(&SAI1A_Handler);                            //��ʼ��SAI
    __HAL_SAI_ENABLE(&SAI1A_Handler);                        //ʹ��SAI 
} 
//SAI�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_SAI_Init()����
//hsdram:SAI���
void HAL_SAI_MspInit(SAI_HandleTypeDef *hsai)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_SAI1_CLK_ENABLE();                //ʹ��SAI1ʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();               //ʹ��GPIOEʱ��
   if(hsai->Instance == SAI1_Block_A){
    //��ʼ��PE2,3,4,5,6
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;  
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    GPIO_Initure.Alternate=GPIO_AF6_SAI1;       //����ΪSAI   
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);         //��ʼ��
	 }
}
//SAI Block B��ʼ��,I2S,�����ֱ�׼,ͬ��ģʽ
//mode:����ģʽ,00,��������;01,��������;10,�ӷ�����;11,�ӽ�����
//cpol:0,ʱ���½���ѡͨ;1,ʱ��������ѡͨ
//datalen:���ݴ�С,0/1,δ�õ�,2,8λ;3,10λ;4,16λ;5,20λ;6,24λ;7,32λ.
void SAIB_Init(u8 mode,u8 cpol,u8 datalen)
{ 
//	u32 tempreg=0;  
//	tempreg|=mode<<0;			//����SAI1����ģʽ
//	tempreg|=0<<2;				//����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
//	tempreg|=datalen<<5;		//�������ݴ�С
//	tempreg|=0<<8;				//����MSBλ����
//	tempreg|=(u16)cpol<<9;		//������ʱ�ӵ�����/�½���ѡͨ
//	tempreg|=1<<10;				//ʹ��ͬ��ģʽ
//	tempreg|=0<<12;				//������ģʽ
//	tempreg|=1<<13;				//����������Ƶģ�����
//	SAI1_Block_B->CR1=tempreg;	//����CR1�Ĵ���
//	
//	tempreg=(64-1)<<0;			//����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
//	tempreg|=(32-1)<<8;			//����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
//	tempreg|=1<<16;				///FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
//	tempreg|=0<<17;				///FS�͵�ƽ��Ч(�½���)
//	tempreg|=1<<18;				//��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	 
//	SAI1_Block_B->FRCR=tempreg;
//	
//	tempreg=0<<0;				//slotƫ��(FBOFF)Ϊ0
//	tempreg|=2<<6;				//slot��СΪ32λ
//	tempreg|=(2-1)<<8;			//slot��Ϊ2��
//	tempreg|=(1<<17)|(1<<16);	//ʹ��slot0��slot1
//	SAI1_Block_B->SLOTR=tempreg;//����slot 
//	
//	SAI1_Block_B->CR2=1<<0;		//����FIFO��ֵ:1/4 FIFO  
//	SAI1_Block_B->CR2|=1<<3;	//FIFOˢ��  
//	SAI1_Block_B->CR1|=1<<17;	//ʹ��DMA
//	SAI1_Block_B->CR1|=1<<16;	//ʹ��SAI1 Block B
	
		 HAL_SAI_DeInit(&SAI1B_Handler);                          //�����ǰ������
    SAI1B_Handler.Instance=SAI1_Block_B;                     //SAI1 Bock B
    SAI1B_Handler.Init.AudioMode=mode;                       //����SAI1����ģʽ
    SAI1B_Handler.Init.Synchro=SAI_ASYNCHRONOUS;             //��Ƶģ���첽
    SAI1B_Handler.Init.OutputDrive=SAI_OUTPUTDRIVE_ENABLE;   //����������Ƶģ�����
    SAI1B_Handler.Init.NoDivider=SAI_MASTERDIVIDER_ENABLE;   //ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
    SAI1B_Handler.Init.FIFOThreshold=SAI_FIFOTHRESHOLD_1QF;  //����FIFO��ֵ,1/4 FIFO
    SAI1B_Handler.Init.ClockSource=SAI_CLKSOURCE_PLLI2S;     //SIAʱ��ԴΪPLL2S
    SAI1B_Handler.Init.MonoStereoMode=SAI_STEREOMODE;        //������ģʽ
    SAI1B_Handler.Init.Protocol=SAI_FREE_PROTOCOL;           //����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
    SAI1B_Handler.Init.DataSize=datalen;                     //�������ݴ�С
    SAI1B_Handler.Init.FirstBit=SAI_FIRSTBIT_MSB;            //����MSBλ����
    SAI1B_Handler.Init.ClockStrobing=cpol;                   //������ʱ�ӵ�����/�½���ѡͨ
    
    //֡����
    SAI1B_Handler.FrameInit.FrameLength=64;                  //����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
    SAI1B_Handler.FrameInit.ActiveFrameLength=32;            //����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
    SAI1B_Handler.FrameInit.FSDefinition=SAI_FS_CHANNEL_IDENTIFICATION;//FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
    SAI1B_Handler.FrameInit.FSPolarity=SAI_FS_ACTIVE_LOW;    //FS�͵�ƽ��Ч(�½���)
    SAI1B_Handler.FrameInit.FSOffset=SAI_FS_BEFOREFIRSTBIT;  //��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	

    //SLOT����
    SAI1B_Handler.SlotInit.FirstBitOffset=0;                 //slotƫ��(FBOFF)Ϊ0
    SAI1B_Handler.SlotInit.SlotSize=SAI_SLOTSIZE_32B;        //slot��СΪ32λ
    SAI1B_Handler.SlotInit.SlotNumber=2;                     //slot��Ϊ2��    
    SAI1B_Handler.SlotInit.SlotActive=SAI_SLOTACTIVE_0|SAI_SLOTACTIVE_1;//ʹ��slot0��slot1
    
    HAL_SAI_Init(&SAI1B_Handler);                            //��ʼ��SAI
    __HAL_SAI_ENABLE(&SAI1B_Handler);                        //ʹ��SAI 
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
//����SAI��DMA����,HAL��û���ṩ�˺���
//���������Ҫ�Լ������Ĵ�����дһ��
void SAIA_DMA_Enable(void)
{
    u32 tempreg=0;
    tempreg=SAI1_Block_A->CR1;          //�ȶ�����ǰ������			
	tempreg|=1<<17;					    //ʹ��DMA
	SAI1_Block_A->CR1=tempreg;		    //д��CR1�Ĵ�����
}
//����SAIA�Ĳ�����(@MCKEN)
//samplerate:������,��λ:Hz
//����ֵ:0,���óɹ�;1,�޷�����.
u8 SAIA_SampleRate_Set(u32 samplerate)
{ 
//	u8 i=0; 
//	u32 tempreg=0;
//	samplerate/=10;//��С10��   
//	for(i=0;i<(sizeof(SAI_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
//	{
//		if(samplerate==SAI_PSC_TBL[i][0])break;
//	}
//	RCC->CR&=~(1<<26);						//�ȹر�PLLI2S  
//	if(i==(sizeof(SAI_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
//	tempreg|=(u32)SAI_PSC_TBL[i][1]<<6;		//����PLLI2SN
//	tempreg|=(u32)SAI_PSC_TBL[i][2]<<24;	//����PLLI2SQ 
//	RCC->PLLI2SCFGR=tempreg;				//����I2SxCLK��Ƶ�� 
//	tempreg=RCC->DCKCFGR;			
//	tempreg&=~(0X1F);						//���PLLI2SDIVQ����.
//	tempreg&=~(0X03<<20);					//���SAI1ASRC����.
//	tempreg|=SAI_PSC_TBL[i][3]<<0;			//����PLLI2SDIVQ 
//	tempreg|=1<<20;							//����SAI1Aʱ����ԴΪPLLI2SQ
//	RCC->DCKCFGR=tempreg;					//����DCKCFGR�Ĵ��� 
//	RCC->CR|=1<<26;							//����I2Sʱ��
//	while((RCC->CR&1<<27)==0);				//�ȴ�I2Sʱ�ӿ����ɹ�. 
//	tempreg=SAI1_Block_A->CR1;			
//	tempreg&=~(0X0F<<20);					//���MCKDIV����
//	tempreg|=(u32)SAI_PSC_TBL[i][4]<<20;	//����MCKDIV
//	tempreg|=1<<16;							//ʹ��SAI1 Block A
//	tempreg|=1<<17;							//ʹ��DMA
//	SAI1_Block_A->CR1=tempreg;				//����MCKDIV,ͬʱʹ��SAI1 Block A 
//	return 0;
	
	    u8 i=0; 
    
    RCC_PeriphCLKInitTypeDef RCCSAI1_Sture;  
	for(i=0;i<(sizeof(SAI_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if((samplerate/10)==SAI_PSC_TBL[i][0])break;
	}
    if(i==(sizeof(SAI_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
    RCCSAI1_Sture.PeriphClockSelection=RCC_PERIPHCLK_SAI_PLLI2S;//����ʱ��Դѡ�� 
    RCCSAI1_Sture.PLLI2S.PLLI2SN=(u32)SAI_PSC_TBL[i][1];        //����PLLI2SN
    RCCSAI1_Sture.PLLI2S.PLLI2SQ=(u32)SAI_PSC_TBL[i][2];        //����PLLI2SQ
    //����PLLI2SDivQ��ʱ��SAI_PSC_TBL[i][3]Ҫ��1����ΪHAL���л��ڰ�PLLI2SDivQ�����Ĵ���DCKCFGR��ʱ���1 
    RCCSAI1_Sture.PLLI2SDivQ=SAI_PSC_TBL[i][3]+1;               //����PLLI2SDIVQ
    HAL_RCCEx_PeriphCLKConfig(&RCCSAI1_Sture);                  //����ʱ��
    
    __HAL_RCC_SAI_BLOCKACLKSOURCE_CONFIG(RCC_SAIACLKSOURCE_PLLI2S); //����SAI1ʱ����ԴΪPLLI2SQ		

    __HAL_SAI_DISABLE(&SAI1A_Handler);                          //�ر�SAI
    SAI1A_Handler.Init.AudioFrequency=samplerate;               //���ò���Ƶ��
    HAL_SAI_Init(&SAI1A_Handler);                               //��ʼ��SAI
    SAIA_DMA_Enable();                                          //����SAI��DMA����
    __HAL_SAI_ENABLE(&SAI1A_Handler);                           //����SAI
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
//	RCC->AHB1ENR|=1<<22;		//DMA2ʱ��ʹ��   
//	while(DMA2_Stream3->CR&0X01);//�ȴ�DMA2_Stream3������ 
//	DMA2->LIFCR|=0X3D<<6*4;		//���ͨ��3�������жϱ�־
//	DMA2_Stream3->FCR=0X0000021;//����ΪĬ��ֵ	
//	
//	DMA2_Stream3->PAR=(u32)&SAI1_Block_A->DR;//�����ַΪ:SAI1_Block_A->DR
//	DMA2_Stream3->M0AR=(u32)buf0;//�ڴ�1��ַ
//	DMA2_Stream3->M1AR=(u32)buf1;//�ڴ�2��ַ
//	DMA2_Stream3->NDTR=num;		//��ʱ���ó���Ϊ1
//	DMA2_Stream3->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
//	DMA2_Stream3->CR|=1<<6;		//�洢��������ģʽ 
//	DMA2_Stream3->CR|=1<<8;		//ѭ��ģʽ
//	DMA2_Stream3->CR|=0<<9;		//���������ģʽ
//	DMA2_Stream3->CR|=1<<10;	//�洢������ģʽ
//	DMA2_Stream3->CR|=(u16)width<<11;//�������ݳ���:16λ/32λ
//	DMA2_Stream3->CR|=(u16)width<<13;//�洢�����ݳ���:16λ/32λ
//	DMA2_Stream3->CR|=2<<16;	//�����ȼ�
//	DMA2_Stream3->CR|=1<<18;	//˫����ģʽ
//	DMA2_Stream3->CR|=0<<21;	//����ͻ�����δ���
//	DMA2_Stream3->CR|=0<<23;	//�洢��ͻ�����δ���
//	DMA2_Stream3->CR|=0<<25;	//ѡ��ͨ��0 SAI1_Aͨ�� 

//	DMA2_Stream3->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
//	DMA2_Stream3->FCR&=~(3<<0);	//��FIFO ����
//	
//	DMA2_Stream3->CR|=1<<4;		//������������ж�
////	MY_NVIC_Init(0,0,DMA2_Stream3_IRQn,2);	//��ռ1�������ȼ�0����2  

u32 memwidth=0,perwidth=0;      //����ʹ洢��λ��
    switch(width)
    {
        case 0:         //8λ
            memwidth=DMA_MDATAALIGN_BYTE;
            perwidth=DMA_PDATAALIGN_BYTE;
            break;
        case 1:         //16λ
            memwidth=DMA_MDATAALIGN_HALFWORD;
            perwidth=DMA_PDATAALIGN_HALFWORD;
            break;
        case 2:         //32λ
            memwidth=DMA_MDATAALIGN_WORD;
            perwidth=DMA_PDATAALIGN_WORD;
            break;
            
    }
    __HAL_RCC_DMA2_CLK_ENABLE();                                    //ʹ��DMA2ʱ��
    __HAL_LINKDMA(&SAI1A_Handler,hdmatx,SAI1_TXDMA_Handler);        //��DMA��SAI��ϵ����
    SAI1_TXDMA_Handler.Instance=DMA2_Stream3;                       //DMA2������3                     
    SAI1_TXDMA_Handler.Init.Channel=DMA_CHANNEL_0;                  //ͨ��0
    SAI1_TXDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;         //�洢��������ģʽ
    SAI1_TXDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;             //���������ģʽ
    SAI1_TXDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                 //�洢������ģʽ
    SAI1_TXDMA_Handler.Init.PeriphDataAlignment=perwidth;           //�������ݳ���:16/32λ
    SAI1_TXDMA_Handler.Init.MemDataAlignment=memwidth;              //�洢�����ݳ���:16/32λ
    SAI1_TXDMA_Handler.Init.Mode=DMA_CIRCULAR;                      //ʹ��ѭ��ģʽ 
    SAI1_TXDMA_Handler.Init.Priority=DMA_PRIORITY_HIGH;             //�����ȼ�
    SAI1_TXDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;          //��ʹ��FIFO
    SAI1_TXDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;             //�洢������ͻ������
    SAI1_TXDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;          //����ͻ�����δ��� 
    HAL_DMA_DeInit(&SAI1_TXDMA_Handler);                            //�������ǰ������
    HAL_DMA_Init(&SAI1_TXDMA_Handler);	                            //��ʼ��DMA

    HAL_DMAEx_MultiBufferStart(&SAI1_TXDMA_Handler,(u32)buf0,(u32)&SAI1_Block_A->DR,(u32)buf1,num);//����˫����
    __HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);                         //�ȹر�DMA 
    delay_us(10);                                                   //10us��ʱ����ֹ-O2�Ż������� 	
    __HAL_DMA_ENABLE_IT(&SAI1_TXDMA_Handler,DMA_IT_TC);             //������������ж�
    __HAL_DMA_CLEAR_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF3_7);     //���DMA��������жϱ�־λ
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn,0,0);                    //DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
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
//	if(DMA2->LISR&(1<<27))	//DMA2_Steam3,������ɱ�־
//	{ 
//		DMA2->LIFCR|=1<<27;	//�����������ж�
//      	sai_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
//	}  
    if(__HAL_DMA_GET_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF3_7)!=RESET) //DMA�������
    {
        __HAL_DMA_CLEAR_FLAG(&SAI1_TXDMA_Handler,DMA_FLAG_TCIF3_7);     //���DMA��������жϱ�־λ
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
//	DMA2_Stream3->CR|=1<<0;		//����DMA TX����  
 __HAL_DMA_ENABLE(&SAI1_TXDMA_Handler);//����DMA TX���� 	
}
//�ر�I2S����
void SAI_Play_Stop(void)
{   	 
//	DMA2_Stream3->CR&=~(1<<0);	//��������	 	
__HAL_DMA_DISABLE(&SAI1_TXDMA_Handler);  //�������� 	
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









