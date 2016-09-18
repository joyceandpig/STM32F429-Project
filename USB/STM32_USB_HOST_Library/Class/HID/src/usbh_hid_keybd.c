/**
  ******************************************************************************
  * @file    usbh_hid_keybd.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    09-November-2015
  * @brief   This file is the application layer for USB Host HID Keyboard handling
  *          QWERTY and AZERTY Keyboard are supported as per the selection in 
  *          usbh_hid_keybd.h              
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_keybd.h"
#include "usbh_hid_gamepad.h"

/** @addtogroup USBH_LIB
* @{
*/

/** @addtogroup USBH_CLASS
* @{
*/

/** @addtogroup USBH_HID_CLASS
* @{
*/

/** @defgroup USBH_HID_KEYBD 
* @brief    This file includes HID Layer Handlers for USB Host HID class.
* @{
*/ 

/** @defgroup USBH_HID_KEYBD_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_HID_KEYBD_Private_FunctionPrototypes
* @{
*/ 
static void  KEYBRD_Init (void);
static void  KEYBRD_Decode(uint8_t *data);

/**
* @}
*/ 

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
 #if defined   (__CC_ARM) /*!< ARM Compiler */
  __align(4) 
 #elif defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4
 #elif defined (__GNUC__) /*!< GNU Compiler */
 #pragma pack(4) 
 #elif defined  (__TASKING__) /*!< TASKING Compiler */                           
  __align(4) 
 #endif /* __CC_ARM */
#endif
 
/** @defgroup USBH_HID_KEYBD_Private_Variables
* @{
*/
HID_cb_TypeDef HID_KEYBRD_cb= 
{
  KEYBRD_Init,
  KEYBRD_Decode
};
 

/**
* @}
*/ 


/** @defgroup USBH_HID_KEYBD_Private_Functions
* @{
*/ 


/**
* @brief  KEYBRD_Init.
*         Initialize the keyboard function.
* @param  None
* @retval None
*/
static void  KEYBRD_Init (void)
{
  /* Call User Init*/
  USR_KEYBRD_Init();
} 

//����Ԫ�ض�Ӧ��ϵ:A  A����  B  B���� Select Start  ��  ��   ��  ��
//                |   |     |   |     |       |    |   |   |   |
//          �ֱ�1:K   I     J   U    �ո�    �س�   W   S   A   D
//                |   |     |   |     |       |    |   |   |   |
//          �ֱ�2:3   6     2   5    �ո�    �س�  ��   ��  ��  ��

