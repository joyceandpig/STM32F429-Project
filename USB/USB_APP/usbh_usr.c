#include "usbh_usr.h"
#include "usb_hcd_int.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"  
#include "usbh_hid_gamepad.h"  
#include "delay.h"  
#include "lcd.h"  
#include "string.h" 
#include "usb_app.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//USBH-USR ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
static u8 AppState;

 
//USB HOST �û��ص�����.
USBH_Usr_cb_TypeDef USR_Callbacks =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};
 
//USB HOST ��ʼ�� 
void USBH_USR_Init(void)
{
	USB_OTG_GINTMSK_TypeDef intmsk;
	intmsk.d32=USB_OTG_READ_REG32(&USB_OTG_Core_dev.regs.GREGS->GINTMSK);
	intmsk.b.usbsuspend=0;	//�رչ����ж� 
	USB_OTG_WRITE_REG32(&USB_OTG_Core_dev.regs.GREGS->GINTMSK,intmsk.d32); 
	printf("USB OTG FS MSC Host\r\n");
	printf("> USB Host library started.\r\n");
	printf("  USB Host Library v2.1.0\r\n\r\n");
	
}
//��⵽U�̲���
void USBH_USR_DeviceAttached(void)//U�̲���
{ 
	usbx.bDeviceState|=1<<6;//��⵽USB����
	printf("��⵽USB�豸����!\r\n");
}
//��⵽U�̰γ�
void USBH_USR_DeviceDisconnected (void)//U���Ƴ�
{ 
	printf("USB�豸�γ�!\r\n");
	usbx.bDeviceState=0;//����ʧЧ
	usbx.hdevclass=0;	//�豸������� 
}  
//��λ�ӻ�
void USBH_USR_ResetDevice(void)
{
	printf("��λ�豸...\r\n");
}
//��⵽�ӻ��ٶ�
//DeviceSpeed:�ӻ��ٶ�(0,1,2 / ����)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("����(HS)USB�豸!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("ȫ��(FS)USB�豸!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("����(LS)USB�豸!\r\n");  
	}
	else
	{
		printf("�豸����!\r\n");  
	}
}
//��⵽�ӻ���������
//DeviceDesc:�豸������ָ��
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}
//�ӻ���ַ����ɹ�
void USBH_USR_DeviceAddressAssigned(void)
{
	printf("�ӻ���ַ����ɹ�!\r\n");   
}
//��������������Ч
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		printf("���ƶ��洢���豸!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		printf("HID �豸!\r\n"); 
	}    
}
//��ȡ���豸Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}
//��ȡ���豸Product String 
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString);  
}
//��ȡ���豸SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 
//�豸USBö�����
void USBH_USR_EnumerationDone(void)
{ 
	printf("�豸ö�����!\r\n\r\n");    
} 
//�޷�ʶ���USB�豸
void USBH_USR_DeviceNotSupported(void)
{  
	printf("�޷�ʶ���USB�豸!\r\n\r\n");    
}  
//�ȴ��û����밴��,ִ����һ������
USBH_USR_Status USBH_USR_UserInput(void)
{ 
	printf("�����û�ȷ�ϲ���!\r\n");
	usbx.bDeviceState|=0X80;		//���������
	return USBH_USR_RESP_OK; 
} 
//USB�ӿڵ�������
void USBH_USR_OverCurrentDetected (void)
{
	printf("�˿ڵ�������!!!\r\n");
}  
//���³�ʼ��
void USBH_USR_DeInit(void)
{
	printf("���³�ʼ��!!!\r\n");
}
//�޷��ָ��Ĵ���!!  
void USBH_USR_UnrecoveredError (void)
{
	printf("�޷��ָ��Ĵ���!!!\r\n\r\n");	
} 
int USBH_USR_MSC_Application(void)
{
	u8 res=0;
  	switch(AppState)
  	{
    	case USH_USR_FS_INIT://��ʼ���ļ�ϵͳ 
			printf("��ʼִ���û�����!!!\r\n");
			AppState=USH_USR_FS_TEST;
      		break;
    	case USH_USR_FS_TEST:	//ִ��USB OTG ����������
			//res=USH_User_App(); //�û�������
			if(HCD_IsDeviceConnected(&USB_OTG_Core_dev))//�豸���ӳɹ�
			{
				usbx.hdevclass=1;	//��⵽U�̲���
				//do nothing
			}else 
			{ 
				AppState=USH_USR_FS_INIT;
      		}
			break;
    	default:break;
  	} 
	return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
//������������,ΪALIENTEK���,�Է�ֹUSB����

//USBö��״̬�������,��ֹUSBö��ʧ�ܵ��µ�����
//phost:USB_HOST�ṹ��ָ��
//����ֵ:0,û������
//       1,������,�ⲿ������������USB����.
u8 USBH_Check_EnumeDead(USBH_HOST *phost)
{
	static u16 errcnt=0;
	//���״̬,�����������,��˵��USB������.
	if(phost->gState==HOST_CTRL_XFER&&(phost->EnumState==ENUM_IDLE||phost->EnumState==ENUM_GET_FULL_DEV_DESC))
	{
		errcnt++;
		if(errcnt>2000)//������
		{ 
			errcnt=0;
			RCC->AHB2RSTR|=1<<7;	//USB OTG FS ��λ
			delay_ms(5);
			RCC->AHB2RSTR&=~(1<<7);	//��λ����  
			return 1;
		} 
	}else errcnt=0;
	return 0;
} 
//USB HIDͨ���������,��ֹUSBͨ������(��ʱ�����:DTERR,��Data toggle error)
//pcore:USB_OTG_Core_dev_HANDLE�ṹ��ָ��
//phidm:HID_Machine_TypeDef�ṹ��ָ�� 
//����ֵ:0,û������
//       1,������,�ⲿ������������USB����.
u8 USBH_Check_HIDCommDead(USB_OTG_CORE_HANDLE *pcore,HID_Machine_TypeDef *phidm)
{
 	if(pcore->host.HC_Status[phidm->hc_num_in]==HC_DATATGLERR)//��⵽DTERR����,ֱ������USB.
	{  
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
//USB����������ݴ���
//USB���/����/��Ϸ�ֱ���غ���
//����ʼ��
void USR_MOUSE_Init(void)
{
	usbx.hdevclass=2;		//��⵽������ 
}
//���̳�ʼ��
void  USR_KEYBRD_Init(void)
{ 
	usbx.hdevclass=3;		//��⵽���̲��� 
}
//��Ϸ�ֱ���ʼ��
void USR_GAMEPAD_Init(void)
{
	usbx.hdevclass=4;		//��⵽�ֱ� 
}

//��ʱ����,���ڴ���������/������������(4.3��,����������2016�ֽ�)
//__align(4) u8 tbuf[2017]; 

//USB������ݴ���
//data:USB������ݽṹ��ָ��
void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *data)
{  
//	static signed short x,y,z; 
//	if(USB_FIRST_PLUGIN_FLAG)//��һ�β���,����������
//	{
//		USB_FIRST_PLUGIN_FLAG=0;
//		x=y=z=0;
//	}
//	x+=(signed char)data->x;
//	if(x>9999)x=9999;
//	if(x<-9999)x=-9999;
//	y+=(signed char)data->y;
//	if(y>9999)y=9999;
//	if(y<-9999)y=-9999;
//	z+=(signed char)data->z;
//	if(z>9999)z=9999;
//	if(z<-9999)z=-9999;
//	POINT_COLOR=BLUE;
//	sprintf((char*)tbuf,"BUTTON:");
//	if(data->button&0X01)strcat((char*)tbuf,"LEFT ");
//	if((data->button&0X03)==0X02)strcat((char*)tbuf,"RIGHT");
//	else if((data->button&0X03)==0X03)strcat((char*)tbuf,"+RIGHT");
//	if((data->button&0X07)==0X04)strcat((char*)tbuf,"MID  ");
//	else if((data->button&0X07)>0X04)strcat((char*)tbuf,"+MID");  
// 	LCD_Fill(30+56,180,lcddev.width-1,180+16,WHITE);	
//	LCD_ShowString(30,180,210,16,16,tbuf);	
//	sprintf((char*)tbuf,"X POS:%05d",x); 
//	LCD_ShowString(30,200,200,16,16,tbuf);	
//	sprintf((char*)tbuf,"Y POS:%05d",y);
//	LCD_ShowString(30,220,200,16,16,tbuf);	
//	sprintf((char*)tbuf,"Z POS:%05d",z);
//	LCD_ShowString(30,240,200,16,16,tbuf);	 
	//printf("btn,X,Y,Z:0x%x,%d,%d,%d\r\n",data->button,(signed char)data->x,(signed char)data->y,(signed char)data->z);   
} 


//USB�������ݴ���
//data:USB������ݽṹ��ָ��
void  USR_KEYBRD_ProcessData (uint8_t data)
{ 
//	static u16 pos; 
//	static u16 endx,endy;
//	static u16 maxinputchar;
//	
//	u8 buf[4];
//	if(USB_FIRST_PLUGIN_FLAG)//��һ�β���,����������
//	{
//		USB_FIRST_PLUGIN_FLAG=0;
//		endx=((lcddev.width-30)/8)*8+30;		//�õ�endxֵ
//		endy=((lcddev.height-220)/16)*16+220;	//�õ�endyֵ
//		maxinputchar=((lcddev.width-30)/8);
//		maxinputchar*=(lcddev.height-220)/16;	//��ǰLCD��������ʾ���ַ���.
// 		pos=0; 
//	}
//	POINT_COLOR=BLUE;
//	sprintf((char*)buf,"%02X",data);
//	LCD_ShowString(30+56,180,200,16,16,buf);//��ʾ��ֵ	 
//	if(data>=' '&&data<='~')
//	{
//		tbuf[pos++]=data;
//		tbuf[pos]=0;		//��ӽ�����. 
//		if(pos>maxinputchar)pos=maxinputchar;//���������ô��
//	}else if(data==0X0D)	//�˸��
//	{
//		if(pos)pos--;
//		tbuf[pos]=0;		//��ӽ�����.  
//	}
//	if(pos<=maxinputchar)	//û�г�����ʾ��
//	{
//		LCD_Fill(30,220,endx,endy,WHITE);
//		LCD_ShowString(30,220,endx-30,endy-220,16,tbuf);
//	}		
	//printf("KEY Board Value:%02X\r\n",data);
	//printf("KEY Board Char:%c\r\n",data); 
}

u8* const JOYPAD_SYMBOL_TBL[8]=
{"Right","Left","Down","Up","Start","Select","A","B"};//�ֱ��������Ŷ���
//USB�������ݴ���
//data:USB������ݽṹ��ָ��
void  USR_GAMEPAD_ProcessData(uint8_t data)
{
//	u8 i;
//	if(data)
//	{ 
//		for(i=0;i<8;i++)
//		{
//			if(data&(1<<i))
//			{
//				printf("key:%s\r\n",(u8*)JOYPAD_SYMBOL_TBL[i]);
// 			}		
//		}		    
//	}
}

////////////////////////////////////////////////////////////////////////////////////////
//�û����庯��,ʵ��fatfs diskio�Ľӿں��� 
//��ȡU��״̬
//����ֵ:0,U��δ����
//      1,����
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core_dev);//����U��״̬
}

//��U��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;		 
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	u32 retry=0;
	if(HCD_IsDeviceConnected(&USB_OTG_Core_dev)&&AppState==USH_USR_FS_TEST)//���ӻ�����,����APP����״̬
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core_dev,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core_dev ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core_dev))
			{
				res=1;//��д����
				break;
			}; 
			retry++;
		}while(res==USBH_MSC_BUSY&&retry<0X7FFFFFF);
		if(retry==0X7FFFFFF)res=1;
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

//дU��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;		 
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core_dev)&&AppState==USH_USR_FS_TEST)//���ӻ�����,����APP����״̬
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core_dev,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core_dev ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core_dev))
			{
				res=1;//��д����
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}














