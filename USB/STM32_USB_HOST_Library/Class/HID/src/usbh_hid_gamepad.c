#include "usbh_hid_gamepad.h"
#include "usart.h"


//FC��Ϸ�ֱ�ģ��
//��ȡfcpad.ctrlval,���ɵõ���ǰ�ֱ���ʽת����FC�ֱ���ʽ.
FC_GamePad_TypeDef fcpad;
FC_GamePad_TypeDef fcpad1;


static void  GAMEPAD_Init (void);
static void  GAMEPAD_Decode(u8	*data);
  

HID_cb_TypeDef HID_GAMEPAD_cb = 
{
	GAMEPAD_Init,
	GAMEPAD_Decode,
};  

//game pad ��ʼ��
static void  GAMEPAD_Init ( void)
{ 
	USR_GAMEPAD_Init();
} 

//ͨ��USB���������,��������fcpad����
//data0,data1:USB�õ��ķ������� 
//���ݸ�ʽ����:
//data0:00,�������;ff,�Ҽ�����,7F,û�а�������
//data1:00,�ϼ�����;ff,�¼�����,7F,û�а�������
void GAMEPAD_nDir_Decode(u8 data0,u8 data1)
{
	switch(data0)
	{
		case 0X7F:
			fcpad.b.left=0;
			fcpad.b.right=0;
			break;
		case 0X00:
			fcpad.b.left=1; 
			break;
		case 0XFF:
			fcpad.b.right=1; 
			break;
	}
	switch(data1)
	{
		case 0X7F:
			fcpad.b.up=0;
			fcpad.b.down=0;
			break;
		case 0X00:
			fcpad.b.up=1; 
			break;
		case 0XFF:
			fcpad.b.down=1; 
			break;
	}
}
//ͨ��USB AB������,��������fcpad����
//data:USB�õ���1/2/3/4������ 
//���ݸ�ʽ����:
//data:���4λ��Ч  
//b4=1,1������(FC:B������)
//b5=1,2������(FC:A������)
//b6=1,3������(FC:A��)
//b7=1,4������(FC:B��)
void GAMEPAD_nAB_Decode(u8 data)
{
	if(data&0X10)fcpad.b.b=!fcpad.b.b;	//B��ȡ��
	else
	{
		if(data&0X80)fcpad.b.b=1;		//B������
		else fcpad.b.b=0;				//B���ɿ�  
	}
	if(data&0X20)fcpad.b.a=!fcpad.b.a;	//A��ȡ�� 
	else
	{
		if(data&0X40)fcpad.b.a=1;		//A������
		else fcpad.b.a=0;				//A���ɿ�
	}	
}
//ͨ��USB ���ܼ�����,��������fcpad����
//data:USB�õ���Select/Start������ 
//���ݸ�ʽ����:
//data:b4,b5��Ч.
//b4=1,Select������
//b5=1,Start������ 
void GAMEPAD_nFun_Decode(u8 data)
{
	if(data&0X10)fcpad.b.select=1;	//Select������
	else fcpad.b.select=0;			//Select���ɿ�
	if(data&0X20)fcpad.b.start=1;	//Start������
	else fcpad.b.start=0;			//Start���ɿ� 
} 

//game pad���ݽ���
static void  GAMEPAD_Decode(uint8_t *data)
{   
	u8 i; 
	if(data[7]==0X00)		//����PU201�ֱ�,���һ���ֽ�Ϊ0X00
	{
		GAMEPAD_nDir_Decode(data[3],data[4]);	//���뷽���
		GAMEPAD_nAB_Decode(data[5]);			//����AB��
		GAMEPAD_nFun_Decode(data[6]);			//���빦�ܼ�
		USR_GAMEPAD_ProcessData(fcpad.ctrlval);	//��ʾ���
	}else if(data[7]==0XC0)	//����PU401��,���һ���ֽ�Ϊ0XC0
	{
		GAMEPAD_nDir_Decode(data[0],data[1]);	//���뷽���
		GAMEPAD_nAB_Decode(data[5]);			//����AB��
		GAMEPAD_nFun_Decode(data[6]);			//���빦�ܼ�
		USR_GAMEPAD_ProcessData(fcpad.ctrlval);	//��ʾ���		
	}else
	{
		for(i=0;i<8;i++)printf("%02x ",data[i]);
		printf("\r\n");
	}
} 




















