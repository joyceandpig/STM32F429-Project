#include "ds18b20.h"
#include "delay.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////
  

//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
	
  DS18B20_DQ_OUT_CLEAR; //����DQ
  delay_us(750);    //����750us
	
  DS18B20_DQ_OUT_SET; //DQ=1 
	delay_us(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET pin INPUT	 
  while (DS18B20_DQ_IN_STA&&retry<200){
		retry++;
		delay_us(1);
	};	 
	if(retry>=200){
		return 1;
	}else{
		retry=0;
	}
  while (!DS18B20_DQ_IN_STA&&retry<240){
		retry++;
		delay_us(1);
	};
	if(retry>=240){
		return 1;	  
	}		
	return 0;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			  
{
	DS18B20_IO_OUT();//SET  OUTPUT
  DS18B20_DQ_OUT_CLEAR; 
	delay_us(2);
	
  DS18B20_DQ_OUT_SET; 
	DS18B20_IO_IN();//SET  INPUT
	delay_us(12);
	
	delay_us(50);           
	return DS18B20_DQ_IN_STA?1:0;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    
{        
  u8 i,j,dat;
  dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit();
		dat=(j<<7)|(dat>>1);
  }						    
  return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
 {             
  u8 j;
  u8 testb;
	 
	DS18B20_IO_OUT();//SET OUTPUT;
  for (j=1;j<=8;j++) 
	{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) {
			DS18B20_DQ_OUT_CLEAR;// Write 1
			delay_us(2);                            
			DS18B20_DQ_OUT_SET;
			delay_us(60);             
		}else {
			DS18B20_DQ_OUT_CLEAR;// Write 0
			delay_us(60);             
			DS18B20_DQ_OUT_SET;
			delay_us(2);                          
		}
  }
}
//��ʼ�¶�ת��
void DS18B20_Start(void) 
{   						               
	DS18B20_Rst();	   
	DS18B20_Check();	 
	DS18B20_Write_Byte(SKIP_ROM);// skip rom
	DS18B20_Write_Byte(CONVERT);// convert
} 
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	DS18B20_GPIO_CLK_ENABLE();			//����GPIOBʱ��

	GPIO_Initure.Pin=DS18B20_GPIO_DATA_CMD_PIN;           //PB12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	HAL_GPIO_Init(DS18B20_GPIO_SDA_GPIO_PORT,&GPIO_Initure);     //��ʼ��

	DS18B20_Rst();
	return DS18B20_Check();
}  
/*******************************************************************************
* Function Name : DS18B20_TEMP_TRANS
* Description   : ��ds18b20�ɼ��������ݽ���ת�����õ������¶�ֵ
* Input         : TMP_MSB���ɼ������ݸ�λ��
									TMP_LSB���ɼ������ݵ�λ��
* Return        : �������¶�ֵ������������
* Author        : zhangchaosong
* Modify Time   : 2016-09-05
*******************************************************************************/ 
static s8 DS18B20_TEMP_TRANS(u8 TMP_MSB, u8 TMP_LSB)
{
	short AfterCaltem;
	u8 tmp_freezel_symbol;
	
	if(TMP_MSB>7){
		TMP_MSB=~TMP_MSB;
		TMP_LSB=~TMP_LSB; 
		tmp_freezel_symbol=0;		//�¶�Ϊ��  
	}else{
		tmp_freezel_symbol=1;	  //�¶�Ϊ��	
	}
	
	AfterCaltem=TMP_MSB; 		//��ø߰�λ
	AfterCaltem<<=8;    
	AfterCaltem+=TMP_LSB;		//��õװ�λ
	AfterCaltem=(double)AfterCaltem * DS18B20_TRANS_FACTOR;	//ת��  
	return (tmp_freezel_symbol?AfterCaltem:(-AfterCaltem));
}
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
	u8 TL,TH;
	DS18B20_Start();// ds1820 start convert
	DS18B20_Rst();
	DS18B20_Check();	 
	DS18B20_Write_Byte(SKIP_ROM);// skip rom
	DS18B20_Write_Byte(READ_SCRATCHPAD);// convert	    
	TL=DS18B20_Read_Byte(); // LSB   
	TH=DS18B20_Read_Byte(); // MSB   
	return DS18B20_TEMP_TRANS(TL,TH); 
}
















