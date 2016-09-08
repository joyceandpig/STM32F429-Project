#include "nandtester.h"
#include "nand.h"
#include "ftl.h"
#include "string.h"
#include "usart.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32¿ª·¢°å
//NAND FLASH USMART²âÊÔ´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2016/1/15
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	  


//ÏòNANDÄ³Ò»Ò³Ğ´ÈëÖ¸¶¨´óĞ¡µÄÊı¾İ
//pagenum:ÒªĞ´ÈëµÄÒ³µØÖ·
//colnum:ÒªĞ´ÈëµÄ¿ªÊ¼ÁĞµØÖ·(Ò³ÄÚµØÖ·)
//writebytes:ÒªĞ´ÈëµÄÊı¾İ´óĞ¡£¬MT29F16G×î´óÎª4320£¬MT29F4G×î´óÎª2112
u8 test_writepage(u32 pagenum,u16 colnum,u16 writebytes)
{
	u8 *pbuf;
	u8 sta=0;
    u16 i=0;
	pbuf=mymalloc(SRAMEX,5000); // SRAMIN
    for(i=0;i<writebytes;i++)//×¼±¸ÒªĞ´ÈëµÄÊı¾İ,Ìî³äÊı¾İ,´Ó0¿ªÊ¼Ôö´ó
    { 
        pbuf[i]=i;	
    }
	sta=NAND_WritePage(pagenum,colnum,pbuf,writebytes);	//ÏònandĞ´ÈëÊı¾İ	
	myfree(SRAMEX,pbuf);	//ÊÍ·ÅÄÚ´æ  SRAMIN
	return sta;
}

//¶ÁÈ¡NANDÄ³Ò»Ò³Ö¸¶¨´óĞ¡µÄÊı¾İ
//pagenum:Òª¶ÁÈ¡µÄÒ³µØÖ·
//colnum:Òª¶ÁÈ¡µÄ¿ªÊ¼ÁĞµØÖ·(Ò³ÄÚµØÖ·)
//readbytes:Òª¶ÁÈ¡µÄÊı¾İ´óĞ¡£¬MT29F16G×î´óÎª4320£¬MT29F4G×î´óÎª2112
u8 test_readpage(u32 pagenum,u16 colnum,u16 readbytes)
{
	u8 *pbuf;
	u8 sta=0;
    u16 i=0;
	pbuf=mymalloc(SRAMEX,5000);  //SRAMIN
	sta=NAND_ReadPage(pagenum,colnum,pbuf,readbytes);	//¶ÁÈ¡Êı¾İ
	if(sta==0||sta==NSTA_ECC1BITERR||sta==NSTA_ECC2BITERR)//¶ÁÈ¡³É¹¦
	{ 
		printf("read page data is:\r\n");
		for(i=0;i<readbytes;i++)	 
		{ 
			printf("%x ",pbuf[i]);  //´®¿Ú´òÓ¡¶ÁÈ¡µ½µÄÊı¾İ
		}
		printf("\r\nend\r\n");
	}
	myfree(SRAMEX,pbuf);	//ÊÍ·ÅÄÚ´æSRAMIN
	return sta;
}

//½«Ò»Ò³Êı¾İ¿½±´µ½ÁíÍâÒ»Ò³,²¢Ğ´ÈëÒ»²¿·ÖÄÚÈİ.
//×¢Òâ:Ô´Ò³ºÍÄ¿±êÒ³ÒªÔÚÍ¬Ò»¸öPlaneÄÚ£¡(Í¬ÎªÆæÊı/Í¬ÎªÅ¼Êı)
//spnum:Ô´Ò³µØÖ·
//epnum:Ä¿±êÒ³µØÖ·
//colnum:ÒªĞ´ÈëµÄ¿ªÊ¼ÁĞµØÖ·(Ò³ÄÚµØÖ·)
//writebytes:ÒªĞ´ÈëµÄÊı¾İ´óĞ¡£¬²»ÄÜ³¬¹ıÒ³´óĞ¡
u8 test_copypageandwrite(u32 spnum,u32 dpnum,u16 colnum,u16 writebytes)
{
	u8 *pbuf;
	u8 sta=0;
    u16 i=0;
	pbuf=mymalloc(SRAMEX,5000);  //SRAMIN
    for(i=0;i<writebytes;i++)//×¼±¸ÒªĞ´ÈëµÄÊı¾İ,Ìî³äÊı¾İ,´Ó0X80¿ªÊ¼Ôö´ó
    { 
        pbuf[i]=i+0X80;	
    }
	sta=NAND_CopyPageWithWrite(spnum,dpnum,colnum,pbuf,writebytes);	//ÏònandĞ´ÈëÊı¾İ	
	myfree(SRAMEX,pbuf);	//ÊÍ·ÅÄÚ´æSRAMIN
	return sta;
}
 
