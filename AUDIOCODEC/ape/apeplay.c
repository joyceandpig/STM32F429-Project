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
//��������ֲ��RockBox��ape�����
//ALIENTEK STM32������
//APE �������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/29
//�汾��V1.1
//********************************************************************************
//V1.0 ˵��
//1,֧��16λ������/������ape�Ľ���
//2,���֧��96K��APE��ʽ(LV1��LV2)
//3,LV1~LV3,��48K��������������,LV4,LV5��.
//4,��ĳЩape�ļ�,���ܲ�֧��,����Monkey's Audio�������ת��һ��,������������
////////////////////////////////////////////////////////////////////////////////// 	 
 
__apectrl * apectrl;	//APE���ſ��ƽṹ��

//apedecoder.c������Ҫ������ 
extern filter_int *filterbuf64;		//��Ҫ2816�ֽ� 
	
vu8 apetransferend=0;	//sai������ɱ�־
vu8 apewitchbuf=0;		//saibufxָʾ��־

//APE DMA���ͻص�����
void ape_sai_dma_tx_callback(void) 
{    
	u16 i;
	if(DMA2_Stream3->CR&(1<<19))
	{
		apewitchbuf=0;
		if((audiodev.status&0X01)==0)//��ͣ��,���0
		{
			for(i=0;i<APE_BLOCKS_PER_LOOP*4;i++)audiodev.saibuf1[i]=0;
		}
	}else 
	{
		apewitchbuf=1;
		if((audiodev.status&0X01)==0)//��ͣ��,���0
		{
			for(i=0;i<APE_BLOCKS_PER_LOOP*4;i++)audiodev.saibuf2[i]=0;
		}
	} 
	apetransferend=1;
} 
//���PCM���ݵ�DAC
//buf:PCM�����׵�ַ
//size:pcm������(32λΪ��λ) 
void ape_fill_buffer(u32* buf,u16 size)
{
	u16 i; 
	u32 *p;
	while(apetransferend==0)delay_ms(1000/OS_TICKS_PER_SEC);//�ȴ��������
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
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//apectrl:apectrl���ſ�����
void ape_get_curtime(FIL*fx,__apectrl *apectrl)
{
	long long fpos=0;  	 
	if(fx->fptr>apectrl->datastart)fpos=fx->fptr-apectrl->datastart;	//�õ���ǰ�ļ����ŵ��ĵط� 
	apectrl->cursec=fpos*apectrl->totsec/(fx->obj.objsize-apectrl->datastart);	//��ǰ���ŵ��ڶ�������?	
}  
//ape�ļ�������˺���
//pos:��Ҫ��λ�����ļ�λ��
//����ֵ:��ǰ�ļ�λ��(����λ��Ľ��)
u32 ape_file_seek(u32 pos)
{ 
	return audiodev.file->fptr;//ape�ļ���֧�ֿ������,ֱ�ӷ��ص�ǰ����λ��
}
//����һ��FLAC����
//fname:FLAC�ļ�·��.
//����ֵ:0,�����������
//[b7]:0,����״̬;1,����״̬
//[b6:0]:b7=0ʱ,��ʾ������ 
//       b7=1ʱ,��ʾ�д���(���ﲻ�ж��������,0X80~0XFF,�����Ǵ���)
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
	if(filterbuf64&&apectrl&&apex&&decoded0&&decoded1&&audiodev.file&&audiodev.saibuf1&&audiodev.saibuf2&&buffer)//�����ڴ����붼OK.
	{ 
		memset(apex,0,sizeof(struct ape_ctx_t));
		memset(apectrl,0,sizeof(__apectrl));
		memset(audiodev.saibuf1,0,APE_BLOCKS_PER_LOOP*4);
		memset(audiodev.saibuf2,0,APE_BLOCKS_PER_LOOP*4);		
		f_open(audiodev.file,(char*)fname,FA_READ);		//���ļ�
		res=ape_parseheader(audiodev.file,apex);//����ape�ļ�ͷ 
		if(res==0)
		{  
			if((apex->compressiontype>3000)||(apex->fileversion<APE_MIN_VERSION)||(apex->fileversion>APE_MAX_VERSION||apex->bps!=16))
			{
				res=AP_ERR;//ѹ���ʲ�֧��/�汾��֧��/����16λ��Ƶ��ʽ
			}else
			{
				apectrl->bps=apex->bps;					//�õ��������(ape,���ǽ�֧��16λ)
				apectrl->samplerate=apex->samplerate;	//�õ�������,ape֧��48Khz���µĲ�����,�ڸ߾Ϳ��ܿ���...
				if(apex->totalframes>1)totalsamples=apex->finalframeblocks+apex->blocksperframe*(apex->totalframes-1);
				else totalsamples=apex->finalframeblocks;
				apectrl->totsec=totalsamples/apectrl->samplerate;//�õ��ļ���ʱ�� 
				apectrl->bitrate=(audiodev.file->obj.objsize-apex->firstframe)*8/apectrl->totsec;//�õ�λ��
				apectrl->outsamples=APE_BLOCKS_PER_LOOP*2;//PCM���������(16λΪ��λ) 
				apectrl->datastart=apex->firstframe;	//�õ���һ֡�ĵ�ַ	 
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
 		WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
		SAIA_Init(0,1,4);		//����SAI,������,16λ����
		SAIA_SampleRate_Set(apex->samplerate);		//���ò����� 
		SAIA_TX_DMA_Init(audiodev.saibuf1,audiodev.saibuf2,APE_BLOCKS_PER_LOOP*2,1);//����TX DMA 
		sai_tx_callback=ape_sai_dma_tx_callback;	//�ص�����ָ��mp3_sai_dma_tx_callback				
 		currentframe=0; 
		f_lseek(audiodev.file,apex->firstframe); 
		res=f_read(audiodev.file,buffer,APE_FILE_BUF_SZ,(u32*)&bytesinbuffer);	
		firstbyte=3;  		//Take account of the little-endian 32-bit byte ordering
		readptr=buffer;
		audio_start(); 
		while(currentframe<apex->totalframes&&res==0)//����δ�����֡?
		{ 
			//����һ֡�����ж��ٸ�blocks?
			if(currentframe==(apex->totalframes-1))nblocks=apex->finalframeblocks;
			else nblocks=apex->blocksperframe; 
			apex->currentframeblocks=nblocks; 
			//��ʼ��֡����
			init_frame_decoder(apex,readptr,&firstbyte,&bytesconsumed);
			readptr+=bytesconsumed;
			bytesinbuffer-=bytesconsumed; 						
			while(nblocks>0)//��ʼ֡����
			{
				blockstodecode=AUDIO_MIN(APE_BLOCKS_PER_LOOP,nblocks);//���һ��Ҫ�����blocks����  
				res=decode_chunk(apex,readptr,&firstbyte,&bytesconsumed,decoded0,decoded1,blockstodecode);
				if(res!=0)
				{
					printf("frame decode err\r\n");
					res=AP_ERR;
					break;
				} 
				ape_fill_buffer((u32*)decoded1,APE_BLOCKS_PER_LOOP);   
				readptr+=bytesconsumed;			//����ָ��ƫ�Ƶ�������λ��
				bytesinbuffer-=bytesconsumed; 	//buffer���������������
				if(bytesconsumed>4*APE_BLOCKS_PER_LOOP)//���ִ�����
				{
					nblocks=0;
					res=AP_ERR;
					printf("bytesconsumed:%d\r\n",bytesconsumed);
				}
				if(bytesinbuffer<4*APE_BLOCKS_PER_LOOP)//��Ҫ������������
				{ 
					memmove(buffer,readptr,bytesinbuffer);
					res=f_read(audiodev.file,buffer+bytesinbuffer,APE_FILE_BUF_SZ-bytesinbuffer,(u32*)&n);
					if(res)//������.
					{ 
						res=AP_ERR;
						break; 
					}
					bytesinbuffer+=n;  
					readptr=buffer;
				} 
				nblocks-=blockstodecode;//block�����ݼ�
				while(audiodev.status&(1<<1))//����������
				{		 
					ape_get_curtime(audiodev.file,apectrl);//�õ���ʱ��͵�ǰ���ŵ�ʱ�� 
					audiodev.totsec=apectrl->totsec;		//��������
					audiodev.cursec=apectrl->cursec;
					audiodev.bitrate=apectrl->bitrate;
					audiodev.samplerate=apectrl->samplerate;
					audiodev.bps=apectrl->bps;	 
					delay_ms(1000/OS_TICKS_PER_SEC);  
					if(audiodev.status&0X01)break;	//û�а�����ͣ   
				}
				if((audiodev.status&(1<<1))==0)		//�����������/�������
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
























