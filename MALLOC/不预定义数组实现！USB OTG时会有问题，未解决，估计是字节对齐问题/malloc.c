#include "malloc.h"	   
#include "debug.h"
/*
By :����� 2016/8/15 
V1.0	����ԭ����ֲ
V1.1	1���޸�������Ϊ�ڴ��Ϊָ��
		2�����SRAMIN �ڴ���������
		3��CCM �� SRAMEX δ��ӷ�����������޸��ڴ�����Сʱ��
		�п�������ڴ�ش�С���ڴ������С�����ܴ�С�ķ��գ���������ӣ�

*/

//0x20000Ϊ�����ڴ��ܴ�С������MCUʱ��ע��
#define CHIP_RAM_START_Addr		0x20000000
#define CHIP_RAM_SIZE			0x30000

extern char Image$$RW_IRAM1$$ZI$$Limit[];

#define SDRAM_BASE_ADD	0XC0000000							//�ⲿSDRAM����ַ
#define MEM2_BASE_ADD	SDRAM_BASE_ADD + 0X001F4000			//�ⲿSDRAM�ڴ��,ǰ��2M��LTDC����(1280*800*2) = 0XC01F4000

#define MEM3_BASE_ADD	0X10000000							//�ڲ�CCM�ڴ��

////�ڴ��(32�ֽڶ���)
//__align(32) u8 mem1base[MEM1_MAX_SIZE];													//�ڲ�SRAM�ڴ��
//__align(32) u8 mem2base[MEM2_MAX_SIZE] __attribute__((at(0XC01F4000)));					//�ⲿSDRAM�ڴ��,ǰ��2M��LTDC����(1280*800*2)
//__align(32) u8 mem3base[MEM3_MAX_SIZE] __attribute__((at(0X10000000)));					//�ڲ�CCM�ڴ��

//�ڴ�����
//u32 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];													//�ڲ�SRAM�ڴ��MAP
//u32 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(MEM2_BASE_ADD+MEM2_MAX_SIZE)));	//�ⲿSRAM�ڴ��MAP
//u32 mem3mapbase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(MEM3_BASE_ADD+MEM3_MAX_SIZE)));	//�ڲ�CCM�ڴ��MAP


//�ڴ���������
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,						//�ڴ��ʼ��
	my_mem_perused,						//�ڴ�ʹ����
	0,0,0,								//�ڴ���׵�ַ
	0,MEM2_MAX_SIZE,MEM3_MAX_SIZE,		//�ڴ�ش�С
	0,0,0,								//�ڴ����״̬��
	0,0,0,								//�ڴ���С	
	MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE,MEM3_BLOCK_SIZE,//�ڴ�ֿ��С
	0,0,0,  		 					//�ڴ����δ����
};

//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void mymemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}

