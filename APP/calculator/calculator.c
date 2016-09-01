#include "calculator.h"
#include "math.h"
#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-��ѧ������ ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

#define CALC_DISP_COLOR  	0X0000	//��ʾ���ֵ���ɫ
#define CALC_DISP_BKCOLOR  	0XA5F4	//��ʾ����ı���ɫ
#define CALC_MAX_EXP		200		//������ָ����Χ,�����趨Ϊ200,����ڴ湻�Ļ�,�������õ�300����.
									//����Ҫ���Ǹ���������ֵ��Χ
//��ֵ
#define CALC_PI 			3.1415926535897932384626433832795 	   
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//��������Ҫ�õ���ȫ������
//��Щ���ݱ������Զ���Ϊ�ֲ�����,��������δ֪ԭ��(���ɶ�ջ����),�ڱ���������,���붨��Ϊȫ�ֱ���.
//����������!!!
static double calc_x1=0,calc_x2=0;	//2��������
static double calc_temp=0;		   	//��ʱ����
u8 *outbuf;					//���������
u8 *tempbuf;				//��ʱ���ݴ洢��
_calcdis_obj *cdis;			//��������ʾ�ṹ��


//���ؼ�����������UI	
//calcdis:��ʾ�ṹ��  
void calc_load_ui(_calcdis_obj*calcdis)
{	
 	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[15][gui_phy.language],0X05);//��ʾ����  
  	gui_fill_rectangle(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight,BLACK);				//����ɫ 		  
  	gui_fill_rectangle(calcdis->xoff,gui_phy.tbheight+calcdis->yoff,calcdis->width,calcdis->height,CALC_DISP_BKCOLOR);//��ʾ�����ɫ���	  
} 

