#include "flacplay.h"
#include "sai.h"
#include "wm8978.h"
#include "malloc.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "audioplay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��������ֲ��RockBox��flac�����
//ALIENTEK STM32������
//FLAC �������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/29
//�汾��V1.0
//********************************************************************************
//V1.0 ˵��
//1,֧��16/24λ������/������flac�Ľ���
//2,���֧��192K/16bit��96K/24bit��flac����  
////////////////////////////////////////////////////////////////////////////////// 	 
 
__flacctrl * flacctrl;	//flac������ƽṹ��

 
//����FLAC�ļ�
//fx:flac�ļ�ָ��
//fc:flac��������
//����ֵ:0,�����ɹ�
//    ����,�������
u8 flac_init(FIL* fx,__flacctrl* fctrl,FLACContext* fc)
{
	FLAC_Tag * flactag;
	MD_Block_Head *flacblkh;
	u8 *buf; 
	u8 endofmetadata=0;			//���һ��metadata���
	int blocklength; 
	u32 br;
	u8 res;

	buf=mymalloc(SRAMEX,512);	//����512�ֽ��ڴ� SRAMIN
	if(!buf)return 1;			//�ڴ�����ʧ�� 
	f_lseek(fx,0);				//ƫ�Ƶ��ļ�ͷ
	f_read(fx,buf,4,&br);		//��ȡ4�ֽ� 
	flactag=(FLAC_Tag*)buf;		//ǿ��ת��Ϊflac tag��ǩ
	if(strncmp("fLaC",(char*)flactag->id,4)!=0) 
	{
		myfree(SRAMEX,buf);		//�ͷ��ڴ� SRAMIN
		return 2;				//��flac�ļ�
    } 
    while(!endofmetadata) 
	{
		f_read(fx,buf,4,&br);
        if(br<4)break;
		flacblkh=(MD_Block_Head*)buf;
		endofmetadata=flacblkh->head&0X80;	//�ж��ǲ������һ��block?
		blocklength=((u32)flacblkh->size[0]<<16)|((u16)flacblkh->size[1]<<8)|(flacblkh->size[2]);//�õ����С
        if((flacblkh->head&0x7f)==0) 		//head���7λΪ0,���ʾ��STREAMINFO��
        { 
			res=f_read(fx,buf,blocklength,&br);
            if(res!=FR_OK)break;  
            fc->min_blocksize=((u16)buf[0]<<8) |buf[1];					//��С���С
            fc->max_blocksize=((u16)buf[2]<<8) |buf[3];					//�����С
            fc->min_framesize=((u32)buf[4]<<16)|((u16)buf[5]<<8)|buf[6];//��С֡��С
            fc->max_framesize=((u32)buf[7]<<16)|((u16)buf[8]<<8)|buf[9];//���֡��С
            fc->samplerate=((u32)buf[10]<<12)|((u16)buf[11]<<4)|((buf[12]&0xf0)>>4);//������
            fc->channels=((buf[12]&0x0e)>>1)+1;							//��Ƶͨ����
            fc->bps=((((u16)buf[12]&0x01)<<4)|((buf[13]&0xf0)>>4))+1;	//����λ��16?24?32? 
            fc->totalsamples=((u32)buf[14]<<24)|((u32)buf[15]<<16)|((u16)buf[16]<<8)|buf[17];//һ���������ܲ�����
			fctrl->samplerate=fc->samplerate;
			fctrl->totsec=(fc->totalsamples/fc->samplerate);//�õ���ʱ�� 
        }else 	//��������֡�Ĵ��� 
		{ 
            if(f_lseek(fx,fx->fptr+blocklength)!=FR_OK)
            { 
				myfree(SRAMIN,buf);
				return 3;
            }
		}
    } 
	myfree(SRAMEX,buf);//�ͷ��ڴ�.SRAMIN
	if(fctrl->totsec)
	{
		fctrl->outsamples=fc->max_blocksize*2;//PCM���������(*2,��ʾ2��������������)
		fctrl->bps=fc->bps;			//����λ��(16/24/32)
		fctrl->datastart=fx->fptr;	//FLAC����֡��ʼ�ĵ�ַ
		fctrl->bitrate=((fx->obj.objsize-fctrl->datastart)*8)/fctrl->totsec;//�õ�FLAC��λ��
	}else return 4;	//��ʱ��Ϊ0?�������flac�ļ�
	return 0;
} 
vu8 flactransferend=0;	//sai������ɱ�־
vu8 flacwitchbuf=0;		//saibufxָʾ��־ 
//FLAC DMA���ͻص�����
void flac_sai_dma_tx_callback(void) 
{    
	u16 i;
	u16 size;
	if(DMA2_Stream3->CR&(1<<19))
	{
		flacwitchbuf=0;
		if((audiodev.status&0X01)==0)//��ͣ��,���0
		{ 
			if(flacctrl->bps==24)size=flacctrl->outsamples*4;
			else size=flacctrl->outsamples*2;
			for(i=0;i<size;i++)audiodev.saibuf1[i]=0;
		}
	}else 
	{
		flacwitchbuf=1;
		if((audiodev.status&0X01)==0)//��ͣ��,���0
		{
			if(flacctrl->bps==24)size=flacctrl->outsamples*4;
			else size=flacctrl->outsamples*2;
			for(i=0;i<size;i++)audiodev.saibuf2[i]=0;
		}
	} 
	flactransferend=1;
} 
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//flacctrl:flac���ſ�����
void flac_get_curtime(FIL*fx,__flacctrl *flacctrl)
{
	long long fpos=0;  	 
	if(fx->fptr>flacctrl->datastart)fpos=fx->fptr-flacctrl->datastart;	//�õ���ǰ�ļ����ŵ��ĵط� 
	flacctrl->cursec=fpos*flacctrl->totsec/(fx->obj.objsize-flacctrl->datastart);	//��ǰ���ŵ��ڶ�������?	
}
//flac�ļ�������˺���
//pos:��Ҫ��λ�����ļ�λ��
//����ֵ:��ǰ�ļ�λ��(����λ��Ľ��)
u32 flac_file_seek(u32 pos)
{
	if(pos>audiodev.file->obj.objsize)
	{
		pos=audiodev.file->obj.objsize;
	}
	f_lseek(audiodev.file,pos);
	return audiodev.file->fptr;
}
//����һ��FLAC����
//fname:FLAC�ļ�·��.
//����ֵ:0,�����������
//[b7]:0,����״̬;1,����״̬
//[b6:0]:b7=0ʱ,��ʾ������ 
//       b7=1ʱ,��ʾ�д���(���ﲻ�ж��������,0X80~0XFF,�����Ǵ���)
u8 flac_play_song(u8* fname) 
{ 
    FLACContext *fc=0; 
    int bytesleft;
    int consumed;
	u8 res=0;  
	u32 br=0; 
	u8* buffer=0;    
	u8* decbuf0=0;   
	u8* decbuf1=0;  
	u8* p8=0;    
	u32 flac_fptr=0; 
	
 	fc=mymalloc(SRAMEX,sizeof(FLACContext)); //SRAMIN
	flacctrl=mymalloc(SRAMEX,sizeof(__flacctrl)); //SRAMIN
	audiodev.file=(FIL*)mymalloc(SRAMEX,sizeof(FIL));//SRAMIN
	audiodev.file_seek=flac_file_seek;
	if(!fc||!audiodev.file||!flacctrl)res=1;//�ڴ��������
	else
	{ 
		memset(fc,0,sizeof(FLACContext));//fc������������ 
		res=f_open(audiodev.file,(char*)fname,FA_READ); //��ȡ�ļ����� 
		if(res==FR_OK)
		{
			res=flac_init(audiodev.file,flacctrl,fc);	//flac�����ʼ��   
			if(fc->min_blocksize==fc->max_blocksize&&fc->max_blocksize!=0)//����min_blocksize����max_blocksize
			{
				if(fc->bps==24)	//24λ��Ƶ����
				{	
					audiodev.saibuf1=mymalloc(SRAMEX,fc->max_blocksize*8);//SRAMIN
					audiodev.saibuf2=mymalloc(SRAMEX,fc->max_blocksize*8);//SRAMIN
				}else			//16λ��Ƶ����
				{
					audiodev.saibuf1=mymalloc(SRAMEX,fc->max_blocksize*4);//SRAMIN
					audiodev.saibuf2=mymalloc(SRAMEX,fc->max_blocksize*4);//SRAMIN 
				}
				buffer=mymalloc(SRAMCCM,fc->max_framesize); 	//�������֡���� 
				decbuf0=mymalloc(SRAMCCM,fc->max_blocksize*4);
				decbuf1=mymalloc(SRAMCCM,fc->max_blocksize*4);
			}else res+=1;//��֧�ֵ���Ƶ��ʽ  
		}
	}
	if(buffer&&audiodev.saibuf1&&audiodev.saibuf2&&decbuf0&&decbuf1&&res==0)
	{ 
		printf("\r\n  Blocksize: %d .. %d\r\n", fc->min_blocksize,fc->max_blocksize);
		printf("  Framesize: %d .. %d\r\n",fc->min_framesize,fc->max_framesize);
		printf("  Samplerate: %d\r\n", fc->samplerate);
		printf("  Channels: %d\r\n", fc->channels);
		printf("  Bits per sample: %d\r\n", fc->bps);
		printf("  Metadata length: %d\r\n", flacctrl->datastart);
		printf("  Total Samples: %lu\r\n",fc->totalsamples);
		printf("  Duration: %d s\r\n",flacctrl->totsec);
		printf("  Bitrate: %d kbps\r\n",flacctrl->bitrate); 
		if(flacctrl->bps==24)		//24λ��Ƶ����
		{
			WM8978_I2S_Cfg(2,2);	//�����ֱ�׼,24λ���ݳ���
			SAIA_Init(0,1,6);		//����SAI,������,24λ���� 
			SAIA_SampleRate_Set(fc->samplerate);	//���ò�����  
			SAIA_TX_DMA_Init(audiodev.saibuf1,audiodev.saibuf2,flacctrl->outsamples,2);//����TX DMA
			memset(audiodev.saibuf1,0,fc->max_blocksize*8);
			memset(audiodev.saibuf2,0,fc->max_blocksize*8);
		}else						//16λ��Ƶ����
		{
			WM8978_I2S_Cfg(2,0);	//�����ֱ�׼,16λ���ݳ���
			SAIA_Init(0,1,4);		//����SAI,������,16λ����
			SAIA_SampleRate_Set(fc->samplerate);	//���ò�����    
			SAIA_TX_DMA_Init(audiodev.saibuf1,audiodev.saibuf2,flacctrl->outsamples,1);//����TX DMA			
			memset(audiodev.saibuf1,0,fc->max_blocksize*4);
			memset(audiodev.saibuf2,0,fc->max_blocksize*4);
		}  
		sai_tx_callback=flac_sai_dma_tx_callback;	//�ص�����ָ��flac_sai_dma_tx_callback
		f_read(audiodev.file,buffer,fc->max_framesize,&br);//��ȡ���֡������		
		bytesleft=br;
		audio_start();					//��ʼ����  
		fc->decoded0=(int*)decbuf0;		//��������0
		fc->decoded1=(int*)decbuf1;  	//��������1 
		flac_fptr=audiodev.file->fptr;	//��¼��ǰ���ļ�λ��.
		while(bytesleft) 
		{   
			while(flactransferend==0)//�ȴ��������
			{ 
				delay_ms(1000/OS_TICKS_PER_SEC);
			};	
			if(flac_fptr!=audiodev.file->fptr)//˵���ⲿ�н����ļ����/���˲���
			{
				if(audiodev.file->fptr<flacctrl->datastart)//�����ݿ�ʼ֮ǰ??
				{
					f_lseek(audiodev.file,flacctrl->datastart);//ƫ�Ƶ����ݿ�ʼ�ĵط�
				} 
				f_read(audiodev.file,buffer,fc->max_framesize,&br); //��ȡһ�����֡��������
				bytesleft=flac_seek_frame(buffer,br,fc);		//����֡ 
				if(bytesleft>=0)								//�ҵ���ȷ��֡ͷ.
				{
					f_lseek(audiodev.file,audiodev.file->fptr-fc->max_framesize+bytesleft);
					f_read(audiodev.file,buffer,fc->max_framesize,&br); 
				}else printf("flac seek error:%d\r\n",bytesleft); 
				bytesleft=br;
			}
			flactransferend=0;  
			if(flacwitchbuf==0)p8=audiodev.saibuf1;
			else p8=audiodev.saibuf2; 
			if(fc->bps==24)res=flac_decode_frame24(fc,buffer,bytesleft,(s32*)p8);
			else res=flac_decode_frame16(fc,buffer,bytesleft,(s16*)p8); 
			if(res!=0)//��������� 
			{
				res=AP_ERR;
				break;
			} 
			consumed=fc->gb.index/8;
			memmove(buffer,&buffer[consumed],bytesleft-consumed);
			bytesleft-=consumed; 
			res=f_read(audiodev.file,&buffer[bytesleft],fc->max_framesize-bytesleft,&br); 
			if(res)//�����ݳ�����
			{
				res=AP_ERR;
				break;
			}
			if(br>0) 
			{
				bytesleft+=br;
			}
			flac_fptr=audiodev.file->fptr;	//��¼��ǰ���ļ�λ��.
			while(audiodev.status&(1<<1))	//����������
			{		  
				flac_get_curtime(audiodev.file,flacctrl);//�õ���ʱ��͵�ǰ���ŵ�ʱ�� 
				audiodev.totsec=flacctrl->totsec;		//��������
				audiodev.cursec=flacctrl->cursec;
				audiodev.bitrate=flacctrl->bitrate;
				audiodev.samplerate=flacctrl->samplerate;
				audiodev.bps=flacctrl->bps;	
  				if(audiodev.status&0X01)break;	//û�а�����ͣ 
				else delay_ms(1000/OS_TICKS_PER_SEC);
			}
			if((audiodev.status&(1<<1))==0)		//�����������/�������
			{  
				break;
			} 	 
		} 
		audio_stop();
	}else res=AP_ERR;
	f_close(audiodev.file);
	myfree(SRAMEX,fc);                  //SRAMIN
	myfree(SRAMEX,flacctrl);            //SRAMIN
	myfree(SRAMEX,audiodev.file);       //SRAMIN
	myfree(SRAMEX,audiodev.saibuf1);    //SRAMIN
	myfree(SRAMEX,audiodev.saibuf2);    //SRAMIN
	myfree(SRAMCCM,buffer);
	myfree(SRAMCCM,decbuf0);
	myfree(SRAMCCM,decbuf1); 
    return res;
} 









































