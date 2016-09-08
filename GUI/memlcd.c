#include "memlcd.h"   
#include "malloc.h"  

_mlcd_dev mlcddev;		//mem lcd 结构体

//初始化mem lcd设备
//width,height:mem lcd的宽度和高度
//lx:要申请多少个层,1~MEMLCD_MAX_LAYER;
//返回值:0,成功;1,lx值非法;2,内存错误
u8 mlcd_init(u16 width,u16 height,u8 lx)
{
	u8 i;
	u32 memsize=0;
	mlcddev.width=width;
	mlcddev.height=height;
	memsize=(u32)width*height*2;
	if(lx>MEMLCD_MAX_LAYER||lx==0)return 1;			//lx非法 
	for(i=0;i<lx;i++)
	{
		mlcddev.grambuf[i]=mymalloc(SRAMIN,memsize);//申请内存
		if(mlcddev.grambuf[i]==NULL)break;
	}
	if(i!=MEMLCD_MAX_LAYER)//申请失败了
	{
		mlcd_delete();
		return 2;
	}
	return 0;
}
//删除memlcd设备
void mlcd_delete(void)
{	
	u8 i;
	for(i=0;i<MEMLCD_MAX_LAYER;i++)
	{
		if(mlcddev.grambuf[i])myfree(SRAMIN,mlcddev.grambuf[i]);//释放内存
		mlcddev.grambuf[i]=0;
	}
	mlcddev.width=0;
	mlcddev.height=0;
	mlcddev.layer=0; 
} 
//设置操作层
//lx:层编号
void mlcd_set_layer(u8 lx)
{
	if(lx<MEMLCD_MAX_LAYER)mlcddev.layer=lx;//设置操作层
}
//在指定位置画点.
//x,y:坐标
//color:颜色.
void mlcd_draw_point(u16 x,u16 y,u16 color)
{	   
	if(lcdltdc.pwidth&&mlcddev.dir==0)//RGB竖屏 
	{	
		x=mlcddev.width-x-1;	//坐标变换
		mlcddev.grambuf[mlcddev.layer][y+x*mlcddev.height]=color;
	}else mlcddev.grambuf[mlcddev.layer][x+y*mlcddev.width]=color; 
}
//读取指定位置点的颜色值
//x,y:坐标
//返回值:颜色
u16 mlcd_read_point(u16 x,u16 y)
{ 
	if(lcdltdc.pwidth&&mlcddev.dir==0)//RGB竖屏 
	{	
		x=mlcddev.width-x-1;	//坐标变换
		return mlcddev.grambuf[mlcddev.layer][y+x*mlcddev.height];
	}else return mlcddev.grambuf[mlcddev.layer][x+y*mlcddev.width];
} 
//清屏函数
//color:颜色值
void mlcd_clear(u16 color)
{
	u32 i,len=0;
	u32 *pbuf=(u32*)mlcddev.grambuf[mlcddev.layer];//改为32位赋值,提高速度
	u32 color32=((u32)color<<16)|color;
	len=(u32)mlcddev.width*mlcddev.height/2;
	for(i=0;i<len;i++)
	{
		pbuf[i]=color32;
	} 
}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void mlcd_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j; 
	u16 xend=0;  
	xend=ex+1;	  
	if(lcdltdc.pwidth&&mlcddev.dir==0)//RGB竖屏 
	{ 	
		for(i=sy;i<=ey;i++)
		{  
			for(j=sx;j<xend;j++)
			{ 
				mlcddev.grambuf[mlcddev.layer][i+(mlcddev.width-j-1)*mlcddev.height]=color;
			}
		}
	}else 
	{
		for(i=sy;i<=ey;i++)
		{  
			for(j=sx;j<xend;j++)
			{ 
				mlcddev.grambuf[mlcddev.layer][j+i*mlcddev.width]=color;
			}
		}
	}	 
}  
//显示层
//lx:层编号
void mlcd_display_layer(u8 lx)
{
	LCD_Color_Fill(0,0,lcddev.width-1,lcddev.height-1,mlcddev.grambuf[lx]);
}









