//�ڴ�����ʼ��  
//memx:�����ڴ��
void my_mem_init(u8 memx)  
{	
	uint32_t malloc_start, malloc_size;
	u32 memtblsize;
	u32 memblksize;
	if(memx == SRAMIN)
	{
		uint32_t Tmp;
		malloc_start = (uint32_t) &Image$$RW_IRAM1$$ZI$$Limit; //ȡ�����ڴ���͵�ַ
		
		malloc_start = (malloc_start + 3) & (~0x03);// ALIGN TO word
		
		Tmp = CHIP_RAM_START_Addr + CHIP_RAM_SIZE - malloc_start;
		if(Tmp < MEM1_MAX_SIZE + MEM1_MAX_SIZE/MEM1_BLOCK_SIZE)
		{//�ڴ�й©
			u_printf(ERR,"SRAMIN�ڴ�й©��");
			while(1);
		}			
		malloc_size = MEM1_MAX_SIZE;//�ڴ�����С
		memblksize = MEM1_BLOCK_SIZE;//�ڴ�ֿ��С
		memtblsize = malloc_size/MEM1_BLOCK_SIZE;//�ڴ���С
	}
	else if(memx == SRAMEX)
	{
		malloc_start = MEM2_BASE_ADD;
		malloc_size = MEM2_MAX_SIZE;//�ڴ�����С
		memtblsize = malloc_size/MEM2_BLOCK_SIZE;//�ڴ���С
		memblksize = MEM2_BLOCK_SIZE;//�ڴ�ֿ��С
	}
	else if(memx == SRAMCCM)
	{
		malloc_start = MEM3_BASE_ADD;
		malloc_size = MEM3_MAX_SIZE;//�ڴ�����С
		memtblsize = malloc_size/MEM3_BLOCK_SIZE;//�ڴ���С
		memblksize = MEM3_BLOCK_SIZE;//�ڴ�ֿ��С
	}
	else
	{
		u_printf(ERR,"�ڴ��ʼ������");
		while(1);
	}
	
	mallco_dev.membase[memx] = (u8*)malloc_start;//��ʼ���ڴ���׵�ַָ��
	mallco_dev.memsize[memx] = malloc_size;	//�ڴ�ش�С
	mallco_dev.memmap[memx] = (u32*)(malloc_start + mallco_dev.memsize[memx]);//�ڴ����״̬���׵�ַָ��	
	mallco_dev.memtblsize[memx] = memtblsize;//�ڴ���С
	mallco_dev.memblksize[memx] = memblksize;//�ڴ�ֿ��С
#if 1	
	u_printf(INFO,"\r\n\r\n%s Init Sucess!",(memx == SRAMEX)?"SRAMEX":(memx == SRAMIN)?"SRAMIN":"SRAMCCM");	
	u_printf(INFO,"Memory Start Address = 0x%08x ��Size = %dBytes",mallco_dev.membase[memx],mallco_dev.memsize[memx]);		
	u_printf(INFO,"Memory status table Address = 0x%08x,Size = %dBytes",mallco_dev.memmap[memx],mallco_dev.memtblsize[memx]);					
#endif
	
    mymemset(mallco_dev.memmap[memx],0,mallco_dev.memtblsize[memx]*4);	//�ڴ�״̬����������  
 	mallco_dev.memrdy[memx]=1;								//�ڴ�����ʼ��OK  
}  
//��ȡ�ڴ�ʹ����
//memx:�����ڴ��
//����ֵ:ʹ����(������10��,0~1000,����0.0%~100.0%)
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
//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
u32 my_mem_malloc(u8 memx,u32 size)  
{  
    signed long offset=0;  
    u32 nmemb;	//��Ҫ���ڴ����  
	u32 cmemb=0;//�������ڴ����
    u32 i;  
    if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);//δ��ʼ��,��ִ�г�ʼ�� 
    if(size==0)return 0XFFFFFFFF;//����Ҫ����
    nmemb=size/mallco_dev.memblksize[memx];  	//��ȡ��Ҫ����������ڴ����
    if(size%mallco_dev.memblksize[memx])nmemb++;  
    for(offset=mallco_dev.memtblsize[memx]-1;offset>=0;offset--)//���������ڴ������  
    {     
		if(!mallco_dev.memmap[memx][offset])cmemb++;//�������ڴ��������
		else cmemb=0;								//�����ڴ������
		if(cmemb==nmemb)							//�ҵ�������nmemb�����ڴ��
		{
            for(i=0;i<nmemb;i++)  					//��ע�ڴ��ǿ� 
            {  
                mallco_dev.memmap[memx][offset+i]=nmemb;  
            }  
            return (offset*mallco_dev.memblksize[memx]);//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  
//�ͷ��ڴ�(�ڲ�����) 
//memx:�����ڴ��
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
u8 my_mem_free(u8 memx,u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy[memx])//δ��ʼ��,��ִ�г�ʼ��
	{
		mallco_dev.init(memx);    
        return 1;//δ��ʼ��  
    }  
    if(offset<mallco_dev.memsize[memx])//ƫ�����ڴ����. 
    {  
        int index=offset/mallco_dev.memblksize[memx];			//ƫ�������ڴ�����  
        int nmemb=mallco_dev.memmap[memx][index];	//�ڴ������
        for(i=0;i<nmemb;i++)  						//�ڴ������
        {  
            mallco_dev.memmap[memx][index+i]=0;  
        }  
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  
}  
//�ͷ��ڴ�(�ⲿ����) 
//memx:�����ڴ��
//ptr:�ڴ��׵�ַ 
void myfree(u8 memx,void *ptr)  
{  
	u32 offset;   
	if(ptr==NULL)return;//��ַΪ0.  
 	offset=(u32)ptr-(u32)mallco_dev.membase[memx];     
    my_mem_free(memx,offset);	//�ͷ��ڴ�      
}  
//�����ڴ�(�ⲿ����)
//memx:�����ڴ��
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *mymalloc(u8 memx,u32 size)  
{  
    u32 offset;   
	offset=my_mem_malloc(memx,size);  	   	 	   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
//���·����ڴ�(�ⲿ����)
//memx:�����ڴ��
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
    u32 offset;    
    offset=my_mem_malloc(memx,size);   	
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);	//�������ڴ����ݵ����ڴ�   
        myfree(memx,ptr);  											  		//�ͷž��ڴ�
        return (void*)((u32)mallco_dev.membase[memx]+offset);  				//�������ڴ��׵�ַ
    }  
}
