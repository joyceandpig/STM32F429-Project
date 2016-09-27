#include "fontupd.h"
#include "ff.h"	  
#include "w25qxx.h"   
#include "lcd.h"  
#include "string.h"
#include "malloc.h"
#include "delay.h"
#include "usart.h"
#include "debug.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32¿ª·¢°å 
//×Ö¿â¸üĞÂ Çı¶¯´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2016/1/7
//°æ±¾£ºV1.1
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved									  
//********************************************************************************
//Éı¼¶ËµÃ÷
//V1.1 20160528
//ĞŞ¸Ä²¿·Ö´úÂë,ÒÔÖ§³Öfatfs  r0.12°æ±¾
////////////////////////////////////////////////////////////////////////////////// 	

//×Ö¿âÇøÓòÕ¼ÓÃµÄ×ÜÉÈÇøÊı´óĞ¡(4¸ö×Ö¿â+unigbk±í+×Ö¿âĞÅÏ¢=6302984×Ö½Ú,Ô¼Õ¼1539¸öW25QXXÉÈÇø,Ò»¸öÉÈÇø4K×Ö½Ú)
#define FONTSECSIZE	 	1539
//×Ö¿â´æ·ÅÆğÊ¼µØÖ· 
#define FONTINFOADDR 	1024*1024*25 					//Apollo STM32¿ª·¢°åÊÇ´Ó25MµØÖ·ÒÔºó¿ªÊ¼´æ·Å×Ö¿â
														//Ç°Ãæ25M±»fatfsÕ¼ÓÃÁË.
														//25MÒÔºó½ô¸ú4¸ö×Ö¿â+UNIGBK.BIN,×Ü´óĞ¡6.01M,±»×Ö¿âÕ¼ÓÃÁË,²»ÄÜ¶¯!
														//31.01MÒÔºó,ÓÃ»§¿ÉÒÔ×ÔÓÉÊ¹ÓÃ.
														
//ÓÃÀ´±£´æ×Ö¿â»ù±¾ĞÅÏ¢£¬µØÖ·£¬´óĞ¡µÈ
_font_info ftinfo;

//×Ö¿â´æ·ÅÔÚ´ÅÅÌÖĞµÄÂ·¾¶
u8*const GBK_PATH[5]=
{
"/SYSTEM/FONT/UNIGBK.BIN",	//UNIGBK.BINµÄ´æ·ÅÎ»ÖÃ
"/SYSTEM/FONT/GBK12.FON",	//GBK12µÄ´æ·ÅÎ»ÖÃ
"/SYSTEM/FONT/GBK16.FON",	//GBK16µÄ´æ·ÅÎ»ÖÃ
"/SYSTEM/FONT/GBK24.FON",	//GBK24µÄ´æ·ÅÎ»ÖÃ
"/SYSTEM/FONT/GBK32.FON",	//GBK32µÄ´æ·ÅÎ»ÖÃ
}; 
//¸üĞÂÊ±µÄÌáÊ¾ĞÅÏ¢
u8*const UPDATE_REMIND_TBL[5]=
{
"Updating UNIGBK.BIN",	//ÌáÊ¾ÕıÔÚ¸üĞÂUNIGBK.bin
"Updating GBK12.FON ",	//ÌáÊ¾ÕıÔÚ¸üĞÂGBK12
"Updating GBK16.FON ",	//ÌáÊ¾ÕıÔÚ¸üĞÂGBK16
"Updating GBK24.FON ",	//ÌáÊ¾ÕıÔÚ¸üĞÂGBK24
"Updating GBK32.FON ",	//ÌáÊ¾ÕıÔÚ¸üĞÂGBK32
}; 

