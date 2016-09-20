#include "wm8978.h"
#include "myiic.h" 
#include "delay.h"
#include "common.h"
#include "settings.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal.h"
#include "24cxx.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//WM8978 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/11
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
GPIO_InitTypeDef WM8978_GPIO_Handler;
extern I2C_HandleTypeDef I2cHandle;
//WM8978Ĭ�ϲ���
_wm8978_obj wm8978set=
{
	50,			//����:50
	0,0,0,0,0,	//Ƶ�ʱ�־��Ϊ0
	0,0,0,0,0,	//����Ƶ�������ȫ0
	0,			//3DЧ��Ϊ0
	1,			//������������		  	 
	0,			 //�����־,0X0A,�������;����,����δ����	   
};

//WM8978���ݱ�����:SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings)֮��
//��ȡWM8978����
//wm8978dev:wm8978����
void wm8978_read_para(_wm8978_obj * wm8978dev)
{
	AT24CXX_Read(SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings),(u8*)wm8978dev,sizeof(_wm8978_obj));
}
//д��WM8978����
//wm8978dev:wm8978����
void wm8978_save_para(_wm8978_obj * wm8978dev)
{										   
  OS_CPU_SR cpu_sr=0;
	OS_ENTER_CRITICAL();	//�����ٽ���(�޷����жϴ��) 
	AT24CXX_Write(SYSTEM_PARA_SAVE_BASE+sizeof(_system_setings),(u8*)wm8978dev,sizeof(_wm8978_obj));
	OS_EXIT_CRITICAL();		//�˳��ٽ���(���Ա��жϴ��)
} 

