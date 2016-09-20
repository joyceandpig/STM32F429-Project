#include "timer.h"
#include "usart.h"
#include "led.h"
#include "includes.h"	 	//ucos 使用	
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/5
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									
//********************************************************************************
//修改说明
//V1.1 20151205
//新增TIM3_PWM_Init函数,用于PWM输出
//V1.2 20151205
//新增TIM5_CH1_Cap_Init函数,用于输入捕获
//V1.3 20151227
//新增TIM9_CH2_PWM_Init函数,用于PWM DAC实验
//V1.4 20160111
//新增TIM6_Int_Init函数,用于视频播放时隙控制
//V1.5 20160128
//新增TIM8_Int_Init函数  
//新增TIM3_CH2_PWM_Init函数
//新增TIM7_Int_Init函数 
////////////////////////////////////////////////////////////////////////////////// 
TIM_HandleTypeDef TIM2_Handler;
TIM_HandleTypeDef TIM3_Handler;
TIM_HandleTypeDef TIM5_Handler;
TIM_HandleTypeDef TIM6_Handler;
TIM_HandleTypeDef TIM7_Handler;
TIM_HandleTypeDef TIM8_Handler;
TIM_HandleTypeDef TIM9_Handler;



TIM_OC_InitTypeDef TIM3_CH4Handler;	    //定时器3通道4句柄
TIM_OC_InitTypeDef TIM9_CH2Handler;	    //定时器9通道2句柄


//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)

extern vu8 aviframeup;
extern vu8 webcam_oensec;
extern u16 reg_time;

extern vu16 USART3_RX_STA;

u16 tsr;

//extern void usbapp_pulling(void);   

vu8 framecnt;				//统一的帧计数器
vu8 framecntout;			//统一的帧计数器输出变量 


//定时器2通道1输入捕获配置	 
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
	  __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟
	
		HAL_GPIO_DeInit(GPIOA,GPIO_PIN_5);
    TPAD_GPIO_Handler.Pin=GPIO_PIN_5; //p5
    TPAD_GPIO_Handler.Mode=GPIO_MODE_AF_PP;  //推挽输出
    TPAD_GPIO_Handler.Pull=GPIO_NOPULL;          //下拉
    TPAD_GPIO_Handler.Speed=GPIO_SPEED_HIGH;     //高速 100m
		TPAD_GPIO_Handler.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA,&TPAD_GPIO_Handler);
	
	  
 	TIM2->ARR=arr;  		//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  		//预分频器,1M的计数频率	 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 配置输入滤波器 不滤波 
 
	TIM2->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
	TIM2->CCER|=0<<1; 		//CC1P=0	上升沿捕获
	TIM2->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中
																 
	TIM2->CR1|=0x01;    	//使能定时器2
	
//	  TIM_IC_InitTypeDef TIM2_CH1Config;  
//    
//    TIM2_Handler.Instance=TIM2;                          //通用定时器2
//    TIM2_Handler.Init.Prescaler=psc;                     //分频系数
//    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
//    TIM2_Handler.Init.Period=arr;                        //自动装载值
//    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频银子
//    HAL_TIM_IC_Init(&TIM2_Handler);//初始化输入捕获时基参数
//    
//    TIM2_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
//    TIM2_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
//    TIM2_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
//    TIM2_CH1Config.ICFilter=0;                          //配置输入滤波器，不滤波
//    HAL_TIM_IC_ConfigChannel(&TIM2_Handler,&TIM2_CH1Config,TIM_CHANNEL_1);//配置TIM2通道1
//	
//    HAL_TIM_IC_Start_IT(&TIM2_Handler,TIM_CHANNEL_1);   //开启TIM2的捕获通道1，并且开启捕获中断
//    __HAL_TIM_ENABLE_IT(&TIM2_Handler,TIM_IT_UPDATE);   //使能更新中断
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为48M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
//	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
// 	TIM3->ARR=arr;  	//设定计数器自动重装值 
//	TIM3->PSC=psc;  	//预分频器	  
//	TIM3->DIER|=1<<0;   //允许更新中断	  
//	TIM3->CR1|=0x01;    //使能定时器3
//  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2		
	
	  TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频系数
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}
//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}else if(htim->Instance == TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();            //使能TIM6时钟
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn,0,3);    //设置中断优先级，抢占优先级0，子优先级3
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);          //开启ITM6中断  
	}else if(htim->Instance == TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();            //使能TIM7时钟
		HAL_NVIC_SetPriority(TIM7_IRQn,0,1);    //设置中断优先级，抢占优先级0，子优先级1
		HAL_NVIC_EnableIRQ(TIM7_IRQn);          //开启ITM7中断  
	}else if(htim->Instance == TIM8)
	{
		__HAL_RCC_TIM8_CLK_ENABLE();            //使能TIM8时钟
		HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);          //开启ITM8中断  
	}
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}


