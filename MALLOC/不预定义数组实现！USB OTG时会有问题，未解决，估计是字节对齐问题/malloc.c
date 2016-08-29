#include "malloc.h"	   
#include "debug.h"
/*
By :冯光荣 2016/8/15 
V1.0	正点原子移植
V1.1	1、修改数组作为内存次为指针
		2、添加SRAMIN 内存防溢出管理
		3、CCM 和 SRAMEX 未添加防溢出管理（再修改内存管理大小时，
		有可能造成内存池大小和内存管理表大小超过总大小的风险，后期需添加）

*/

//0x20000为板子内存总大小，更换MCU时需注意
#define CHIP_RAM_START_Addr		0x20000000
#define CHIP_RAM_SIZE			0x30000

extern char Image$$RW_IRAM1$$ZI$$Limit[];

#define SDRAM_BASE_ADD	0XC0000000							//外部SDRAM基地址
#define MEM2_BASE_ADD	SDRAM_BASE_ADD + 0X001F4000			//外部SDRAM内存池,前面2M给LTDC用了(1280*800*2) = 0XC01F4000

#define MEM3_BASE_ADD	0X10000000							//内部CCM内存池

////内存池(32字节对齐)
//__align(32) u8 mem1base[MEM1_MAX_SIZE];													//内部SRAM内存池
//__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0XC01F4000)));					//外部SDRAM内存池,前面2M给LTDC用了(1280*800*2)
//__align(32) u8 mem3base[MEM3_MAX_SIZE] __attribute__((at(0X10000000)));					//内部CCM内存池

//内存管理表
//u32 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													//内部SRAM内存池MAP
//u32 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(MEM2_BASE_ADD+MEM2_MAX_SIZE)));	//外部SRAM内存池MAP
//u32 mem3mapbase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(MEM3_BASE_ADD+MEM3_MAX_SIZE)));	//内部CCM内存池MAP


//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,						//内存初始化
	my_mem_perused,						//内存使用率
	0,0,0,								//内存池首地址
	0,MEM2_MAX_SIZE,MEM3_MAX_SIZE,		//内存池大小
	0,0,0,								//内存管理状态表
	0,0,0,								//内存表大小	
	MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE,MEM3_BLOCK_SIZE,//内存分块大小
	0,0,0,  		 					//内存管理未就绪
};

//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}

