#include "timer.h"
#include "usart.h"
#include "led.h"
#include "includes.h"	 	//ucos ʹ��	
#include "stm32f4xx_hal.h"
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
TIM_HandleTypeDef TIM2_Handler;
TIM_HandleTypeDef TIM3_Handler;
TIM_HandleTypeDef TIM5_Handler;
TIM_HandleTypeDef TIM6_Handler;
TIM_HandleTypeDef TIM7_Handler;
TIM_HandleTypeDef TIM8_Handler;
TIM_HandleTypeDef TIM9_Handler;



TIM_OC_InitTypeDef TIM3_CH4Handler;	    //��ʱ��3ͨ��4���
TIM_OC_InitTypeDef TIM9_CH2Handler;	    //��ʱ��9ͨ��2���


//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)

extern vu8 aviframeup;
extern vu8 webcam_oensec;
extern u16 reg_time;

extern vu16 USART3_RX_STA;

u16 tsr;

//extern void usbapp_pulling(void);   

vu8 framecnt;				//ͳһ��֡������
vu8 framecntout;			//ͳһ��֡������������� 


//��ʱ��2ͨ��1���벶������	 
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_CH1_Cap_Init(u32 arr,u16 psc)
{
//	RCC->APB1ENR|=1<<0;		//TIM2ʱ��ʹ��    
//	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
//	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//PA5,���ù���,����������
//	GPIO_AF_Set(GPIOA,5,1);	//PA5,AF1 
		GPIO_InitTypeDef TPAD_GPIO_Handler;
		__HAL_RCC_TIM2_CLK_ENABLE();//TIM2ʱ��ʹ��    
	  __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
	
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_5);
    TPAD_GPIO_Handler.Pin=GPIO_PIN_5; //p5
    TPAD_GPIO_Handler.Mode=GPIO_MODE_AF_PP;  //�������
    TPAD_GPIO_Handler.Pull=GPIO_NOPULL;          //����
    TPAD_GPIO_Handler.Speed=GPIO_SPEED_HIGH;     //���� 100m
		TPAD_GPIO_Handler.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA,&TPAD_GPIO_Handler);
	
	  
 	TIM2->ARR=arr;  		//�趨�������Զ���װֵ//�պ�1ms    
	TIM2->PSC=psc;  		//Ԥ��Ƶ��,1M�ļ���Ƶ��	 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 ���������˲��� ���˲� 
 
	TIM2->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
	TIM2->CCER|=0<<1; 		//CC1P=0	�����ز���
	TIM2->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����
																 
	TIM2->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	
//	  TIM_IC_InitTypeDef TIM2_CH1Config;  
//    
//    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��2
//    TIM2_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
//    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
//    TIM2_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
//    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
//    HAL_TIM_IC_Init(&TIM2_Handler);//��ʼ�����벶��ʱ������
//    
//    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
//    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
//    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
//    TIM2_CH1Config.ICFilter=0;                          //���������˲��������˲�
//    HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);//����TIM2ͨ��1
//	
//    HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);   //����TIM2�Ĳ���ͨ��1�����ҿ��������ж�
//    __HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
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
//	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
// 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
//	TIM3->PSC=psc;  	//Ԥ��Ƶ��	  
//	TIM3->DIER|=1<<0;   //��������ж�	  
//	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
//  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//��ռ1�������ȼ�3����2		
	
	  TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE   
}
//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�   
	}else if(htim->Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();            //ʹ��TIM6ʱ��
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,3);    //�����ж����ȼ�����ռ���ȼ�0�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);          //����ITM6�ж�  
	}else if(htim->Instance == TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();            //ʹ��TIM7ʱ��
		HAL_NVIC_SetPriority(TIM7_IRQn,0,1);    //�����ж����ȼ�����ռ���ȼ�0�������ȼ�1
		HAL_NVIC_EnableIRQ(TIM7_IRQn);          //����ITM7�ж�  
	}else if(htim->Instance == TIM8)
	{
		__HAL_RCC_TIM8_CLK_ENABLE();            //ʹ��TIM8ʱ��
		HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);          //����ITM8�ж�  
	}
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}


