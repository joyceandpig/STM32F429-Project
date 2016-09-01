#include <apeplay.h>
#include "sys.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
#include "malloc.h"
#include "key.h"
#include "sai.h"
#include "wm8978.h"
#include "audioplay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自RockBox的ape解码库
//ALIENTEK STM32开发板
//APE 解码代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/29
//版本：V1.1
//********************************************************************************
//V1.0 说明
//1,支持16位单声道/立体声ape的解码
//2,最高支持96K的APE格式(LV1和LV2)
//3,LV1~LV3,在48K及以下流畅播放,LV4,LV5大卡.
//4,对某些ape文件,可能不支持,请用Monkey's Audio软件进行转换一下,即可正常播放
////////////////////////////////////////////////////////////////////////////////// 	 
 
__apectrl * apectrl;	//APE播放控制结构体

//apedecoder.c里面需要的数组 
extern filter_int *filterbuf64;		//需要2816字节 
	
vu8 apetransferend=0;	//sai传输完成标志
vu8 apewitchbuf=0;		//saibufx指示标志

//APE DMA发送回调函数
void ape_sai_dma_tx_callback(void) 
{    
	u16 i;
	if(DMA2_Stream3->CR&(1<<19))
	{
		apewitchbuf=0;
		if((audiodev.status&0X01)==0)//暂停了,填充0
		{
			for(i=0;i<APE_BLOCKS_PER_LOOP*4;i++)audiodev.saibuf1[i]=0;
		}
	}else 
	{
		apewitchbuf=1;
		if((audiodev.status&0X01)==0)//暂停了,填充0
		{
			for(i=0;i<APE_BLOCKS_PER_LOOP*4;i++)audiodev.saibuf2[i]=0;
		}
	} 
	apetransferend=1;
} 
//填充PCM数据到DAC
//buf:PCM数据首地址
//size:pcm数据量(32位为单位) 
void ape_fill_buffer(u32* buf,u16 size)
{
	u16 i; 
	u32 *p;
	while(apetransferend==0)delay_ms(1000/OS_TICKS_PER_SEC);//等待传输完成
	apetransferend=0;
	if(apewitchbuf==0)
	{
		p=(u32*)audiodev.saibuf1;
	}else 
	{
		p=(u32*)audiodev.saibuf2;
	}
	for(i=0;i<size;i++)p[i]=buf[i];
} 
//得到当前播放时间
//fx:文件指针
//apectrl:apectrl播放控制器
void ape_get_curtime(FIL*fx,__apectrl *apectrl)
{
	long long fpos=0;  	 
	if(fx->fptr>apectrl->datastart)fpos=fx->fptr-apectrl->datastart;	//得到当前文件播放到的地方 
	apectrl->cursec=fpos*apectrl->totsec/(fx->obj.objsize-apectrl->datastart);	//当前播放到第多少秒了?	
}  
//ape文件快进快退函数
//pos:需要定位到的文件位置
//返回值:当前文件位置(即定位后的结果)
u32 ape_file_seek(u32 pos)
{ 
	return audiodev.file->fptr;//ape文件不支持快进快退,直接返回当前播放位置
}
//播放一曲FLAC音乐
//fname:FLAC文件路径.
//返回值:0,正常播放完成
//[b7]:0,正常状态;1,错误状态
//[b6:0]:b7=0时,表示操作码 
//       b7=1时,表示有错误(这里不判定具体错误,0X80~0XFF,都算是错误)
u8 ape_play_song(u8* fname) 
{
	struct ape_ctx_t *apex; 
	
    int currentframe;
    int nblocks;
    int bytesconsumed;
    int bytesinbuffer;
    int blockstodecode;
    int firstbyte;
	int n; 
	
    u8 res=AP_ERR;
	u8 *readptr;
	u8 *buffer;
	int *decoded0;
	int *decoded1;
	u32 totalsamples; 
	 
	filterbuf64=mymalloc(SRAMIN,2816);  
	apectrl=mymalloc(SRAMIN,sizeof(__apectrl));
	apex=mymalloc(SRAMIN,sizeof(struct ape_ctx_t));
	decoded0=mymalloc(SRAMCCM,APE_BLOCKS_PER_LOOP*4);
	decoded1=mymalloc(SRAMCCM,APE_BLOCKS_PER_LOOP*4); 
	audiodev.file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	audiodev.saibuf1=mymalloc(SRAMIN,APE_BLOCKS_PER_LOOP*4);
	audiodev.saibuf2=mymalloc(SRAMIN,APE_BLOCKS_PER_LOOP*4);  
	audiodev.file_seek=ape_file_seek;
	buffer=mymalloc(SRAMIN,APE_FILE_BUF_SZ);
	if(filterbuf64&&apectrl&&apex&&decoded0&&decoded1&&audiodev.file&&audiodev.saibuf1&&audiodev.saibuf2&&buffer)//所有内存申请都OK.
	{ 
		memset(apex,0,sizeof(struct ape_ctx_t));
		memset(apectrl,0,sizeof(__apectrl));
		memset(audiodev.saibuf1,0,APE_BLOCKS_PER_LOOP*4);
		memset(audiodev.saibuf2,0,APE_BLOCKS_PER_LOOP*4);		
		f_open(audiodev.file,(char*)fname,FA_READ);		//打开文件
		res=ape_parseheader(audiodev.file,apex);//分析ape文件头 
		if(res==0)
		{  
			if((apex->compressiontype>3000)||(apex->fileversion<APE_MIN_VERSION)||(apex->fileversion>APE_MAX_VERSION||apex->bps!=16))
			{
				res=AP_ERR;//压缩率不支持/版本不支持/不是16位音频格式
			}else
			{
				apectrl->bps=apex->bps;					//得到采样深度(ape,我们仅支持16位)
				apectrl->samplerate=apex->samplerate;	//得到采样率,ape支持48Khz以下的采样率,在高就可能卡了...
				if(apex->totalframes>1)totalsamples=apex->finalframeblocks+apex->blocksperframe*(apex->totalframes-1);
				else totalsamples=apex->finalframeblocks;
				apectrl->totsec=totalsamples/apectrl->samplerate;//得到文件总时长 
				apectrl->bitrate=(audiodev.file->obj.objsize-apex->firstframe)*8/apectrl->totsec;//得到位速
				apectrl->outsamples=APE_BLOCKS_PER_LOOP*2;//PCM输出数据量(16位为单位) 
				apectrl->datastart=apex->firstframe;	//得到第一帧的地址	 
			}
		}
	}
	if(res==0)
	{   
		printf("  Samplerate: %d\r\n", apectrl->samplerate); 
		printf("  Bits per sample: %d\r\n",apectrl->bps);
		printf("  First frame pos: %d\r\n",apectrl->datastart); 
		printf("  Duration: %d s\r\n",apectrl->totsec);
		printf("  Bitrate: %d kbps\r\n",apectrl->bitrate); 
 		//ape_dumpinfo(apex);
 		WM8978_I2S_Cfg(2,0);	//飞利浦标准,16位数据长度
		SAIA_Init(0,1,4);		//设置SAI,主发送,16位数据
		SAIA_SampleRate_Set(apex->samplerate);		//设置采样率 
		SAIA_TX_DMA_Init(audiodev.saibuf1,audiodev.saibuf2,APE_BLOCKS_PER_LOOP*2,1);//配置TX DMA 
		sai_tx_callback=ape_sai_dma_tx_callback;	//回调函数指向mp3_sai_dma_tx_callback				
 		currentframe=0; 
		f_lseek(audiodev.file,apex->firstframe); 
		res=f_read(audiodev.file,buffer,APE_FILE_BUF_SZ,(u32*)&bytesinbuffer);	
		firstbyte=3;  		//Take account of the little-endian 32-bit byte ordering
		readptr=buffer;
		audio_start(); 
		while(currentframe<apex->totalframes&&res==0)//还有未解码的帧?
		{ 
			//计算一帧里面有多少个blocks?
			if(currentframe==(apex->totalframes-1))nblocks=apex->finalframeblocks;
			else nblocks=apex->blocksperframe; 
			apex->currentframeblocks=nblocks; 
			//初始化帧解码
			init_frame_decoder(apex,readptr,&firstbyte,&bytesconsumed);
			readptr+=bytesconsumed;
			bytesinbuffer-=bytesconsumed; 						
			while(nblocks>0)//开始帧解码
			{
				blockstodecode=AUDIO_MIN(APE_BLOCKS_PER_LOOP,nblocks);//获得一次要解码的blocks个数  
				res=decode_chunk(apex,readptr,&firstbyte,&bytesconsumed,decoded0,decoded1,blockstodecode);
				if(res!=0)
				{
					printf("frame decode err\r\n");
					res=AP_ERR;
					break;
				} 
				ape_fill_buffer((u32*)decoded1,APE_BLOCKS_PER_LOOP);   
				readptr+=bytesconsumed;			//解码指针偏移到新数据位置
				bytesinbuffer-=bytesconsumed; 	//buffer里面的数据量减少
				if(bytesconsumed>4*APE_BLOCKS_PER_LOOP)//出现错误了
				{
					nblocks=0;
					res=AP_ERR;
					printf("bytesconsumed:%d\r\n",bytesconsumed);
				}
				if(bytesinbuffer<4*APE_BLOCKS_PER_LOOP)//需要补充新数据了
				{ 
					memmove(buffer,readptr,bytesinbuffer);
					res=f_read(audiodev.file,buffer+bytesinbuffer,APE_FILE_BUF_SZ-bytesinbuffer,(u32*)&n);
					if(res)//出错了.
					{ 
						res=AP_ERR;
						break; 
					}
					bytesinbuffer+=n;  
					readptr=buffer;
				} 
				nblocks-=blockstodecode;//block计数递减
				while(audiodev.status&(1<<1))//正常播放中
				{		 
					ape_get_curtime(audiodev.file,apectrl);//得到总时间和当前播放的时间 
					audiodev.totsec=apectrl->totsec;		//参数传递
					audiodev.cursec=apectrl->cursec;
					audiodev.bitrate=apectrl->bitrate;
					audiodev.samplerate=apectrl->samplerate;
					audiodev.bps=apectrl->bps;	 
					delay_ms(1000/OS_TICKS_PER_SEC);  
					if(audiodev.status&0X01)break;	//没有按下暂停   
				}
				if((audiodev.status&(1<<1))==0)		//请求结束播放/播放完成
				{  
					nblocks=0;
					res=AP_PREV;
					break;
				}  
			} 
			currentframe++; 
		} 
		audio_stop();
	}
	f_close(audiodev.file);
	myfree(SRAMIN,filterbuf64); 
	myfree(SRAMIN,apectrl);
	myfree(SRAMIN,apex->seektable);
	myfree(SRAMIN,apex);
	myfree(SRAMCCM,decoded0);
	myfree(SRAMCCM,decoded1);
	myfree(SRAMIN,audiodev.file);
	myfree(SRAMIN,audiodev.saibuf1);
	myfree(SRAMIN,audiodev.saibuf2); 
	myfree(SRAMIN,buffer);  
	return res;
}
























