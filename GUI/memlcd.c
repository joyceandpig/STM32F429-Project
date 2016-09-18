#include "memlcd.h"   
#include "malloc.h"  

_mlcd_dev mlcddev;		//mem lcd �ṹ��

//��ʼ��mem lcd�豸
//width,height:mem lcd�Ŀ�Ⱥ͸߶�
//lx:Ҫ������ٸ���,1~MEMLCD_MAX_LAYER;
//����ֵ:0,�ɹ�;1,lxֵ�Ƿ�;2,�ڴ����
u8 mlcd_init(u16 width,u16 height,u8 lx)
{
	u8 i;
	u32 memsize=0;
	mlcddev.width=width;
	mlcddev.height=height;
	memsize=(u32)width*height*2;
	if(lx>MEMLCD_MAX_LAYER||lx==0)return 1;			//lx�Ƿ� 
	for(i=0;i<lx;i++)
	{
		mlcddev.grambuf[i]=mymalloc(SRAMEX,memsize);//�����ڴ�
		if(mlcddev.grambuf[i]==NULL)break;
	}
	if(i!=MEMLCD_MAX_LAYER)//����ʧ����
	{
		mlcd_delete();
		return 2;
	}
	return 0;
}
//ɾ��memlcd�豸
void mlcd_delete(void)
{	
	u8 i;
	for(i=0;i<MEMLCD_MAX_LAYER;i++)
	{
		if(mlcddev.grambuf[i])myfree(SRAMEX,mlcddev.grambuf[i]);//�ͷ��ڴ�
		mlcddev.grambuf[i]=0;
	}
	mlcddev.width=0;
	mlcddev.height=0;
	mlcddev.layer=0; 
} 
//���ò�����
//lx:����
void mlcd_set_layer(u8 lx)
{
	if(lx<MEMLCD_MAX_LAYER)mlcddev.layer=lx;//���ò�����
}
//��ָ��λ�û���.
//x,y:����
//color:��ɫ.
void mlcd_draw_point(u16 x,u16 y,u16 color)
{	   
	if(lcdltdc.pwidth&&mlcddev.dir==0)//RGB���� 
	{	
		x=mlcddev.width-x-1;	//����任
		mlcddev.grambuf[mlcddev.layer][y+x*mlcddev.height]=color;
	}else mlcddev.grambuf[mlcddev.layer][x+y*mlcddev.width]=color; 
}
//��ȡָ��λ�õ����ɫֵ
//x,y:����
//����ֵ:��ɫ
u16 mlcd_read_point(u16 x,u16 y)
{ 
	if(lcdltdc.pwidth&&mlcddev.dir==0)//RGB���� 
	{	
		x=mlcddev.width-x-1;	//����任
		return mlcddev.grambuf[mlcddev.layer][y+x*mlcddev.height];
	}else return mlcddev.grambuf[mlcddev.layer][x+y*mlcddev.width];
} 
//��������
//color:��ɫֵ
void mlcd_clear(u16 color)
{
	u32 i,len=0;
	u32 *pbuf=(u32*)mlcddev.grambuf[mlcddev.layer];//��Ϊ32λ��ֵ,����ٶ�
	u32 color32=((u32)color<<16)|color;
	len=(u32)mlcddev.width*mlcddev.height/2;
	for(i=0;i<len;i++)
	{
		pbuf[i]=color32;
	} 
}
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void mlcd_fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j; 
	u16 xend=0;  
	xend=ex+1;	  
	if(lcdltdc.pwidth&&mlcddev.dir==0)//RGB���� 
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
//��ʾ��
//lx:����
void mlcd_display_layer(u8 lx)
{
	LCD_Color_Fill(0,0,lcddev.width-1,lcddev.height-1,mlcddev.grambuf[lx]);
}









