//回调函数，定时器中断服务函数HAL_TIM_IRQHandler调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
			
    if(htim==(&TIM3_Handler))
    {
//        LED1=!LED1;        //LED1反转
    }else if(htim == (&TIM5_Handler))
		{
			tsr=TIM5->SR;
			if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
			{
				if(tsr&0X01)//溢出
				{	     
					if(TIM5CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
					{
						if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
						{
							TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获了一次
							TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
						}else TIM5CH1_CAPTURE_STA++;
					}	 
				}
				if(tsr&0x02)//捕获1发生捕获事件
				{	
					if(TIM5CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
					{	  			
						TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
							TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//获取当前的捕获值.
						TIM5->CCER&=~(1<<1);			//CC1P=0 设置为上升沿捕获
					}else  								//还未开始,第一次捕获上升沿
					{
						TIM5CH1_CAPTURE_STA=0;			//清空
						TIM5CH1_CAPTURE_VAL=0;
						TIM5CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
						TIM5->CR1&=~(1<<0)		;    	//使能定时器2
						TIM5->CNT=0;					//计数器清空
						TIM5->CCER|=1<<1; 				//CC1P=1 设置为下降沿捕获
						TIM5->CR1|=0x01;    			//使能定时器2
					}		    
				}			     	    					   
			}
			TIM5->SR=0;//清除中断标志位   
		}else if(htim==(&TIM6_Handler))
    { 
				aviframeup=1;
				webcam_oensec=1;
				reg_time++;
    }else if(htim==(&TIM7_Handler))
		{
//			if(TIM7->SR&0X01)//是更新中断
			{	 			   
				USART3_RX_STA|=1<<15;	//标记接收完成
				TIM7->SR&=~(1<<0);		//清除中断标志位		   
				TIM7->CR1&=~(1<<0);		//关闭定时器7	  
			}	
		}else if(htim==(&TIM8_Handler))
		{
				if(OSRunning!=TRUE)//OS还没运行,借TIM3的中断,10ms一次,来扫描USB
				{
		//			usbapp_pulling();
				}else
				{
					framecntout=framecnt;
					printf("frame:%d\r\n",framecntout);//打印帧率
					framecnt=0;
				}				    
		}	
}

//定时器输入捕获中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获
	{
		if(TIM5CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
                TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&TIM5_Handler,TIM_CHANNEL_1);//获取当前的捕获值.
                TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
                TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//配置TIM5通道1上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_STA=0;			//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				__HAL_TIM_DISABLE(&TIM5_Handler);        //关闭定时器5
				__HAL_TIM_SET_COUNTER(&TIM5_Handler,0);
				TIM_RESET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&TIM5_Handler,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//定时器5通道1设置为下降沿捕获
				__HAL_TIM_ENABLE(&TIM5_Handler);//使能定时器5
			}		    
	}		
	
}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
//	//此部分需手动修改IO口设置
//	RCC->APB1ENR|=1<<1;		//TIM3时钟使能    
//	RCC->AHB1ENR|=1<<2;   	//使能PORTB时钟	
////	GPIO_Set(GPIOB,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
////	GPIO_AF_Set(GPIOB,1,2);	//PB1,AF2 
//	
//	TIM3->ARR=arr;			//设定计数器自动重装值 
//	TIM3->PSC=psc;			//预分频器不分频 
//	TIM3->CCMR2|=6<<12;  	//CH4 PWM1模式		 
//	TIM3->CCMR2|=1<<11; 	//CH4 预装载使能	   
//	TIM3->CCER|=1<<12;   	//OC4 输出使能	
//	TIM3->CCER|=1<<13;   	//OC4 低电平有效	   
//	TIM3->CR1|=1<<7;   		//ARPE使能 
//	TIM3->CR1|=1<<0;    	//使能定时器3					

    TIM3_Handler.Instance=TIM3;            //定时器3
    TIM3_Handler.Init.Prescaler=psc;       //定时器分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数模式
    TIM3_Handler.Init.Period=arr;          //自动重装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);       //初始化PWM
    
    TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; //模式选择PWM1
    TIM3_CH4Handler.Pulse=arr/2;            //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; //输出比较极性为低 
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_4);//配置TIM3通道4
	
    HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_4);//开启PWM通道4	
}  


