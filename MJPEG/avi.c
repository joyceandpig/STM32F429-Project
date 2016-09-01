#include "avi.h"
#include "mjpeg.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//AVI��Ƶ��ʽ���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

AVI_INFO avix;					//avi�ļ������Ϣ 
u8*const AVI_VIDS_FLAG_TBL[2]={"00dc","01dc"};//��Ƶ�����־�ַ���,00dc/01dc
u8*const AVI_AUDS_FLAG_TBL[2]={"00wb","01wb"};//��Ƶ�����־�ַ���,00wb/01wb

//avi�����ʼ��
//buf:���뻺����
//size:��������С
//����ֵ:AVI_OK,avi�ļ������ɹ�
//         ����,�������
AVISTATUS avi_init(u8 *buf,u16 size)		 
{
	u16 offset;
	u8 *tbuf;
	AVISTATUS res=AVI_OK;
	AVI_HEADER *aviheader;
	LIST_HEADER *listheader;
	AVIH_HEADER *avihheader; 
	STRH_HEADER *strhheader; 
	
	STRF_BMPHEADER *bmpheader; 
	STRF_WAVHEADER *wavheader; 
	
	tbuf=buf;
	aviheader=(AVI_HEADER*)buf; 
	if(aviheader->RiffID!=AVI_RIFF_ID)return AVI_RIFF_ERR;		//RIFF ID����
	if(aviheader->AviID!=AVI_AVI_ID)return AVI_AVI_ERR;			//AVI ID����
	buf+=sizeof(AVI_HEADER);									//ƫ��
	listheader=(LIST_HEADER*)(buf);						
	if(listheader->ListID!=AVI_LIST_ID)return AVI_LIST_ERR;		//LIST ID���� 
	if(listheader->ListType!=AVI_HDRL_ID)return AVI_HDRL_ERR;	//HDRL ID���� 
	buf+=sizeof(LIST_HEADER);									//ƫ��
	avihheader=(AVIH_HEADER*)(buf);
	if(avihheader->BlockID!=AVI_AVIH_ID)return AVI_AVIH_ERR;	//AVIH ID���� 
	avix.SecPerFrame=avihheader->SecPerFrame;					//�õ�֡���ʱ��
	avix.TotalFrame=avihheader->TotalFrame;						//�õ���֡��  
	buf+=avihheader->BlockSize+8;								//ƫ��
	listheader=(LIST_HEADER*)(buf);
	if(listheader->ListID!=AVI_LIST_ID)return AVI_LIST_ERR;		//LIST ID���� 
	if(listheader->ListType!=AVI_STRL_ID)return AVI_STRL_ERR;	//STRL ID����  
	strhheader=(STRH_HEADER*)(buf+12);
	if(strhheader->BlockID!=AVI_STRH_ID)return AVI_STRH_ERR;	//STRH ID���� 
 	if(strhheader->StreamType==AVI_VIDS_STREAM)					//��Ƶ֡��ǰ
	{
		if(strhheader->Handler!=AVI_FORMAT_MJPG)return AVI_FORMAT_ERR;	//��MJPG��Ƶ��,��֧��
		avix.VideoFLAG=(u8*)AVI_VIDS_FLAG_TBL[0];					//��Ƶ�����  "00dc"
		avix.AudioFLAG=(u8*)AVI_AUDS_FLAG_TBL[1];					//��Ƶ�����  "01wb"
		bmpheader=(STRF_BMPHEADER*)(buf+12+strhheader->BlockSize+8);//strf
		if(bmpheader->BlockID!=AVI_STRF_ID)return AVI_STRF_ERR;		//STRF ID����  
		avix.Width=bmpheader->bmiHeader.Width;
		avix.Height=bmpheader->bmiHeader.Height; 
		buf+=listheader->BlockSize+8;								//ƫ��
		listheader=(LIST_HEADER*)(buf);
		if(listheader->ListID!=AVI_LIST_ID)//�ǲ�������Ƶ֡����Ƶ�ļ�
		{
			avix.SampleRate=0;						//��Ƶ������
			avix.Channels=0;						//��Ƶͨ����
			avix.AudioType=0;						//��Ƶ��ʽ
			
		}else
		{			
			if(listheader->ListType!=AVI_STRL_ID)return AVI_STRL_ERR;	//STRL ID����   
			strhheader=(STRH_HEADER*)(buf+12);
			if(strhheader->BlockID!=AVI_STRH_ID)return AVI_STRH_ERR;	//STRH ID���� 
			if(strhheader->StreamType!=AVI_AUDS_STREAM)return AVI_FORMAT_ERR;//��ʽ����
			wavheader=(STRF_WAVHEADER*)(buf+12+strhheader->BlockSize+8);//strf
			if(wavheader->BlockID!=AVI_STRF_ID)return AVI_STRF_ERR;		//STRF ID����   
			avix.SampleRate=wavheader->SampleRate;						//��Ƶ������
			avix.Channels=wavheader->Channels;							//��Ƶͨ����
			avix.AudioType=wavheader->FormatTag;						//��Ƶ��ʽ
		}
	}else if(strhheader->StreamType==AVI_AUDS_STREAM)		 		//��Ƶ֡��ǰ
	{ 
		avix.VideoFLAG=(u8*)AVI_VIDS_FLAG_TBL[1];					//��Ƶ�����  "01dc"
		avix.AudioFLAG=(u8*)AVI_AUDS_FLAG_TBL[0];					//��Ƶ�����  "00wb"
		wavheader=(STRF_WAVHEADER*)(buf+12+strhheader->BlockSize+8);//strf
		if(wavheader->BlockID!=AVI_STRF_ID)return AVI_STRF_ERR;		//STRF ID���� 
		avix.SampleRate=wavheader->SampleRate;						//��Ƶ������
		avix.Channels=wavheader->Channels;							//��Ƶͨ����
		avix.AudioType=wavheader->FormatTag;						//��Ƶ��ʽ
		buf+=listheader->BlockSize+8;								//ƫ��
		listheader=(LIST_HEADER*)(buf);
		if(listheader->ListID!=AVI_LIST_ID)return AVI_LIST_ERR;		//LIST ID���� 
		if(listheader->ListType!=AVI_STRL_ID)return AVI_STRL_ERR;	//STRL ID����   
		strhheader=(STRH_HEADER*)(buf+12);
		if(strhheader->BlockID!=AVI_STRH_ID)return AVI_STRH_ERR;	//STRH ID���� 
		if(strhheader->StreamType!=AVI_VIDS_STREAM)return AVI_FORMAT_ERR;//��ʽ����  
		bmpheader=(STRF_BMPHEADER*)(buf+12+strhheader->BlockSize+8);//strf
		if(bmpheader->BlockID!=AVI_STRF_ID)return AVI_STRF_ERR;		//STRF ID����  
		if(bmpheader->bmiHeader.Compression!=AVI_FORMAT_MJPG)return AVI_FORMAT_ERR;//��ʽ����  
		avix.Width=bmpheader->bmiHeader.Width;
		avix.Height=bmpheader->bmiHeader.Height; 	
	}
	offset=avi_srarch_id(tbuf,size,"movi");					//����movi ID
	if(offset==0)return AVI_MOVI_ERR;						//MOVI ID����
	if(avix.SampleRate)//����Ƶ��,�Ų���
	{
		tbuf+=offset;
		offset=avi_srarch_id(tbuf,size,avix.AudioFLAG);			//������Ƶ�����
		if(offset==0)return AVI_STREAM_ERR;						//������
		tbuf+=offset+4;
		avix.AudioBufSize=*((u16*)tbuf);						//�õ���Ƶ��buf��С.
	}		
	printf("avi init ok\r\n");
	printf("avix.SecPerFrame:%d\r\n",avix.SecPerFrame);
	printf("avix.TotalFrame:%d\r\n",avix.TotalFrame);
	printf("avix.Width:%d\r\n",avix.Width);
	printf("avix.Height:%d\r\n",avix.Height);
	printf("avix.AudioType:%d\r\n",avix.AudioType);
	printf("avix.SampleRate:%d\r\n",avix.SampleRate);
	printf("avix.Channels:%d\r\n",avix.Channels);
	printf("avix.AudioBufSize:%d\r\n",avix.AudioBufSize);
	printf("avix.VideoFLAG:%s\r\n",avix.VideoFLAG); 
	printf("avix.AudioFLAG:%s\r\n",avix.AudioFLAG); 
	return res;
}

//���� ID
//buf:���黺����
//size:�����С
//id:Ҫ���ҵ�id,������4�ֽڳ���
//����ֵ:0,����ʧ��,����:movi IDƫ����
u16 avi_srarch_id(u8* buf,u16 size,u8 *id)
{
	u16 i;
	size-=4;
	for(i=0;i<size;i++)
	{
	   	if(buf[i]==id[0])
			if(buf[i+1]==id[1])
				if(buf[i+2]==id[2])	
					if(buf[i+3]==id[3])return i;//�ҵ�"id"���ڵ�λ��	
	}
	return 0;		
}
//�õ�stream����Ϣ
//buf:����ʼ��ַ(������01wb/00wb/01dc/00dc��ͷ)
AVISTATUS avi_get_streaminfo(u8* buf)
{
	avix.StreamID=MAKEWORD(buf+2);			//�õ�������
	avix.StreamSize=MAKEDWORD(buf+4);		//�õ�����С 
	if(avix.StreamSize%2)avix.StreamSize++;	//������1(avix.StreamSize,������ż��)
	if(avix.StreamID==AVI_VIDS_FLAG||avix.StreamID==AVI_AUDS_FLAG)return AVI_OK;
	return AVI_STREAM_ERR;	
}




















