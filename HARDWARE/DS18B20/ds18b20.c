#include "ds18b20.h"
#include "delay.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////
  

//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
	
  DS18B20_DQ_OUT_CLEAR; //拉低DQ
  delay_us(750);    //拉低750us
	
  DS18B20_DQ_OUT_SET; //DQ=1 
	delay_us(15);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
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
//从DS18B20读取一个位
//返回值：1/0
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
//从DS18B20读取一个字节
//返回值：读到的数据
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
//写一个字节到DS18B20
//dat：要写入的字节
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
//开始温度转换
void DS18B20_Start(void) 
{   						               
	DS18B20_Rst();	   
	DS18B20_Check();	 
	DS18B20_Write_Byte(SKIP_ROM);// skip rom
	DS18B20_Write_Byte(CONVERT);// convert
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	DS18B20_GPIO_CLK_ENABLE();			//开启GPIOB时钟

	GPIO_Initure.Pin=DS18B20_GPIO_DATA_CMD_PIN;           //PB12
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(DS18B20_GPIO_SDA_GPIO_PORT,&GPIO_Initure);     //初始化

	DS18B20_Rst();
	return DS18B20_Check();
}  
/*******************************************************************************
* Function Name : DS18B20_TEMP_TRANS
* Description   : 对ds18b20采集到的数据进行转换，得到最终温度值
* Input         : TMP_MSB：采集的数据高位，
									TMP_LSB：采集的数据低位，
* Return        : 计算后的温度值（包含正负）
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
		tmp_freezel_symbol=0;		//温度为负  
	}else{
		tmp_freezel_symbol=1;	  //温度为正	
	}
	
	AfterCaltem=TMP_MSB; 		//获得高八位
	AfterCaltem<<=8;    
	AfterCaltem+=TMP_LSB;		//获得底八位
	AfterCaltem=(double)AfterCaltem * DS18B20_TRANS_FACTOR;	//转换  
	return (tmp_freezel_symbol?AfterCaltem:(-AfterCaltem));
}
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
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
















