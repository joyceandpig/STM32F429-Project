#include "usart.h"

///////////////////////////////宏定义操作////////////////////////////////////////
#define __HAL_USART_SET_DAT_TO_DR(__HANDLE__, __VAL__) (((__HANDLE__)->Instance->DR) == (__VAL__))


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////// 
//如果使用os,则包括下面的头文件即可.
#ifdef OS_UCOS
#include "includes.h"					//os 使用	  
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
/*加入以下代码,支持printf函数,而不需要选择use MicroLIB	*/  
#if defined(__MICROLIB)
	#warning	"Use MicroLIB Is Enable! "
#else
	#pragma import(__use_no_semihosting)             
	//标准库需要的支持函数                 
	struct __FILE 
	{ 
		int handle; 
	}; 

	FILE __stdout;       
	//定义_sys_exit()以避免使用半主机模式    
	void _sys_exit(int x) 
	{ 
		x = x; 
	} 
#endif 
	
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 			
	if(HAL_UART_Transmit(&USART1_Handler, (u8*)&ch, 1,20000) != HAL_OK){};
	return ch;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////变量定义区////////////////////////////////////////
#ifdef USART1_EN
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
char aTxBuffer[1024];	
u8 aRxBuffer;//HAL库使用的串口接收缓冲
UART_HandleTypeDef USART1_Handler; //UART1句柄
#endif

#ifdef USART2_EN
UART_HandleTypeDef USART2_Handler; //UART2句柄
#endif

#ifdef USART3_EN
UART_HandleTypeDef USART3_Handler; //UART3句柄

//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.

//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;
#endif

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////串口初始化定义区//////////////////////////////////
//初始化IO 串口1 
//bound:波特率
#ifdef USART1_EN
void uart1_init(u32 bound)
{	
	//UART 初始化设置
	USART1_Handler.Instance=USART1;					    //USART1
	USART1_Handler.Init.BaudRate=bound;				    //波特率
	USART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	USART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	USART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	USART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	USART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&USART1_Handler);					    //HAL_UART_Init()会使能UART1
	HAL_UART_Receive_IT(&USART1_Handler,&aRxBuffer,1);	//串口中断接收1个字节，同时会自动开启中断
}
#endif
#ifdef USART1_EN
void uart3_init(u32 bound)
{  	 
//	float temp;
//	u16 mantissa;
//	u16 fraction;	   
//	temp=(float)(pclk1*1000000)/(bound*16);//得到USARTDIV,OVER8设置为0
//	mantissa=temp;				 	//得到整数部分
//	fraction=(temp-mantissa)*16; 	//得到小数部分,OVER8设置为0	 
//    mantissa<<=4;
//	mantissa+=fraction; 
//	RCC->AHB1ENR|=1<<1;   			//使能PORTB口时钟  
//	RCC->APB1ENR|=1<<18;  			//使能串口3时钟 
////	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
//// 	GPIO_AF_Set(GPIOB,10,7);		//PB10,AF7
////	GPIO_AF_Set(GPIOB,11,7);		//PB11,AF7  	   
//	//波特率设置
// 	USART3->BRR=mantissa; 			// 波特率设置	 
//	USART3->CR1|=1<<3;  			//串口发送使能  
//	USART3->CR1|=1<<2;  			//串口接收使能
//	USART3->CR1|=1<<5;    			//接收缓冲区非空中断使能	
//	USART3->CR1|=1<<13;  			//串口使能  
////	MY_NVIC_Init(0,2,USART3_IRQn,2);//组2，优先级0,2,最高优先级 
////	TIM7_Int_Init(100-1,9600-1);	//10ms中断一次
//	TIM7->CR1&=~(1<<0);				//关闭定时器7
//	USART3_RX_STA=0;				//清零 
	
	//UART 初始化设置
	USART3_Handler.Instance=USART3;					    //USART1
	USART3_Handler.Init.BaudRate=bound;				    //波特率
	USART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	USART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	USART3_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	USART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	USART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&USART3_Handler);					    //HAL_UART_Init()会使能UART1
	HAL_UART_Receive_IT(&USART3_Handler,USART3_RX_BUF,1);	//串口中断接收1个字节，同时会自动开启中断
}
#endif
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////初始化回调函数定义区/////////////////////////////////
//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
		
