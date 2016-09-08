#include "tpad.h"
#include "delay.h"		    
#include "usart.h"
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//TPAD驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved		
//********************************************************************************
//修改说明
//V1.1  20160218
//1,去掉TPAD_GATE_VAL的定义,改为判断tpad_default_val的4/3为门限
//2,修改TPAD_Get_MaxVal函数的实现方式,提高抗干扰性
////////////////////////////////////////////////////////////////////////////////// 	





#define TPAD_ARR_MAX_VAL  0X4000		//最大的ARR值(TIM2是32位定时器)	  
vu16 tpad_default_val=0;				//空载的时候(没有手按下),计数器需要的时间
//初始化触摸按键
//获得空载的时候触摸按键的取值.
//psc:分频系数,越小,灵敏度越高.
//返回值:0,初始化成功;1,初始化失败
u8 TPAD_Init(u8 psc)
{
	u16 buf[10];
	u16 temp;
	u8 j,i;
	TIM2_CH1_Cap_Init(TPAD_ARR_MAX_VAL,psc-1);//设置分频系数
	for(i=0;i<10;i++)//连续读取10次
	{				 
		buf[i]=TPAD_Get_Val();
		delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//排序
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//取中间的8个数据进行平均
	tpad_default_val=temp/6;
	printf("tpad_default_val:%d\r\n",tpad_default_val);	
	if(tpad_default_val>TPAD_ARR_MAX_VAL/2)return 1;//初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常!
	return 0;		     	    					   
}
//复位一次
//释放电容电量，并清除定时器的计数值
void TPAD_Reset(void)
{	
//	GPIO_Set(GPIOA,PIN5,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//PA5推挽输出
//	PAout(5)=0; 	//输出0,放电
		GPIO_InitTypeDef TPAD_GPIO_Handler;
	  TPAD_RST_GPIO_PORT_CLK_ENABLE();           //开启GPIOA时钟
		HAL_GPIO_DeInit(TPAD_RST_GPIO_PORT,TPAD_RST_GPIO_PIN);
    TPAD_GPIO_Handler.Pin=TPAD_RST_GPIO_PIN; //p5
    TPAD_GPIO_Handler.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    TPAD_GPIO_Handler.Pull=GPIO_PULLDOWN;          //下拉
    TPAD_GPIO_Handler.Speed=GPIO_SPEED_HIGH;     //高速 100m
    HAL_GPIO_Init(TPAD_RST_GPIO_PORT,&TPAD_GPIO_Handler);
	
		TPAD_RST_SET();
	
	delay_ms(5);
	TIM2->SR=0;   	//清除标记
	TIM2->CNT=0;	//归零     
//	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//PA5,复用功能,不带上下拉 
	    
	 TPAD_GPIO_Handler.Mode=GPIO_MODE_AF_PP;  //复用功能
   TPAD_GPIO_Handler.Pull=GPIO_NOPULL;          //不带上下拉 
	 HAL_GPIO_Init(TPAD_RST_GPIO_PORT,&TPAD_GPIO_Handler);
}
//得到定时器捕获值
//如果超时,则直接返回定时器的计数值.
//返回值：捕获值/计数值（超时的情况下返回）
u16 TPAD_Get_Val(void)
{				   
	TPAD_Reset();
	while((TIM2->SR&0X02)==0)//等待捕获上升沿
	{
		if(TIM2->CNT>TPAD_ARR_MAX_VAL-500)return TIM2->CNT;//超时了,直接返回CNT的值
	};	
	return TIM2->CCR1;	  
} 	  
//读取n次,取最大值
//n：连续获取的次数
//返回值：n次读数里面读到的最大读数值
u16 TPAD_Get_MaxVal(u8 n)
{ 
	u16 temp=0; 
	u16 res=0; 
	u8 lcntnum=n*2/3;//至少2/3*n的有效个触摸,才算有效
	u8 okcnt=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//得到一次值
		if(temp>(tpad_default_val*5/4))okcnt++;//至少大于默认值的5/4才算有效
		if(temp>res)res=temp;
	}
	if(okcnt>=lcntnum)return res;//至少2/3的概率,要大于默认值的5/4才算有效
	else return 0;
}  
//扫描触摸按键
//mode:0,不支持连续触发(按下一次必须松开才能按下一次);1,支持连续触发(可以一直按下)
//返回值:0,没有按下;1,有按下;										  
u8 TPAD_Scan(u8 mode)
{
	static u8 keyen=0;	//0,可以开始检测;>0,还不能开始检测	 
	u8 res=0;
	u8 sample=3;	//默认采样次数为3次	 
	u16 rval;
	if(mode)
	{
		sample=6;	//支持连按的时候，设置采样次数为6次
		keyen=0;	//支持连按	  
	}
	rval=TPAD_Get_MaxVal(sample); 
	if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val))//大于tpad_default_val+(1/3)*大于tpad_default_val,且小于10倍tpad_default_val,则有效
	{							 
		if(keyen==0)res=1;	//keyen==0,有效 
		printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//至少要再过3次之后才能按键有效   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 
//定时器2通道2输入捕获配置	 
//arr：自动重装值
//psc：时钟预分频数
void TIM2_CH1_Cap_Init(u32 arr,u16 psc)
{
//	RCC->APB1ENR|=1<<0;		//TIM2时钟使能    
//	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
//	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);//PA5,复用功能,不带上下拉
//	GPIO_AF_Set(GPIOA,5,1);	//PA5,AF1 
		GPIO_InitTypeDef TPAD_GPIO_Handler;
		__HAL_RCC_TIM2_CLK_ENABLE();//TIM2时钟使能    
	  TPAD_RST_GPIO_PORT_CLK_ENABLE();           //开启GPIOA时钟
		HAL_GPIO_DeInit(TPAD_RST_GPIO_PORT,TPAD_RST_GPIO_PIN);
    TPAD_GPIO_Handler.Pin=TPAD_RST_GPIO_PIN; //p5
    TPAD_GPIO_Handler.Mode=GPIO_MODE_AF_PP;  //推挽输出
    TPAD_GPIO_Handler.Pull=GPIO_NOPULL;          //下拉
    TPAD_GPIO_Handler.Speed=GPIO_SPEED_HIGH;     //高速 100m
		TPAD_GPIO_Handler.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(TPAD_RST_GPIO_PORT,&TPAD_GPIO_Handler);
	
	  
 	TIM2->ARR=arr;  		//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  		//预分频器,1M的计数频率	 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 配置输入滤波器 不滤波 
 
	TIM2->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
	TIM2->CCER|=0<<1; 		//CC1P=0	上升沿捕获
	TIM2->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中
																 
	TIM2->CR1|=0x01;    	//使能定时器2
}













