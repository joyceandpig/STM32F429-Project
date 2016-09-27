#include "usbplay.h"
#include "usb_app.h" 
#include "sdio_sdcard.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-USB���� ����	   
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
//USB����  				 
u8 usb_play(void)
{			  
	MSC_BOT_Data=mymalloc(SRAMEX,MSC_MEDIA_PACKET);			//�����ڴ�SRAMIN
	USBD_Init(&USB_OTG_Core_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);		    
	Sleep(2000);
//	u8 rval=0;			//����ֵ	  
// 	u8 offline_cnt=0;
//	u8 tct=0;
// 	u8 USB_STA=0; 
//	u8 busycnt=0;		//USBæ������
//	u8 errcnt=0;		//USB���������
//	if(gui_phy.memdevflag&(1<<3))
//	{
////		window_msg_box((lcddev.width-220)/2,(lcddev.height-100)/2,220,100,(u8*)nes_remindmsg_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
//		while(gui_phy.memdevflag&(1<<3))
//		{
//			delay_ms(5);//��ѭ���ȴ�U�̱��γ�
//		}
//	}
//	POINT_COLOR=BLUE;  
//  BACK_COLOR=LGRAY; 
//	LCD_Clear(BACK_COLOR);
//	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[12][gui_phy.language],0X05);//��ʾ���� 
//	usbapp_mode_set(USBD_MSC_MODE);//DEVICE MSC 
//	LCD_ShowString(60+(lcddev.width-240)/2,130+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB DisConnected ");//������ʾUSB������
// 	while(rval==0)
//	{
// 		delay_ms(1000/OS_TICKS_PER_SEC);//��ʱһ��ʱ�ӽ���
//		if(system_task_return)
//		{
//			break;
//			delay_ms(10);
//			TPAD_Scan(1);
//			delay_ms(10);
//			if(TPAD_Scan(1))
//			{
//				if(busycnt)//USB���ڶ�д
//				{
//					POINT_COLOR=RED;  					    
//					LCD_ShowString(60+(lcddev.width-240)/2,110+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB BUSY!!!");//��ʾSD���Ѿ�׼����
//					POINT_COLOR=BLUE;  
//					system_task_return=0;	//ȡ��   
//				}else break;				//USB����,���˳�USB	 
//			}else system_task_return=0;  		
//		} 
//		tct++; 
//		if(tct==40)//ÿ40ms����һ��
//		{
//			tct=0;
//			if(busycnt){
//				busycnt--;
//			}else {
//				gui_fill_rectangle(60+(lcddev.width-240)/2,110+(lcddev.height-320)/2,100,16,BACK_COLOR);//�����ʾ
//			}
//			if(errcnt){
//				errcnt--;
//			}else {
//				gui_fill_rectangle(60+(lcddev.width-240)/2,170+(lcddev.height-320)/2,128,16,BACK_COLOR);//�����ʾ
//			}
//			if(usbx.bDeviceState&0x10){//����ѯ����
//				offline_cnt=0;				//USB������,�����offline������
//				usbx.bDeviceState|=0X80;	//���USB��������
//				usbx.bDeviceState&=~(1<<4);	//�����ѯ��־λ
//			}else{
//				offline_cnt++;  
//				if(offline_cnt>50)usbx.bDeviceState=0;//2s��û�յ����߱��,����USB���γ���
//			} 
//		} 
//		if(USB_STA!=usbx.bDeviceState){//״̬�ı��� 
//			gui_fill_rectangle(60+(lcddev.width-240)/2,150+(lcddev.height-320)/2,120,16,BACK_COLOR);//�����ʾ 
//			if(usbx.bDeviceState&0x01){//����д		  
//				if(busycnt<5){
//					busycnt++;
//				}
//				LCD_ShowString(60+(lcddev.width-240)/2,150+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Writing...");//��ʾUSB����д������
//			}
//			if(usbx.bDeviceState&0x02)//���ڶ�
//			{					
//				if(busycnt<5)busycnt++;
//				LCD_ShowString(60+(lcddev.width-240)/2,150+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Reading...");//��ʾUSB���ڶ�������  
//			}	
//			POINT_COLOR=RED; 
//			if(usbx.bDeviceState&0x04)
//			{
//				if(errcnt<5)errcnt++;
//				LCD_ShowString(60+(lcddev.width-240)/2,170+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Write Error");//��ʾд�����
// 			}
//			if(usbx.bDeviceState&0x08)
//			{
//				if(errcnt<5)errcnt++;
//				LCD_ShowString(60+(lcddev.width-240)/2,170+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Read  Error");//��ʾ��������
//			}
//			POINT_COLOR=BLUE;  
//			if(usbx.bDeviceState&0X80)LCD_ShowString(60+(lcddev.width-240)/2,130+(lcddev.height-320)/2,lcddev.width,lcddev.height,16,"USB Connected   ");//��ʾUSB�����Ѿ�����
// 			else LCD_ShowString(60+(lcddev.width-240)/2,130+(lcddev.height-320)/2,lcddev.width,320,16,"USB DisConnected");//��ʾUSB���γ���
// 			usbx.bDeviceState&=0X90;	//���������״̬&��ѯ��־�����������λ.
//			USB_STA=usbx.bDeviceState;	//��¼����״̬
//		}
//	}	
//	BACK_COLOR=BLACK;
//	usbapp_mode_set(USBH_MSC_MODE);//HOST MSC 
//	return rval; 
}







