#ifndef __SPI_H
#define __SPI_H
#include "mytypes.h"
						  	    													  
void SPI5_Init(void);			 //初始化SPI口
u8 SPI5_ReadWriteByte(u8 TxData);//SPI总线读写一个字节

//void SPI2_Init(void);
//void SPI2_SetSpeed(u8 SpeedSet);
//u8 SPI2_ReadWriteByte(u8 TxData);
#endif