//WM8978�Ĵ���ֵ������(�ܹ�58���Ĵ���,0~57),ռ��116�ֽ��ڴ�
//��ΪWM8978��IIC������֧�ֶ�����,�����ڱ��ر������мĴ���ֵ
//дWM8978�Ĵ���ʱ,ͬ�����µ����ؼĴ���ֵ,���Ĵ���ʱ,ֱ�ӷ��ر��ر���ļĴ���ֵ.
//ע��:WM8978�ļĴ���ֵ��9λ��,����Ҫ��u16���洢. 
static u16 WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 
//WM8978��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
u8 WM8978_Init(void)
{
//	u8 res; 
// 	RCC->AHB1ENR|=1<<4;			//ʹ������PORTEʱ��   
//// 	GPIO_Set(GPIOE,PIN2|PIN3|PIN4|PIN5|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PE2~6 ���ù������
// 	GPIO_AF_Set(GPIOE,2,6);		//PE2,AF6  SAI_MCLKA
//	GPIO_AF_Set(GPIOE,3,6);		//PE3,AF6  SAI_SDB    WM8978 I2S ADC���������
//	GPIO_AF_Set(GPIOE,4,6);		//PE4,AF6  SAI_FSA
//	GPIO_AF_Set(GPIOE,5,6);		//PE5,AF6  SAI_SCKA
// 	GPIO_AF_Set(GPIOE,6,6);		//PE6,AF6  SAI_SDA  
//	
//	IIC_Init();//��ʼ��IIC�ӿ�
//	res=WM8978_Write_Reg(0,0);	//��λWM8978
//	if(res)return 1;			//����ָ��ʧ��,WM8978�쳣
//	//����Ϊͨ������
//	WM8978_Write_Reg(1,0X1B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
//	WM8978_Write_Reg(2,0X1B0);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
//	WM8978_Write_Reg(3,0X6C);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
//	WM8978_Write_Reg(6,0);		//R6,MCLK���ⲿ�ṩ
//	WM8978_Write_Reg(43,1<<4);	//R43,INVROUT2����,��������
//	WM8978_Write_Reg(47,1<<8);	//R47����,PGABOOSTL,��ͨ��MIC���20������
//	WM8978_Write_Reg(48,1<<8);	//R48����,PGABOOSTR,��ͨ��MIC���20������
//	WM8978_Write_Reg(49,1<<1);	//R49,TSDEN,�������ȱ��� 
//	WM8978_Write_Reg(49,1<<2);	//R49,SPEAKER BOOST,1.5x 
//	WM8978_Write_Reg(10,1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
//	WM8978_Write_Reg(14,1<<3);	//R14,ADC 128x������
//	return 0;
	u8 res; 
	__HAL_RCC_GPIOE_CLK_ENABLE();
	WM8978_GPIO_Handler.Mode = GPIO_MODE_AF_PP;
	WM8978_GPIO_Handler.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
	WM8978_GPIO_Handler.Pull = GPIO_PULLUP;
	WM8978_GPIO_Handler.Speed = GPIO_SPEED_HIGH;
	WM8978_GPIO_Handler.Alternate = GPIO_AF6_SAI1;
	HAL_GPIO_Init(GPIOE,&WM8978_GPIO_Handler);
	
	IIC_Init();//��ʼ��IIC�ӿ�
	res=WM8978_Write_Reg(0,0);	//��λWM8978
	if(res)return 1;			//����ָ��ʧ��,WM8978�쳣
	//����Ϊͨ������
	WM8978_Write_Reg(WM8978_REG_R1,0X1B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	WM8978_Write_Reg(WM8978_REG_R2,0X1B0);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	WM8978_Write_Reg(WM8978_REG_R3,0X6C);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
	WM8978_Write_Reg(WM8978_REG_R6,0);		//R6,MCLK���ⲿ�ṩ
	WM8978_Write_Reg(WM8978_REG_R43,1<<4);	//R43,INVROUT2����,��������
	WM8978_Write_Reg(WM8978_REG_R47,1<<8);	//R47����,PGABOOSTL,��ͨ��MIC���20������
	WM8978_Write_Reg(WM8978_REG_R48,1<<8);	//R48����,PGABOOSTR,��ͨ��MIC���20������
	WM8978_Write_Reg(WM8978_REG_R49,1<<1);	//R49,TSDEN,�������ȱ��� 
	WM8978_Write_Reg(WM8978_REG_R49,1<<2);	//R49,SPEAKER BOOST,1.5x 
	WM8978_Write_Reg(WM8978_REG_R10,1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
	WM8978_Write_Reg(WM8978_REG_R14,1<<3);	//R14,ADC 128x������
	return 0;
} 

//WM8978д�Ĵ���
//reg:�Ĵ�����ַ
//val:Ҫд��Ĵ�����ֵ 
//����ֵ:0,�ɹ�;
//    ����,�������
u8 WM8978_Write_Reg(u8 reg,u16 val)
{ 	
//	IIC_Start(); 
//	IIC_Send_Byte((WM8978_ADDR<<1)|0);//����������ַ+д����	 
//	if(IIC_Wait_Ack())return 1;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
//	
//  IIC_Send_Byte((reg<<1)|((val>>8)&0X01));//д�Ĵ�����ַ+���ݵ����λ
//	if(IIC_Wait_Ack())return 2;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
//	
//	IIC_Send_Byte(val&0XFF);	//��������
//	if(IIC_Wait_Ack())return 3;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
//	
//  IIC_Stop();
//	WM8978_REGVAL_TBL[reg]=val;	//����Ĵ���ֵ������
//	return 0;	

	static u8 pbuf[3] = {0};
	pbuf[0] = WM8978_ADDR_MSB_BIT(reg,val);//д�Ĵ�����ַ+���ݵ����λ
	pbuf[1] = val>>8;//val��8λ
	//pbuf[2] = val;
	u32 time_out = 0;
	
	while(HAL_I2C_Master_Transmit(&I2cHandle, (WM8978_ADDR<<1), pbuf, 2, 2000)!= HAL_OK)
	{
		if(++time_out > 2)
			break;		
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//ͨ�Ŵ�����
			
		}
	}
	return 0;
}  
//WM8978���Ĵ���
//���Ƕ�ȡ���ؼĴ���ֵ�������ڵĶ�Ӧֵ
//reg:�Ĵ�����ַ 
//����ֵ:�Ĵ���ֵ
u16 WM8978_Read_Reg(u8 reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 
//WM8978 DAC/ADC����
//adcen:adcʹ��(1)/�ر�(0)
//dacen:dacʹ��(1)/�ر�(0)
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen)
{
	u16 regval;
	regval=WM8978_Read_Reg(3);	//��ȡR3
	if(dacen){
		regval|=3<<0;		//R3���2��λ����Ϊ1,����DACR&DACL
	}else{
		regval&=~(3<<0);		//R3���2��λ����,�ر�DACR&DACL.
	}
	WM8978_Write_Reg(WM8978_REG_R3,regval);	//����R3
	regval=WM8978_Read_Reg(2);	//��ȡR2
	if(adcen){
		regval|=3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
	}else{
		regval&=~(3<<0);		//R2���2��λ����,�ر�ADCR&ADCL.
	}
	WM8978_Write_Reg(WM8978_REG_R2,regval);	//����R2	
}
//WM8978 ����ͨ������ 
//micen:MIC����(1)/�ر�(0)
//lineinen:Line In����(1)/�ر�(0)
//auxen:aux����(1)/�ر�(0) 
void WM8978_Input_Cfg(u8 micen,u8 lineinen,u8 auxen)
{
	u16 regval;  
	regval=WM8978_Read_Reg(2);	//��ȡR2
	if(micen)regval|=3<<2;		//����INPPGAENR,INPPGAENL(MIC��PGA�Ŵ�)
	else regval&=~(3<<2);		//�ر�INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(WM8978_REG_R2,regval);	//����R2 
	
	regval=WM8978_Read_Reg(44);	//��ȡR44
	if(micen)regval|=3<<4|3<<0;	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else regval&=~(3<<4|3<<0);	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(WM8978_REG_R44,regval);//����R44
	
	if(lineinen)WM8978_LINEIN_Gain(5);//LINE IN 0dB����
	else WM8978_LINEIN_Gain(0);	//�ر�LINE IN
	if(auxen)WM8978_AUX_Gain(7);//AUX 6dB����
	else WM8978_AUX_Gain(0);	//�ر�AUX����  
}
//WM8978 ������� 
//dacen:DAC���(����)����(1)/�ر�(0)
//bpsen:Bypass���(¼��,����MIC,LINE IN,AUX��)����(1)/�ر�(0) 
void WM8978_Output_Cfg(u8 dacen,u8 bpsen)
{
	u16 regval=0;
	if(dacen)regval|=1<<0;	//DAC���ʹ��
	if(bpsen)
	{
		regval|=1<<1;		//BYPASSʹ��
		regval|=5<<2;		//0dB����
	} 
	WM8978_Write_Reg(WM8978_REG_R50,regval);//R50����
	WM8978_Write_Reg(WM8978_REG_R51,regval);//R51���� 
}
//WM8978 MIC��������(������BOOST��20dB,MIC-->ADC���벿�ֵ�����)
//gain:0~63,��Ӧ-12dB~35.25dB,0.75dB/Step
void WM8978_MIC_Gain(u8 gain)
{
	gain&=0X3F;
	WM8978_Write_Reg(WM8978_REG_R45,gain);		//R45,��ͨ��PGA���� 
	WM8978_Write_Reg(WM8978_REG_R46,gain|1<<8);	//R46,��ͨ��PGA����
}
//WM8978 L2/R2(Ҳ����Line In)��������(L2/R2-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);	//��ȡR47
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_REG_R47,regval|gain<<4);//����R47
	regval=WM8978_Read_Reg(48);	//��ȡR48
	regval&=~(7<<4);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_REG_R48,regval|gain<<4);//����R48
} 
//WM8978 AUXR,AUXL(PWM��Ƶ����)��������(AUXR/L-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);	//��ȡR47
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_REG_R47,regval|gain<<0);//����R47
	regval=WM8978_Read_Reg(48);	//��ȡR48
	regval&=~(7<<0);			//���ԭ�������� 
 	WM8978_Write_Reg(WM8978_REG_R48,regval|gain<<0);//����R48
}  
//����I2S����ģʽ
//fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;
//len:0,16λ;1,20λ;2,24λ;3,32λ;  
void WM8978_I2S_Cfg(u8 fmt,u8 len)
{
	fmt&=0X03;
	len&=0X03;//�޶���Χ
	WM8978_Write_Reg(WM8978_REG_R4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����	
}	

//���ö���������������
//voll:����������(0~63)
//volr:����������(0~63)
void WM8978_HPvol_Set(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	if(voll==0)voll|=1<<6;//����Ϊ0ʱ,ֱ��mute
	if(volr==0)volr|=1<<6;//����Ϊ0ʱ,ֱ��mute 
	WM8978_Write_Reg(WM8978_REG_R52,voll);			//R52,������������������
	WM8978_Write_Reg(WM8978_REG_R53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}
//������������
//voll:����������(0~63) 
void WM8978_SPKvol_Set(u8 volx)
{ 
	volx&=0X3F;//�޶���Χ
	if(volx==0)volx|=1<<6;//����Ϊ0ʱ,ֱ��mute 
 	WM8978_Write_Reg(WM8978_REG_R54,volx);			//R54,������������������
	WM8978_Write_Reg(WM8978_REG_R55,volx|(1<<8));	//R55,������������������,ͬ������(SPKVU=1)	
}
//����3D������
//depth:0~15(3Dǿ��,0����,15��ǿ)
void WM8978_3D_Set(u8 depth)
{ 
	depth&=0XF;//�޶���Χ 
 	WM8978_Write_Reg(WM8978_REG_R41,depth);	//R41,3D�������� 	
}
//����EQ/3D���÷���
//dir:0,��ADC������
//    1,��DAC������(Ĭ��)
void WM8978_EQ_3D_Dir(u8 dir)
{
	u16 regval; 
	regval=WM8978_Read_Reg(0X12);
	if(dir)regval|=1<<8;
	else regval&=~(1<<8); 
 	WM8978_Write_Reg(WM8978_REG_R18,regval);//R18,EQ1�ĵ�9λ����EQ/3D����
}

//����EQ1
//cfreq:��ֹƵ��,0~3,�ֱ��Ӧ:80/105/135/175Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ1_Set(u8 cfreq,u8 gain)
{ 
	u16 regval;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain;
	regval=WM8978_Read_Reg(18);
	regval&=0X100;
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_REG_R18,regval);//R18,EQ1���� 	
}
//����EQ2
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:230/300/385/500Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ2_Set(u8 cfreq,u8 gain)
{ 
	u16 regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_REG_R19,regval);//R19,EQ2���� 	
}
//����EQ3
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:650/850/1100/1400Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ3_Set(u8 cfreq,u8 gain)
{ 
	u16 regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_REG_R20,regval);//R20,EQ3���� 	
}
//����EQ4
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:1800/2400/3200/4100Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ4_Set(u8 cfreq,u8 gain)
{ 
	u16 regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_REG_R21,regval);//R21,EQ4���� 	
}
//����EQ5
//cfreq:����Ƶ��,0~3,�ֱ��Ӧ:5300/6900/9000/11700Hz
//gain:����,0~24,��Ӧ-12~+12dB
void WM8978_EQ5_Set(u8 cfreq,u8 gain)
{ 
	u16 regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	WM8978_Write_Reg(WM8978_REG_R22,regval);//R22,EQ5���� 	
}












