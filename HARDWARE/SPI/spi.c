#include "spi.h"
#include "sys.h"

/* Definition for SPIx clock resources */
#define SPIx                             SPI5
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI5_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE() 
#define SPIx_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI5_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI5_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_7
#define SPIx_SCK_GPIO_PORT               GPIOF
#define SPIx_SCK_AF                      GPIO_AF5_SPI4

#define SPIx_MISO_PIN                    GPIO_PIN_8
#define SPIx_MISO_GPIO_PORT              GPIOF
#define SPIx_MISO_AF                     GPIO_AF5_SPI4

#define SPIx_MOSI_PIN                    GPIO_PIN_9
#define SPIx_MOSI_GPIO_PORT              GPIOF
#define SPIx_MOSI_AF                     GPIO_AF5_SPI4


SPI_HandleTypeDef SpiHandle;
//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI5的初始化
void SPI5_Init(void)
{	

  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SpiHandle.Instance               = SPIx;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  
  SpiHandle.Init.Mode = SPI_MODE_MASTER;

  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
  }	 
}   

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SPIx_SCK_GPIO_CLK_ENABLE();
  SPIx_MISO_GPIO_CLK_ENABLE();
  SPIx_MOSI_GPIO_CLK_ENABLE();
  /* Enable SPI clock */
  SPIx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* SPI SCK GPIO pin configuration  */
  GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = SPIx_SCK_AF;
  
  HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
  /* SPI MISO GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = SPIx_MISO_AF;
  
  HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
  
  /* SPI MOSI GPIO pin configuration  */
  GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    
  HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);    
}


//SPI5速度设置函数
//SpeedSet:0~7
//SPI速度=fAPB2/2^(SpeedSet+1)
//fAPB2时钟一般为96Mhz
void SPI5_SetSpeed(u8 SpeedSet)
{
//	SpeedSet&=0X07;			//限制范围
//	SPI5->CR1&=0XFFC7; 
//	SPI5->CR1|=SpeedSet<<3;	//设置SPI5速度  
//	SPI5->CR1|=1<<6; 		//SPI设备使能	  
} 
//SPI5 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI5_ReadWriteByte(u8 TxData)
{	
	u8 Res;
	while(HAL_SPI_TransmitReceive(&SpiHandle, &TxData, &Res, 1, 5000) != HAL_OK)
	{
		
	}	
	return Res;				    
}

////以下是SPI模块的初始化代码，配置成主机模式 						  
////SPI口初始化
////这里针是对SPI2的初始化
//void SPI2_Init(void)
//{	 
////	u16 tempreg=0;
////	RCC->AHB1ENR|=1<<1;		//使能PORTB时钟 
////	RCC->APB1ENR|=1<<14;   	//SPI2时钟使能 
////	GPIO_Set(GPIOB,7<<13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PB13~15复用功能输出	
////  	GPIO_AF_Set(GPIOB,13,5);//PB13,AF5
//// 	GPIO_AF_Set(GPIOB,14,5);//PB14,AF5
//// 	GPIO_AF_Set(GPIOB,15,5);//PB15,AF5 

////	//这里只针对SPI口初始化
////	RCC->APB1RSTR|=1<<14;	//复位SPI2
////	RCC->APB1RSTR&=~(1<<14);//停止复位SPI2
////	tempreg|=0<<10;			//全双工模式	
////	tempreg|=1<<9;			//软件nss管理
////	tempreg|=1<<8;			 
////	tempreg|=1<<2;			//SPI主机  
////	tempreg|=0<<11;			//8位数据格式	
////	tempreg|=1<<1;			//空闲模式下SCK为1 CPOL=1 
////	tempreg|=1<<0;			//数据采样从第2个时间边沿开始,CPHA=1  
//// 	//对SPI2属于APB1的外设.时钟频率最大为48Mhz频率.
////	tempreg|=7<<3;			//Fsck=Fpclk1/256
////	tempreg|=0<<7;			//MSB First  
////	tempreg|=1<<6;			//SPI启动 
////	SPI2->CR1=tempreg; 		//设置CR1  
////	SPI2->I2SCFGR&=~(1<<11);//选择SPI模式
////	SPI2_ReadWriteByte(0xff);//启动传输		 
//}   
////SPI2速度设置函数
////SpeedSet:0~7
////SPI速度=fAPB1/2^(SpeedSet+1)
////fAPB2时钟一般为45Mhz
//void SPI2_SetSpeed(u8 SpeedSet)
//{
////	SpeedSet&=0X07;			//限制范围
////	SPI2->CR1&=0XFFC7; 
////	SPI2->CR1|=SpeedSet<<3;	//设置SPI2速度  
////	SPI2->CR1|=1<<6; 		//SPI设备使能	  
//} 
////SPI2 读写一个字节
////TxData:要写入的字节
////返回值:读取到的字节
//u8 SPI2_ReadWriteByte(u8 TxData)
//{		 			 
////	while((SPI2->SR&1<<1)==0);		//等待发送区空 
////	SPI2->DR=TxData;	 	  		//发送一个byte  
////	while((SPI2->SR&1<<0)==0);		//等待接收完一个byte  
//// 	return SPI2->DR;          		//返回收到的数据				    
//}






