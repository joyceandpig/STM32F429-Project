#include "timer.h"
#include "usart.h"
#include "led.h"
#include "includes.h"	 	//ucos ʹ��	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/5
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									
//********************************************************************************
//�޸�˵��
//V1.1 20151205
//����TIM3_PWM_Init����,����PWM���
//V1.2 20151205
//����TIM5_CH1_Cap_Init����,�������벶��
//V1.3 20151227
//����TIM9_CH2_PWM_Init����,����PWM DACʵ��
//V1.4 20160111
//����TIM6_Int_Init����,������Ƶ����ʱ϶����
//V1.5 20160128
//����TIM8_Int_Init����  
//����TIM3_CH2_PWM_Init����
//����TIM7_Int_Init���� 
////////////////////////////////////////////////////////////////////////////////// 	 
	 

//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    		  	 
	if(TIM3->SR&0X0001)//����ж� 
	{ 
		LED1=!LED1;
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ  
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ48M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//��ռ1�������ȼ�3����2									 
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��    
	RCC->AHB1ENR|=1<<2;   	//ʹ��PORTBʱ��	
	GPIO_Set(GPIOB,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOB,1,2);	//PB1,AF2 
	
	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ 
	TIM3->CCMR2|=6<<12;  	//CH4 PWM1ģʽ		 
	TIM3->CCMR2|=1<<11; 	//CH4 Ԥװ��ʹ��	   
	TIM3->CCER|=1<<12;   	//OC4 ���ʹ��	
	TIM3->CCER|=1<<13;   	//OC4 �͵�ƽ��Ч	   
	TIM3->CR1|=1<<7;   		//ARPEʹ�� 
	TIM3->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3											  
}  
//TIM3 CH2 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_CH2_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��    
	RCC->AHB1ENR|=1<<2;   	//ʹ��PORTBʱ��	
	GPIO_Set(GPIOB,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOB,5,2);	//PB1,AF2 
	
	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ 
	TIM3->CCMR1|=6<<12;  	//CH2 PWM1ģʽ		 
	TIM3->CCMR1|=1<<11; 	//CH2 Ԥװ��ʹ��	   
	TIM3->CCER|=1<<4;   	//OC1 ���ʹ��	
	TIM3->CCER|=0<<5;   	//OC1 �ߵ�ƽ��Ч	   
	TIM3->CR1|=1<<7;   		//ARPEʹ�� 
	TIM3->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3											  
}  

//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<3;   	//TIM5 ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
	  
 	TIM5->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM5->PSC=psc;  		//Ԥ��Ƶ�� 

	TIM5->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
 	TIM5->CCMR1|=0<<4; 		//IC1F=0000 ���������˲��� ���˲�
 	TIM5->CCMR1|=0<<10; 	//IC1PS=00 	���������Ƶ,����Ƶ 

	TIM5->CCER|=0<<1; 		//CC1P=0	�����ز���
	TIM5->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����

	TIM5->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
	TIM5->DIER|=1<<1;   	//������1�ж�				
	TIM5->DIER|=1<<0;   	//��������ж�	
	TIM5->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//��ռ2�������ȼ�0����2	   
}
//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 		    
	u16 tsr;
	tsr=TIM5->SR;
 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(tsr&0X01)//���
		{	     
			if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;
			}	 
		}
		if(tsr&0x02)//����1���������¼�
		{	
			if(TIM5CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			    TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//��ȡ��ǰ�Ĳ���ֵ.
	 			TIM5->CCER&=~(1<<1);			//CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;			//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM5->CR1&=~(1<<0)		;    	//ʹ�ܶ�ʱ��2
	 			TIM5->CNT=0;					//���������
	 			TIM5->CCER|=1<<1; 				//CC1P=1 ����Ϊ�½��ز���
				TIM5->CR1|=0x01;    			//ʹ�ܶ�ʱ��2
			}		    
		}			     	    					   
 	}
	TIM5->SR=0;//����жϱ�־λ   
}

