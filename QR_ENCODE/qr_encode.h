#ifndef __QR_ENCODE_H
#define __QR_ENCODE_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//��������ֲ����������Ķ�ά�����ɹ���
//ԭ������:http://www.openedv.com/forum.php?mod=viewthread&tid=58283
//ALIENTEK STM32������
//��ά����� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/2/28
//�汾��V1.0  			  
////////////////////////////////////////////////////////////////////////////////// 	 
 
 
#define QR_TRUE 			1		//TRUE����
#define QR_FALSE 			0 		//FALSE����


#define min(a,b)	(((a) < (b)) ? (a) : (b))

//����ȼ�(4��)
#define QR_LEVEL_L			0		//7%������ɱ�����
#define QR_LEVEL_M			1		//15%������ɱ�����
#define QR_LEVEL_Q			2		//25%������ɱ�����
#define QR_LEVEL_H			3		//30%������ɱ�����

//������ʽ
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3
#define QR_MODE_CHINESE		4

//Number of bits per length field
//Encoding  Ver.1�C9      10�C26      27�C40
//Numeric		10			12			14
//Alphanumeric	9			11			13
//u8			8			16			16
//Kanji			8			10			12 
//Chinese		
//P17 �ַ�����ָʾ��λ��
#define QR_VRESION_S		0 
#define QR_VRESION_M		1 
#define QR_VRESION_L		2 

#define QR_MARGIN			4
#define	QR_VER1_SIZE		21		// �汾��������

#define MAX_ALLCODEu32		3706	//400// //P14,P35 ��������[����]* (E) (VER:40), ��������Ϊ8λ
#define MAX_DATACODEu32		2956	//400// //P27     �����Ϣ����(Ver��40-L)����������Ϊ8λ
#define MAX_CODEBLOCK		153		//���������� Ver��36.37.38_L_�ڶ���
#define MAX_MODULESIZE		177		// 21:Version=1,����ַ�=17(8.5������)
									// 25:Version=2,����ַ�=32(16������)
									// 29:Version=3,����ַ�=49(24.5������)
									// 33:Version=4,����ַ�=78(39������)
									// 37:Version=5,����ַ�=106(53������) 
									// 41:Version=6,����ַ�=134(67������)
									// 45:Version=7,����ַ�=154(77������)
									// 49:Version=8,����ַ�=192(96������)
									// 53:
									//P14 ÿ�ߵ�ģ������A�� (VER:40   ) Ver��40 = 21+��Ver-1��*4
//QR ENCODE������
typedef struct  
{		 	 
	int m_nSymbleSize;
	u8 m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE];

	int m_ncDataCodeu32Bit;
	u8 m_byDataCodeu32[MAX_DATACODEu32];

	int m_ncDataBlock;
	u8 m_byBlockMode[MAX_DATACODEu32];
	u8 m_nBlockLength[MAX_DATACODEu32];

	int m_ncAllCodeu32; 
	u8 m_byAllCodeu32[MAX_ALLCODEu32]; 
	u8 m_byRSWork[MAX_CODEBLOCK];

	int m_nLevel;	
	int m_nVersion;	
	u8 m_bAutoExtent;	
	int m_nMaskingNo; 
}_qr_encode; 	  

extern _qr_encode *qrx;  				//��QR_ENCODE.C���涨��.ʹ��ǰ,���������ڴ�

u8 EncodeData(char *lpsSource);
int GetEncodeVersion(int nVersion, char *lpsSource, int ncLength); 
int EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup); 
int GetBitLength(u8 nMode, int ncData, int nVerGroup); 
int SetBitStream(int nIndex, u32 wData, int ncData); 
u8 IsNumeralData(unsigned char c);
u8 IsAlphabetData(unsigned char c);
u8 IsKanjiData(unsigned char c1, unsigned char c2);
u8 IsChineseData(unsigned char c1, unsigned char c2); 
u8 AlphabetToBinaly(unsigned char c);
u32 KanjiToBinaly(u32 wc);
u32 ChineseToBinaly(u32 wc);
void GetRSCodeu32(u8 *lpbyRSWork, int ncDataCodeu32, int ncRSCodeu32); 
void FormatModule(void); 
void SetFunctionModule(void);
void SetFinderPattern(int x, int y);
void SetAlignmentPattern(int x, int y);
void SetVersionPattern(void);
void SetCodeu32Pattern(void);
void SetMaskingPattern(int nPatternNo);
void SetFormatInfoPattern(int nPatternNo);
int CountPenalty(void);
#endif

















