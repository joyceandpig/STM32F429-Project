#include "usb_bsp.h"
#include "sys.h"  
#include "delay.h" 
#include "usart.h" 
//#include "pcf8574.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//USB-BSP ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/21
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   


  
//USB OTG �ײ�IO��ʼ��
//pdev:USB OTG�ں˽ṹ��ָ��
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
//	RCC->AHB1ENR|=1<<0;    		//ʹ��PORTAʱ��	    
// 	RCC->AHB2ENR|=1<<7;    		//ʹ��USB OTGʱ��	
//	GPIO_Set(GPIOA,3<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);	//PA11/12���ù������	
//	GPIO_Set(GPIOA,PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);	//PA15�������	
////  	PCF8574_WriteBit(USB_PWR_IO,1);//����USB HOST��Դ����
//   	GPIO_AF_Set(GPIOA,11,10);	//PA11,AF10(USB)
// 	GPIO_AF_Set(GPIOA,12,10);	//PA12,AF10(USB)
	
	GPIO_InitTypeDef USB_PORT_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();       //ʹ��PORTAʱ��	  
	__HAL_RCC_USB_OTG_FS_CLK_ENABLE();  //ʹ��USB OTGʱ��	

	USB_PORT_Initure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
	USB_PORT_Initure.Mode = GPIO_MODE_AF_PP;
	USB_PORT_Initure.Pull = GPIO_NOPULL;
	USB_PORT_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	USB_PORT_Initure.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOA, &USB_PORT_Initure);//PA10��11���ù������

	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_15);
	USB_PORT_Initure.Pin = GPIO_PIN_15;
	USB_PORT_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	USB_PORT_Initure.Pull = GPIO_NOPULL;
	USB_PORT_Initure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &USB_PORT_Initure);//PA10��11���ù������
}

//USB OTG �ж�����,����USB FS�ж�
//pdev:USB OTG�ں˽ṹ��ָ��
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
//   	MY_NVIC_Init(0,3,OTG_FS_IRQn,2);	//���ȼ�����Ϊ��ռ0,�����ȼ�3����2	 
	HAL_NVIC_SetPriority(OTG_FS_IRQn,0,3);        //��ռ���ȼ�0�������ȼ�3
}

//USB OTG �ж�����,����USB FS�ж�
//pdev:USB OTG�ں˽ṹ��ָ��
void USB_OTG_BSP_DisableInterrupt(void)
{ 
}
//USB OTG �˿ڹ�������(������δ�õ�)
//pdev:USB OTG�ں˽ṹ��ָ��
//state:0,�ϵ�;1,�ϵ�
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{ 
}
//USB_OTG �˿ڹ���IO����(������δ�õ�)
//pdev:USB OTG�ں˽ṹ��ָ��
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{ 
} 
//USB_OTG us����ʱ����
//�����̲���SYSTEM�ļ��е�delay.c�����delay_us����ʵ��
//�ٷ����̲��õ��Ƕ�ʱ��2��ʵ�ֵ�.
//usec:Ҫ��ʱ��us��.
void USB_OTG_BSP_uDelay (const uint32_t usec)
{ 
   	delay_us(usec);
}
//USB_OTG ms����ʱ����
//�����̲���SYSTEM�ļ��е�delay.c�����delay_ms����ʵ��
//�ٷ����̲��õ��Ƕ�ʱ��2��ʵ�ֵ�.
//msec:Ҫ��ʱ��ms��.
void USB_OTG_BSP_mDelay (const uint32_t msec)
{  
	delay_ms(msec);
}






















