#ifndef __WM8978_H
#define __WM8978_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//WM8978 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/11
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

__packed typedef struct 
{							  
	u8 mvol;		//音量(耳机和喇叭一起调),范围:0~63
	u8 cfreq[5];	//中心频率选择数组,每个元素可选范围为0~3,且代表的频率各不相同 
					//[0]对应频率:80,105,135,175
					//[1]对应频率:230,300,385,500
					//[2]对应频率:650,850,1100,1400
					//[3]对应频率:1800,2400,3200,4100
					//[4]对应频率:5300,6900,9000,11700
	u8 freqval[5];	//中心频率增益表
	u8 d3;			//3d设置	
	u8 speakersw;	//板载喇叭开关,0,关闭;1,打开 
	u8 saveflag; 	//保存标志,0X0A,保存过了;其他,还从未保存	   
}_wm8978_obj;

extern _wm8978_obj wm8978set;//WM8978的设置

#define WM8978_ADDR_MSB_BIT(__ADDR__, __VAL__)      ((__ADDR__ << 1) | ((__VAL__ >> 8) & 0x01))


#define WM8978_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 
 
#define WM8978_EQ1_80Hz		0X00
#define WM8978_EQ1_105Hz		0X01
#define WM8978_EQ1_135Hz		0X02
#define WM8978_EQ1_175Hz		0X03

#define WM8978_EQ2_230Hz		0X00
#define WM8978_EQ2_300Hz		0X01
#define WM8978_EQ2_385Hz		0X02
#define WM8978_EQ2_500Hz		0X03

#define WM8978_EQ3_650Hz		0X00
#define WM8978_EQ3_850Hz		0X01
#define WM8978_EQ3_1100Hz		0X02
#define WM8978_EQ3_14000Hz		0X03

#define WM8978_EQ4_1800Hz		0X00
#define WM8978_EQ4_2400Hz		0X01
#define WM8978_EQ4_3200Hz		0X02
#define WM8978_EQ4_4100Hz		0X03

#define WM8978_EQ5_5300Hz		0X00
#define WM8978_EQ5_6900Hz		0X01
#define WM8978_EQ5_9000Hz		0X02
#define WM8978_EQ5_11700Hz		0X03

//WM8978 寄存器数值
#define WM8978_REG_R1        1
#define WM8978_REG_R2        2 
#define WM8978_REG_R3        3 
#define WM8978_REG_R4        4 
#define WM8978_REG_R5        5 
#define WM8978_REG_R6        6 
#define WM8978_REG_R7        7 
#define WM8978_REG_R8        8 
#define WM8978_REG_R9        9 
#define WM8978_REG_R10       10
#define WM8978_REG_R11       11
#define WM8978_REG_R12       12
#define WM8978_REG_R13       13 
#define WM8978_REG_R14       14 
#define WM8978_REG_R15       15 
#define WM8978_REG_R16       16 
#define WM8978_REG_R17       17 
#define WM8978_REG_R18       18 
#define WM8978_REG_R19       19 
#define WM8978_REG_R20       20 
#define WM8978_REG_R21       21 
#define WM8978_REG_R22       22
#define WM8978_REG_R23       23
#define WM8978_REG_R24       24
#define WM8978_REG_R25       25
#define WM8978_REG_R26       26 
#define WM8978_REG_R27       27 
#define WM8978_REG_R28       28 
#define WM8978_REG_R29       29 
#define WM8978_REG_R30       30 
#define WM8978_REG_R31       31 
#define WM8978_REG_R32       32 
#define WM8978_REG_R33       33 
#define WM8978_REG_R34       34
#define WM8978_REG_R35       35
#define WM8978_REG_R36       36
#define WM8978_REG_R37       37 
#define WM8978_REG_R38       38 
#define WM8978_REG_R39       39 
#define WM8978_REG_R40       40 
#define WM8978_REG_R41       41 
#define WM8978_REG_R42       42 
#define WM8978_REG_R43       43 
#define WM8978_REG_R44       44 
#define WM8978_REG_R45       45 
#define WM8978_REG_R46       46
#define WM8978_REG_R47       47
#define WM8978_REG_R48       48
#define WM8978_REG_R49       49 
#define WM8978_REG_R50       50 
#define WM8978_REG_R51       51 
#define WM8978_REG_R52       52 
#define WM8978_REG_R53       53 
#define WM8978_REG_R54       54 
#define WM8978_REG_R55       55 
#define WM8978_REG_R56       56 
#define WM8978_REG_R57       57 
#define WM8978_REG_R58       58
      

  
void wm8978_read_para(_wm8978_obj * wm8978dev);
void wm8978_save_para(_wm8978_obj * wm8978dev);

u8 WM8978_Init(void); 
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen);
void WM8978_Input_Cfg(u8 micen,u8 lineinen,u8 auxen);
void WM8978_Output_Cfg(u8 dacen,u8 bpsen);
void WM8978_MIC_Gain(u8 gain);
void WM8978_LINEIN_Gain(u8 gain);
void WM8978_AUX_Gain(u8 gain);
u8 WM8978_Write_Reg(u8 reg,u16 val); 
u16 WM8978_Read_Reg(u8 reg);
void WM8978_HPvol_Set(u8 voll,u8 volr);
void WM8978_SPKvol_Set(u8 volx);
void WM8978_I2S_Cfg(u8 fmt,u8 len);
void WM8978_3D_Set(u8 depth);
void WM8978_EQ_3D_Dir(u8 dir); 
void WM8978_EQ1_Set(u8 cfreq,u8 gain); 
void WM8978_EQ2_Set(u8 cfreq,u8 gain);
void WM8978_EQ3_Set(u8 cfreq,u8 gain);
void WM8978_EQ4_Set(u8 cfreq,u8 gain);
void WM8978_EQ5_Set(u8 cfreq,u8 gain);

#endif





















