#include "mpu9250.h"
#include "myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//MPU9250��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//��ʼ��MPU9250
//����ֵ:0,�ɹ�
//    ����,�������
u8 MPU9250_Init(void)
{
//    u8 res=0;
//    IIC_Init();     //��ʼ��IIC����
//    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//��λMPU9250
//    delay_ms(100);  //��ʱ100ms
//    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//����MPU9250
//    MPU_Set_Gyro_Fsr(3);					        	//�����Ǵ�����,��2000dps
//	  MPU_Set_Accel_Fsr(0);					       	 	//���ٶȴ�����,��2g
//    MPU_Set_Rate(50);						       	 	//���ò�����50Hz
//    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //�ر������ж�
//	  MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C��ģʽ�ر�
//	  MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
//	  MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
//    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
//    if(res==MPU6500_ID) //����ID��ȷ
//    {
//        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
//        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
//		    MPU_Set_Rate(50);						       	//���ò�����Ϊ50Hz   
//    }else return 1;

//    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//��ȡAK8963 ID   
//    if(res==AK8963_ID)
//    {
//        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//����AK8963Ϊ���β���ģʽ
//    }else return 1;

//    return 0;
    u8 res=0;
    IIC_Init();     //��ʼ��IIC����
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80);//��λMPU9250
    delay_ms(100);  //��ʱ100ms
    MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00);//����MPU9250
    MPU_Set_Gyro_Fsr(3);					        	//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					       	 	//���ٶȴ�����,��2g
    MPU_Set_Rate(50);						       	 	//���ò�����50Hz
    MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00);   //�ر������ж�
	MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00);//I2C��ģʽ�ر�
	MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82);//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
    if(res==MPU6500_ID) //����ID��ȷ
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
		MPU_Set_Rate(50);						       	//���ò�����Ϊ50Hz   
    }else return 1;
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//��ȡAK8963 ID   
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL2,0X01);		//��λAK8963
		delay_ms(50);
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//����AK8963Ϊ���β���
    }else return 1;

    return 0;
}

//����MPU9250�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}
//����MPU9250���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

//����MPU9250�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}

//����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
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
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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

//�õ�������ֵ(ԭʼֵ)
//mx,my,mz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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
    MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
    return res;;
}

extern I2C_HandleTypeDef I2cHandle;
//IIC����д
//addr:������ַ 
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 pbuf[20] = {0};
	u16 i;
	pbuf[0] = reg;
	u32 time_out;
	
    for(i=0;i<len;i++)
    {
        pbuf[i+1] =buf[i];  //��������
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
		{//ͨ�Ŵ�����
			
		}
	}
	return 0;
} 

//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
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
		{//ͨ�Ŵ�����
			
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
		{//ͨ�Ŵ�����
			
		}   
	}
	return 0;	       
}

//IICдһ���ֽ� 
//devaddr:����IIC��ַ
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
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
		{//ͨ�Ŵ�����
			
		}
	}
	return 0;
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
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
		{//ͨ�Ŵ�����
			
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
		{//ͨ�Ŵ�����
			
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

//�������ݸ�����������λ�����(V2.6�汾)
//fun:������. 0X01~0X1C
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽ����� 
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0XAA;	//֡ͷ
	send_buf[1]=0XAA;	//֡ͷ
	send_buf[2]=fun;	//������
	send_buf[3]=len;	//���ݳ���
	for(i=0;i<len;i++)send_buf[4+i]=data[i];			//��������
	for(i=0;i<len+4;i++)send_buf[len+4]+=send_buf[i];	//����У���	
	for(i=0;i<len+5;i++)MyPrintf("%c",send_buf[i]);	//�������ݵ�����1 
}
//���ͼ��ٶȴ���������+����������(������֡)
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ 
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
	tbuf[12]=0;//��Ϊ����MPL��,�޷�ֱ�Ӷ�ȡ����������,��������ֱ�����ε�.��0���.
	tbuf[13]=0;
	tbuf[14]=0;
	tbuf[15]=0;
	tbuf[16]=0;
	tbuf[17]=0;
	usart1_niming_report(0X02,tbuf,18);//������֡,0X02
}	
//ͨ������1�ϱ���������̬���ݸ�����(״̬֡)
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
//csb:�������߶�,��λ:cm
//prs:��ѹ�Ƹ߶�,��λ:mm
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
	usart1_niming_report(0X01,tbuf,12);//״̬֡,0X01
}  

void MPU_Test(void)
{
	u8 t=0,report=0;	            //Ĭ�ϲ��ϱ�0��1�ϱ�

	float pitch,roll,yaw; 	        //ŷ����
	short aacx,aacy,aacz;	        //���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;        //������ԭʼ���� 
	short temp;		                //�¶�
    
	LCD_Clear(BLACK);	//����
    POINT_COLOR=BLUE;     //��������Ϊ��ɫ 
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
//		LED0=!LED0;//DS0��˸ 
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
            temp=MPU_Get_Temperature();	//�õ��¶�ֵ
		    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		    MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
  			if(report)mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//���ͼ��ٶ�+������ԭʼ����
			if(report)usart1_report_imu((int)(roll*100),(int)(pitch*100),(int)(yaw*100),0,0);
			if((t%10)==0)
		    { 
				if(temp<0)
				{
					LCD_ShowChar(30+48,16+16*1,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,16+16*1,' ',16,0);		//ȥ������ 
				
				LCD_ShowNum(30+48+8,16+16*1,temp/100,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,16+16*1,temp%10,1,16);		//��ʾС������ 
				temp=pitch*10;
				if(temp<0)
			    {
					LCD_ShowChar(30+48,16+16*2,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,16+16*2,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,16+16*2,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,16+16*2,temp%10,1,16);		//��ʾС������ 
				temp=roll*10;
				if(temp<0)
				{
					LCD_ShowChar(30+48,16+16*3,'-',16,0);		//��ʾ����
					temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,16+16*3,' ',16,0);		//ȥ������ 
				   
				LCD_ShowNum(30+48+8,16+16*3,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,16+16*3,temp%10,1,16);		//��ʾС������ 
				temp=yaw*10;
				if(temp<0)
				{
				   	LCD_ShowChar(30+48,16+16*4,'-',16,0);		//��ʾ����
				   	temp=-temp;		//תΪ����
				}else LCD_ShowChar(30+48,16+16*4,' ',16,0);		//ȥ������ 
				LCD_ShowNum(30+48+8,16+16*4,temp/10,3,16);		//��ʾ��������	    
				LCD_ShowNum(30+48+40,16+16*4,temp%10,1,16);		//��ʾС������  
				t=0;
//				LED0=!LED0;//DS0��˸ 
		 	}
		 }
        t++;
		delay_ms(10);
	 } 	    
}
#endif
