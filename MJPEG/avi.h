#ifndef __AVI_H
#define __AVI_H 
#include "sys.h" 
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

//��������
typedef enum {
	AVI_OK=0,				//0,�ɹ�
	AVI_RIFF_ERR,			//1,RIFF ID��ȡʧ��
	AVI_AVI_ERR,			//2,AVI ID��ȡʧ��
	AVI_LIST_ERR,			//3,LIST ID��ȡʧ��
	AVI_HDRL_ERR,			//4,HDRL ID��ȡʧ��
	AVI_AVIH_ERR,			//5,AVIH ID��ȡʧ��
	AVI_STRL_ERR,			//6,STRL ID��ȡʧ��
	AVI_STRH_ERR,			//7,STRH ID��ȡʧ��
	AVI_STRF_ERR,			//8,STRF ID��ȡʧ��
	AVI_MOVI_ERR,			//9,MOVI ID��ȡʧ��
	AVI_FORMAT_ERR,			//10,��ʽ����
	AVI_STREAM_ERR,			//11,������
}AVISTATUS;



#define AVI_RIFF_ID			0X46464952  
#define AVI_AVI_ID			0X20495641
#define AVI_LIST_ID			0X5453494C  
#define AVI_HDRL_ID			0X6C726468		//��Ϣ���־
#define AVI_MOVI_ID			0X69766F6D 		//���ݿ��־
#define AVI_STRL_ID			0X6C727473		//strl��־

#define AVI_AVIH_ID			0X68697661 		//avih�ӿ��AVI_HDRL_ID
#define AVI_STRH_ID			0X68727473		//strh(��ͷ)�ӿ��AVI_STRL_ID
#define AVI_STRF_ID			0X66727473 		//strf(����ʽ)�ӿ��AVI_STRL_ID
#define AVI_STRD_ID			0X64727473 		//strd�ӿ��AVI_STRL_ID (��ѡ��)

#define AVI_VIDS_STREAM		0X73646976		//��Ƶ��
#define AVI_AUDS_STREAM		0X73647561 		//��Ƶ��


#define AVI_VIDS_FLAG		0X6463			//��Ƶ����־
#define AVI_AUDS_FLAG		0X7762 			//��Ƶ����־
//////////////////////////////////////////////////////////////////////////////////////////

#define AVI_FORMAT_MJPG		0X47504A4D  


//AVI ��Ϣ�ṹ��
//��һЩ��Ҫ������,���������,�������
typedef __packed struct
{	  
	u32 SecPerFrame;		//��Ƶ֡���ʱ��(��λΪus)
	u32 TotalFrame;			//�ļ���֡��
	u32 Width;				//ͼ���
	u32 Height;				//ͼ���
	u32 SampleRate; 		//��Ƶ������
	u16 Channels;	  		//������,һ��Ϊ2,��ʾ������ 
	u16 AudioBufSize;		//��Ƶ��������С
	u16 AudioType;	  		//��Ƶ����:0X0001=PCM;0X0050=MP2;0X0055=MP3;0X2000=AC3;
	u16 StreamID;			//������ID,StreamID=='dc'==0X6463 /StreamID=='wb'==0X7762
	u32 StreamSize;			//����С,������ż��,�����ȡ��Ϊ����,���1.��Ϊż��.
	u8* VideoFLAG;			//��Ƶ֡���,VideoFLAG="00dc"/"01dc"
	u8* AudioFLAG;			//��Ƶ֡���,AudioFLAG="00wb"/"01wb"
}AVI_INFO;

extern AVI_INFO avix;		//avi�ļ������Ϣ

//////////////////////////////////////////////////////////////////////////////////////////
//AVI ����Ϣ
typedef struct
{	
	u32 RiffID;				//RiffID=='RIFF'==0X61766968
	u32 FileSize;			//AVI�ļ���С(�����������8�ֽ�,ҲRIFFID��FileSize����������)
	u32 AviID;				//AviID=='AVI '==0X41564920 
}AVI_HEADER;

//AVI ����Ϣ
typedef struct
{	
	u32 FrameID;			//֡ID,FrameID=='RIFF'==0X61766968
	u32 FrameSize;			//֡��С 
}FRAME_HEADER;


//LIST ����Ϣ
typedef struct
{	
	u32 ListID;				//ListID=='LIST'==0X4c495354
	u32 BlockSize;			//���С(�����������8�ֽ�,ҲListID��BlockSize����������)
	u32 ListType;			//LIST�ӿ�����:hdrl(��Ϣ��)/movi(���ݿ�)/idxl(������,�Ǳ���,�ǿ�ѡ��)
}LIST_HEADER;

//avih �ӿ���Ϣ
typedef struct
{	
	u32 BlockID;			//���־:avih==0X61766968
	u32 BlockSize;			//���С(�����������8�ֽ�,Ҳ����BlockID��BlockSize����������)
	u32 SecPerFrame;		//��Ƶ֡���ʱ��(��λΪus)
	u32 MaxByteSec; 		//������ݴ�����,�ֽ�/��
	u32 PaddingGranularity; //������������
	u32 Flags;				//AVI�ļ���ȫ�ֱ�ǣ������Ƿ����������
	u32 TotalFrame;			//�ļ���֡��
	u32 InitFrames;  		//Ϊ������ʽָ����ʼ֡�����ǽ�����ʽӦ��ָ��Ϊ0��
	u32 Streams;			//�������������������,ͨ��Ϊ2
	u32 RefBufSize;			//�����ȡ���ļ��Ļ����С��Ӧ���������Ŀ飩Ĭ�Ͽ�����1M�ֽ�!!!
	u32 Width;				//ͼ���
	u32 Height;				//ͼ���
	u32 Reserved[4];		//����
}AVIH_HEADER;