//内存管理初始化  
//memx:所属内存块
void my_mem_init(u8 memx)  
{	
	uint32_t malloc_start, malloc_size;
	u32 memtblsize;
	u32 memblksize;
	if(memx == SRAMIN)
	{
		uint32_t Tmp;
		malloc_start = (uint32_t) &Image$$RW_IRAM1$$ZI$$Limit; //取空闲内存最低地址
		
		malloc_start = (malloc_start + 3) & (~0x03);// ALIGN TO word
		
		Tmp = CHIP_RAM_START_Addr + CHIP_RAM_SIZE - malloc_start;
		if(Tmp < MEM1_MAX_SIZE + MEM1_MAX_SIZE/MEM1_BLOCK_SIZE)
		{//内存泄漏
			u_printf(ERR,"SRAMIN内存泄漏！");
			while(1);
		}			
		malloc_size = MEM1_MAX_SIZE;//内存管理大小
		memblksize = MEM1_BLOCK_SIZE;//内存分块大小
		memtblsize = malloc_size/MEM1_BLOCK_SIZE;//内存表大小
	}
	else if(memx == SRAMEX)
	{
		malloc_start = MEM2_BASE_ADD;
		malloc_size = MEM2_MAX_SIZE;//内存管理大小
		memtblsize = malloc_size/MEM2_BLOCK_SIZE;//内存表大小
		memblksize = MEM2_BLOCK_SIZE;//内存分块大小
	}
	else if(memx == SRAMCCM)
	{
		malloc_start = MEM3_BASE_ADD;
		malloc_size = MEM3_MAX_SIZE;//内存管理大小
		memtblsize = malloc_size/MEM3_BLOCK_SIZE;//内存表大小
		memblksize = MEM3_BLOCK_SIZE;//内存分块大小
	}
	else
	{
		u_printf(ERR,"内存初始化错误！");
		while(1);
	}
	
	mallco_dev.membase[memx] = (u8*)malloc_start;//初始化内存池首地址指针
	mallco_dev.memsize[memx] = malloc_size;	//内存池大小
	mallco_dev.memmap[memx] = (u32*)(malloc_start + mallco_dev.memsize[memx]);//内存管理状态表首地址指针	
	mallco_dev.memtblsize[memx] = memtblsize;//内存表大小
	mallco_dev.memblksize[memx] = memblksize;//内存分块大小
#if 1	
	u_printf(INFO,"\r\n\r\n%s Init Sucess!",(memx == SRAMEX)?"SRAMEX":(memx == SRAMIN)?"SRAMIN":"SRAMCCM");	
	u_printf(INFO,"Memory Start Address = 0x%08x ，Size = %dBytes",mallco_dev.membase[memx],mallco_dev.memsize[memx]);		
	u_printf(INFO,"Memory status table Address = 0x%08x,Size = %dBytes",mallco_dev.memmap[memx],mallco_dev.memtblsize[memx]);					
#endif
	
    mymemset(mallco_dev.memmap[memx],0,mallco_dev.memtblsize[memx]*4);	//内存状态表数据清零  
 	mallco_dev.memrdy[memx]=1;								//内存管理初始化OK  
}  
//获取内存使用率
//memx:所属内存块
//返回值:使用率(扩大了10倍,0~1000,代表0.0%~100.0%)
u16 my_mem_perused(u8 memx)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<mallco_dev.memtblsize[memx];i++)  
    {  
        if(mallco_dev.memmap[memx][i])used++; 
    } 
    return (used*1000)/(mallco_dev.memtblsize[memx]);  
}  
//内存分配(内部调用)
//memx:所属内存块
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
u32 my_mem_malloc(u8 memx,u32 size)  
{  
    signed long offset=0;  
    u32 nmemb;	//需要的内存块数  
	u32 cmemb=0;//连续空内存块数
    u32 i;  
    if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);//未初始化,先执行初始化 
    if(size==0)return 0XFFFFFFFF;//不需要分配
    nmemb=size/mallco_dev.memblksize[memx];  	//获取需要分配的连续内存块数
    if(size%mallco_dev.memblksize[memx])nmemb++;  
    for(offset=mallco_dev.memtblsize[memx]-1;offset>=0;offset--)//搜索整个内存控制区  
    {     
		if(!mallco_dev.memmap[memx][offset])cmemb++;//连续空内存块数增加
		else cmemb=0;								//连续内存块清零
		if(cmemb==nmemb)							//找到了连续nmemb个空内存块
		{
            for(i=0;i<nmemb;i++)  					//标注内存块非空 
            {  
                mallco_dev.memmap[memx][offset+i]=nmemb;  
            }  
            return (offset*mallco_dev.memblksize[memx]);//返回偏移地址  
		}
    }  
    return 0XFFFFFFFF;//未找到符合分配条件的内存块  
}  
//释放内存(内部调用) 
//memx:所属内存块
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  
u8 my_mem_free(u8 memx,u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy[memx])//未初始化,先执行初始化
	{
		mallco_dev.init(memx);    
        return 1;//未初始化  
    }  
    if(offset<mallco_dev.memsize[memx])//偏移在内存池内. 
    {  
        int index=offset/mallco_dev.memblksize[memx];			//偏移所在内存块号码  
        int nmemb=mallco_dev.memmap[memx][index];	//内存块数量
        for(i=0;i<nmemb;i++)  						//内存块清零
        {  
            mallco_dev.memmap[memx][index+i]=0;  
        }  
        return 0;  
    }else return 2;//偏移超区了.  
}  
//释放内存(外部调用) 
//memx:所属内存块
//ptr:内存首地址 
void myfree(u8 memx,void *ptr)  
{  
	u32 offset;   
	if(ptr==NULL)return;//地址为0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase[memx];     
    my_mem_free(memx,offset);	//释放内存      
}  
//分配内存(外部调用)
//memx:所属内存块
//size:内存大小(字节)
//返回值:分配到的内存首地址.
void *mymalloc(u8 memx,u32 size)  
{  
    u32 offset;   
	offset=my_mem_malloc(memx,size);  	   	 	   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
//重新分配内存(外部调用)
//memx:所属内存块
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
    u32 offset;    
    offset=my_mem_malloc(memx,size);   	
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);	//拷贝旧内存内容到新内存   
        myfree(memx,ptr);  											  		//释放旧内存
        return (void*)((u32)mallco_dev.membase[memx]+offset);  				//返回新内存首地址
    }  
}
