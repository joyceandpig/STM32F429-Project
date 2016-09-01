#ifndef __USBH_HID_GAMEPAD_H
#define __USBH_HID_GAMEPAD_H

#include "usbh_hid_core.h"
 

//FC��Ϸ�ֱ����ݸ�ʽ����
//1,��ʾû�а���,0��ʾ����.
typedef union _FC_GamePad_TypeDef 
{
	u8 ctrlval;
	struct
	{
		u8 a:1;		//A��
		u8 b:1;		//B��
		u8 select:1;//Select��
		u8 start:1; //Start��
		u8 up:1;	//��
		u8 down:1;	//��
		u8 left:1;	//��
		u8 right:1; //��
	}b;
}FC_GamePad_TypeDef ; 
extern FC_GamePad_TypeDef fcpad;	//fc��Ϸ�ֱ�1
extern FC_GamePad_TypeDef fcpad1;	//fc��Ϸ�ֱ�2
extern HID_cb_TypeDef HID_GAMEPAD_cb; 


 

void USR_GAMEPAD_Init(void);
void USR_GAMEPAD_ProcessData(uint8_t data);
 


#endif 






















