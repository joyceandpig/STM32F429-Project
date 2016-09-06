#ifndef __SPI_H
#define __SPI_H
#include "mytypes.h"

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


void SPI5_Init(void);			 //初始化SPI口
u8 SPI5_ReadWriteByte(u8 TxData);//SPI总线读写一个字节

//void SPI2_Init(void);
//void SPI2_SetSpeed(u8 SpeedSet);
//u8 SPI2_ReadWriteByte(u8 TxData);
#endif