//�ص���������ʱ���жϷ�����HAL_TIM_IRQHandler����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
			
    if(htim==(&TIM3_Handler))
    {
//        LED1=!LED1;        //LED1��ת
    }else if(htim == (&TIM5_Handler))
		{
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
		}else if(htim==(&TIM6_Handler))
    { 
				aviframeup=1;
				webcam_oensec=1;
				reg_time++;
    }else if(htim==(&TIM7_Handler))
		{
//			if(TIM7->SR&0X01)//�Ǹ����ж�
			{	 			   
				USART3_RX_STA|=1<<15;	//��ǽ������
				TIM7->SR&=~(1<<0);		//����жϱ�־λ		   
				TIM7->CR1&=~(1<<0);		//�رն�ʱ��7	  
			}	
		}else if(htim==(&TIM8_Handler))
		{
				if(OSRunning!=TRUE)//OS��û����,��TIM3���ж�,10msһ��,��ɨ��USB
				{
		//			usbapp_pulling();
				}else
				{
					framecntout=framecnt;
					printf("frame:%d\r\n",framecntout);//��ӡ֡��
					framecnt=0;
				}				    
		}	
}

//��ʱ�����벶���жϴ���ص��������ú�����HAL_TIM_IRQHandler�лᱻ����
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//�����жϷ���ʱִ��
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����
	{
		if(TIM5CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
                TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);//��ȡ��ǰ�Ĳ���ֵ.
                TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   //һ��Ҫ�����ԭ�������ã���
                TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//����TIM5ͨ��1�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;			//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
				__HAL_TIM_DISABLE(&TIM5_Handler);        //�رն�ʱ��5
				__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);
				TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   //һ��Ҫ�����ԭ�������ã���
				TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//��ʱ��5ͨ��1����Ϊ�½��ز���
				__HAL_TIM_ENABLE(&TIM5_Handler);//ʹ�ܶ�ʱ��5
			}		    
	}		
	
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
//	//�˲������ֶ��޸�IO������
//	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��    
//	RCC->AHB1ENR|=1<<2;   	//ʹ��PORTBʱ��	
////	GPIO_Set(GPIOB,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
////	GPIO_AF_Set(GPIOB,1,2);	//PB1,AF2 
//	
//	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
//	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ 
//	TIM3->CCMR2|=6<<12;  	//CH4 PWM1ģʽ		 
//	TIM3->CCMR2|=1<<11; 	//CH4 Ԥװ��ʹ��	   
//	TIM3->CCER|=1<<12;   	//OC4 ���ʹ��	
//	TIM3->CCER|=1<<13;   	//OC4 �͵�ƽ��Ч	   
//	TIM3->CR1|=1<<7;   		//ARPEʹ�� 
//	TIM3->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3					

    TIM3_Handler.Instance=TIM3;            //��ʱ��3
    TIM3_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM3_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);       //��ʼ��PWM
    
    TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM3_CH4Handler.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_4);//����TIM3ͨ��4
	
    HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_4);//����PWMͨ��4	
}  