//��ʾ����������������
//calcdis:��ʾ�ṹ��
//buf:�ַ�����ַ
void calc_show_inbuf(_calcdis_obj*calcdis,u8 *buf)
{
	u16 xoff;
 	u8 len=strlen((const char*)buf);//�õ��ַ�������
   	xoff=calcdis->xoff+calcdis->xdis/2+calcdis->fsize/2; 
	gui_fill_rectangle(xoff,gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,13*(calcdis->fsize/2),calcdis->fsize,CALC_DISP_BKCOLOR);//���֮ǰ����ʾ	  
	gui_phy.back_color=CALC_DISP_BKCOLOR;
	gui_show_string(buf,xoff+13*(calcdis->fsize/2)-len*(calcdis->fsize/2),gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,lcddev.width,lcddev.height,calcdis->fsize,BLACK);//��ʾ�ַ���
}	 				
//��������������
//calcdis:��ʾ�ṹ��
//x1,x2:����������
//buf:�������ݴ洢��
//ctype:���㷨��
//calc_sta:������״̬
//����ֵ:0,û�и�����ʾ;1,�Ѿ�������ʾ;
u8 calc_exe(_calcdis_obj*calcdis,double *x1,double *x2,u8 *buf,u8 ctype,u8 *calc_sta)
{
	u8 inlen=0;
	u8 res=0;
	inlen=(*calc_sta)&0X0F;//�õ����볤��
	if((*calc_sta&0X80)==0)	//��û�в�����
	{	  
		*x1=atof((const char *)buf);//ת��Ϊ����
		if((*calc_sta&0X40))
		{
			*x1=-*x1;
			*calc_sta&=~(0X40);//����λ��Ϊ��   
 		}
		*calc_sta|=0X80; 		//����Ѿ���һ����������.
	}else if(inlen)				//����������
	{	 	
		*x2=atof((const char *)buf);	//ת��Ϊ����
		if((*calc_sta&0X40))*x2=-*x2; 	//������Ǹ���
 		switch(ctype)
		{
			case 1://�ӷ�
				*x1=*x1+*x2;
				break;
			case 2://����
				*x1=*x1-*x2;
				break;
			case 3://�˷�
				*x1=*x1*(*x2);
				break;
			case 4://����
				*x1=*x1/(*x2);
				break;
			case 5://x^y�η�
				*x1=pow(*x1,*x2);
				break;
			case 0://û���κ������
				*x1=*x2;
 				break;
		}	 
		res=1;//��Ҫ������ʾ
	}
	if(ctype>5)//������������
	{
		switch(ctype)
		{
			case 6://sin����
				*x1=sin((*x1*CALC_PI)/180);//ת��Ϊ�Ƕȼ�����
				break;
			case 7://cos����
				*x1=cos((*x1*CALC_PI)/180);//ת��Ϊ�Ƕȼ�����
				break;
			case 8://tan����
				*x1=tan((*x1*CALC_PI)/180);//ת��Ϊ�Ƕȼ�����
				break;
			case 9://log����
				*x1=log10(*x1);
				break;
			case 10://ln����
				*x1=log(*x1);
				break;
			case 11://x^2����
				*x1=*x1*(*x1);
				break;
			case 12://��������
				*x1=sqrt(*x1);
				break;
			case 13://��������
				*x1=1/(*x1);
				break;	   	    
		}					 
		res=1;//��Ҫ������ʾ
	}
	if(res)//������ʾ
	{
		*calc_sta&=~(0X40);				//����λ��Ϊ��
 		if(calc_show_res(calcdis,*x1))	//��ʾ�д���,�������
		{
			*calc_sta=0;
			*x1=0;
			*x2=0;
			gui_memset(buf,0,14); 	//���뻺������
 		}	    
	}
	*calc_sta&=0XF0;  		//������볤��
	gui_memset(buf,0,14); 	//���뻺������
 	return res;
}
//��ʾָ�� 
//calcdis:��ʾ�ṹ��
//exp:ָ��ֵ
void calc_show_exp(_calcdis_obj*calcdis,short exp)			   
{	 
	u16 xoff,yoff; 
	xoff=calcdis->xoff+calcdis->xdis+(calcdis->fsize/2)*14;
	yoff=gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12+calcdis->fsize-28-calcdis->fsize/7;
	gui_fill_rectangle(xoff,yoff,32,28,CALC_DISP_BKCOLOR);//���֮ǰ����ʾ	  
 	if(exp!=0) 
	{ 
 		if(exp<0)
		{
			exp=-exp;
 			gui_show_ptchar(xoff,yoff,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,16,'-',1);	//��ʾ����
		}
		gui_show_num(xoff+8,yoff,3,CALC_DISP_COLOR,16,exp,0X81);							//�����ʾָ��
		gui_show_ptchar(xoff,yoff+16,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,12,'X',1);	//���ӷ�ʽ��ʾ�˺�
		gui_show_num(xoff+6,yoff+16,2,CALC_DISP_COLOR,12,10,0X81);						//�����ʾ����
 	}
}
//��ʾ������
//calcdis:��ʾ�ṹ��
//fg:0,ȥ������;1,��ʾ����.
void calc_show_flag(_calcdis_obj*calcdis,u8 fg)
{ 
	gui_phy.back_color=CALC_DISP_BKCOLOR;
	if(fg==0)gui_show_ptchar(calcdis->xoff+calcdis->xdis/2,gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,lcddev.width,lcddev.height,0,BLACK,cdis->fsize,' ',0);//ȥ������
	else gui_show_ptchar(calcdis->xoff+calcdis->xdis/2,gui_phy.tbheight+calcdis->yoff+calcdis->ydis+12,lcddev.width,lcddev.height,0,BLACK,cdis->fsize,'-',0);	//��ʾ����
}  
//����������ʾ
//calcdis:��ʾ�ṹ��
//calc_sta:������״̬
//inbuf:��������
//len:���볤��
void calc_input_fresh(_calcdis_obj*calcdis,u8* calc_sta,u8 *inbuf,u8 len)
{	     
	*calc_sta&=0XF0;
	*calc_sta|=len&0X0F;						//���²���������
	calc_show_flag(calcdis,(*calc_sta)&0X40);	//���Ŵ���
	calc_show_inbuf(calcdis,inbuf);  			//��ʾ��������
 	calc_show_exp(calcdis,0);					//��ʾָ������	 
} 
//��ʾ���㷽��
//calcdis:��ʾ�ṹ��
//ctype:���㷽��
void calc_ctype_show(_calcdis_obj*calcdis,u8 ctype)
{
	u8 *chx;
	u16 xoff,yoff;
	xoff=calcdis->xoff+calcdis->xdis*2+calcdis->fsize/2;
	yoff=gui_phy.tbheight+calcdis->yoff+calcdis->ydis/2;
	switch(ctype)
	{			    
		case 1:	//�ӷ�
			chx="��";
			break;
		case 2:	//����
			chx="��";
			break;
		case 3:	//�˷�
			chx="��";
			break;
		case 4:	//����
			chx="��";
			break;
		case 5://x^y�η�
			chx="x^y";
			break;		  
		case 6://sin
			chx="sin";
			break;
		case 7://cos
			chx="cos";
			break;
		case 8://tan
			chx="tan";
			break;
		case 9://log
			chx="log";
			break;
		case 10://ln
			chx="ln";
			break;
		case 11://x^2
			chx="x^2";
			break;
		case 12://
			chx="�̣�";
			break;
		case 13://1/x
			chx="1/x";
			break;	    
		default:
			chx="";
			break;
	}
	gui_fill_rectangle(xoff,yoff,6*4,12,CALC_DISP_BKCOLOR);//���֮ǰ����ʾ	  
	gui_show_ptstr(xoff,yoff,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,12,chx,1);//��ʾ���㷽��				 
}


