#include "ctiic.h"
#include "delay.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//���ݴ�����-IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/28
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//��
////////////////////////////////////////////////////////////////////////////////// 	

//����I2C�ٶȵ���ʱ
void CT_Delay(void)
{
	delay_us(2);
} 
//���ݴ���оƬIIC�ӿڳ�ʼ��
void CT_IIC_Init(void)
{					     
    GPIO_InitTypeDef GPIO_Initure;
    CT_IIC_GPIO_SCL_CLK_ENABLE();			//����GPIOHʱ��
		CT_IIC_GPIO_SDA_CLK_ENABLE();			//����GPIOIʱ��
    
    GPIO_Initure.Pin=CT_IIC_GPIO_SCL_PIN;            //PH6
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(CT_IIC_GPIO_SCL_PORT,&GPIO_Initure);     //��ʼ��
	
    GPIO_Initure.Pin=CT_IIC_GPIO_SDA_PIN;            //PI3
    HAL_GPIO_Init(CT_IIC_GPIO_SDA_PORT,&GPIO_Initure);     //��ʼ��
    CT_IIC_SDA_SET;	  	  
		CT_IIC_SCL_SET;
}
//����IIC��ʼ�ź�
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda�����
	CT_IIC_SDA_SET;	  	  
	CT_IIC_SCL_SET;
	delay_us(30);
 	CT_IIC_SDA_CLEAR;//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL_CLEAR;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda�����
	CT_IIC_SCL_CLEAR; 
	CT_IIC_SDA_CLEAR;//STOP:when CLK is high DATA change form low to high
	delay_us(30);
	CT_IIC_SCL_SET; 
	CT_Delay(); 
	CT_IIC_SDA_SET;//����I2C���߽����ź�  
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	CT_SDA_IN();      //SDA����Ϊ����  
	CT_IIC_SDA_SET;	   
	CT_IIC_SCL_SET;
	CT_Delay();
	while(CT_READ_SDA_STA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL_CLEAR;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void CT_IIC_Ack(void)
{
	CT_IIC_SCL_CLEAR;
	CT_SDA_OUT(); 
	CT_IIC_SDA_CLEAR;
	CT_Delay();
	CT_IIC_SCL_SET;
	CT_Delay();
	CT_IIC_SCL_CLEAR;
}
//������ACKӦ��		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL_CLEAR;
	CT_SDA_OUT(); 
	CT_IIC_SDA_SET;
	CT_Delay();
	CT_IIC_SCL_SET;
	CT_Delay();
	CT_IIC_SCL_CLEAR;
}					 				 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CT_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	CT_SDA_OUT(); 	    
  CT_IIC_SCL_CLEAR;//����ʱ�ӿ�ʼ���ݴ���
	CT_Delay();
	for(t=0;t<8;t++)
    {              
        CT_IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	
        CT_Delay();        
		CT_IIC_SCL_SET; 
		CT_Delay();
		CT_IIC_SCL_CLEAR;	
		CT_Delay();
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ����
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL_CLEAR; 	    	   
		CT_Delay();
		CT_IIC_SCL_SET;	 
		receive<<=1;
		if(CT_READ_SDA_STA)receive++;   
        CT_Delay(); 
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}




























