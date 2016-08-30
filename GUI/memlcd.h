#ifndef __MEMLCD_H
#define __MEMLCD_H			 
#include "guix.h"  
#include "stdlib.h" 

#define MEMLCD_MAX_LAYER	1			//MEM LCD所支持的显存层数

//LCD重要参数集
typedef struct  
{		 	 
	u16 width;							//MEM LCD 宽度
	u16 height;							//MEM LCD 高度 
	u8  dir;							//横屏还是竖屏控制：0，竖屏；1，横屏。
	u16 *grambuf[MEMLCD_MAX_LAYER];		//MEM LCD GRAM
	u8 layer;							//操作层,0,第一层;1,第二层;2,第三层,以此类推.
}_mlcd_dev; 	  

extern _mlcd_dev mlcddev;



u8 mlcd_init(u16 width,u16 height,u8 lx);
void mlcd_delete(void);
void mlcd_draw_point(u16 x,u16 y,u16 color);
u16 mlcd_read_point(u16 x,u16 y);
void mlcd_clear(u16 color);
void mlcd_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
void mlcd_display_layer(u8 lx);

#endif















