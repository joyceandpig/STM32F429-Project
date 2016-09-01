#include "lyric.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//�����ʾʵ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//�Ѻ�׺����Ϊsfx
//name:�ļ���(����Ϊx.x�ṹ)
//sfx:��׺
void lrc_chg_suffix(u8*name,u8*sfx)
{		    	     
	while(*name!='\0')name++;	//�ҵ���β
	while(*name!='.')name--;	//�ҵ�'.'	  
	*(++name)=sfx[0];
	*(++name)=sfx[1];
	*(++name)=sfx[2];
	*(++name)='\0';//��ӽ�����
}

//������ʽṹ��
_lyric_obj* lrc_creat(void)
{
 	_lyric_obj * lrc_crt;							 	  
	lrc_crt=(_lyric_obj*)gui_memin_malloc(sizeof(_lyric_obj));//�����ڴ�
	if(lrc_crt==NULL)return NULL;//�ڴ���䲻��.	 
 	gui_memset((u8 *)lrc_crt,0,sizeof(_lyric_obj));			//����	   
	lrc_crt->flrc=(FIL*)gui_memin_malloc(sizeof(FIL));//�����ڴ�
	if(lrc_crt->flrc==NULL)//�ڴ���䲻��.
	{
		gui_memin_free(lrc_crt);//ɾ��ǰ�������
	}
	gui_memset((u8 *)lrc_crt->flrc,0,sizeof(FIL));//����	 
	return lrc_crt; 
}	   
//�ͷ�LRC��������ڴ�
void lrc_delete(_lyric_obj* lcrdelete)
{								 
	gui_memin_free(lcrdelete->flrc);//ɾ���ļ��ṹ��		  
 	gui_memin_free(lcrdelete);
} 