#if USART1_IRQn_EN
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,USART1_IRQn_Priority,0);			//抢占优先级，子优先级
#endif	
	}
	else if(huart->Instance==USART3)//如果是串口3，进行串口3 MSP初始化
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//使能GPIOB时钟
		__HAL_RCC_USART3_CLK_ENABLE();			//使能USART3时钟
	
		GPIO_Initure.Pin=GPIO_PIN_10;			//PB10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate=GPIO_AF7_USART3;	//复用为USART3
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB10

		GPIO_Initure.Pin=GPIO_PIN_11;			//PB11
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//初始化PB11
		
#if USART1_IRQn_EN
		HAL_NVIC_EnableIRQ(USART3_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART3_IRQn,USART3_IRQn_Priority,0);			//抢占优先级，子优先级
#endif	
	}

}
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////串口发送函数定义区////////////////////////////////
#ifdef USART1_EN
void UART_Printf(char *pData)
{
#if	USART1_TX_EN	
	if(HAL_UART_Transmit(&USART1_Handler, (uint8_t *)pData,(uint16_t)strlen(pData),2000) != HAL_OK)
	{//发送失败
		
	}
#else
	printf("%s",pData);	
#endif
}
#endif

#ifdef USART3_EN
//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
//	u16 i,j;
//	va_list ap;
//	va_start(ap,fmt);
//	vsprintf((char*)USART3_TX_BUF,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
//	for(j=0;j<i;j++)//循环发送数据
//	{
//		while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
//		USART3->DR=USART3_TX_BUF[j];  
//	}
	#if USART3_TX_EN
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
		while(!__HAL_UART_GET_FLAG(&USART3_Handler,UART_FLAG_TC));		//循环发送,直到发送完毕 
		__HAL_USART_SET_DAT_TO_DR(&USART3_Handler,USART3_TX_BUF[j]);
	}
	#endif
}
#endif
/****************************************************************************************/
/****************************************************************************************/
/*************************下面程序通过在回调函数中编写中断控制逻辑*********************/
/****************************************************************************************
*****************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//如果是串口1
	{//中断接收完成		
		USMART_Recv_Process(aRxBuffer);//处理上一次接收到的数据
		HAL_UART_Receive_IT(&USART1_Handler,&aRxBuffer,1);//中断接收1个字节
	}
	else if(huart->Instance==USART3)//如果是串口1
	{//中断接收完成		
		USMART_Recv_Process(USART3_RX_BUF[0]);//处理上一次接收到的数据
		HAL_UART_Receive_IT(&USART3_Handler,USART3_RX_BUF,1);//中断接收1个字节
	}
}
/*******************************************************************************
* Function Name : 
* Description   :
* Input         : 
* Return        : 
* Author        :
* Modify Time   :
*******************************************************************************/ 
//串口1中断服务程序
#ifdef USART1_EN
void USART1_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&USART1_Handler);	//调用HAL库中断处理公用函数

#ifdef OS_UCOS	 	//使用OS
	OSIntExit();  											 
#endif

}
#endif
//串口2中断服务程序
#ifdef USART2_EN
void USART2_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&USART2_Handler);	//调用HAL库中断处理公用函数

#ifdef OS_UCOS	 	//使用OS
	OSIntExit();  											 
#endif

}
#endif
//串口3中断服务程序
#ifdef USART3_EN
void USART3_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&USART3_Handler);	//调用HAL库中断处理公用函数

#ifdef OS_UCOS	 	//使用OS
	OSIntExit();  											 
#endif
}
#endif