//TIM3 CH2 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_CH2_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
////	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��    
////	RCC->AHB1ENR|=1<<2;   	//ʹ��PORTBʱ��	
////	GPIO_Set(GPIOB,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
////	GPIO_AF_Set(GPIOB,5,2);	//PB1,AF2 
////	
////	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
////	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ 
////	TIM3->CCMR1|=6<<12;  	//CH2 PWM1ģʽ		 
////	TIM3->CCMR1|=1<<11; 	//CH2 Ԥװ��ʹ��	   
////	TIM3->CCER|=1<<4;   	//OC1 ���ʹ��	
////	TIM3->CCER|=0<<5;   	//OC1 �ߵ�ƽ��Ч	   
////	TIM3->CR1|=1<<7;   		//ARPEʹ�� 
////	TIM3->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3				

    TIM3_Handler.Instance=TIM3;            //��ʱ��3
    TIM3_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM3_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);       //��ʼ��PWM
    
    TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM3_CH4Handler.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; //����Ƚϼ���Ϊ�� 
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_2);//����TIM3ͨ��4
	
    HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_2);//����PWMͨ��4		
}  
//��ʱ���ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_PWM_Init()����
//htim:��ʱ�����
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_TIM3_CLK_ENABLE();			//ʹ�ܶ�ʱ��3
		__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
		__HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
		
	if((htim->Instance == TIM3) && (htim->Channel == TIM_CHANNEL_4))
	{
    GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
		GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PB1����ΪTIM3_CH4
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}
	else if((htim->Instance == TIM3) && (htim->Channel == TIM_CHANNEL_2))
	{
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PB5
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
		GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PB5����ΪTIM3_CH2
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}else if((htim->Instance == TIM9) && (htim->Channel == TIM_CHANNEL_2))
	{
    GPIO_Initure.Pin=GPIO_PIN_3;           	//PA3
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//�����������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
		GPIO_Initure.Alternate= GPIO_AF3_TIM9;	//PA3����ΪTIM9_CH2
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	}
}

//����TIMͨ��4��ռ�ձ�
//compare:�Ƚ�ֵ
void TIM_SetTIM3Compare4(u32 compare)
{
	TIM3->CCR4=compare;
}
//��ȡTIM����/�ȽϼĴ���ֵ
u32 TIM_GetTIM3Capture4(void)
{
	return  HAL_TIM_ReadCapturedValue(&TIM3_Handler,TIM_CHANNEL_4);
}

//����TIMͨ��2��ռ�ձ�
//compare:�Ƚ�ֵ
void TIM_SetTIM3Compare2(u32 compare)
{
	TIM3->CCR2=compare;
}
//��ȡTIM����/�ȽϼĴ���ֵ
u32 TIM_GetTIM3Capture2(void)
{
	return  HAL_TIM_ReadCapturedValue(&TIM3_Handler,TIM_CHANNEL_2);
}

//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
//	RCC->APB1ENR|=1<<3;   	//TIM5 ʱ��ʹ�� 
//	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
////	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
////	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
//	  
// 	TIM5->ARR=arr;  		//�趨�������Զ���װֵ   
//	TIM5->PSC=psc;  		//Ԥ��Ƶ�� 

//	TIM5->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
// 	TIM5->CCMR1|=0<<4; 		//IC1F=0000 ���������˲��� ���˲�
// 	TIM5->CCMR1|=0<<10; 	//IC1PS=00 	���������Ƶ,����Ƶ 

//	TIM5->CCER|=0<<1; 		//CC1P=0	�����ز���
//	TIM5->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����

