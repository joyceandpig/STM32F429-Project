#include "usart.h"

///////////////////////////////�궨�����////////////////////////////////////////
#define __HAL_USART_SET_DAT_TO_DR(__HANDLE__, __VAL__) (((__HANDLE__)->Instance->DR) == (__VAL__))


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////// 
//���ʹ��os,����������ͷ�ļ�����.
#ifdef OS_UCOS
#include "includes.h"					//os ʹ��	  
#endif

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)	
/*�������´���,֧��printf����,������Ҫѡ��use MicroLIB	*/  
#if defined(__MICROLIB)
	#warning	"Use MicroLIB Is Enable! "
#else
	#pragma import(__use_no_semihosting)             
	//��׼����Ҫ��֧�ֺ���                 
	struct __FILE 
	{ 
		int handle; 
	}; 

	FILE __stdout;       
	//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
	void _sys_exit(int x) 
	{ 
		x = x; 
	} 
#endif 
	
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 			
	if(HAL_UART_Transmit(&USART1_Handler, (u8*)&ch, 1,20000) != HAL_OK){};
	return ch;
}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////����������////////////////////////////////////////
#ifdef USART1_EN
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
char aTxBuffer[1024];	
u8 aRxBuffer;//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef USART1_Handler; //UART1���
#endif

#ifdef USART2_EN
UART_HandleTypeDef USART2_Handler; //UART2���
#endif

#ifdef USART3_EN
UART_HandleTypeDef USART3_Handler; //UART3���

//���ڷ��ͻ����� 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART3_RX_STA=0;
#endif

//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////���ڳ�ʼ��������//////////////////////////////////
//��ʼ��IO ����1 
//bound:������
#ifdef USART1_EN
void uart1_init(u32 bound)
{	
	//UART ��ʼ������
	USART1_Handler.Instance=USART1;					    //USART1
	USART1_Handler.Init.BaudRate=bound;				    //������
	USART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	USART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	USART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	USART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	USART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	HAL_UART_Receive_IT(&USART1_Handler,&aRxBuffer,1);	//�����жϽ���1���ֽڣ�ͬʱ���Զ������ж�
}
#endif
#ifdef USART1_EN
void uart3_init(u32 bound)
{  	 
//	float temp;
//	u16 mantissa;
//	u16 fraction;	   
//	temp=(float)(pclk1*1000000)/(bound*16);//�õ�USARTDIV,OVER8����Ϊ0
//	mantissa=temp;				 	//�õ���������
//	fraction=(temp-mantissa)*16; 	//�õ�С������,OVER8����Ϊ0	 
//    mantissa<<=4;
//	mantissa+=fraction; 
//	RCC->AHB1ENR|=1<<1;   			//ʹ��PORTB��ʱ��  
//	RCC->APB1ENR|=1<<18;  			//ʹ�ܴ���3ʱ�� 
////	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PB10,PB11,���ù���,�������
//// 	GPIO_AF_Set(GPIOB,10,7);		//PB10,AF7
////	GPIO_AF_Set(GPIOB,11,7);		//PB11,AF7  	   
//	//����������
// 	USART3->BRR=mantissa; 			// ����������	 
//	USART3->CR1|=1<<3;  			//���ڷ���ʹ��  
//	USART3->CR1|=1<<2;  			//���ڽ���ʹ��
//	USART3->CR1|=1<<5;    			//���ջ������ǿ��ж�ʹ��	
//	USART3->CR1|=1<<13;  			//����ʹ��  
////	MY_NVIC_Init(0,2,USART3_IRQn,2);//��2�����ȼ�0,2,������ȼ� 
////	TIM7_Int_Init(100-1,9600-1);	//10ms�ж�һ��
//	TIM7->CR1&=~(1<<0);				//�رն�ʱ��7
//	USART3_RX_STA=0;				//���� 
	
	//UART ��ʼ������
	USART3_Handler.Instance=USART3;					    //USART1
	USART3_Handler.Init.BaudRate=bound;				    //������
	USART3_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	USART3_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	USART3_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	USART3_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	USART3_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&USART3_Handler);					    //HAL_UART_Init()��ʹ��UART1
	HAL_UART_Receive_IT(&USART3_Handler,USART3_RX_BUF,1);	//�����жϽ���1���ֽڣ�ͬʱ���Զ������ж�
}
#endif
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////��ʼ���ص�����������/////////////////////////////////
//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART1;	//����ΪUSART1
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
		