//TIM3 CH2 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_CH2_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
////	RCC->APB1ENR|=1<<1;		//TIM3时钟使能    
////	RCC->AHB1ENR|=1<<2;   	//使能PORTB时钟	
////	GPIO_Set(GPIOB,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
////	GPIO_AF_Set(GPIOB,5,2);	//PB1,AF2 
////	
////	TIM3->ARR=arr;			//设定计数器自动重装值 
////	TIM3->PSC=psc;			//预分频器不分频 
////	TIM3->CCMR1|=6<<12;  	//CH2 PWM1模式		 
////	TIM3->CCMR1|=1<<11; 	//CH2 预装载使能	   
////	TIM3->CCER|=1<<4;   	//OC1 输出使能	
////	TIM3->CCER|=0<<5;   	//OC1 高电平有效	   
////	TIM3->CR1|=1<<7;   		//ARPE使能 
////	TIM3->CR1|=1<<0;    	//使能定时器3				

    TIM3_Handler.Instance=TIM3;            //定时器3
    TIM3_Handler.Init.Prescaler=psc;       //定时器分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数模式
    TIM3_Handler.Init.Period=arr;          //自动重装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler);       //初始化PWM
    
    TIM3_CH4Handler.OCMode=TIM_OCMODE_PWM1; //模式选择PWM1
    TIM3_CH4Handler.Pulse=arr/2;            //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM3_CH4Handler.OCPolarity=TIM_OCPOLARITY_LOW; //输出比较极性为低 
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler,&TIM3_CH4Handler,TIM_CHANNEL_2);//配置TIM3通道4
	
    HAL_TIM_PWM_Start(&TIM3_Handler,TIM_CHANNEL_2);//开启PWM通道4		
}  
//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
		__HAL_RCC_TIM3_CLK_ENABLE();			//使能定时器3
		__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
		__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
		
	if((htim->Instance == TIM3) && (htim->Channel == TIM_CHANNEL_4))
	{
    GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
		GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PB1复用为TIM3_CH4
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}
	else if((htim->Instance == TIM3) && (htim->Channel == TIM_CHANNEL_2))
	{
    GPIO_Initure.Pin=GPIO_PIN_5;           	//PB5
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
		GPIO_Initure.Alternate= GPIO_AF2_TIM3;	//PB5复用为TIM3_CH2
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	}else if((htim->Instance == TIM9) && (htim->Channel == TIM_CHANNEL_2))
	{
    GPIO_Initure.Pin=GPIO_PIN_3;           	//PA3
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	//复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
		GPIO_Initure.Alternate= GPIO_AF3_TIM9;	//PA3复用为TIM9_CH2
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	}
}

//设置TIM通道4的占空比
//compare:比较值
void TIM_SetTIM3Compare4(u32 compare)
{
	TIM3->CCR4=compare;
}
//获取TIM捕获/比较寄存器值
u32 TIM_GetTIM3Capture4(void)
{
	return  HAL_TIM_ReadCapturedValue(&TIM3_Handler,TIM_CHANNEL_4);
}

//设置TIM通道2的占空比
//compare:比较值
void TIM_SetTIM3Compare2(u32 compare)
{
	TIM3->CCR2=compare;
}
//获取TIM捕获/比较寄存器值
u32 TIM_GetTIM3Capture2(void)
{
	return  HAL_TIM_ReadCapturedValue(&TIM3_Handler,TIM_CHANNEL_2);
}

//定时器5通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
//	RCC->APB1ENR|=1<<3;   	//TIM5 时钟使能 
//	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
////	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//复用功能,下拉
////	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
//	  
// 	TIM5->ARR=arr;  		//设定计数器自动重装值   
//	TIM5->PSC=psc;  		//预分频器 

//	TIM5->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
// 	TIM5->CCMR1|=0<<4; 		//IC1F=0000 配置输入滤波器 不滤波
// 	TIM5->CCMR1|=0<<10; 	//IC1PS=00 	配置输入分频,不分频 

//	TIM5->CCER|=0<<1; 		//CC1P=0	上升沿捕获
//	TIM5->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