//	TIM5->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
//	TIM5->DIER|=1<<1;   	//������1�ж�				
//	TIM5->DIER|=1<<0;   	//��������ж�	
//	TIM5->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
////	MY_NVIC_Init(2,0,TIM5_IRQn,2);//��ռ2�������ȼ�0����2	 

    TIM_IC_InitTypeDef TIM5_CH1Config;  
    
    TIM5_Handler.Instance=TIM5;                          //ͨ�ö�ʱ��5
    TIM5_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM5_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_IC_Init(&TIM5_Handler);//��ʼ�����벶��ʱ������
    
    TIM5_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //�����ز���
    TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//ӳ�䵽TI1��
    TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //���������Ƶ������Ƶ
    TIM5_CH1Config.ICFilter=0;                          //���������˲��������˲�
    HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);//����TIM5ͨ��1
	
    HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);   //����TIM5�Ĳ���ͨ��1�����ҿ��������ж�
    __HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);   //ʹ�ܸ����ж�
}
//��ʱ��5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_TIM_IC_Init()����
//htim:��ʱ��5���
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM5_CLK_ENABLE();            //ʹ��TIM5ʱ��
	__HAL_RCC_TIM2_CLK_ENABLE();            //ʹ��TIM5ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	
	if(htim->Instance == TIM5)
	{
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //�����������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF2_TIM5;   //PA0����ΪTIM5ͨ��1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM5_IRQn,2,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //����ITM5�ж�ͨ��  
	}else if(htim->Instance == TIM2)
	{
	    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //�����������
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF1_TIM2;   //PA5����ΪTIM2ͨ��1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM2_IRQn,2,0);    //�����ж����ȼ�����ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(TIM2_IRQn);          //����ITM5�ж�ͨ�� 
	}
}
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM2_Handler);//��ʱ�����ô�����
}
////����״̬
////[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
////[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
////[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
//u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬		    				
//u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
//��ʱ��5�жϷ������	 
//��ʱ��5�жϷ�����
void TIM5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);//��ʱ�����ô�����
}
//void TIM5_IRQHandler(void)
//{ 		    
//	u16 tsr;
//	tsr=TIM5->SR;
// 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
//	{
//		if(tsr&0X01)//���
//		{	     
//			if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
//			{
//				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
//				{
//					TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
//					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
//				}else TIM5CH1_CAPTURE_STA++;
//			}	 
//		}
//		if(tsr&0x02)//����1���������¼�
//		{	
//			if(TIM5CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
//			{	  			
//				TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
//			    TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//��ȡ��ǰ�Ĳ���ֵ.
//	 			TIM5->CCER&=~(1<<1);			//CC1P=0 ����Ϊ�����ز���
//			}else  								//��δ��ʼ,��һ�β���������
//			{
//				TIM5CH1_CAPTURE_STA=0;			//���
//				TIM5CH1_CAPTURE_VAL=0;
//				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
//				TIM5->CR1&=~(1<<0)		;    	//ʹ�ܶ�ʱ��2
//	 			TIM5->CNT=0;					//���������
//	 			TIM5->CCER|=1<<1; 				//CC1P=1 ����Ϊ�½��ز���
//				TIM5->CR1|=0x01;    			//ʹ�ܶ�ʱ��2
//			}		    
//		}			     	    					   
// 	}
//	TIM5->SR=0;//����жϱ�־λ   
//}

//PWM DAC��ʼ��(Ҳ����TIM9ͨ��2��ʼ��)
//TIM9 CH2 PWM������� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{		 					 
//	RCC->APB2ENR|=1<<16;   	//TIM9 ʱ��ʹ�� 
//	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
////	GPIO_Set(GPIOA,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA3,���ù���,����
////	GPIO_AF_Set(GPIOA,3,3);	//PA3,AF3
// 	TIM9->ARR=arr;  		//�趨�������Զ���װֵ   
//	TIM9->PSC=psc;  		//Ԥ��Ƶ�� 
//	TIM9->CCMR1|=6<<12;  	//CH2 PWM1ģʽ		 
//	TIM9->CCMR1|=1<<11; 	//CH2 Ԥװ��ʹ��	
//	TIM9->CCER|=1<<4;   	//OC2 ���ʹ��	  
//	TIM9->CR1|=1<<7;   		//ARPEʹ�� 
//	TIM9->CR1|=1<<0;    	//ʹ�ܶ�ʱ��9 
	
	  TIM9_Handler.Instance=TIM9;            //��ʱ��3
    TIM9_Handler.Init.Prescaler=psc;       //��ʱ����Ƶ
    TIM9_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//���ϼ���ģʽ
    TIM9_Handler.Init.Period=arr;          //�Զ���װ��ֵ
    TIM9_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM9_Handler);       //��ʼ��PWM
    
    TIM9_CH2Handler.OCMode=TIM_OCMODE_PWM1; //ģʽѡ��PWM1
    TIM9_CH2Handler.Pulse=arr/2;            //���ñȽ�ֵ,��ֵ����ȷ��ռ�ձȣ�Ĭ�ϱȽ�ֵΪ�Զ���װ��ֵ��һ��,��ռ�ձ�Ϊ50%
    TIM9_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //����Ƚϼ���Ϊ��
    HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH2Handler,TIM_CHANNEL_2);//����TIM3ͨ��2
	
    HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);//����PWMͨ��2	
} 
//����TIMͨ��2��ռ�ձ�
//TIM_TypeDef:��ʱ��
//compare:�Ƚ�ֵ
void TIM_SetTIM9Compare2(u32 compare)
{
	TIM9->CCR2=compare;
}