//��ʾ���
//calcdis:��ʾ�ṹ��
//res:���
//����ֵ:0,�޴���;1,�д���
u8 calc_show_res(_calcdis_obj*calcdis,double res)
{									
	signed short exp=0;
	u8 temp=13;
	u8 i;
	if(res<0)
	{
		res=-res;									    
		calc_show_flag(calcdis,1);		//��ʾ����
	}else calc_show_flag(calcdis,0);	//ȡ������		
	gui_memset(outbuf,0,20);
	gui_memset(tempbuf,0,14);
  	sprintf((char*)outbuf,"%0.11e",res);//��������outbuf����
   	outbuf[temp]=0;
  	exp=(signed short)atof((const char*)(outbuf+temp+1));		//�õ����ݵ�ָ������.
	if((outbuf[0]=='i')||(outbuf[0]=='n')||exp>CALC_MAX_EXP)	//����Ľ��
	{
		calc_show_flag(calcdis,0);	//ȡ������
 		outbuf[0]='E';
		outbuf[1]=0;
		calc_show_inbuf(calcdis,outbuf);//��������LCD
 		calc_show_exp(calcdis,0);	 
		return 1;
 	}	  
 	if(exp>-5&&exp<12)
	{
		if(exp<0)
		{
			for(i=0;i<(-exp+1);i++)
			{	   
				if(i==1)tempbuf[i]='.';
				else tempbuf[i]='0';	
			}
			exp+=12;
			for(i=0;i<exp;i++)
			{	   
				if(i==0)tempbuf[12+1-exp+i]=outbuf[i];	//����	
				else tempbuf[12+1-exp+i]=outbuf[i+1];		//����	   	
			}   
			tempbuf[13]=0;//ĩβ��ӽ�����
			outbuf[0]=0;
			strcpy((char*)outbuf,(char*)tempbuf);//����tempbuf�����ݵ�outbuf.
		}else//exp>=0
		{
			for(i=0;i<exp;i++)
			{	   
				temp=outbuf[2+i];
			 	outbuf[1+i]=temp;
				outbuf[2+i]='.';	
			}   									  
		}
		exp=0;//����Ҫ��ʾexp	 					    
 	}
	for(i=12;i>0;i--)//�Ѷ����0ȥ��
	{
		if(outbuf[i]=='0')outbuf[i]=0;
		else if(outbuf[i]=='.')
		{
			outbuf[i]=0;
			break;
		}else break;
	}	
	calc_show_inbuf(calcdis,outbuf);//��������LCD
	calc_show_exp(calcdis,exp);	 
 	return 0;   
}
//��ʽ����ʾ
//calcdis:��ʾ�ṹ��
//res:Ҫת��������,���֧��0XFFFF
//fmt:0:ʮ����
//    1:ʮ������
//    2:������
void calc_fmt_show(_calcdis_obj*calcdis,u16 res,u8 fmt)
{
	u8 *fmtstr="";
	u8 outbuf[17];
	u8 i=0;
	u8 len;
	u16 xoff,yoff;
	xoff=calcdis->xoff+calcdis->xdis*2+calcdis->fsize/2;
	yoff=gui_phy.tbheight+calcdis->yoff+calcdis->ydis;
	gui_memset(outbuf,0,17);
  	gui_fill_rectangle(calcdis->xoff,gui_phy.tbheight+calcdis->yoff,calcdis->width,calcdis->height,CALC_DISP_BKCOLOR);	//������ʾ�����ɫ���	  
	switch(fmt)
	{
		case 0://ʮ����
			fmtstr="DEG"; 
			sprintf((char*)outbuf,"%d",res);	//��������outbuf����	  
			break;
		case 1://ʮ������ 
			fmtstr="HEX";
			sprintf((char*)outbuf,"%X",res);	//��������outbuf����	  
			calc_show_inbuf(calcdis,outbuf);	//��������LCD
 			break;
  		case 2://������
			fmtstr="BIN"; 
 		  	for(i=0;i<16;i++)
			{
				outbuf[15-i]='0'+(res&0x01);
				res>>=1;
			}
			for(i=0;i<16;i++)
			{
				if(outbuf[i]=='0')outbuf[i]=0;
				else break;
			}   
			len=strlen((const char*)(outbuf+i));//�õ��ַ�������
			gui_phy.back_color=CALC_DISP_BKCOLOR;
			gui_show_string(outbuf+i,calcdis->xoff+calcdis->xdis+(calcdis->fsize/2)*16-len*(calcdis->fsize/2),yoff+12,lcddev.width,lcddev.height,calcdis->fsize,BLACK);//��ʾ�ַ���	
  			break;	 
	}    
	if(fmt<2)calc_show_inbuf(calcdis,outbuf);	//ʮ���ƺ�16����,��������LCD
	gui_show_ptstr(xoff+40,yoff-calcdis->ydis/2,gui_phy.lcdwidth,gui_phy.lcdheight,0,CALC_DISP_COLOR,12,fmtstr,1);//��ʾ��ǰ��ʽ				 
}
u8*const calc_btnstr_tbl[29]=
{   						   
"sin","cos","tan","log","ln",
"x^2","x^y","sqrt","1/x","FMT",
"7","8","9","DEL","AC",
"4","5","6","��","��",
"1","2","3","��","��",
"0","+/-",".","=",	
};
	  