const u8 KEYBD_FCPAD1_TABLE[10]={0X0E,0X0C,0X0D,0X18,0X2C,0X28,0X1A,0X16,0X04,0X07};//ģ���ֱ�1
const u8 KEYBD_FCPAD2_TABLE[10]={0X5B,0X5E,0X5A,0X5D,0X2C,0X28,0X52,0X51,0X50,0X4F};//ģ���ֱ�2
//�������ݽ����FC�ֱ�����.
//fcbuf:������������
//mode:0,�����¼�����������;
//     1,���¼�����������
void KEYBRD_FCPAD_Decode(uint8_t *fcbuf,uint8_t mode)
{
	static u8 pbuf[6];		//�������һ��fc�ֱ���ֵ
	u8 j;
	u8 da1=0,db1=0;
	u8 da2=0,db2=0; 
	if(mode)
	{
		for(j=0;j<6;j++)pbuf[j]=fcbuf[j];//��������
	}
	for(j=0;j<6;j++)//���ж�A������?
	{ 
		if(KEYBD_FCPAD1_TABLE[1]==pbuf[j])
		{
			fcpad.b.a=!fcpad.b.a;	//�ֱ�1 A��ȡ��
			da1=1; 					//����ֱ�1 A������Ч
		} 
		if(KEYBD_FCPAD2_TABLE[1]==pbuf[j])
		{
			fcpad1.b.a=!fcpad1.b.a;	//�ֱ�2 A��ȡ��
			da2=1; 					//����ֱ�2 A������Ч
		}
		if(KEYBD_FCPAD1_TABLE[3]==pbuf[j])
		{
			fcpad.b.b=!fcpad.b.b;	//�ֱ�1 B��ȡ��
			db1=1; 					//����ֱ�1 B������Ч
		} 
		if(KEYBD_FCPAD2_TABLE[3]==pbuf[j])
		{
			fcpad1.b.b=!fcpad1.b.b;	//�ֱ�2 B��ȡ��
			db2=1; 					//����ֱ�2 B������Ч
		} 		
	}	
	if(da1==0)fcpad.b.a=0;
	if(da2==0)fcpad1.b.a=0;
	if(db1==0)fcpad.b.b=0;
	if(db2==0)fcpad1.b.b=0;
	fcpad.b.select=0;
	fcpad1.b.select=0;
 	fcpad.b.start=0;
	fcpad1.b.start=0;
 	fcpad.b.up=0;
	fcpad1.b.up=0;
 	fcpad.b.down=0;
	fcpad1.b.down=0;
 	fcpad.b.left=0;
	fcpad1.b.left=0;
 	fcpad.b.right=0;
	fcpad1.b.right=0; 
	for(j=0;j<6;j++)//���ж�A������?
	{ 
		if(da1==0)//�ֱ�1,A����û�а���
		{
			if(KEYBD_FCPAD1_TABLE[0]==pbuf[j])fcpad.b.a=1;	//�ֱ�1 A������ 
		}
		if(da2==0)//�ֱ�2,A����û�а���
		{
			if(KEYBD_FCPAD2_TABLE[0]==pbuf[j])fcpad1.b.a=1;	//�ֱ�2 A������ 
		}
		if(db1==0)//�ֱ�1,B����û�а���
		{
			if(KEYBD_FCPAD1_TABLE[2]==pbuf[j])fcpad.b.b=1;	//�ֱ�1 B������ 
		}
		if(db2==0)//�ֱ�2,B����û�а���
		{
			if(KEYBD_FCPAD2_TABLE[2]==pbuf[j])fcpad1.b.b=1;	//�ֱ�2 B������ 
		} 
		if(KEYBD_FCPAD1_TABLE[4]==pbuf[j])fcpad.b.select=1;	//�ֱ�1 select���� 
		if(KEYBD_FCPAD2_TABLE[4]==pbuf[j])fcpad1.b.select=1;//�ֱ�2 select���� 
		if(KEYBD_FCPAD1_TABLE[5]==pbuf[j])fcpad.b.start=1;	//�ֱ�1 start���� 
		if(KEYBD_FCPAD2_TABLE[5]==pbuf[j])fcpad1.b.start=1;	//�ֱ�2 start���� 		
		if(KEYBD_FCPAD1_TABLE[6]==pbuf[j])fcpad.b.up=1;		//�ֱ�1 up���� 
		if(KEYBD_FCPAD2_TABLE[6]==pbuf[j])fcpad1.b.up=1;	//�ֱ�2 up���� 
		if(KEYBD_FCPAD1_TABLE[7]==pbuf[j])fcpad.b.down=1;	//�ֱ�1 down���� 
		if(KEYBD_FCPAD2_TABLE[7]==pbuf[j])fcpad1.b.down=1;	//�ֱ�2 down���� 
		if(KEYBD_FCPAD1_TABLE[8]==pbuf[j])fcpad.b.left=1;	//�ֱ�1 left���� 
		if(KEYBD_FCPAD2_TABLE[8]==pbuf[j])fcpad1.b.left=1;	//�ֱ�2 left���� 
		if(KEYBD_FCPAD1_TABLE[9]==pbuf[j])fcpad.b.right=1;	//�ֱ�1 right���� 
		if(KEYBD_FCPAD2_TABLE[9]==pbuf[j])fcpad1.b.right=1;	//�ֱ�2 right���� 
	}	
//	printf("pad1:%x\r\n",fcpad);
//	printf("pad2:%x\r\n",fcpad1);
//	for(i=0;i<8;i++)
//	{
//		printf("%02x ",pbuf[i]);
//	}
//	printf("\r\n"); 
}

//�������ݽ���
static void KEYBRD_Decode(uint8_t *pbuf)
{
	static u8 fcbuf[6];//�������һ��fc�ֱ���ֵ
	u8 i;
	pbuf+=2;
	for(i=0;i<6;i++)if(pbuf[i]!=fcbuf[i])break;//�����,˵�����µİ�������仯 
 	if(i==6)return;						//���ΰ���û���κα仯,ֱ�ӷ���
	for(i=0;i<6;i++)fcbuf[i]=pbuf[i];	//�����µİ���ֵ 
	KEYBRD_FCPAD_Decode(fcbuf,1);		//����FC�ֱ�����
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/


/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