//��ʱ��6�жϷ������	 
void TIM6_DAC_IRQHandler(void)
{ 		    		  			    
//	OSIntEnter();    		    
//	if(TIM6->SR&0X0001)//����ж�
//	{ 
//		aviframeup=1;
//		webcam_oensec=1;
//		reg_time++;
//	}				   
//	TIM6->SR&=~(1<<0);//����жϱ�־λ 	    
//	OSIntExit(); 	    
	OSIntEnter();  
	 HAL_TIM_IRQHandler(&TIM6_Handler);
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
//	u32 temp0;	
//	u32 temp,temp1;
//	RCC->APB1ENR|=1<<4;	//TIM6ʱ��ʹ��    
// 	TIM6->ARR=arr;  	//�趨�������Զ���װֵ 
//	TIM6->PSC=psc;  	//Ԥ��Ƶ��	  
//	TIM6->CNT=0;  		//����������	  
//	TIM6->DIER|=1<<0;   //��������ж�	  
//	TIM6->CR1|=0x01;    //ʹ�ܶ�ʱ��6
////  	MY_NVIC_Init(3,0,TIM6_DAC_IRQn,2);	//��ռ0�������ȼ�3����2		
//  
////	MY_NVIC_PriorityGroupConfig(2);//���÷���
//			  
//	temp1=(~2)&0x07;//ȡ����λ
//	temp1<<=8;
//	temp=SCB->AIRCR;  //��ȡ��ǰ������
//	temp&=0X0000F8FF; //�����ǰ����
//	temp|=0X05FA0000; //д��Կ��
//	temp|=temp1;	   
//	SCB->AIRCR=temp;  //���÷���	  
//	
//	temp0=3<<(4-2);	  
//	temp0|=0&(0x0f>>2);
//	temp0&=0xf;								//ȡ����λ
//	NVIC->ISER[TIM6_DAC_IRQn/32]|=1<<TIM6_DAC_IRQn%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
//	NVIC->IP[TIM6_DAC_IRQn]|=temp0<<4;				//������Ӧ���ȼ����������ȼ�   	
	
	  TIM6_Handler.Instance=TIM6;                          //ͨ�ö�ʱ��6
    TIM6_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM6_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM6_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM6_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    TIM6_Handler.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init(&TIM6_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM6_Handler); //ʹ�ܶ�ʱ��6�����жϣ�TIM_IT_UPDATE   	
}


//��ʱ��7�жϷ������		    
void TIM7_IRQHandler(void)
{ 	
//	OSIntEnter();    		    
//	if(TIM7->SR&0X01)//�Ǹ����ж�
//	{	 			   
//		USART3_RX_STA|=1<<15;	//��ǽ������
//		TIM7->SR&=~(1<<0);		//����жϱ�־λ		   
//		TIM7->CR1&=~(1<<0);		//�رն�ʱ��7	  
//	}	    
//	OSIntExit();  

	OSIntEnter();  
	 HAL_TIM_IRQHandler(&TIM7_Handler);
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
//	RCC->APB1ENR|=1<<5;	//TIM7ʱ��ʹ��    
// 	TIM7->ARR=arr;  	//�趨�������Զ���װֵ 
//	TIM7->PSC=psc;  	//Ԥ��Ƶ��	  
//	TIM7->CNT=0;  		//����������	  
//	TIM7->DIER|=1<<0;   //��������ж�	  
//	TIM7->CR1|=0x01;    //ʹ�ܶ�ʱ��7
////  	MY_NVIC_Init(0,1,TIM7_IRQn,2);	//��ռ0�������ȼ�1����2				

	  TIM7_Handler.Instance=TIM7;                          //ͨ�ö�ʱ��7
    TIM7_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM7_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM7_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM7_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM7_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM7_Handler); //ʹ�ܶ�ʱ��7�����жϣ�TIM_IT_UPDATE   		
} 


