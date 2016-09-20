#include "mpu9250.h"
#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F746开发板
//MPU9250驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//初始化MPU9250
//返回值:0,成功
//    其他,错误代码
u8 MPU9250_Init(void)
{
//    u8 res=0;
//    IIC_Init();     //初始化IIC总线
//    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU9250
//    delay_ms(100);  //延时100ms
//    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU9250
//    MPU_Set_Gyro_Fsr(3);					        	//陀螺仪传感器,±2000dps
//	  MPU_Set_Accel_Fsr(0);					       	 	//加速度传感器,±2g
//    MPU_Set_Rate(50);						       	 	//设置采样率50Hz
//    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //关闭所有中断
//	  MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
//	  MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//关闭FIFO
//	  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
//    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //读取MPU6500的ID
//    if(res==MPU6500_ID) //器件ID正确
//    {
//        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
//        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
//		    MPU_Set_Rate(50);						       	//设置采样率为50Hz   
//    }else return 1;

//    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//读取AK8963 ID   
//    if(res==AK8963_ID)
//    {
//        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//设置AK8963为单次测量模式
//    }else return 1;

//    return 0;
    u8 res=0;
    IIC_Init();     //初始化IIC总线
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//复位MPU9250
    delay_ms(100);  //延时100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//唤醒MPU9250
    MPU_Set_Gyro_Fsr(3);					        	//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					       	 	//加速度传感器,±2g
    MPU_Set_Rate(50);						       	 	//设置采样率50Hz
    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //关闭所有中断
	MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C主模式关闭
	MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //读取MPU6500的ID
    if(res==MPU6500_ID) //器件ID正确
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						       	//设置采样率为50Hz   
    }else return 1;
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//读取AK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL2,0X01);		//复位AK8963
		delay_ms(50);
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//设置AK8963为单次测量
    }else return 1;

    return 0;
}

//设置MPU9250陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}
//设置MPU9250加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

//设置MPU9250的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器  
}

//设置MPU9250的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=21+((double)raw)/333.87;  
    return temp*100;;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res; 
	res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{
		*mx=((u16)buf[1]<<8)|buf[0];  
		*my=((u16)buf[3]<<8)|buf[2];  
		*mz=((u16)buf[5]<<8)|buf[4];
	} 	
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
    return res;;
}

extern I2C_HandleTypeDef I2cHandle;
//IIC连续写
//addr:器件地址 
//reg:寄存器地址
//len:写入长度
//buf:数据区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 pbuf[20] = {0};
	u16 i;
	pbuf[0] = reg;
	u32 time_out;
	
    for(i=0;i<len;i++)
    {
        pbuf[i+1] =buf[i];  //发送数据
    }	
	time_out = 0;
	while(HAL_I2C_Master_Transmit(&I2cHandle, (addr<<1), pbuf, len+1, 20000)!= HAL_OK)
	{
		if(++time_out > 2)
			break;			
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}
	}
	return 0;
} 

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
	u8 pbuf[3] = {0};
	pbuf[0] = reg;
	u32 time_out;
	
	time_out = 0;	
	while(HAL_I2C_Master_Transmit(&I2cHandle, (addr<<1), pbuf, 1, 20000)!= HAL_OK)
	{
		if(++time_out > 2)
			break;		
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}
	}
	
	time_out = 0;	
	while(HAL_I2C_Master_Receive(&I2cHandle, (addr<<1), buf, len, 20000) != HAL_OK)
	{
		if(++time_out > 2)
			break;		
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}   
	}
	return 0;	       
}

//IIC写一个字节 
//devaddr:器件IIC地址
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
	u8 pbuf[3] = {0};
	pbuf[0] = reg;
	pbuf[1] = data;
	u32 time_out;
	
	time_out = 0;
	while(HAL_I2C_Master_Transmit(&I2cHandle, (addr<<1), pbuf, 2, 20000)!= HAL_OK)
	{
		if(++time_out > 2)
			break;		
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}
	}
	return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
	u8 Res;
	u8 pbuf[3] = {0};
	pbuf[0] = reg;
	u32 time_out;
	
	time_out = 0;
	while(HAL_I2C_Master_Transmit(&I2cHandle, (addr<<1), pbuf, 1, 20000)!= HAL_OK)
	{		
		if(++time_out > 2)
			break;
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}
	}
	
	time_out = 0;
	while(HAL_I2C_Master_Receive(&I2cHandle, (addr<<1), &Res, 1, 20000) != HAL_OK)
	{
		if(++time_out > 2)
			break;
		/* Error_Handler() function is called when Timeout error occurs.
		   When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */
		if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
		{//通信错误处理
			
		}   
	}
	return Res;	
}

#if 1
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

#include "lcd.h"
#include "sdram.h"
#include "mpu9250.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#include "debug.h"