//¶ÁÈ¡NANDÄ³Ò»Ò³SpareÇøÖ¸¶¨´óĞ¡µÄÊı¾İ
//pagenum:Òª¶ÁÈ¡µÄÒ³µØÖ·
//colnum:Òª¶ÁÈ¡µÄspareÇø¿ªÊ¼µØÖ·
//readbytes:Òª¶ÁÈ¡µÄÊı¾İ´óĞ¡£¬MT29F16G×î´óÎª64£¬MT29F4G×î´óÎª224
u8 test_readspare(u32 pagenum,u16 colnum,u16 readbytes)
{
	u8 *pbuf;
	u8 sta=0;
    u16 i=0;
	pbuf=mymalloc(SRAMEX,512); // SRAMIN
	sta=NAND_ReadSpare(pagenum,colnum,pbuf,readbytes);	//¶ÁÈ¡Êı¾İ
	if(sta==0)//¶ÁÈ¡³É¹¦
	{ 
		printf("read spare data is:\r\n");
		for(i=0;i<readbytes;i++)	 
		{ 
			printf("%x ",pbuf[i]);  //´®¿Ú´òÓ¡¶ÁÈ¡µ½µÄÊı¾İ
		}
		printf("\r\nend\r\n");
	}
	myfree(SRAMEX,pbuf);	//ÊÍ·ÅÄÚ´æSRAMIN
	return sta;
}

//´ÓÖ¸¶¨Î»ÖÃ¿ªÊ¼,¶ÁÈ¡Õû¸öNAND,Ã¿¸öBLOCKµÄµÚÒ»¸öpageµÄÇ°5¸ö×Ö½Ú
//sblock:Ö¸¶¨¿ªÊ¼µÄblock±àºÅ
void test_readallblockinfo(u32 sblock)
{
    u8 j=0;
    u32 i=0;
	u8 sta;
    u8 buffer[5];
    for(i=sblock;i<nand_dev.block_totalnum;i++)
    {
        printf("block %d info:",i);
        sta=NAND_ReadSpare(i*nand_dev.block_pagenum,0,buffer,5);//¶ÁÈ¡Ã¿¸öblock,µÚÒ»¸öpageµÄÇ°5¸ö×Ö½Ú
        if(sta)printf("failed\r\n");
		for(j=0;j<5;j++)
        {
            printf("%x ",buffer[j]);
        }
        printf("\r\n");
    }	
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//FTL²ã²âÊÔ´úÂë

//´ÓÄ³¸öÉÈÇø¿ªÊ¼,Ğ´Èëseccnt¸öÉÈÇøµÄÊı¾İ
//secx:¿ªÊ¼µÄÉÈÇø±àºÅ
//secsize:ÉÈÇø´óĞ¡
//seccnt:ÒªĞ´ÈëµÄÉÈÇø¸öÊı
u8 test_ftlwritesectors(u32 secx,u16 secsize,u16 seccnt)
{
	u8 *pbuf;
	u8 sta=0;
    u32 i=0;
	pbuf=mymalloc(SRAMEX,secsize*seccnt);  //SRAMIN
    for(i=0;i<secsize*seccnt;i++)	//×¼±¸ÒªĞ´ÈëµÄÊı¾İ,Ìî³äÊı¾İ,´Ó0¿ªÊ¼Ôö´ó
    { 
        pbuf[i]=i;	
    }
	sta=FTL_WriteSectors(pbuf,secx,secsize,seccnt);	//ÏònandĞ´ÈëÊı¾İ	
	myfree(SRAMEX,pbuf);	//ÊÍ·ÅÄÚ´æSRAMIN
	return sta;
}


//´ÓÄ³¸öÉÈÇø¿ªÊ¼,¶Á³öseccnt¸öÉÈÇøµÄÊı¾İ
//secx:¿ªÊ¼µÄÉÈÇø±àºÅ
//secsize:ÉÈÇø´óĞ¡
//seccnt:Òª¶ÁÈ¡µÄÉÈÇø¸öÊı
u8 test_ftlreadsectors(u32 secx,u16 secsize,u16 seccnt)
{
	u8 *pbuf;
	u8 sta=0;
    u32 i=0;
	pbuf=mymalloc(SRAMEX,secsize*seccnt);   //SRAMIN
	sta=FTL_ReadSectors(pbuf,secx,secsize,seccnt);	//¶ÁÈ¡Êı¾İ
	if(sta==0)
	{
		printf("read sec %d data is:\r\n",secx); 
		for(i=0;i<secsize*seccnt;i++)	//×¼±¸ÒªĞ´ÈëµÄÊı¾İ,Ìî³äÊı¾İ,´Ó0¿ªÊ¼Ôö´ó
		{ 
			printf("%x ",pbuf[i]);  //´®¿Ú´òÓ¡¶ÁÈ¡µ½µÄÊı¾İ
		}
		printf("\r\nend\r\n");
	}
	myfree(SRAMEX,pbuf);	//ÊÍ·ÅÄÚ´SRAMINæ
	return sta;
}





