//strh ��ͷ�ӿ���Ϣ(strh��strl)
typedef struct
{	
	u32 BlockID;			//���־:strh==0X73747268
	u32 BlockSize;			//���С(�����������8�ֽ�,Ҳ����BlockID��BlockSize����������)
	u32 StreamType;			//���������࣬vids(0X73646976):��Ƶ;auds(0X73647561):��Ƶ
	u32 Handler;			//ָ�����Ĵ����ߣ���������Ƶ��˵���ǽ�����,����MJPG/H264֮���.
	u32 Flags;  			//��ǣ��Ƿ�����������������ɫ���Ƿ�仯��
	u16 Priority;			//�������ȼ������ж����ͬ���͵���ʱ���ȼ���ߵ�ΪĬ������
	u16 Language;			//��Ƶ�����Դ���
	u32 InitFrames;  		//Ϊ������ʽָ����ʼ֡��
	u32 Scale;				//������, ��Ƶÿ��Ĵ�С������Ƶ�Ĳ�����С
	u32 Rate; 				//Scale/Rate=ÿ�������
	u32 Start;				//��������ʼ���ŵ�λ�ã���λΪScale
	u32 Length;				//������������������λΪScale
 	u32 RefBufSize;  		//����ʹ�õĻ�������С
    u32 Quality;			//��ѹ������������ֵԽ������Խ��
	u32 SampleSize;			//��Ƶ��������С
	struct					//��Ƶ֡��ռ�ľ��� 
	{				
	   	short Left;
		short Top;
		short Right;
		short Bottom;
	}Frame;				
}STRH_HEADER;

//BMP�ṹ��
typedef struct
{
	u32	 BmpSize;			//bmp�ṹ���С,����(BmpSize����)
 	long Width;				//ͼ���
	long Height;			//ͼ���
	u16  Planes;			//ƽ����������Ϊ1
	u16  BitCount;			//����λ��,0X0018��ʾ24λ
	u32  Compression;		//ѹ�����ͣ�����:MJPG/H264��
	u32  SizeImage;			//ͼ���С
	long XpixPerMeter;		//ˮƽ�ֱ���
	long YpixPerMeter;		//��ֱ�ֱ���
	u32  ClrUsed;			//ʵ��ʹ���˵�ɫ���е���ɫ��,ѹ����ʽ�в�ʹ��
	u32  ClrImportant;		//��Ҫ����ɫ
}BMP_HEADER;

//��ɫ��
typedef struct 
{
	u8  rgbBlue;			//��ɫ������(ֵ��ΧΪ0-255)
	u8  rgbGreen; 			//��ɫ������(ֵ��ΧΪ0-255)
	u8  rgbRed; 			//��ɫ������(ֵ��ΧΪ0-255)
	u8  rgbReserved;		//����������Ϊ0
}AVIRGBQUAD;

//����strh,�������Ƶ��,strf(����ʽ)ʹSTRH_BMPHEADER��
typedef struct 
{
	u32 BlockID;			//���־,strf==0X73747266
	u32 BlockSize;			//���С(�����������8�ֽ�,Ҳ����BlockID�ͱ�BlockSize����������)
	BMP_HEADER bmiHeader;  	//λͼ��Ϣͷ
	AVIRGBQUAD bmColors[1];	//��ɫ��
}STRF_BMPHEADER;  

//����strh,�������Ƶ��,strf(����ʽ)ʹSTRH_WAVHEADER��
typedef struct 
{
	u32 BlockID;			//���־,strf==0X73747266
	u32 BlockSize;			//���С(�����������8�ֽ�,Ҳ����BlockID�ͱ�BlockSize����������)
   	u16 FormatTag;			//��ʽ��־:0X0001=PCM,0X0055=MP3...
	u16 Channels;	  		//������,һ��Ϊ2,��ʾ������
	u32 SampleRate; 		//��Ƶ������
	u32 BaudRate;   		//������ 
	u16 BlockAlign; 		//���ݿ�����־
	u16 Size;				//�ýṹ��С
}STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(u16)(((u16)*((u8*)(ptr))<<8)|(u16)*(u8*)((ptr)+1))
#define  MAKEDWORD(ptr)	(u32)(((u16)*(u8*)(ptr)|(((u16)*(u8*)(ptr+1))<<8)|\
						(((u16)*(u8*)(ptr+2))<<16)|(((u16)*(u8*)(ptr+3))<<24))) 


AVISTATUS avi_init(u8 *buf,u16 size);		//��ʼ��avi������
u16 avi_srarch_id(u8* buf,u16 size,u8* id);	//����ID,ID������4���ֽڳ���
AVISTATUS avi_get_streaminfo(u8* buf);		//��ȡ����Ϣ


u8 Avih_Parser(u8 *buffer);
u8 Strl_Parser(u8 *buffer);
u8 Strf_Parser(u8 *buffer);



u16 Search_Movi(u8* buffer);
u16 Search_Fram(u8* buffer);
u32 ReadUnit(u8 *buffer,u8 index,u8 Bytes,u8 Format);

#endif