//	TIM5->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
//	TIM5->DIER|=1<<1;   	//允许捕获1中断				
//	TIM5->DIER|=1<<0;   	//允许更新中断	
//	TIM5->CR1|=0x01;    	//使能定时器2
////	MY_NVIC_Init(2,0,TIM5_IRQn,2);//抢占2，子优先级0，组2	 

    TIM_IC_InitTypeDef TIM5_CH1Config;  
    
    TIM5_Handler.Instance=TIM5;                          //通用定时器5
    TIM5_Handler.Init.Prescaler=psc;                     //分频系数
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频银子
    HAL_TIM_IC_Init(&TIM5_Handler);//初始化输入捕获时基参数
    
    TIM5_CH1Config.ICPolarity=TIM_ICPOLARITY_RISING;    //上升沿捕获
    TIM5_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM5_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM5_CH1Config.ICFilter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_IC_ConfigChannel(&TIM5_Handler,&TIM5_CH1Config,TIM_CHANNEL_1);//配置TIM5通道1
	
    HAL_TIM_IC_Start_IT(&TIM5_Handler,TIM_CHANNEL_1);   //开启TIM5的捕获通道1，并且开启捕获中断
    __HAL_TIM_ENABLE_IT(&TIM5_Handler,TIM_IT_UPDATE);   //使能更新中断
}
//定时器5底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器5句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM5时钟
	__HAL_RCC_TIM2_CLK_ENABLE();            //使能TIM5时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	
	if(htim->Instance == TIM5)
	{
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    GPIO_Initure.Alternate=GPIO_AF2_TIM5;   //PA0复用为TIM5通道1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM5_IRQn,2,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM5中断通道  
	}else if(htim->Instance == TIM2)
	{
	    GPIO_Initure.Pin=GPIO_PIN_5;            //PA5
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    GPIO_Initure.Alternate=GPIO_AF1_TIM2;   //PA5复用为TIM2通道1
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM2_IRQn,2,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM2_IRQn);          //开启ITM5中断通道 
	}
}
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM2_Handler);//定时器共用处理函数
}
////捕获状态
////[7]:0,没有成功的捕获;1,成功捕获到一次.
////[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
////[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
//u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
//u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
//定时器5中断服务程序	 
//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM5_Handler);//定时器共用处理函数
}
//void TIM5_IRQHandler(void)
//{ 		    
//	u16 tsr;
//	tsr=TIM5->SR;
// 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
//	{
//		if(tsr&0X01)//溢出
//		{	     
//			if(TIM5CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
//			{
//				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
//				{
//					TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获了一次
//					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
//				}else TIM5CH1_CAPTURE_STA++;
//			}	 
//		}
//		if(tsr&0x02)//捕获1发生捕获事件
//		{	
//			if(TIM5CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
//			{	  			
//				TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
//			    TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//获取当前的捕获值.
//	 			TIM5->CCER&=~(1<<1);			//CC1P=0 设置为上升沿捕获
//			}else  								//还未开始,第一次捕获上升沿
//			{
//				TIM5CH1_CAPTURE_STA=0;			//清空
//				TIM5CH1_CAPTURE_VAL=0;
//				TIM5CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
//				TIM5->CR1&=~(1<<0)		;    	//使能定时器2
//	 			TIM5->CNT=0;					//计数器清空
//	 			TIM5->CCER|=1<<1; 				//CC1P=1 设置为下降沿捕获
//				TIM5->CR1|=0x01;    			//使能定时器2
//			}		    
//		}			     	    					   
// 	}
//	TIM5->SR=0;//清除中断标志位   
//}

