#include "usart.h"
#include "usmart.h"
#include "user_config.h"
 
//如果使用os,则包括下面的头文件即可.
#ifdef OS_UCOS
#include "includes.h"					//os 使用	  
#endif

////////////////////////////////////////////////////////////////////////////////// 	  
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
	if(HAL_UART_Transmit(&UART1_Handler, (u8*)&ch, 1,20000) != HAL_OK)
	{
		
	}			
	return ch;
}
	
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
char aTxBuffer[1024];	
u8 aRxBuffer;//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART1_Handler; //UART句柄

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	HAL_UART_Receive_IT(&UART1_Handler,&aRxBuffer,1);	//串口中断接收1个字节，同时会自动开启中断
}

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
		
#if 1
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,USART1_IRQn_Priority,0);			//抢占优先级，子优先级
#endif	
	}

}

void UART_Printf(char *pData)
{
#if	1	
	if(HAL_UART_Transmit(&UART1_Handler, (uint8_t *)pData,(uint16_t)strlen(pData),2000) != HAL_OK)
	{//发送失败
		
	}
#else
	printf("%s",pData);	
#endif
}

/****************************************************************************************/
/****************************************************************************************/
/*************************下面程序通过在回调函数中编写中断控制逻辑*********************/
/****************************************************************************************
***************************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//如果是串口1
	{//中断接收完成		
		USMART_Recv_Process(aRxBuffer);//处理上一次接收到的数据
		HAL_UART_Receive_IT(&UART1_Handler,&aRxBuffer,1);//中断接收1个字节
	}
}

//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//使用OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART1_Handler);	//调用HAL库中断处理公用函数

#ifdef OS_UCOS	 	//使用OS
	OSIntExit();  											 
#endif

}