//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0X01~0X1C
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0XAA;	//帧头
	send_buf[1]=0XAA;	//帧头
	send_buf[2]=fun;	//功能字
	send_buf[3]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[4+i]=data[i];			//复制数据
	for(i=0;i<len+4;i++)send_buf[len+4]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+5;i++)MyPrintf("%c",send_buf[i]);	//发送数据到串口1 
}
//发送加速度传感器数据+陀螺仪数据(传感器帧)
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值 
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[18]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	tbuf[12]=0;//因为开启MPL后,无法直接读取磁力计数据,所以这里直接屏蔽掉.用0替代.
	tbuf[13]=0;
	tbuf[14]=0;
	tbuf[15]=0;
	tbuf[16]=0;
	tbuf[17]=0;
	usart1_niming_report(0X02,tbuf,18);//传感器帧,0X02
}	
//通过串口1上报结算后的姿态数据给电脑(状态帧)
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
//csb:超声波高度,单位:cm
//prs:气压计高度,单位:mm
void usart1_report_imu(short roll,short pitch,short yaw,short csb,int prs)
{
	u8 tbuf[12];   	
	tbuf[0]=(roll>>8)&0XFF;
	tbuf[1]=roll&0XFF;
	tbuf[2]=(pitch>>8)&0XFF;
	tbuf[3]=pitch&0XFF;
	tbuf[4]=(yaw>>8)&0XFF;
	tbuf[5]=yaw&0XFF;
	tbuf[6]=(csb>>8)&0XFF;
	tbuf[7]=csb&0XFF;
	tbuf[8]=(prs>>24)&0XFF;
	tbuf[9]=(prs>>16)&0XFF;
	tbuf[10]=(prs>>8)&0XFF;
	tbuf[11]=prs&0XFF;
	usart1_niming_report(0X01,tbuf,12);//状态帧,0X01
}  

void MPU_Test(void)
{
	u8 t=0,report=0;	            //默认不上报0，1上报

	float pitch,roll,yaw; 	        //欧拉角
	short aacx,aacy,aacz;	        //加速度传感器原始数据
	short gyrox,gyroy,gyroz;        //陀螺仪原始数据 
	short temp;		                //温度
    
	LCD_Clear(BLACK);	//清屏
    POINT_COLOR=BLUE;     //设置字体为蓝色 
	BACK_COLOR = BLACK;		
	LCD_DrawRectangle(0,0,lcddev.width-1,102);	
   	LCD_ShowString(30,16+16*1,200,16,16," Temp:   0.0C");	
   	LCD_ShowString(30,16+16*2,200,16,16,"Pitch:   0.0C");	
   	LCD_ShowString(30,16+16*3,200,16,16," Roll:   0.0C");	 
   	LCD_ShowString(30,16+16*4,200,16,16," Yaw :   0.0C");	
	
	POINT_COLOR=RED;		
    LCD_ShowString(170,0,200,16,16,"MPU9250 Test!! ");
	while(mpu_dmp_init())         
	{   
		LCD_ShowString(170,0,200,16,16,"MPU9250 error! ");
 		delay_ms(200);
//		LED0=!LED0;//DS0闪烁 
	}
	LCD_ShowString(170,0,200,16,16,"F429 Develop Kit.");

    while(1)
    {					
//        key=KEY_Scan(0);
//	    if(key==KEY0_PRES)
//		{
//			report=!report;
//			if(report)LCD_ShowString(30,170,200,16,16,"UPLOAD ON ");
//			else LCD_ShowString(30,170,200,16,16,"UPLOAD OFF");
//		}
		POINT_COLOR=BLUE;
        if(mpu_mpl_get_data(&pitch,&roll,&yaw)==0)
        {
            temp=MPU_Get_Temperature();	//得到温度值
		    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
  			if(report)mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//发送加速度+陀螺仪原始数据
			if(report)usart1_report_imu((int)(roll*100),(int)(pitch*100),(int)(yaw*100),0,0);
			if((t%10)==0)
		    { 
				if(temp<0)
				{
					LCD_ShowChar(30+48,16+16*1,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,16+16*1,' ',16,0);		//去掉负号 
				
				LCD_ShowNum(30+48+8,16+16*1,temp/100,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,16+16*1,temp%10,1,16);		//显示小数部分 
				temp=pitch*10;
				if(temp<0)
			    {
					LCD_ShowChar(30+48,16+16*2,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,16+16*2,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,16+16*2,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,16+16*2,temp%10,1,16);		//显示小数部分 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,16+16*3,'-',16,0);		//显示负号
					temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,16+16*3,' ',16,0);		//去掉负号 
				   
				LCD_ShowNum(30+48+8,16+16*3,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,16+16*3,temp%10,1,16);		//显示小数部分 
				temp=yaw*10;
				if(temp<0)
				{
				   	LCD_ShowChar(30+48,16+16*4,'-',16,0);		//显示负号
				   	temp=-temp;		//转为正数
				}else LCD_ShowChar(30+48,16+16*4,' ',16,0);		//去掉负号 
				LCD_ShowNum(30+48+8,16+16*4,temp/10,3,16);		//显示整数部分	    
				LCD_ShowNum(30+48+40,16+16*4,temp%10,1,16);		//显示小数部分  
				t=0;
//				LED0=!LED0;//DS0闪烁 
		 	}
		 }
        t++;
		delay_ms(10);
	 } 	    
}
#endif