//PWM DAC初始化(也就是TIM9通道2初始化)
//TIM9 CH2 PWM输出设置 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{		 					 
//	RCC->APB2ENR|=1<<16;   	//TIM9 时钟使能 
//	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
////	GPIO_Set(GPIOA,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//PA3,复用功能,上拉
////	GPIO_AF_Set(GPIOA,3,3);	//PA3,AF3
// 	TIM9->ARR=arr;  		//设定计数器自动重装值   
//	TIM9->PSC=psc;  		//预分频器 
//	TIM9->CCMR1|=6<<12;  	//CH2 PWM1模式		 
//	TIM9->CCMR1|=1<<11; 	//CH2 预装载使能	
//	TIM9->CCER|=1<<4;   	//OC2 输出使能	  
//	TIM9->CR1|=1<<7;   		//ARPE使能 
//	TIM9->CR1|=1<<0;    	//使能定时器9 
	
	  TIM9_Handler.Instance=TIM9;            //定时器3
    TIM9_Handler.Init.Prescaler=psc;       //定时器分频
    TIM9_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;//向上计数模式
    TIM9_Handler.Init.Period=arr;          //自动重装载值
    TIM9_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM9_Handler);       //初始化PWM
    
    TIM9_CH2Handler.OCMode=TIM_OCMODE_PWM1; //模式选择PWM1
    TIM9_CH2Handler.Pulse=arr/2;            //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM9_CH2Handler.OCPolarity=TIM_OCPOLARITY_HIGH; //输出比较极性为高
    HAL_TIM_PWM_ConfigChannel(&TIM9_Handler,&TIM9_CH2Handler,TIM_CHANNEL_2);//配置TIM3通道2
	
    HAL_TIM_PWM_Start(&TIM9_Handler,TIM_CHANNEL_2);//开启PWM通道2	
} 
//设置TIM通道2的占空比
//TIM_TypeDef:定时器
//compare:比较值
void TIM_SetTIM9Compare2(u32 compare)
{
	TIM9->CCR2=compare;
}

//定时器6中断服务程序	 
void TIM6_DAC_IRQHandler(void)
{ 		    		  			    
//	OSIntEnter();    		    
//	if(TIM6->SR&0X0001)//溢出中断
//	{ 
//		aviframeup=1;
//		webcam_oensec=1;
//		reg_time++;
//	}				   
//	TIM6->SR&=~(1<<0);//清除中断标志位 	    
//	OSIntExit(); 	    
	OSIntEnter();  
	 HAL_TIM_IRQHandler(&TIM6_Handler);
	OSIntExit();
}
//通用定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为48M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM6_Int_Init(u16 arr,u16 psc)
{		
//	u32 temp0;	
//	u32 temp,temp1;
//	RCC->APB1ENR|=1<<4;	//TIM6时钟使能    
// 	TIM6->ARR=arr;  	//设定计数器自动重装值 
//	TIM6->PSC=psc;  	//预分频器	  
//	TIM6->CNT=0;  		//计数器清零	  
//	TIM6->DIER|=1<<0;   //允许更新中断	  
//	TIM6->CR1|=0x01;    //使能定时器6
////  	MY_NVIC_Init(3,0,TIM6_DAC_IRQn,2);	//抢占0，子优先级3，组2		
//  
////	MY_NVIC_PriorityGroupConfig(2);//设置分组
//			  
//	temp1=(~2)&0x07;//取后三位
//	temp1<<=8;
//	temp=SCB->AIRCR;  //读取先前的设置
//	temp&=0X0000F8FF; //清空先前分组
//	temp|=0X05FA0000; //写入钥匙
//	temp|=temp1;	   
//	SCB->AIRCR=temp;  //设置分组	  
//	
//	temp0=3<<(4-2);	  
//	temp0|=0&(0x0f>>2);
//	temp0&=0xf;								//取低四位
//	NVIC->ISER[TIM6_DAC_IRQn/32]|=1<<TIM6_DAC_IRQn%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
//	NVIC->IP[TIM6_DAC_IRQn]|=temp0<<4;				//设置响应优先级和抢断优先级   	
	
	  TIM6_Handler.Instance=TIM6;                          //通用定时器6
    TIM6_Handler.Init.Prescaler=psc;                     //分频系数
    TIM6_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM6_Handler.Init.Period=arr;                        //自动装载值
    TIM6_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    TIM6_Handler.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init(&TIM6_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM6_Handler); //使能定时器6更新中断：TIM_IT_UPDATE   	
}


//定时器7中断服务程序		    
void TIM7_IRQHandler(void)
{ 	
//	OSIntEnter();    		    
//	if(TIM7->SR&0X01)//是更新中断
//	{	 			   
//		USART3_RX_STA|=1<<15;	//标记接收完成
//		TIM7->SR&=~(1<<0);		//清除中断标志位		   
//		TIM7->CR1&=~(1<<0);		//关闭定时器7	  
//	}	    
//	OSIntExit();  

	OSIntEnter();  
	 HAL_TIM_IRQHandler(&TIM7_Handler);
	OSIntExit();	
} 
//通用定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为48M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
void TIM7_Int_Init(u16 arr,u16 psc)
{
//	RCC->APB1ENR|=1<<5;	//TIM7时钟使能    
// 	TIM7->ARR=arr;  	//设定计数器自动重装值 
//	TIM7->PSC=psc;  	//预分频器	  
//	TIM7->CNT=0;  		//计数器清零	  
//	TIM7->DIER|=1<<0;   //允许更新中断	  
//	TIM7->CR1|=0x01;    //使能定时器7
////  	MY_NVIC_Init(0,1,TIM7_IRQn,2);	//抢占0，子优先级1，组2				

	  TIM7_Handler.Instance=TIM7;                          //通用定时器7
    TIM7_Handler.Init.Prescaler=psc;                     //分频系数
    TIM7_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM7_Handler.Init.Period=arr;                        //自动装载值
    TIM7_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM7_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM7_Handler); //使能定时器7更新中断：TIM_IT_UPDATE   		
} 