//��ʱ��8/13�жϷ������	 
void TIM8_UP_TIM13_IRQHandler(void)
{ 		    		  			    
//	OSIntEnter();   
//	if(TIM8->SR&0X0001)//����ж� 
//	{
//		if(OSRunning!=TRUE)//OS��û����,��TIM3���ж�,10msһ��,��ɨ��USB
//		{
////			usbapp_pulling();
//		}else
//		{
//			framecntout=framecnt;
//			printf("frame:%d\r\n",framecntout);//��ӡ֡��
//			framecnt=0;
//		}
//	}				   
//	TIM8->SR&=~(1<<0);//����жϱ�־λ 	    
//	OSIntExit(); 
	OSIntEnter();  
	HAL_TIM_IRQHandler(&TIM8_Handler);
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
//	u32 temp0;	
//	u32 temp,temp1;
//	RCC->APB2ENR|=1<<1;	//TIM8ʱ��ʹ��    
// 	TIM8->ARR=arr;  	//�趨�������Զ���װֵ 
//	TIM8->PSC=psc;  	//Ԥ��Ƶ��	  
//	TIM8->DIER|=1<<0;   //��������ж�	  
//	TIM8->CR1|=0x01;    //ʹ�ܶ�ʱ��8
////  	MY_NVIC_Init(1,3,TIM8_UP_TIM13_IRQn,2);	//��ռ1�������ȼ�3����2		
//		temp1=(~2)&0x07;//ȡ����λ
//	temp1<<=8;
//	temp=SCB->AIRCR;  //��ȡ��ǰ������
//	temp&=0X0000F8FF; //�����ǰ����
//	temp|=0X05FA0000; //д��Կ��
//	temp|=temp1;	   
//	SCB->AIRCR=temp;  //���÷���	  
//	
//	temp0=1<<(4-2);	  
//	temp0|=3&(0x0f>>2);
//	temp0&=0xf;								//ȡ����λ
//	NVIC->ISER[TIM8_UP_TIM13_IRQn/32]|=1<<TIM8_UP_TIM13_IRQn%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
//	NVIC->IP[TIM8_UP_TIM13_IRQn]|=temp0<<4;				//������Ӧ���ȼ����������ȼ�   	
	
	
	  TIM8_Handler.Instance=TIM8;                          //ͨ�ö�ʱ��8
    TIM8_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM8_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM8_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM8_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
		HAL_TIM_Base_Init(&TIM8_Handler);

    HAL_TIM_Base_Start_IT(&TIM8_Handler); //ʹ�ܶ�ʱ��8�����жϣ�TIM_IT_UPDATE  	

//  TIM_ClockConfigTypeDef sClockSourceConfig;
//  TIM_SlaveConfigTypeDef sSlaveConfig;
//  TIM_MasterConfigTypeDef sMasterConfig;
//	
//	TIM8_Handler.Instance = TIM8;
//  TIM8_Handler.Init.Prescaler = psc;
//  TIM8_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
//  TIM8_Handler.Init.Period = arr;
//  TIM8_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  TIM8_Handler.Init.RepetitionCounter = 0;
//  HAL_TIM_Base_Init(&TIM8_Handler);

////  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
////  HAL_TIM_ConfigClockSource(&TIM8_Handler, &sClockSourceConfig);

//  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
//  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
//  HAL_TIM_SlaveConfigSynchronization(&TIM8_Handler, &sSlaveConfig);

//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  HAL_TIMEx_MasterConfigSynchronization(&TIM8_Handler, &sMasterConfig);
//	HAL_TIM_Base_Start_IT(&TIM8_Handler);
}














