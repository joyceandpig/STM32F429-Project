#include "atk_frec.h" 
#include "malloc.h"	
#include "ff.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//本人脸识别程序由ALIENTEK提供,我们提供1个LIB(即:ATKFREC.lib),供大家使用 

//功能:利用摄像头,实现人脸识别.
//说明:本识别库,需要用到内存管理,内存总占用数在560KB左右(20个人脸).每增加一张人脸,内存占增加10KB左右.
//限制:由于本识别库以M3/M4为目标处理器,内存有限,算法上进行了大量阉割,所以,很多功能不太完善,效果也不是
//     很好.且没有做识别有效检查(输入错误的人脸,也会有结果输出).所以,此代码,仅供大家参考用.
//
//其他需求:
//1,摄像头模块一个.
//2,SD卡一张
//
//使用方法:					   
//第一步:调用atk_frec_initialization函数,初始化人脸识别库
//第二步:调用atk_frec_add_a_face函数,添加人脸模板(如果已经有了,可以忽略次步)
//第三步:调用atk_frec_load_data_model函数,加载所有模板到内存里面(仅在添加新模板后需要,如没有添加新模板,则可忽略此步)
//第四步:调用atk_frec_recognition_face函数,获取识别结果.
//第五步:调用atk_frec_delete_data函数,可以删除一个人脸模板
//第六步:如果不想再用识别库,则调用atk_frec_destroy函数,释放所有内存,结束人脸识别.
//
//版本:V1.0
//正点原子@ALIENTEK
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved													    								  
//版权所有，盗版必究。
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//内存设置函数
void atk_frec_memset(char *p,char c,unsigned long len) 
{
	mymemset((u8*)p,(u8)c,(u32)len);
}	 					  
//内存申请函数
void *atk_frec_malloc(unsigned int size) 
{
	return mymalloc(SRAMIN,size);
}
//内存释放函数
void atk_frec_free(void *ptr) 
{
	myfree(SRAMIN,ptr);
}

//保存人脸识别所需的数据
//index:要保存的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1
//buf:要保存的数据缓存区首地址
//size:要保存的数据大小
//返回值:0,正常
//    其他,错误代码
u8 atk_frec_save_data(u8 index,u8* buf,u32 size)
{
	u8* path;
	FIL *fp; 
	u32 fw;
	u8 res;
	path=atk_frec_malloc(30);			//申请内存
	fp=atk_frec_malloc(sizeof(FIL));	//申请内存
	if(!fp)
	{
		atk_frec_free(path);
		return ATK_FREC_MEMORY_ERR;
	}
	sprintf((char*)path,ATK_FREC_DATA_PNAME,index);
	f_mkdir(ATK_FREC_DATA_PDIR);		//创建文件夹
	res=f_open(fp,(char*)path,FA_WRITE|FA_CREATE_NEW);
	if(res==FR_OK)
	{
		res=f_write(fp,buf,size,&fw);	//写入文件		
	}
	f_close(fp);
	if(res)res=ATK_FREC_READ_WRITE_ERR;
	atk_frec_free(path);
	atk_frec_free(fp);
	return res;	
}
//读取人脸识别所需的数据
//index:要读取的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1
//buf:要读取的数据缓存区首地址
//size:要读取的数据大小(size=0,则表示不需要读数据出来)
//返回值:0,正常
//    其他,错误代码
u8 atk_frec_read_data(u8 index,u8* buf,u32 size)
{
	u8* path;
	FIL *fp; 
	u32 fr;
	u8 res;
	path=atk_frec_malloc(30);			//申请内存
	fp=atk_frec_malloc(sizeof(FIL));	//申请内存
	if(!fp)
	{
		atk_frec_free(path);
		return ATK_FREC_MEMORY_ERR;
	}
	sprintf((char*)path,ATK_FREC_DATA_PNAME,index); 
	res=f_open(fp,(char*)path,FA_READ);
	if(res==FR_OK&&size)
	{
		res=f_read(fp,buf,size,&fr);	//读取文件	
		if(fr==size)res=0;
		else res=ATK_FREC_READ_WRITE_ERR;
	} 
	f_close(fp);
	if(res)res=ATK_FREC_READ_WRITE_ERR;
	atk_frec_free(path);
	atk_frec_free(fp);
	return res;	
} 
//删除一个人脸数据
//index:要保存的数据位置(一张脸占一个位置),范围:0~MAX_LEBEL_NUM-1  
//返回值:0,正常
//    其他,错误代码
u8 atk_frec_delete_data(u8 index)
{
	u8* path;  
	u8 res;
	path=atk_frec_malloc(30);			//申请内存 
	if(!path)
	{ 
		return ATK_FREC_MEMORY_ERR;
	}
	sprintf((char*)path,ATK_FREC_DATA_PNAME,index); 
	res=f_unlink((char*)path);  
	atk_frec_free(path);
	return res;	
}




 