//定时器8/13中断服务程序	 
void TIM8_UP_TIM13_IRQHandler(void)
{ 		    		  			    
//	OSIntEnter();   
//	if(TIM8->SR&0X0001)//溢出中断 
//	{
//		if(OSRunning!=TRUE)//OS还没运行,借TIM3的中断,10ms一次,来扫描USB
//		{
////			usbapp_pulling();
//		}else
//		{
//			framecntout=framecnt;
//			printf("frame:%d\r\n",framecntout);//打印帧率
//			framecnt=0;
//		}
//	}				   
//	TIM8->SR&=~(1<<0);//清除中断标志位 	    
//	OSIntExit(); 
	OSIntEnter();  
	HAL_TIM_IRQHandler(&TIM8_Handler);
	OSIntExit();
}
//定时器8中断初始化
//这里时钟选择为APB2的2倍，而APB2为96M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM8_Int_Init(u16 arr,u16 psc)
{
//	u32 temp0;	
//	u32 temp,temp1;
//	RCC->APB2ENR|=1<<1;	//TIM8时钟使能    
// 	TIM8->ARR=arr;  	//设定计数器自动重装值 
//	TIM8->PSC=psc;  	//预分频器	  
//	TIM8->DIER|=1<<0;   //允许更新中断	  
//	TIM8->CR1|=0x01;    //使能定时器8
////  	MY_NVIC_Init(1,3,TIM8_UP_TIM13_IRQn,2);	//抢占1，子优先级3，组2		
//		temp1=(~2)&0x07;//取后三位
//	temp1<<=8;
//	temp=SCB->AIRCR;  //读取先前的设置
//	temp&=0X0000F8FF; //清空先前分组
//	temp|=0X05FA0000; //写入钥匙
//	temp|=temp1;	   
//	SCB->AIRCR=temp;  //设置分组	  
//	
//	temp0=1<<(4-2);	  
//	temp0|=3&(0x0f>>2);
//	temp0&=0xf;								//取低四位
//	NVIC->ISER[TIM8_UP_TIM13_IRQn/32]|=1<<TIM8_UP_TIM13_IRQn%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
//	NVIC->IP[TIM8_UP_TIM13_IRQn]|=temp0<<4;				//设置响应优先级和抢断优先级   	
	
	
	  TIM8_Handler.Instance=TIM8;                          //通用定时器8
    TIM8_Handler.Init.Prescaler=psc;                     //分频系数
    TIM8_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM8_Handler.Init.Period=arr;                        //自动装载值
    TIM8_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
		HAL_TIM_Base_Init(&TIM8_Handler);

    HAL_TIM_Base_Start_IT(&TIM8_Handler); //使能定时器8更新中断：TIM_IT_UPDATE  	

//  TIM_ClockConfigTypeDef sClockSourceConfig;
//  TIM_SlaveConfigTypeDef sSlaveConfig;
//  TIM_MasterConfigTypeDef sMasterConfig;
//	
//	TIM8_Handler.Instance = TIM8;
//  TIM8_Handler.Init.Prescaler = psc;
//  TIM8_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
//  TIM8_Handler.Init.Period = arr;
//  TIM8_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  TIM8_Handler.Init.RepetitionCounter = 0;
//  HAL_TIM_Base_Init(&TIM8_Handler);

////  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
////  HAL_TIM_ConfigClockSource(&TIM8_Handler, &sClockSourceConfig);

//  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
//  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
//  HAL_TIM_SlaveConfigSynchronization(&TIM8_Handler, &sSlaveConfig);

//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  HAL_TIMEx_MasterConfigSynchronization(&TIM8_Handler, &sMasterConfig);
//	HAL_TIM_Base_Start_IT(&TIM8_Handler);
}