#if USART1_IRQn_EN
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,USART1_IRQn_Priority,0);			//��ռ���ȼ��������ȼ�
#endif	
	}
	else if(huart->Instance==USART3)//����Ǵ���3�����д���3 MSP��ʼ��
	{
		__HAL_RCC_GPIOB_CLK_ENABLE();			//ʹ��GPIOBʱ��
		__HAL_RCC_USART3_CLK_ENABLE();			//ʹ��USART3ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_10;			//PB10
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FAST;		//����
		GPIO_Initure.Alternate=GPIO_AF7_USART3;	//����ΪUSART3
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PB10

		GPIO_Initure.Pin=GPIO_PIN_11;			//PB11
		HAL_GPIO_Init(GPIOB,&GPIO_Initure);	   	//��ʼ��PB11
		
#if USART1_IRQn_EN
		HAL_NVIC_EnableIRQ(USART3_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART3_IRQn,USART3_IRQn_Priority,0);			//��ռ���ȼ��������ȼ�
#endif	
	}

}
//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////���ڷ��ͺ���������////////////////////////////////
#ifdef USART1_EN
void UART_Printf(char *pData)
{
#if	USART1_TX_EN	
	if(HAL_UART_Transmit(&USART1_Handler, (uint8_t *)pData,(uint16_t)strlen(pData),2000) != HAL_OK)
	{//����ʧ��
		
	}
#else
	printf("%s",pData);	
#endif
}
#endif

#ifdef USART3_EN
//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
//	u16 i,j;
//	va_list ap;
//	va_start(ap,fmt);
//	vsprintf((char*)USART3_TX_BUF,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)USART3_TX_BUF);//�˴η������ݵĳ���
//	for(j=0;j<i;j++)//ѭ����������
//	{
//		while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
//		USART3->DR=USART3_TX_BUF[j];  
//	}
	#if USART3_TX_EN
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
		while(!__HAL_UART_GET_FLAG(&USART3_Handler,UART_FLAG_TC));		//ѭ������,ֱ��������� 
		__HAL_USART_SET_DAT_TO_DR(&USART3_Handler,USART3_TX_BUF[j]);
	}
	#endif
}
#endif
/****************************************************************************************/
/****************************************************************************************/
/*************************�������ͨ���ڻص������б�д�жϿ����߼�*********************/
/****************************************************************************************
*****************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//����Ǵ���1
	{//�жϽ������		
		USMART_Recv_Process(aRxBuffer);//������һ�ν��յ�������
		HAL_UART_Receive_IT(&USART1_Handler,&aRxBuffer,1);//�жϽ���1���ֽ�
	}
	else if(huart->Instance==USART3)//����Ǵ���1
	{//�жϽ������		
		USMART_Recv_Process(USART3_RX_BUF[0]);//������һ�ν��յ�������
		HAL_UART_Receive_IT(&USART3_Handler,USART3_RX_BUF,1);//�жϽ���1���ֽ�
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
//����1�жϷ������
#ifdef USART1_EN
void USART1_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&USART1_Handler);	//����HAL���жϴ����ú���

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntExit();  											 
#endif

}
#endif
//����2�жϷ������
#ifdef USART2_EN
void USART2_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&USART2_Handler);	//����HAL���жϴ����ú���

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntExit();  											 
#endif

}
#endif
//����3�жϷ������
#ifdef USART3_EN
void USART3_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&USART3_Handler);	//����HAL���жϴ����ú���

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntExit();  											 
#endif
}
#endif