//��2���ַ�תΪ����
//str:�ַ���(��':'/'.'/'\0'����,��ֻ��������/':'/'.')		    
//����ֵ:ת����Ľ��
u8 lrc_str2num(u8 *str)
{
	u8 chr;
	if(*str==':'||*str=='.'||*str=='\0')return 0;//��һ�����ݾ���:/.,���������.
	chr=*str;
	str++;
	if(*str==':'||*str=='.'||*str=='\0')
	{
		return chr-'0';//ֻ��һ������λ
	}			 
	return (chr-'0')*10+(*str-'0');//2λ����
}
//���TAG����
//lrcx:��ʽṹ��.
//lrcpos:��ǰstring����ʼ��ַ
//str:��ǰstring.
void lrc_analyze(_lyric_obj* lrcx,u16 lrcpos,u8*str)
{
	u8 *p;		 
	u8 i=0;
	u16 lrcoffset;
	u8 sta=0;	//���λ��ʾ�Ƿ���ʱ��TAG
				//bit1:0,����TAG��;1,����TAG��
			 	//bit0:0,ʱ��TAG��ȷ;1,ʱ��TAG����.
 	u8 tagbuf[9];//tag�Ϊ9���ֽ�
	u32 pstime=0;//10����
	
	lrcoffset=0;
 	p=str;
	while(1)
	{
		if(*p=='[')sta|=1<<0;
		else if(*p==']')sta=0;
	    else if(sta==0&&*p!=' ')break;//��tag�ⷢ���˷ǿո���ַ�.
		lrcoffset++;//ͳ�Ƹ�ʿ�ʼ�ĵط�
		*p++; 
	}
	if(lrcoffset==0)return;//���û��TAG����,ֱ���˳�
 	sta=0;		   
	while(1)
	{
		if(*str=='[')
		{									   
			sta|=1<<7;//Ĭ�ϱ������һ��ʱ��ͷ
			sta|=1<<1;//�����TAG����
			i=0;
		}else if(*str==']')//�ҵ���һ��TAG�Ľ�β
		{   
			if(sta&(1<<7))//��һ���Ϸ���ʱ��TAG
			{
				tagbuf[i]='\0';//��ĩβ��ӽ�����
				p=tagbuf;
				pstime=(u32)lrc_str2num(p)*6000;//һ������6000��10ms
				while(*p!=':'&&*p!='.'&&*p!='\0')p++;//ƫ�Ƶ���һ��λ��
 			 	if(*p!=0)//��û�н���
				{
 					p++;  
					pstime+=(u32)lrc_str2num(p)*100;//һ������100��10ms	 
					while(*p!=':'&&*p!='.'&&*p!='\0')p++;//ƫ�Ƶ���һ��λ��	
					if(*p!='\0')//��Ȼû�н���
					{
 						p++;  
						pstime+=(u32)lrc_str2num(p);//����10msΪ��λ������
					}
					lrcx->time_tbl[lrcx->indexsize]=pstime;				//��¼ʱ��:��λ,10ms
					lrcx->addr_tbl[lrcx->indexsize]=lrcpos+lrcoffset;	//��¼��ַ
					lrcx->indexsize++;									//������1
				} 				  
			}
			sta=0;//��ǽ���һ�β���
			i=0;
		}else if(*str!=' ')//�ǿո�
		{
			if(sta&(1<<1)) //��TAG�ڲ�
			{
				if(sta&(1<<7))//��һ��ʱ��TAG
				{
					if((*str<='9'&&*str>='0')||*str==':'||*str=='.')
					{
						if(i>8)sta&=~(1<<7);//�����TAG,̫����
				     	else
						{
							tagbuf[i]=*str;//��¼����
							i++;
						}
					}else sta&=~(1<<7);//����ʱ��TAG  
				}
			}else break;//������TAG��ķǿո�,��ʾ������	 
		}
		if(*str=='\0')break;//�ǽ�����
		str++;	   
	}	
}
//�������
//����С�򵽵�ʱ��˳������
void lrc_sequence(_lyric_obj* lrcx)
{
	u16 i,j;
	u16 temp;
	if(lrcx->indexsize==0)return;//û������,ֱ�Ӳ�������.
	for(i=0;i<lrcx->indexsize-1; i++)//����
	{
		for(j=i+1;j<lrcx->indexsize;j++)
		{
			if(lrcx->time_tbl[i]>lrcx->time_tbl[j])//��������
			{
				temp=lrcx->time_tbl[i];
				lrcx->time_tbl[i]=lrcx->time_tbl[j];
				lrcx->time_tbl[j]=temp;

				temp=lrcx->addr_tbl[i];
				lrcx->addr_tbl[i]=lrcx->addr_tbl[j];
				lrcx->addr_tbl[j]=temp;
			}
		}
	}	
}	   
//��ȡ���
//lrcx:lrc����ṹ��
//path:·������
//name:MP3����������
//����ֵ:0,��LRC,��ȡ����.
//	  ����,����
u8 lrc_read(_lyric_obj* lrcx,u8*path,u8 *name)
{
	u8*pname;
	u16 fpos;
	u8 *p;
	u8 res=0;
    //���֮ǰ������
 	lrcx->indexsize=0;
 	lrcx->curindex=0;
	lrcx->oldostime=0;
 	lrcx->curtime=0;
   	lrcx->detatime=0;
   	lrcx->updatetime=0;
	lrcx->namelen=0;
 	lrcx->curnamepos=0;

	pname=gui_memin_malloc(strlen((const char*)path)+strlen((const char*)name)+2);//�����ڴ�
	if(pname==NULL)res=1;//����ʧ��	    
 	if(res==0)
	{
		pname=gui_path_name(pname,path,name);	//�ļ�������·��
		if(pname)
		{
			lrc_chg_suffix(pname,"lrc");			//�޸ĺ�׺Ϊ.lrc	    
			res=f_open(lrcx->flrc,(const TCHAR*)pname,FA_READ);//���ļ�
			if(res==0)
			{
				while(1)
				{
					fpos=f_tell(lrcx->flrc);//�õ�Ƭƫ��ǰ��λ��
					p=(u8*)f_gets((TCHAR*)lrcx->buf,LYRIC_BUFFER_SIZE,lrcx->flrc);//��ȡ���
					if(p==0)break;//����������
					else
					{
						lrc_analyze(lrcx,fpos,p);	
					}
 				}
				lrc_sequence(lrcx);	  
			}
		}
	}
	gui_memset((u8 *)lrcx->buf,0,LYRIC_BUFFER_SIZE);//��������������	   
	gui_memin_free(pname);
	return res;	 	  	
} 
//��ʾһ�и��
//lrcx:��ʽṹ��
//x,y,width,height:����,��height=0ʱ,����ʾ���
void lrc_show_linelrc(_lyric_obj* lrcx,u16 x,u16 y,u16 width,u16 height)
{
	u8 *p;
	f_lseek(lrcx->flrc,lrcx->addr_tbl[lrcx->curindex]);
	p=(u8*)f_gets((TCHAR*)lrcx->buf,LYRIC_BUFFER_SIZE,lrcx->flrc);	//��ȡ���
	if(p)
	{
		lrcx->namelen=strlen((const char*)p)*(lrcx->font/2);		//��������ռ����
 		//gui_phy.back_color=lrcx->bkcolor;//���ñ���ɫ
		if(height)gui_show_strmid(x,y,width,height,lrcx->color,lrcx->font,p);//��ʾ���
   		lrcx->namelen=strlen((const char*)p)*lrcx->font/2;//�õ���������ռ��ĳ���
 		lrcx->curnamepos=0;
		lrcx->detatime=0;	//ʱ������
	}					 
}

u8 lrc_test(u8 * path,u8 *name)
{
	_lyric_obj* lrcx;
	u8 i;
	u8 *p;
	u8 res=0;
	lrcx=lrc_creat();//����
	if(lrcx)
	{
		res=lrc_read(lrcx,path,name);
		if(res==0)
		{
			printf("�������:%d\r\n",lrcx->indexsize);
			for(i=0;i<lrcx->indexsize;i++)
			{
				printf("%3d ʱ��:%5d ",i,lrcx->time_tbl[i]);
				f_lseek(lrcx->flrc,lrcx->addr_tbl[i]);
				p=(u8*)f_gets((TCHAR*)lrcx->buf,LYRIC_BUFFER_SIZE,lrcx->flrc);//��ȡ���
				printf("���:%s\r\n",p);   
			}
		} 		
	}
	lrc_delete(lrcx);
	return res;
}				    



























