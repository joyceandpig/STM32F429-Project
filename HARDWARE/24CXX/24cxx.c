#include "24cxx.h"
#include "delay.h"
	
extern I2C_HandleTypeDef I2cHandle;
//#define I2C_ADDRESS        0xA0
//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();//IIC��ʼ��
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{	
	u8 Res;
	u8 data[10];
	if(EE_TYPE>AT24C16)
	{
		data[0] = ReadAddr>>8;//�ߵ�ַ
		data[1] = ReadAddr&0x00ff;//�͵�ַ
		
		while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 2, 10000)!= HAL_OK)
		{
			/* Error_Handler() function is called when Timeout error occurs.
			   When Acknowledge failure occurs (Slave don't acknowledge it's address)
			   Master restarts communication */
			if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
			{//ͨ�Ŵ�����
				
			}
		}	
	}
	else
	{
		data[0] = ReadAddr&0x00ff;//��ַ
	  while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 1, 10000)!= HAL_OK)
	  {
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//ͨ�Ŵ�����
			
		}
	  }			
	}	
	
	while(HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS, &Res, 1, 10000) != HAL_OK)
	{
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//ͨ�Ŵ�����
			
		}   
	} 	
	return Res;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{	
	u8 data[10];
	if(EE_TYPE>AT24C16)
	{
		data[0] = WriteAddr>>8;//�ߵ�ַ
		data[1] = WriteAddr&0x00ff;//�͵�ַ
		data[2] = DataToWrite;//����
		
		while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 3, 10000)!= HAL_OK)
		{
			/* Error_Handler() function is called when Timeout error occurs.
			   When Acknowledge failure occurs (Slave don't acknowledge it's address)
			   Master restarts communication */
			if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
			{//ͨ�Ŵ�����
				
			}
		}	
	}
	else
	{
		data[0] = WriteAddr&0x00ff;//��ַ
		data[1] = DataToWrite;//����
	  while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 2, 10000)!= HAL_OK)
	  {
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//ͨ�Ŵ�����

		}
	  }			
	}
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

#ifdef	DEBUG
#include "debug.h"
#include "delay.h"
#include "user_config.h"

void AT24CXX_Test(u8 n)
{//д����
	u16 trytime = 0;
	while(AT24CXX_Check())//��ⲻ��24c02
	{
		trytime ++;
		u_printf(INFO,"24C02 Check Failed!ReTry %d",trytime);
		Sleep(500);
		if(trytime == 10)
		{
			u_printf(WARN,"Exit AT24CXX_Test!");
			break;
		}
	}
	if(trytime <= 10)
	{
		u8 i;
		u8 TxBuf[256] = {0} ;
		u8 RxBuf[256] = {0};
		for(i = 0;i<255 ;i++)
		{
			TxBuf[i] = n+0x30;		
		}
		u_printf(INFO,"24C02 Ready!");
		u_printf(INFO,"24C02 Try Write ...!");
		TxBuf[0] = n+0x30;
		AT24CXX_Write(0,TxBuf,255);
		u_printf(INFO,"AT24CXX_Write:%s",TxBuf);
		AT24CXX_Read(0,RxBuf,255);
		u_printf(INFO,"AT24CXX_Read:%s",RxBuf);
		if(strcmp((const char *)TxBuf,(const char *)RxBuf) == 0)
		{//��д����һ��
			u_printf(INFO,"AT24CXX Write/Read is OK!");
		}
		else
			u_printf(INFO,"AT24CXX Write/Read is Error!");
	}		
	
}
#endif
