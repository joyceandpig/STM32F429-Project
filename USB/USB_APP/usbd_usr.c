#include "usbd_usr.h"
#include "usb_dcd_int.h"
#include <stdio.h> 
#include "usart.h" 
#include "usb_app.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//USBD-USR ����	   
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



//ָ��DEVICE_PROP�ṹ��
//USB Device �û��ص�����. 
USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};
//USB Device �û��Զ����ʼ������
void USBD_USR_Init(void)
{
	//printf("USBD_USR_Init\r\n");
} 
//USB Device ��λ
//speed:USB�ٶ�,0,����;1,ȫ��;����,����.
void USBD_USR_DeviceReset (uint8_t speed)
{
	switch (speed)
	{
		case USB_OTG_SPEED_HIGH:
			printf("USB Device Library v1.1.0  [HS]\r\n");
			break; 
		case USB_OTG_SPEED_FULL: 
			printf("USB Device Library v1.1.0  [FS]\r\n");
			break;
		default:
			printf("USB Device Library v1.1.0  [??]\r\n"); 
			break;
	}
}
//USB Device ���óɹ�
void USBD_USR_DeviceConfigured (void)
{
	usbx.bDeviceState|=0X80;
	printf("MSC Interface started.\r\n"); 
} 
//USB Device����
void USBD_USR_DeviceSuspended(void)
{
	usbx.bDeviceState=0;//USB������
	printf("Device In suspend mode.\r\n");
} 
//USB Device�ָ�
void USBD_USR_DeviceResumed(void)
{ 
	printf("Device Resumed\r\n");
}
//USB Device���ӳɹ�
void USBD_USR_DeviceConnected (void)
{
	printf("USB Device Connected.\r\n");
}
//USB Deviceδ����
void USBD_USR_DeviceDisconnected (void)
{
	usbx.bDeviceState=0;
	printf("USB Device Disconnected.\r\n");
} 