//������������
u8 calc_play(void)
{
	_btn_obj* c_btn[29];
	u8 mbtnysize,fbtnysize;
	u8 btnxsize; 
	u8 btnxdis,btnydis;
	
	u8 *inbuf;		//�������12���ַ�+������,�ܹ�13�� 
	u8 calc_sta=0;		//������״̬
						//[7]:0,��û�в�����;1,�Ѿ���һ����������
						//[6]:����λ;0,����;1,����;
						//[5]:0,������2δ����;1,������2�ѱ���;
						//[3:0]:�������������
	
	u8 ctype=0;			//���㷨��
						//0,û�м��㷨��
						//1,�ӷ�.
						//2,����
						//3,�˷�
						//4,����
						//5,x^y�η�.
						//6,sin����
						//7,cos����
						//8,tan����
						//9,log����
						//10,ln����
						//11,x^2����
						//12,����
						//13,1/x����
						//14,��ʽת��
 	u8 maxlen=12;		//��������ַ���.Ĭ��12��,��������С�����Ժ�,��Ϊ13��.
	   
 	u8 i,j;
	u8 rval=0;
	u8 res;
	u8 ttype=0;			//��һ�β����������
	u8 cfmt=0;			//��ʽ,Ĭ��Ϊʮ���Ƹ�ʽ 

	FIL* f_calc=0;	  
	inbuf=(u8*)gui_memin_malloc(14);	//���뻺����
	outbuf=(u8*)gui_memin_malloc(20);	//��������� 
	tempbuf=(u8*)gui_memin_malloc(14);	//��ʱ�洢�� 
	cdis=gui_memin_malloc(sizeof(_calcdis_obj));
  	if(!inbuf||!outbuf||!tempbuf||!cdis)rval=1;//����ʧ�� 
   	f_calc=(FIL *)gui_memin_malloc(sizeof(FIL));//����FIL�ֽڵ��ڴ����� 
	if(f_calc==NULL)rval=1;		//����ʧ��
	else
	{
		if(lcddev.width<=272)
		{
			res=f_open(f_calc,(const TCHAR*)APP_ASCII_2814,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_2814=(u8*)gui_memex_malloc(f_calc->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_2814==0)rval=1;
				else res=f_read(f_calc,asc2_2814,f_calc->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
			} 
		}else if(lcddev.width==320)
		{
			res=f_open(f_calc,(const TCHAR*)APP_ASCII_3618,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_3618=(u8*)gui_memex_malloc(f_calc->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_3618==0)rval=1;
				else res=f_read(f_calc,asc2_3618,f_calc->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
			} 
		}else if(lcddev.width>=480)
		{
			res=f_open(f_calc,(const TCHAR*)APP_ASCII_5427,FA_READ);//���ļ� 
			if(res==FR_OK)
			{
				asc2_5427=(u8*)gui_memex_malloc(f_calc->obj.objsize);	//Ϊ�����忪�ٻ����ַ
				if(asc2_5427==0)rval=1;
				else res=f_read(f_calc,asc2_5427,f_calc->obj.objsize,(UINT*)&br);	//һ�ζ�ȡ�����ļ�
			}  
		} 
		if(res)rval=res;
	} 	
	if(rval==0)
	{
		if(lcddev.width==240)
		{ 
			cdis->yoff=10;		//��ʾ������Y�����ƫ����(�������������ĸ߶�)
			cdis->width=236;	//����С��LCD�Ŀ���Ҵ���8*cdis->fsize
			cdis->height=50;	//�������cdis->fsize+12+8  
			cdis->fsize=28; 	//��ʾ����
			btnxsize=44;		//�������
			mbtnysize=30;		//�������߶�
			fbtnysize=20;		//���ܰ����߶� 
			btnydis=8;
		}else if(lcddev.width==272)
		{ 
			cdis->yoff=20;
			cdis->width=260;
			cdis->height=60; 
			cdis->fsize=28; 
			btnxsize=50;
			mbtnysize=40;
			fbtnysize=30; 
			btnydis=20;
		}else if(lcddev.width==320)
		{ 
			cdis->yoff=20;
			cdis->width=300;
			cdis->height=70; 
			cdis->fsize=36; 
			btnxsize=60;
			mbtnysize=40;
			fbtnysize=30; 
			btnydis=18;
		}else if(lcddev.width==480)
		{ 
			cdis->yoff=50;
			cdis->width=460;
			cdis->height=110; 
			cdis->fsize=54; 
			btnxsize=90;
			mbtnysize=60;
			fbtnysize=40; 
			btnydis=35;
		}else if(lcddev.width==600)
		{ 
			cdis->yoff=80;
			cdis->width=540;
			cdis->height=130; 
			cdis->fsize=54; 
			btnxsize=110;
			mbtnysize=80;
			fbtnysize=50; 
			btnydis=40; 
		}
		cdis->xoff=(lcddev.width-cdis->width)/2;		
		cdis->xdis=(cdis->width-16*cdis->fsize/2)/2;
		cdis->ydis=(cdis->height-cdis->fsize-12)/2;
		btnxdis=(lcddev.width-5*btnxsize)/5;
		
		
		for(i=0;i<6;i++)//����29����ť
		{
			for(j=0;j<5;j++)
			{
				res=i*5+j;
				if(i<2)
				{
					c_btn[res]=btn_creat(j*(btnxsize+btnxdis)+btnxdis/2,i*(mbtnysize+btnydis)+gui_phy.tbheight+cdis->yoff*2+cdis->height+(mbtnysize-fbtnysize)/2,btnxsize,fbtnysize,0,2);//����Բ�ǰ�ť
					c_btn[res]->bkctbl[0]=BLACK;//�߿���ɫ
					c_btn[res]->bkctbl[1]=0X8410;//��һ�е���ɫ				
					c_btn[res]->bkctbl[2]=0X8410;//�ϰ벿����ɫ
					c_btn[res]->bkctbl[3]=0X630C;//�°벿����ɫ	 
				}else if(i==5&&j==3)
				{
					c_btn[res]=btn_creat(j*(btnxsize+btnxdis)+btnxdis/2,i*(mbtnysize+btnydis)+gui_phy.tbheight+cdis->yoff*2+cdis->height,btnxsize*2+btnxdis,mbtnysize,0,2);//����Բ�ǰ�ť
					c_btn[res]->bkctbl[0]=BLACK;//�߿���ɫ
					c_btn[res]->bkctbl[1]=0X4A49;//��һ�е���ɫ				
					c_btn[res]->bkctbl[2]=0X4A49;//�ϰ벿����ɫ
					c_btn[res]->bkctbl[3]=0X3186;//�°벿����ɫ	 
					j=4;
				}else 
				{
					c_btn[res]=btn_creat(j*(btnxsize+btnxdis)+btnxdis/2,i*(mbtnysize+btnydis)+gui_phy.tbheight+cdis->yoff*2+cdis->height,btnxsize,mbtnysize,0,2);//����Բ�ǰ�ť
					if(i==2&&(j==3||j==4))//DEL.AC��ť,ȫ��ɫ
					{
						c_btn[res]->bkctbl[0]=BLACK;//�߿���ɫ
						c_btn[res]->bkctbl[1]=RED;//��һ�е���ɫ				
						c_btn[res]->bkctbl[2]=RED;//�ϰ벿����ɫ
						c_btn[res]->bkctbl[3]=RED;//�°벿����ɫ	 
					}else
					{
						c_btn[res]->bkctbl[0]=BLACK;//�߿���ɫ
						c_btn[res]->bkctbl[1]=0X4A49;//��һ�е���ɫ				
						c_btn[res]->bkctbl[2]=0X4A49;//�ϰ벿����ɫ
						c_btn[res]->bkctbl[3]=0X3186;//�°벿����ɫ	 
					}
				}
				if(i<2)//���ܰ���
				{
					if(lcddev.width==240)c_btn[res]->font=12;
					else c_btn[res]->font=16;			
					c_btn[res]->bcfucolor=BLACK;//�ɿ�ʱΪ��ɫ
					c_btn[res]->bcfdcolor=WHITE;//����ʱΪ��ɫ
				}else	//������
				{
					if(lcddev.width==240)c_btn[res]->font=16;
					else c_btn[res]->font=24;			
					c_btn[res]->bcfucolor=WHITE;//�ɿ�ʱΪ��ɫ
					c_btn[res]->bcfdcolor=BLACK;//����ʱΪ��ɫ
				}
				if(c_btn[res]==NULL){rval=1;break;}//����ʧ��. 
				c_btn[res]->caption=(u8*)calc_btnstr_tbl[res]; 
				c_btn[res]->sta=0;
			}	 
		}
	}
	if(rval==0)//׼�����������
	{	   
		calc_load_ui(cdis);					//����������
		calc_fmt_show(cdis,calc_x1,cfmt);	//��ʾ��ʽ��
		for(i=0;i<29;i++)btn_draw(c_btn[i]);
		gui_memset(inbuf,0,14);				//���뻺������
		calc_show_inbuf(cdis,"0");			//��ʾ0
		while(1)
		{
			tp_dev.scan(0);    
			in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
			delay_ms(5);							//��ʱ2��ʱ�ӽ���
			if(system_task_return)break;	  		//TPAD����
 
	 		for(i=0;i<29;i++)
			{
				res=btn_check(c_btn[i],&in_obj);   
				if(res&&((c_btn[i]->sta&(1<<7))==0)&&(c_btn[i]->sta&(1<<6)))//�а����������ɿ�,����TP�ɿ���
				{	
					res=calc_sta&0X0F;		//�õ����볤��
					if((i!=9)&&(ctype==14))	//���Ǹ�ʽ����ť,����һ�β���ʱ��ʽת��
					{
						cfmt=0;
						calc_x1=0;
						calc_x2=0;
						calc_sta=0;
						calc_temp=0;
						ttype=1;	//ģ��һ������						  
						ctype=0;	   
						calc_fmt_show(cdis,calc_x1,cfmt);
					}
 					switch(i)
					{
						case 0://sin����
						case 1://cos����
						case 2://tan����
						case 3://log����
						case 4://ln����
						case 5://x^2����   
						case 7://sqrt����   
						case 8://��������   
							if(i<6)ctype=6+i; 
							else ctype=5+i;   
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//���㴦��  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);	 	//���뻺������
							maxlen=12;
							calc_ctype_show(cdis,ctype);		//��ʾ��εĲ�����
							ctype=0;
							ttype=0;
							break;		 
						case 6://x^y����
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//���㴦��  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//���뻺������
							maxlen=12;
							ctype=5;   
							break; 	  
						case 9://��ʽת��
							cfmt++;
							if(cfmt>2)cfmt=0;
						 	if((calc_sta&0X80)==0)
							{
								calc_x1=atof((const char *)inbuf);	//ת��Ϊ����	  
								calc_sta|=0X80; 				//����Ѿ���һ��������.
							} 
							calc_sta=0XF0;
							gui_memset(inbuf,0,14);				//���뻺������
							maxlen=12;	
							ctype=14;//���Ϊ��ʽת��					   	
							calc_fmt_show(cdis,calc_x1,cfmt);
							break; 
						case 10://����7
						case 11://����8
						case 12://����9
							if(res==1&&inbuf[0]=='0')res=0;					//��һ�����ݲ���Ϊ0 
							if(res<maxlen)inbuf[res++]='7'+i-10;			//����7/8/9
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//������ʾ
							break;	    
						case 13://�˸�
							if(res>1)
							{	       
								res--;
								if(inbuf[res]=='.')maxlen=12;//�ָ�12�����ݳ���
								inbuf[res]=0;
  							}else if(inbuf[0]!='0')inbuf[0]='0';//���һ����������Ϊ0
 							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//������ʾ  
							break;
						case 14://����
							maxlen=12;
							gui_memset(inbuf,0,14);//���뻺������
							calc_sta=0;
							calc_show_inbuf(cdis,"0");	//��ʾ0
							calc_show_flag(cdis,calc_sta&0X40);	//����	  
						  	calc_show_exp(cdis,0);				//ȥ��ָ������	
							calc_x1=0;
							calc_x2=0;
							calc_temp=0;
							ttype=1;	//ģ��һ������						  
							ctype=0;
							break;
						case 15://����4
						case 16://����5
						case 17://����6
							if(res==1&&inbuf[0]=='0')res=0;					//��һ�����ݲ���Ϊ0 
							if(res<maxlen)inbuf[res++]='4'+i-15;			//����4/5/6
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//������ʾ
							break;
						case 18://�ӷ�				 
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//���㴦��  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//���뻺������
							maxlen=12;
							ctype=1;   
							break;
						case 19://����
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//���㴦��  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//���뻺������
							maxlen=12;
							ctype=2;   
							break; 	 
						case 20://����1
						case 21://����2
						case 22://����3
							if(res==1&&inbuf[0]=='0')res=0;					//��һ�����ݲ���Ϊ0 
							if(res<maxlen)inbuf[res++]='1'+i-20;			//����1/2/3
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//������ʾ
							break;	  
						case 23://�˷�	   
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//���㴦��  
 							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//���뻺������
							maxlen=12;
							ctype=3;   
							break;
						case 24://����	    
						 	calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta);//���㴦��  
							calc_sta&=0XF0;
							gui_memset(inbuf,0,14);				//���뻺������
							maxlen=12;
							ctype=4;   
							break; 	 
						case 25://����0
							if(res!=1||inbuf[0]!='0')
							{
								if(res<maxlen)inbuf[res++]='0';//����0
							} 
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//������ʾ  
							break;
 						case 26://���ŷ�ת						
							if(calc_sta&(1<<6))calc_sta&=~(1<<6);	 
							else calc_sta|=1<<6;	   
							calc_show_flag(cdis,calc_sta&0X40);	//���Ŵ���
							break;
						case 27://����С����
							if(maxlen==12)
							{
								if(res<maxlen)
								{
									if(res==0)inbuf[res++]='0';//������.	  
									inbuf[res++]='.';//����.
									maxlen=13;
								}
							}
							calc_input_fresh(cdis,&calc_sta,inbuf,res);	//������ʾ  
							break;
						case 28://������	   
							if(calc_sta&0X80)//�Ѿ���һ����������
							{
								if(res==0)		//û������������
								{
									if((calc_sta&0X10)==0)//������2��δ����
									{
										calc_sta|=0X10;	//����Ѿ�������
										calc_temp=calc_x1;		//������2�����ڲ�����1
									} 
								}else calc_sta&=~0X10;	//ȡ�����������2��־ 
							} 
							if(calc_exe(cdis,&calc_x1,&calc_x2,inbuf,ctype,&calc_sta)==0)		//����ִ�м���?
							{															//��
								if(calc_sta&0X10)//�в�����2
								{
									switch(ctype)
									{
										case 1://�ӷ�
											calc_x1=calc_x1+calc_temp;
											break;
										case 2://����
											calc_x1=calc_x1-calc_temp;
											break;
										case 3://�˷�
											calc_x1=calc_x1*calc_temp;
											break;
										case 4://����
											calc_x1=calc_x1/calc_temp;
											break;
									}							
								}
						 		if(calc_show_res(cdis,calc_x1))	//��ʾ�д���,�������
								{
									calc_sta=0;
									calc_x1=0;
									calc_x2=0;
									gui_memset(inbuf,0,14); 	//���뻺������
						 		}									   
							}
							if((calc_sta&0X10)==0)//������2��δ����
							{
								calc_sta|=0X10;	//����Ѿ�������
								calc_temp=calc_x2;		//���������2
							} 	 				   
							break;	  
					}
					//printf("key:%d pressed!\r\n",i);
				}
				if(ctype!=ttype)//���������
				{
					ttype=ctype;
					calc_ctype_show(cdis,ctype);
				}
			}
		}
	}			 
   	gui_memin_free(inbuf);	//�ͷ��ڴ�
   	gui_memin_free(outbuf);	//�ͷ��ڴ�
   	gui_memin_free(tempbuf);//�ͷ��ڴ�
 	gui_memin_free(f_calc);	//�ͷ��ڴ� 
	gui_memex_free(cdis);	//�ͷ��ڴ�
	if(lcddev.width==240)
	{
		gui_memex_free(asc2_2814);
		asc2_2814=0;
	}else if(lcddev.width==320)
	{
		gui_memex_free(asc2_3618);
		asc2_3618=0;
	}else if(lcddev.width==480)
	{
		gui_memex_free(asc2_5427);
		asc2_5427=0;
	} 
	for(i=0;i<29;i++)
	{
		btn_delete(c_btn[i]);//ɾ����Щ����
	}
	return 0;
}


























