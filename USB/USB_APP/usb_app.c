#include "usb_app.h" 
#include "delay.h"   
#include "malloc.h"    
#include "exfuns.h"      
#include "includes.h"      
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//USB-APP ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

//USB HOST
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core_dev; 
_usb_app usbx;//USB APP������ 


//USB OTG �жϷ�����
//��������USB�ж�
void OTG_FS_IRQHandler(void)
{ 
	OSIntEnter();    		    
	if (USB_OTG_IsHostMode(&USB_OTG_Core_dev)) //ȷ���ǲ���USB����ģʽ?
	{  
		USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);//USB�����ж�
	}
	else
	{
		USBD_OTG_ISR_Handler(&USB_OTG_Core_dev);//USB�Ի��ж�
	}
	OSIntExit();  											 
}  
//��ʼ��USB
void usbapp_init(void)
{
	usbx.bDeviceState=0;
	usbx.hdevclass=0;
	usbx.mode=0XFF; 	//����Ϊһ���Ƿ���ģʽ,�����ȵ���usbapp_mode_set����ģʽ,������������
} 
extern HID_Machine_TypeDef HID_Machine;	
//USB��ѯ����,���������Եı�����.
void usbapp_pulling(void)
{
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			if(HCD_IsDeviceConnected(&USB_OTG_Core_dev)==0||usbx.hdevclass!=1)//U������ʧ����,��Ҫ���´���USB����
			{
				USBH_Process(&USB_OTG_Core_dev, &USB_Host);//����USB����
				if(USBH_Check_EnumeDead(&USB_Host))	//���USB HOST �Ƿ�������?������,�����³�ʼ�� 
				{ 	    
					usbapp_mode_set(USBH_MSC_MODE);//����
				}	
			}			
			break;
		case USBH_HID_MODE: 
			USBH_Process(&USB_OTG_Core_dev, &USB_Host); 
			if(usbx.bDeviceState&0X80)//���ӽ�����
			{ 
				if(USBH_Check_HIDCommDead(&USB_OTG_Core_dev,&HID_Machine))//���USB HIDͨ��,�Ƿ�����? 
				{ 	    
					usbapp_mode_set(USBH_HID_MODE);//����
				}				
				
			}else	//����δ������ʱ��,���
			{
				if(USBH_Check_EnumeDead(&USB_Host))	//���USB HOST �Ƿ�������?������,�����³�ʼ�� 
				{ 	    
					usbapp_mode_set(USBH_HID_MODE);//����
				}			
			}		
			break;		
		case USBD_MSC_MODE:
			break;	  
	}	
}
//USB������ǰ����ģʽ
void usbapp_mode_stop(void)
{
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			myfree(SRAMIN,MSC_Machine.buff);	//�ͷ��ڴ�
			myfree(SRAMIN,USBH_DataInBuffer);	//�ͷ��ڴ�
			myfree(SRAMIN,USBH_DataOutBuffer);	//�ͷ��ڴ�
		case USBH_HID_MODE: 
			USBH_DeInit(&USB_OTG_Core_dev,&USB_Host);	//��λUSB HOST
			USB_OTG_StopHost(&USB_OTG_Core_dev);		//ֹͣUSBhost
			if(USB_Host.usr_cb->DeviceDisconnected)		//����,�Ž�ֹ
			{
				USB_Host.usr_cb->DeviceDisconnected(); 	//�ر�USB����
				USBH_DeInit(&USB_OTG_Core_dev, &USB_Host);
				USB_Host.usr_cb->DeInit();
				USB_Host.class_cb->DeInit(&USB_OTG_Core_dev,&USB_Host.device_prop);
			}
			USB_OTG_DisableGlobalInt(&USB_OTG_Core_dev);//�ر������ж�
			myfree(SRAMIN,USBH_CfgDesc);	//�ͷ��ڴ�
			break;		
		case USBD_MSC_MODE:
			DCD_DevDisconnect(&USB_OTG_Core_dev);
			USB_OTG_StopDevice(&USB_OTG_Core_dev); 
			myfree(SRAMIN,MSC_BOT_Data);	//�ͷ��ڴ�
			break;	 
	} 
	RCC->AHB2RSTR|=1<<7;	//USB OTG FS ��λ
	delay_ms(5);
	RCC->AHB2RSTR&=~(1<<7);	//��λ����   
	memset(&USB_OTG_Core_dev,0,sizeof(USB_OTG_CORE_HANDLE));
	memset(&USB_Host,0,sizeof(USB_Host));
	usbx.mode=0XFF;
	usbx.bDeviceState=0;
	usbx.hdevclass=0; 
}
//����USB����ģʽ
//mode:0,USB HOST MSCģʽ(Ĭ��ģʽ,��U��)
//	   1,USB HOST HIDģʽ(���������̵�)
//	   2,USB Device MSCģʽ(USB������) 
void usbapp_mode_set(u8 mode)
{
	usbapp_mode_stop();//��ֹͣ��ǰUSB����ģʽ
	usbx.mode=mode;
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			MSC_Machine.buff=mymalloc(SRAMIN,USBH_MSC_MPS_SIZE);
			USBH_DataInBuffer=mymalloc(SRAMIN,512);
			USBH_DataOutBuffer=mymalloc(SRAMIN,512);
		
			USBH_CfgDesc=mymalloc(SRAMIN,512); 
			USBH_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
			break;
		case USBH_HID_MODE: 
			USBH_CfgDesc=mymalloc(SRAMIN,512);
			USBH_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USB_Host,&HID_cb,&USR_Callbacks);  
			break;		
		case USBD_MSC_MODE:
			MSC_BOT_Data=mymalloc(SRAMIN,MSC_MEDIA_PACKET);
			USBD_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
			break;	  
	}
}







