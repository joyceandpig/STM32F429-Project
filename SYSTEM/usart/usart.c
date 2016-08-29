#include "usart.h"
#include "usmart.h"
#include "user_config.h"
 
//���ʹ��os,����������ͷ�ļ�����.
#ifdef OS_UCOS
#include "includes.h"					//os ʹ��	  
#endif

////////////////////////////////////////////////////////////////////////////////// 	  
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
	if(HAL_UART_Transmit(&UART1_Handler, (u8*)&ch, 1,20000) != HAL_OK)
	{
		
	}			
	return ch;
}
	
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
char aTxBuffer[1024];	
u8 aRxBuffer;//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{	
	//UART ��ʼ������
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	HAL_UART_Receive_IT(&UART1_Handler,&aRxBuffer,1);	//�����жϽ���1���ֽڣ�ͬʱ���Զ������ж�
}

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
		
#if 1
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,USART1_IRQn_Priority,0);			//��ռ���ȼ��������ȼ�
#endif	
	}

}

void UART_Printf(char *pData)
{
#if	1	
	if(HAL_UART_Transmit(&UART1_Handler, (uint8_t *)pData,(uint16_t)strlen(pData),2000) != HAL_OK)
	{//����ʧ��
		
	}
#else
	printf("%s",pData);	
#endif
}

/****************************************************************************************/
/****************************************************************************************/
/*************************�������ͨ���ڻص������б�д�жϿ����߼�*********************/
/****************************************************************************************
***************************************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//����Ǵ���1
	{//�жϽ������		
		USMART_Recv_Process(aRxBuffer);//������һ�ν��յ�������
		HAL_UART_Receive_IT(&UART1_Handler,&aRxBuffer,1);//�жϽ���1���ֽ�
	}
}

//����1�жϷ������
void USART1_IRQHandler(void)                	
{ 

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART1_Handler);	//����HAL���жϴ����ú���

#ifdef OS_UCOS	 	//ʹ��OS
	OSIntExit();  											 
#endif

}
