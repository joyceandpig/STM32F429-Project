#include "24cxx.h"
#include "delay.h"
	
extern I2C_HandleTypeDef I2cHandle;
//#define I2C_ADDRESS        0xA0
//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();//IIC初始化
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{	
	u8 Res;
	u8 data[10];
	if(EE_TYPE>AT24C16)
	{
		data[0] = ReadAddr>>8;//高地址
		data[1] = ReadAddr&0x00ff;//低地址
		
		while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 2, 10000)!= HAL_OK)
		{
			/* Error_Handler() function is called when Timeout error occurs.
			   When Acknowledge failure occurs (Slave don't acknowledge it's address)
			   Master restarts communication */
			if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
			{//通信错误处理
				
			}
		}	
	}
	else
	{
		data[0] = ReadAddr&0x00ff;//地址
	  while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 1, 10000)!= HAL_OK)
	  {
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}
	  }			
	}	
	
	while(HAL_I2C_Master_Receive(&I2cHandle, I2C_ADDRESS, &Res, 1, 10000) != HAL_OK)
	{
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}   
	} 	
	return Res;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{	
	u8 data[10];
	if(EE_TYPE>AT24C16)
	{
		data[0] = WriteAddr>>8;//高地址
		data[1] = WriteAddr&0x00ff;//低地址
		data[2] = DataToWrite;//数据
		
		while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 3, 10000)!= HAL_OK)
		{
			/* Error_Handler() function is called when Timeout error occurs.
			   When Acknowledge failure occurs (Slave don't acknowledge it's address)
			   Master restarts communication */
			if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
			{//通信错误处理
				
			}
		}	
	}
	else
	{
		data[0] = WriteAddr&0x00ff;//地址
		data[1] = DataToWrite;//数据
	  while(HAL_I2C_Master_Transmit(&I2cHandle, I2C_ADDRESS, data, 2, 10000)!= HAL_OK)
	  {
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理

		}
	  }			
	}
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
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
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
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
{//写测试
	u16 trytime = 0;
	while(AT24CXX_Check())//检测不到24c02
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
		{//读写数据一致
			u_printf(INFO,"AT24CXX Write/Read is OK!");
		}
		else
			u_printf(INFO,"AT24CXX Write/Read is Error!");
	}		
	
}
#endif
