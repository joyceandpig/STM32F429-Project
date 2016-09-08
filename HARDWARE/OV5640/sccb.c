#include "sys.h"
#include "sccb.h"
#include "delay.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//OVϵ������ͷ SCCB ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void SCCB_Delay(void)
{
	delay_us(5);
}
//��ʼ��SCCB�ӿ� 
void SCCB_Init(void)
{											      	 
//	RCC->AHB1ENR|=1<<1;    	//ʹ������PORTBʱ��	
////	GPIO_Set(GPIOB,PIN3|PIN4,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);	//PB3/4�������
//	GPIOB->ODR|=3<<3;			//PB3/4���1  
	
	   GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOBʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4; //PB3,4
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);
	
	SCCB_SDA_OUT();	   
}			 

//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void SCCB_Start(void)
{
    SCCB_SDA_SET();     //�����߸ߵ�ƽ	   
    SCCB_SCL_SET();	    //��ʱ���߸ߵ�ʱ���������ɸ�����
    SCCB_Delay();  
    SCCB_SDA_CLEAR();
    SCCB_Delay();	 
    SCCB_SCL_CLEAR();	    //�����߻ָ��͵�ƽ��������������Ҫ	  
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void SCCB_Stop(void)
{
    SCCB_SDA_CLEAR();
    SCCB_Delay();	 
    SCCB_SCL_SET();	    
    SCCB_Delay();
    SCCB_SDA_SET();    
    SCCB_Delay();
}  
//����NA�ź�
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
//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
u8 SCCB_WR_Byte(u8 dat)
{
	u8 j,res;	 
	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if(dat&0x80)SCCB_SDA_SET();
		else SCCB_SDA_CLEAR();
		dat<<=1;
		SCCB_Delay();
		SCCB_SCL_SET();
		SCCB_Delay();
		SCCB_SCL_CLEAR();		   
	}			 
	SCCB_SDA_IN();		//����SDAΪ���� 
	SCCB_Delay();
	SCCB_SCL_SET();			//���յھ�λ,���ж��Ƿ��ͳɹ�
	SCCB_Delay();
	if(SCCB_READ_SDA_STA())res=1;  //SDA=1����ʧ�ܣ�����1
	else res=0;         //SDA=0���ͳɹ�������0
	SCCB_SCL_CLEAR();		 
	SCCB_SDA_OUT();		//����SDAΪ���    
	return res;  
}	 
//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
u8 SCCB_RD_Byte(void)
{
	u8 temp=0,j;    
	SCCB_SDA_IN();		//����SDAΪ����  
	for(j=8;j>0;j--) 	//ѭ��8�ν�������
	{		     	  
		SCCB_Delay();
		SCCB_SCL_SET();
		temp=temp<<1;
		if(SCCB_READ_SDA_STA())temp++;   
		SCCB_Delay();
		SCCB_SCL_CLEAR();
	}	
	SCCB_SDA_OUT();		//����SDAΪ���    
	return temp;
} 							    
 
\