//TIM9 CH2 PWM������� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{		 					 
	RCC->APB2ENR|=1<<16;   	//TIM9 ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	GPIO_Set(GPIOA,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA3,���ù���,����
	GPIO_AF_Set(GPIOA,3,3);	//PA3,AF3
 	TIM9->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM9->PSC=psc;  		//Ԥ��Ƶ�� 
	TIM9->CCMR1|=6<<12;  	//CH2 PWM1ģʽ		 
	TIM9->CCMR1|=1<<11; 	//CH2 Ԥװ��ʹ��	
	TIM9->CCER|=1<<4;   	//OC2 ���ʹ��	  
	TIM9->CR1|=1<<7;   		//ARPEʹ�� 
	TIM9->CR1|=1<<0;    	//ʹ�ܶ�ʱ��9 
} 
extern vu8 aviframeup;
extern vu8 webcam_oensec;
extern u16 reg_time;
//��ʱ��6�жϷ������	 
void TIM6_DAC_IRQHandler(void)
{ 		    		  			    
	OSIntEnter();    		    
	if(TIM6->SR&0X0001)//����ж�
	{ 
		aviframeup=1;
		webcam_oensec=1;
		reg_time++;
	}				   
	TIM6->SR&=~(1<<0);//����жϱ�־λ 	    
	OSIntExit(); 	    
}
//ͨ�ö�ʱ��6�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ48M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM6_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<4;	//TIM6ʱ��ʹ��    
 	TIM6->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM6->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM6->CNT=0;  		//����������	  
	TIM6->DIER|=1<<0;   //��������ж�	  
	TIM6->CR1|=0x01;    //ʹ�ܶ�ʱ��6
  	MY_NVIC_Init(3,0,TIM6_DAC_IRQn,2);	//��ռ0�������ȼ�3����2									 
}

extern vu16 USART3_RX_STA;
//��ʱ��7�жϷ������		    
void TIM7_IRQHandler(void)
{ 	
	OSIntEnter();    		    
	if(TIM7->SR&0X01)//�Ǹ����ж�
	{	 			   
		USART3_RX_STA|=1<<15;	//��ǽ������
		TIM7->SR&=~(1<<0);		//����жϱ�־λ		   
		TIM7->CR1&=~(1<<0);		//�رն�ʱ��7	  
	}	    
	OSIntExit();  											 
} 
//ͨ�ö�ʱ��7�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ48M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz 
void TIM7_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<5;	//TIM7ʱ��ʹ��    
 	TIM7->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM7->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM7->CNT=0;  		//����������	  
	TIM7->DIER|=1<<0;   //��������ж�	  
	TIM7->CR1|=0x01;    //ʹ�ܶ�ʱ��7
  	MY_NVIC_Init(0,1,TIM7_IRQn,2);	//��ռ0�������ȼ�1����2									 
} 

extern void usbapp_pulling(void);   

vu8 framecnt;				//ͳһ��֡������
vu8 framecntout;			//ͳһ��֡������������� 
//��ʱ��8/13�жϷ������	 
void TIM8_UP_TIM13_IRQHandler(void)
{ 		    		  			    
	OSIntEnter();   
	if(TIM8->SR&0X0001)//����ж� 
	{
		if(OSRunning!=TRUE)//OS��û����,��TIM3���ж�,10msһ��,��ɨ��USB
		{
			usbapp_pulling();
		}else
		{
			framecntout=framecnt;
			printf("frame:%d\r\n",framecntout);//��ӡ֡��
			framecnt=0;
		}
	}				   
	TIM8->SR&=~(1<<0);//����жϱ�־λ 	    
	OSIntExit(); 
}
//��ʱ��8�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB2��2������APB2Ϊ96M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM8_Int_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<1;	//TIM8ʱ��ʹ��    
 	TIM8->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM8->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM8->DIER|=1<<0;   //��������ж�	  
	TIM8->CR1|=0x01;    //ʹ�ܶ�ʱ��8
  	MY_NVIC_Init(1,3,TIM8_UP_TIM13_IRQn,2);	//��ռ1�������ȼ�3����2									 
}