//ÏÔÊ¾µ±Ç°×ÖÌå¸üĞÂ½ø¶È
//x,y:×ø±ê
//size:×ÖÌå´óĞ¡
//fsize:Õû¸öÎÄ¼ş´óĞ¡
//pos:µ±Ç°ÎÄ¼şÖ¸ÕëÎ»ÖÃ
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(x,y,t,3,size);//ÏÔÊ¾ÊıÖµ
	}
	return 0;					    
} 
//¸üĞÂÄ³Ò»¸ö
//x,y:×ø±ê
//size:×ÖÌå´óĞ¡
//fxpath:Â·¾¶
//fx:¸üĞÂµÄÄÚÈİ 0,ungbk;1,gbk12;2,gbk16;3,gbk24;4,gbk32;
//·µ»ØÖµ:0,³É¹¦;ÆäËû,Ê§°Ü.
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;								    
	FIL * fftemp;
	u8 *tempbuf;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	u8 rval=0;	     
	fftemp=(FIL*)mymalloc(SRAMEX,sizeof(FIL));	//·ÖÅäÄÚ´æ	SRAMIN
	if(fftemp==NULL)rval=1;
	tempbuf=mymalloc(SRAMEX,4096);				//·ÖÅä4096¸ö×Ö½Ú¿Õ¼SRAMINä
	if(tempbuf==NULL)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//´ò¿ªÎÄ¼şÊ§°Ü  
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:												//¸üĞÂUNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//ĞÅÏ¢Í·Ö®ºó£¬½ô¸úUNIGBK×ª»»Âë±í
				ftinfo.ugbksize=fftemp->obj.objsize;					//UNIGBK´óĞ¡
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBKÖ®ºó£¬½ô¸úGBK12×Ö¿â
				ftinfo.gbk12size=fftemp->obj.objsize;					//GBK12×Ö¿â´óĞ¡
				flashaddr=ftinfo.f12addr;						//GBK12µÄÆğÊ¼µØÖ·
				break;
			case 2:
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12Ö®ºó£¬½ô¸úGBK16×Ö¿â
				ftinfo.gbk16size=fftemp->obj.objsize;					//GBK16×Ö¿â´óĞ¡
				flashaddr=ftinfo.f16addr;						//GBK16µÄÆğÊ¼µØÖ·
				break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16Ö®ºó£¬½ô¸úGBK24×Ö¿â
				ftinfo.gbk24size=fftemp->obj.objsize;					//GBK24×Ö¿â´óĞ¡
				flashaddr=ftinfo.f24addr;						//GBK24µÄÆğÊ¼µØÖ·
				break;
			case 4:
				ftinfo.f32addr=ftinfo.f24addr+ftinfo.gbk24size;	//GBK24Ö®ºó£¬½ô¸úGBK32×Ö¿â
				ftinfo.gbk32size=fftemp->obj.objsize;					//GBK32×Ö¿â´óĞ¡
				flashaddr=ftinfo.f32addr;						//GBK32µÄÆğÊ¼µØÖ·
				break;
		} 
			
		while(res==FR_OK)//ËÀÑ­»·Ö´ĞĞ
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//¶ÁÈ¡Êı¾İ	 
			if(res!=FR_OK)break;								//Ö´ĞĞ´íÎó
			W25QXX_Write(tempbuf,offx+flashaddr,4096);		//´Ó0¿ªÊ¼Ğ´Èë4096¸öÊı¾İ  
	  		offx+=bread;	  
			fupd_prog(x,y,size,fftemp->obj.objsize,offx);	 			//½ø¶ÈÏÔÊ¾
			if(bread!=4096)break;								//¶ÁÍêÁË.
	 	} 	
		f_close(fftemp);		
	}			 
	myfree(SRAMEX,fftemp);	//ÊÍ·ÅÄÚ´æSRAMIN
	myfree(SRAMEX,tempbuf);	//ÊÍ·ÅÄÚ´æSRAMIN
	return res;
} 
//¸üĞÂ×ÖÌåÎÄ¼ş,UNIGBK,GBK12,GBK16,GBK24,GBK32Ò»Æğ¸üĞÂ
//x,y:ÌáÊ¾ĞÅÏ¢µÄÏÔÊ¾µØÖ·
//size:×ÖÌå´óĞ¡
//src:×Ö¿âÀ´Ô´´ÅÅÌ."0:",SD¿¨;"1:",FLASHÅÌ,"2:"NANDÅÌ,"3:",UÅÌ.
//ÌáÊ¾ĞÅÏ¢×ÖÌå´óĞ¡										  
//·µ»ØÖµ:0,¸üĞÂ³É¹¦;
//		 ÆäËû,´íÎó´úÂë.	  
u8 update_font(u16 x,u16 y,u8 size,u8* src)
{	
	u8 *pname;
	u32 *buf;
	u8 res=0;		   
 	u16 i,j;
	FIL *fftemp;
	u8 rval=0; 
	res=0XFF;		
	ftinfo.fontok=0XFF;
	pname=mymalloc(SRAMEX,100);	//ÉêÇë100×Ö½ÚÄÚ´æ  SRAMIN
	buf=mymalloc(SRAMEX,4096);	//ÉêÇë4K×Ö½ÚÄÚ´æ SRAMIN 
	fftemp=(FIL*)mymalloc(SRAMEX,sizeof(FIL));	//·ÖÅäÄÚ´  SRAMIN
	if(buf==NULL||pname==NULL||fftemp==NULL)
	{
		myfree(SRAMEX,fftemp);//SRAMIN
		myfree(SRAMEX,pname);//SRAMIN
		myfree(SRAMEX,buf);//SRAMIN
		return 5;		//ÄÚ´æÉêÇëÊ§°Ü
	}
	for(i=0;i<5;i++)	//ÏÈ²éÕÒÎÄ¼şUNIGBK,GBK12,GBK16,GBK24,GBK32ÊÇ·ñÕı³£ 
	{ 
		strcpy((char*)pname,(char*)src);				//copy srcÄÚÈİµ½pname
		strcat((char*)pname,(char*)GBK_PATH[i]);		//×·¼Ó¾ßÌåÎÄ¼şÂ·¾¶ 
		res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//³¢ÊÔ´ò¿ª
		if(res)
		{
			rval|=1<<7;	//±ê¼Ç´ò¿ªÎÄ¼şÊ§°Ü  
			break;		//³ö´íÁË,Ö±½ÓÍË³ö
		}
	} 
	myfree(SRAMEX,fftemp);	//ÊÍ·ÅÄÚ´SRAMINæ
	if(rval==0)				//×Ö¿âÎÄ¼ş¶¼´æÔÚ.
	{  
		LCD_ShowString(x,y,240,320,size,"Erasing sectors... ");//ÌáÊ¾ÕıÔÚ²Á³ıÉÈÇø	
		for(i=0;i<FONTSECSIZE;i++)			//ÏÈ²Á³ı×Ö¿âÇøÓò,Ìá¸ßĞ´ÈëËÙ¶È
		{
			fupd_prog(x+20*size/2,y,size,FONTSECSIZE,i);//½ø¶ÈÏÔÊ¾
			W25QXX_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);//¶Á³öÕû¸öÉÈÇøµÄÄÚÈİ
			for(j=0;j<1024;j++)//Ğ£ÑéÊı¾İ
			{
				if(buf[j]!=0XFFFFFFFF)break;//ĞèÒª²Á³ı  	  
			}
			if(j!=1024)W25QXX_Erase_Sector((FONTINFOADDR/4096)+i);	//ĞèÒª²Á³ıµÄÉÈÇø
		}
		for(i=0;i<5;i++)	//ÒÀ´Î¸üĞÂUNIGBK,GBK12,GBK16,GBK24,GBK32
		{
			LCD_ShowString(x,y,240,320,size,UPDATE_REMIND_TBL[i]);		
			strcpy((char*)pname,(char*)src);				//copy srcÄÚÈİµ½pname
			strcat((char*)pname,(char*)GBK_PATH[i]); 		//×·¼Ó¾ßÌåÎÄ¼şÂ·¾¶ 
			res=updata_fontx(x+20*size/2,y,size,pname,i);	//¸üĞÂ×Ö¿â
			if(res)
			{
				myfree(SRAMEX,buf);//SRAMIN
				myfree(SRAMEX,pname);//SRAMIN
				return 1+i;
			} 
		} 
		//È«²¿¸üĞÂºÃÁË
		ftinfo.fontok=0XAA;
		W25QXX_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//±£´æ×Ö¿âĞÅÏ¢
	}
	myfree(SRAMEX,pname);//ÊÍ·ÅÄÚ´æ SRAMIN
	myfree(SRAMEX,buf);//SRAMIN
	return rval;//ÎŞ´íÎó.			 
} 

