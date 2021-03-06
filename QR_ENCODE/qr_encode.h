#ifndef __QR_ENCODE_H
#define __QR_ENCODE_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序移植自网友旮旯旭的二维码生成工程
//原文连接:http://www.openedv.com/forum.php?mod=viewthread&tid=58283
//ALIENTEK STM32开发板
//二维码编码 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/2/28
//版本：V1.0  			  
////////////////////////////////////////////////////////////////////////////////// 	 
 
 
#define QR_TRUE 			1		//TRUE定义
#define QR_FALSE 			0 		//FALSE定义


#define min(a,b)	(((a) < (b)) ? (a) : (b))

//纠错等级(4种)
#define QR_LEVEL_L			0		//7%的字码可被修正
#define QR_LEVEL_M			1		//15%的字码可被修正
#define QR_LEVEL_Q			2		//25%的字码可被修正
#define QR_LEVEL_H			3		//30%的字码可被修正

//编码形式
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3
#define QR_MODE_CHINESE		4

//Number of bits per length field
//Encoding  Ver.1–9      10–26      27–40
//Numeric		10			12			14
//Alphanumeric	9			11			13
//u8			8			16			16
//Kanji			8			10			12 
//Chinese		
//P17 字符计数指示的位数
#define QR_VRESION_S		0 
#define QR_VRESION_M		1 
#define QR_VRESION_L		2 

#define QR_MARGIN			4
#define	QR_VER1_SIZE		21		// 版本的行列数

#define MAX_ALLCODEu32		3706	//400// //P14,P35 数据容量[码字]* (E) (VER:40), 所有码字为8位
#define MAX_DATACODEu32		2956	//400// //P27     最大信息码子(Ver：40-L)，所有码字为8位
#define MAX_CODEBLOCK		153		//最大纠错码字 Ver：36.37.38_L_第二块
#define MAX_MODULESIZE		177		// 21:Version=1,最大字符=17(8.5个汉字)
									// 25:Version=2,最大字符=32(16个汉字)
									// 29:Version=3,最大字符=49(24.5个汉字)
									// 33:Version=4,最大字符=78(39个汉字)
									// 37:Version=5,最大字符=106(53个汉字) 
									// 41:Version=6,最大字符=134(67个汉字)
									// 45:Version=7,最大字符=154(77个汉字)
									// 49:Version=8,最大字符=192(96个汉字)
									// 53:
									//P14 每边的模块数（A） (VER:40   ) Ver：40 = 21+（Ver-1）*4
//QR ENCODE参数集
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

extern _qr_encode *qrx;  				//在QR_ENCODE.C里面定义.使用前,必须申请内存

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

















