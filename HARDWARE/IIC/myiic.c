#include "myiic.h"
#include "delay.h"

/* Definition for I2Cx clock resources */
#define I2Cx                             I2C2
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOH_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_4
#define I2Cx_SCL_GPIO_PORT              GPIOH
#define I2Cx_SCL_AF                     GPIO_AF4_I2C2

#define I2Cx_SDA_PIN                    GPIO_PIN_5
#define I2Cx_SDA_GPIO_PORT              GPIOH
#define I2Cx_SDA_AF                     GPIO_AF4_I2C2



I2C_HandleTypeDef I2cHandle;

void IIC_Init(void)
{
  /*##-1- Configure the I2C peripheral #######################################*/
  I2cHandle.Instance             = I2Cx;
  
  I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2cHandle.Init.ClockSpeed      = 800000;//400000;
  I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
  I2cHandle.Init.OwnAddress2     = 0xFE;
  
  if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
    /* Initialization Error */
  }
}

//IIC底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_I2C_Init()调用
//huart:串口句柄

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(hi2c->Instance==I2C2)//如果是I2C2，进行I2C2 MSP初始化
	{
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		/* Enable GPIO TX/RX clock */
		I2Cx_SCL_GPIO_CLK_ENABLE();
		I2Cx_SDA_GPIO_CLK_ENABLE();
		/* Enable I2C1 clock */
		I2Cx_CLK_ENABLE(); 

		/*##-2- Configure peripheral GPIO ##########################################*/  
		/* I2C TX GPIO pin configuration  */
		GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = I2Cx_SCL_AF;

		HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

		/* I2C RX GPIO pin configuration  */
		GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
		GPIO_InitStruct.Alternate = I2Cx_SDA_AF;

		HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);	
	}
}