#include "exfuns.h"
#ifdef	OS_UCOS
	#include "delay.h"
#endif
//³õÊ¼»¯×ÖÌå
//²ÎÊı  1:Ç¿ÖÆ¸üĞÂ×Ö¿â
//		0:¼ì²é×Ö¿â
//·µ»ØÖµ:0,×Ö¿âÍêºÃ.
//		 ÆäËû,×Ö¿â¶ªÊ§
u8 font_init(u8 cmd)
{		
	u8 t;
	W25QXX_Init(); 
	if(cmd)
	{

	#ifdef	OS_UCOS
		delay_osschedlock();//ÁÙÊ±¹Ø±ÕÈÎÎñµ÷¶È	
		u_printf(INFO,"ÈÎÎñÔİÍ££¡");
	#endif
		
		u_printf(INFO,"³¢ÊÔ¸üĞÂ×Ö¿â£¬ÇëÉÔºó...");
		LCD_Clear(BLACK);		
		if(0==update_font(0,0,16,"2:"))//¸üĞÂ³É¹¦
		{
			u_printf(INFO,"×Ö¿â¸üĞÂ³É¹¦£¡");
		}
		else
			u_printf(INFO,"×Ö¿â¸üĞÂÊ§°Ü£¡");	
	#ifdef	OS_UCOS	
		delay_osschedunlock();//»Ö¸´ÈÎÎñµ÷¶È	
		u_printf(INFO,"ÕıÔÚ»Ö¸´ÈÎÎñ...");		
	#endif
	}
	
	t=0;
	while(t<10)//Á¬Ğø¶ÁÈ¡10´Î,¶¼ÊÇ´íÎó,ËµÃ÷È·ÊµÊÇÓĞÎÊÌâ,µÃ¸üĞÂ×Ö¿âÁË
	{
		t++;
		W25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//¶Á³öftinfo½á¹¹ÌåÊı¾İ
		if(ftinfo.fontok==0XAA)break;
		delay_ms(20);
	}		
	if(ftinfo.fontok!=0XAA)return 1;
	return 0;		    
}
