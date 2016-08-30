#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
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
	
 		 
//ͨ���ı�TIM3->CCR4��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define LED0_PWM_VAL TIM3->CCR4     
//TIM9 CH2��ΪPWM DAC�����ͨ�� 
#define PWM_DAC_VAL  TIM9->CCR2  

//LCD PWM�������� 
#define LCD_BLPWM_VAL   TIM3->CCR2 		

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u32 arr,u32 psc);
void TIM3_CH2_PWM_Init(u32 arr,u32 psc);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void TIM9_CH2_PWM_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);	
void TIM7_Int_Init(u16 arr,u16 psc);	
void TIM8_Int_Init(u16 arr,u16 psc);
#endif























