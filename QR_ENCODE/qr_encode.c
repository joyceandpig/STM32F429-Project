#include "string.h"
#include "qr_encode.h"
#include "qr_data.h"
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
//��ά������ԭ����� http://coolshell.cn/articles/10590.html#jtss-tsina

_qr_encode *qrx;		//��ά�����,���ֲ����ṹ��

//��������
//lpsSource Դ����
u8 EncodeData(char *lpsSource)
{
	int i, j, nVersion=1, bAutoExtent=1, ncSource;
	int ncLength, nEncodeVersion, ncDataCodeu32, ncTerminater;
	u8 byPaddingCode = 0xec;
	int nDataCwIndex = 0, ncBlock1, ncBlock2, ncBlockSum; 
	int nBlockNo = 0, ncDataCw1, ncDataCw2;
	int ncRSCw1, ncRSCw2;
 	ncSource = 0;			
	qrx->m_nLevel = 0;		//����ȼ�ΪLOW,7%���޸�
	qrx->m_nMaskingNo = 0;
 	ncLength = ncSource > 0 ? ncSource : strlen(lpsSource);//����Ҫ��������ݵĳ���
	//printf("ncLength = %d\n",ncLength);
	if (ncLength == 0) return QR_FALSE;  
	nEncodeVersion = GetEncodeVersion(nVersion, lpsSource, ncLength);//�������������Ҫ�İ汾
	//printf("nEncodeVersion = %d\n",nEncodeVersion);
	if (nEncodeVersion == 0) return QR_FALSE;  
	if (nVersion == 0) qrx->m_nVersion = nEncodeVersion; 
	else
	{
		if (nEncodeVersion <= nVersion) qrx->m_nVersion = nVersion; 
		else
		{
			if (bAutoExtent)qrx->m_nVersion = nEncodeVersion; 
 			else return QR_FALSE; 
 		}
	}
	//printf("qrx->m_nVersion = %d\n",qrx->m_nVersion);
	ncDataCodeu32 = QR_VersonInfo[qrx->m_nVersion].ncDataCodeu32[qrx->m_nLevel];
	ncTerminater = min(4, (ncDataCodeu32 * 8) - qrx->m_ncDataCodeu32Bit);
	if (ncTerminater > 0)qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 0, ncTerminater);
	for (i = (qrx->m_ncDataCodeu32Bit + 7) / 8; i < ncDataCodeu32; ++i)
	{
		qrx->m_byDataCodeu32[i] = byPaddingCode;
		byPaddingCode = (u8)(byPaddingCode == 0xec ? 0x11 : 0xec);
	}
	qrx->m_ncAllCodeu32 = QR_VersonInfo[qrx->m_nVersion].ncAllCodeu32;
	memset(qrx->m_byAllCodeu32, 0, qrx->m_ncAllCodeu32);
	ncBlock1 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo1[qrx->m_nLevel].ncRSBlock;
	ncBlock2 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo2[qrx->m_nLevel].ncRSBlock;
	ncBlockSum = ncBlock1 + ncBlock2;
	ncDataCw1 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo1[qrx->m_nLevel].ncDataCodeu32;
	ncDataCw2 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo2[qrx->m_nLevel].ncDataCodeu32;
	for (i = 0; i < ncBlock1; ++i)
	{
		for (j = 0; j < ncDataCw1; ++j)
		{
			qrx->m_byAllCodeu32[(ncBlockSum * j) + nBlockNo] = qrx->m_byDataCodeu32[nDataCwIndex++];
		}
		++nBlockNo;
	} 
	for (i = 0; i < ncBlock2; ++i)
	{
		for (j = 0; j < ncDataCw2; ++j)
		{
			if (j < ncDataCw1) qrx->m_byAllCodeu32[(ncBlockSum * j) + nBlockNo] = qrx->m_byDataCodeu32[nDataCwIndex++];
 			else qrx->m_byAllCodeu32[(ncBlockSum * ncDataCw1) + i]  = qrx->m_byDataCodeu32[nDataCwIndex++];
 		} 
		++nBlockNo;
	} 
	ncRSCw1 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo1[qrx->m_nLevel].ncAllCodeu32 - ncDataCw1;
	ncRSCw2 = QR_VersonInfo[qrx->m_nVersion].RS_BlockInfo2[qrx->m_nLevel].ncAllCodeu32 - ncDataCw2;
	nDataCwIndex = 0;
	nBlockNo = 0;
	for (i = 0; i < ncBlock1; ++i)
	{
		memset(qrx->m_byRSWork, 0, sizeof(qrx->m_byRSWork));
		memmove(qrx->m_byRSWork, qrx->m_byDataCodeu32 + nDataCwIndex, ncDataCw1);
		GetRSCodeu32(qrx->m_byRSWork, ncDataCw1, ncRSCw1);
		for (j = 0; j < ncRSCw1; ++j)
		{
			qrx->m_byAllCodeu32[ncDataCodeu32 + (ncBlockSum * j) + nBlockNo] = qrx->m_byRSWork[j];
		}
		nDataCwIndex += ncDataCw1;
		++nBlockNo;
	}
	for (i = 0; i < ncBlock2; ++i)
	{
		memset(qrx->m_byRSWork, 0, sizeof(qrx->m_byRSWork));
		memmove(qrx->m_byRSWork, qrx->m_byDataCodeu32 + nDataCwIndex, ncDataCw2);
		GetRSCodeu32(qrx->m_byRSWork, ncDataCw2, ncRSCw2); 
		for (j = 0; j < ncRSCw2; ++j)
		{
			qrx->m_byAllCodeu32[ncDataCodeu32 + (ncBlockSum * j) + nBlockNo] = qrx->m_byRSWork[j];
		} 
		nDataCwIndex += ncDataCw2;
		++nBlockNo;
	}
	qrx->m_nSymbleSize = qrx->m_nVersion * 4 + 17;
	FormatModule();
	return QR_TRUE;
}
//ÿ���汾�����ֶε�λ��
//nVersion	�汾
//lpsSource	Դ����
//ncLength	����
/*
Number of bits per length field
Encoding		Ver.1�C9		10�C26		27�C40
Numeric			10			12			14
Alphanumeric	9			11			13
u8			8			16			16
Kanji			8			10			12 
*/
int GetEncodeVersion(int nVersion, char *lpsSource, int ncLength)
{
	int nVerGroup = nVersion >= 27 ? QR_VRESION_L : (nVersion >= 10 ? QR_VRESION_M : QR_VRESION_S);
	int i, j;

	for (i = nVerGroup; i <= QR_VRESION_L; ++i)
	{
		if (EncodeSourceData(lpsSource, ncLength, i))
		{
			if (i == QR_VRESION_S)
			{
				for (j = 1; j <= 9; ++j)
				{
					if ((qrx->m_ncDataCodeu32Bit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeu32[qrx->m_nLevel])
						return j;
				}
			}
			else if (i == QR_VRESION_M)
			{
				for (j = 10; j <= 26; ++j)
				{
					if ((qrx->m_ncDataCodeu32Bit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeu32[qrx->m_nLevel])
						return j;
				}
			}
			else if (i == QR_VRESION_L)
			{
				for (j = 27; j <= 40; ++j)
				{
					if ((qrx->m_ncDataCodeu32Bit + 7) / 8 <= QR_VersonInfo[j].ncDataCodeu32[qrx->m_nLevel])
						return j;
				}
			}
		}
	}
	return 0;
}
//ģʽָ�꣬����ģʽ
//lpsSource Դ����
//ncLength	���ݳ���
//nVerGroup	�汾��
//u8 EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup)
int EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup)
{
	int i, j;
	int ncSrcBits, ncDstBits;
	int nBlock = 0;
	int ncComplete = 0; 
	u32 wBinCode; 
	memset(qrx->m_nBlockLength, 0, sizeof(qrx->m_nBlockLength)); 
	for (qrx->m_ncDataBlock = i = 0; i < ncLength; ++i)
	{
		u8 byMode; 
		if (i < ncLength - 1 && IsKanjiData(lpsSource[i], lpsSource[i + 1]))
			byMode = QR_MODE_KANJI;				//����
		//else if(i < ncLength - 1 && IsChineseData(lpsSource[i], lpsSource[i + 1]))
			//byMode = QR_MODE_CHINESE;			//����
		else if (IsNumeralData(lpsSource[i]))	
			byMode = QR_MODE_NUMERAL;			//����
		else if (IsAlphabetData(lpsSource[i]))
			byMode = QR_MODE_ALPHABET;			//�ַ�
		else
			byMode = QR_MODE_8BIT;				//�ֽ�

		if (i == 0) qrx->m_byBlockMode[0] = byMode;			// 
		if (qrx->m_byBlockMode[qrx->m_ncDataBlock] != byMode)//���ǰ���������ݵ�ģʽ��һ��
			qrx->m_byBlockMode[++qrx->m_ncDataBlock] = byMode; 
		++qrx->m_nBlockLength[qrx->m_ncDataBlock]; 
		if (byMode == QR_MODE_KANJI)//���������ģʽ����ôǰ��2���ֽ�Ϊһ������
		{
			++qrx->m_nBlockLength[qrx->m_ncDataBlock];
			++i;
		}
	} 
	++qrx->m_ncDataBlock; 
	while (nBlock < qrx->m_ncDataBlock - 1)
	{
		int ncJoinFront, ncJoinBehind; 
		int nJoinPosition = 0;  
		if ((qrx->m_byBlockMode[nBlock] == QR_MODE_NUMERAL  && qrx->m_byBlockMode[nBlock + 1] == QR_MODE_ALPHABET) ||
			(qrx->m_byBlockMode[nBlock] == QR_MODE_ALPHABET && qrx->m_byBlockMode[nBlock + 1] == QR_MODE_NUMERAL))
		{//���ǰ��2�����ݿ�Ϊ��ĸ�����֣����Ժϲ�����ĸ����ģʽ(��ĸģʽ����������)  ��������λ��
			ncSrcBits = GetBitLength(qrx->m_byBlockMode[nBlock], qrx->m_nBlockLength[nBlock], nVerGroup) +
						GetBitLength(qrx->m_byBlockMode[nBlock + 1], qrx->m_nBlockLength[nBlock + 1], nVerGroup);
 			ncDstBits = GetBitLength(QR_MODE_ALPHABET, qrx->m_nBlockLength[nBlock] + qrx->m_nBlockLength[nBlock + 1], nVerGroup);
 			if (ncSrcBits > ncDstBits)
			{//����ϲ�ǰ���ںϲ��󳤶�
				if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)
				{//�ж�֮ǰ�����ݿ��ǲ���8bitģʽ������ǵĻ��ϲ�����(����Ҫ��3�����ݿ������ж�)
					ncJoinFront = GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock - 1] + qrx->m_nBlockLength[nBlock], nVerGroup) +
								  GetBitLength(qrx->m_byBlockMode[nBlock + 1], qrx->m_nBlockLength[nBlock + 1], nVerGroup);
 					if (ncJoinFront > ncDstBits + GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock - 1], nVerGroup))
						ncJoinFront = 0; //����ϲ�������ݳ������� �Ƿ�
				}else ncJoinFront = 0;//������ϲ�������
 				if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
				{//�����������ݿ�Ϊ8bitģʽ���ϲ�����(����Ҫ��3�����ݿ������ж�)
					ncJoinBehind = GetBitLength(qrx->m_byBlockMode[nBlock], qrx->m_nBlockLength[nBlock], nVerGroup) +
								   GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock + 1] + qrx->m_nBlockLength[nBlock + 2], nVerGroup);
 					if (ncJoinBehind > ncDstBits + GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock + 2], nVerGroup))
						ncJoinBehind = 0; //����ϲ�������ݳ������� �Ƿ�
				}
				else ncJoinBehind = 0;//������ϲ������� 
				if (ncJoinFront != 0 && ncJoinBehind != 0)//ǰ������ݿ�ϲ��󶼲�Ϊ0
				{
					nJoinPosition = (ncJoinFront < ncJoinBehind) ? -1 : 1;//λ�õı�ǣ����ǰ���С�ں����Ϊ-1�����ڵ���Ϊ1
				}
				else//�����һ�����߶�Ϊ0		���ǰ�治Ϊ0Ϊ-1�����治Ϊ0Ϊ1  ����Ϊ0
				{
					nJoinPosition = (ncJoinFront != 0) ? -1 : ((ncJoinBehind != 0) ? 1 : 0);
				} 
				if (nJoinPosition != 0)
				{//����ϲ�������ݲ�Ϊ0
					if (nJoinPosition == -1)//ǰ��������
					{
						qrx->m_nBlockLength[nBlock - 1] += qrx->m_nBlockLength[nBlock]; 
						for (i = nBlock; i < qrx->m_ncDataBlock - 1; ++i)
						{
							qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
							qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
						}
					}
					else
					{//����������
						qrx->m_byBlockMode[nBlock + 1] = QR_MODE_8BIT;//�������ݿ�Ϊ8BITģʽ�����µ����ݿ�ģʽ�����8BIT
						qrx->m_nBlockLength[nBlock + 1] += qrx->m_nBlockLength[nBlock + 2];//����

						for (i = nBlock + 2; i < qrx->m_ncDataBlock - 1; ++i)
						{
							qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
							qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
						}
					} 
					--qrx->m_ncDataBlock;
				}
				else
				{//�ϲ��������쳣�����߲�����ϲ�����
					if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_ALPHABET)
					{//�������һ������Ҳ����ĸ���֣��ϲ�����
						qrx->m_nBlockLength[nBlock + 1] += qrx->m_nBlockLength[nBlock + 2];
 						for (i = nBlock + 2; i < qrx->m_ncDataBlock - 1; ++i)
						{
							qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
							qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
						}
 						--qrx->m_ncDataBlock;
					}
					//���������ݿ�Ϊ��ĸ����
					qrx->m_byBlockMode[nBlock] = QR_MODE_ALPHABET;
					qrx->m_nBlockLength[nBlock] += qrx->m_nBlockLength[nBlock + 1];
 					for (i = nBlock + 1; i < qrx->m_ncDataBlock - 1; ++i)
					{
						qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
						qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
					}
 					--qrx->m_ncDataBlock;
					//���ǰ��һ������Ҳ����ĸ���֣��ϲ�����
					if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_ALPHABET)
					{
						qrx->m_nBlockLength[nBlock - 1] += qrx->m_nBlockLength[nBlock];
 						for (i = nBlock; i < qrx->m_ncDataBlock - 1; ++i)
						{
							qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
							qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
						}
 						--qrx->m_ncDataBlock;
					}
				} 
				continue;
			}
		} 
		++nBlock; 
	} 
	nBlock = 0; 
	while (nBlock < qrx->m_ncDataBlock - 1)
	{//�ϲ�2����������
		ncSrcBits = GetBitLength(qrx->m_byBlockMode[nBlock], qrx->m_nBlockLength[nBlock], nVerGroup)
					+ GetBitLength(qrx->m_byBlockMode[nBlock + 1], qrx->m_nBlockLength[nBlock + 1], nVerGroup);
 		ncDstBits = GetBitLength(QR_MODE_8BIT, qrx->m_nBlockLength[nBlock] + qrx->m_nBlockLength[nBlock + 1], nVerGroup);
 		if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)//ǰһ���ݿ�ΪBITģʽ
			ncDstBits -= (4 + nIndicatorLen8Bit[nVerGroup]);
 		if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
			ncDstBits -= (4 + nIndicatorLen8Bit[nVerGroup]);
 		if (ncSrcBits > ncDstBits)
		{//����ϲ����������ӣ�ǰһ��������8bit����
			if (nBlock >= 1 && qrx->m_byBlockMode[nBlock - 1] == QR_MODE_8BIT)
			{
				qrx->m_nBlockLength[nBlock - 1] += qrx->m_nBlockLength[nBlock]; 
				for (i = nBlock; i < qrx->m_ncDataBlock - 1; ++i)
				{
					qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
					qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
				} 
				--qrx->m_ncDataBlock;
				--nBlock;
			} 
			if (nBlock < qrx->m_ncDataBlock - 2 && qrx->m_byBlockMode[nBlock + 2] == QR_MODE_8BIT)
			{//���ݿ�+2Ҳ��8bit
				qrx->m_nBlockLength[nBlock + 1] += qrx->m_nBlockLength[nBlock + 2]; 
				for (i = nBlock + 2; i < qrx->m_ncDataBlock - 1; ++i)
				{
					qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
					qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
				} 
				--qrx->m_ncDataBlock;
			}
			//�����µ����ݿ�Ϊ8BITģʽ
			qrx->m_byBlockMode[nBlock] = QR_MODE_8BIT;
			qrx->m_nBlockLength[nBlock] += qrx->m_nBlockLength[nBlock + 1]; 
			for (i = nBlock + 1; i < qrx->m_ncDataBlock - 1; ++i)
			{
				qrx->m_byBlockMode[i]  = qrx->m_byBlockMode[i + 1];
				qrx->m_nBlockLength[i] = qrx->m_nBlockLength[i + 1];
			} 
			--qrx->m_ncDataBlock; 
			if (nBlock >= 1) --nBlock; 
			continue;
		} 
		++nBlock; 
	} 
	qrx->m_ncDataCodeu32Bit = 0;  
	memset(qrx->m_byDataCodeu32, 0, MAX_DATACODEu32); 
	for (i = 0; i < qrx->m_ncDataBlock && qrx->m_ncDataCodeu32Bit != -1; ++i)
	{
		if (qrx->m_byBlockMode[i] == QR_MODE_NUMERAL)
		{//������ݿ�������ģʽ
			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 1, 4); 
 			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (u32)qrx->m_nBlockLength[i], nIndicatorLenNumeral[nVerGroup]);
 			for (j = 0; j < qrx->m_nBlockLength[i]; j += 3)
			{
				if (j < qrx->m_nBlockLength[i] - 2)
				{
					wBinCode = (u32)(((lpsSource[ncComplete + j]	  - '0') * 100) +
									  ((lpsSource[ncComplete + j + 1] - '0') * 10) +
									   (lpsSource[ncComplete + j + 2] - '0'));
 					qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 10);
				}
				else if (j == qrx->m_nBlockLength[i] - 2)
				{
					wBinCode = (u32)(((lpsSource[ncComplete + j] - '0') * 10) +
									   (lpsSource[ncComplete + j + 1] - '0'));
 					qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 7);
				}
				else if (j == qrx->m_nBlockLength[i] - 1)
				{
					wBinCode = (u32)(lpsSource[ncComplete + j] - '0');
 					qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 4);
				}
			}
 			ncComplete += qrx->m_nBlockLength[i];
		}else if (qrx->m_byBlockMode[i] == QR_MODE_ALPHABET)
		{
			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 2, 4);
 			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (u32)qrx->m_nBlockLength[i], nIndicatorLenAlphabet[nVerGroup]);
 			for (j = 0; j < qrx->m_nBlockLength[i]; j += 2)
			{
				if (j < qrx->m_nBlockLength[i] - 1)
				{
					wBinCode = (u32)((AlphabetToBinaly(lpsSource[ncComplete + j]) * 45) +
									   AlphabetToBinaly(lpsSource[ncComplete + j + 1]));
 					qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 11);
				}
				else
				{
					wBinCode = (u32)AlphabetToBinaly(lpsSource[ncComplete + j]);
 					qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 6);
				}
			}
 			ncComplete += qrx->m_nBlockLength[i];
		}
 		else if (qrx->m_byBlockMode[i] == QR_MODE_8BIT)
		{
			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 4, 4);
 			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (u32)qrx->m_nBlockLength[i], nIndicatorLen8Bit[nVerGroup]);
 			for (j = 0; j < qrx->m_nBlockLength[i]; ++j)
			{
				qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (u32)lpsSource[ncComplete + j], 8);
			}
 			ncComplete += qrx->m_nBlockLength[i];
		}
		else// if(qrx->m_byBlockMode[i] == QR_MODE_KANJI)
		{//�ձ�ģʽ
			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 8, 4);
 			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (u32)(qrx->m_nBlockLength[i] / 2), nIndicatorLenKanji[nVerGroup]);
 			for (j = 0; j < qrx->m_nBlockLength[i] / 2; ++j)
			{
				u32 wBinCode = KanjiToBinaly((u32)(((u8)lpsSource[ncComplete + (j * 2)] << 8) + (u8)lpsSource[ncComplete + (j * 2) + 1]));
 				qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 13);
			}
 			ncComplete += qrx->m_nBlockLength[i];
		}
//		else
//		{//����ģʽ
//			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, 8, 4);

//			qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, (u32)(qrx->m_nBlockLength[i] / 2), nIndicatorLenKanji[nVerGroup]);

//			for (j = 0; j < qrx->m_nBlockLength[i] / 2; ++j)
//			{
//				u32 wBinCode = ChineseToBinaly((u32)(((u8)lpsSource[ncComplete + (j * 2)] << 8) + (u8)lpsSource[ncComplete + (j * 2) + 1]));

//				qrx->m_ncDataCodeu32Bit = SetBitStream(qrx->m_ncDataCodeu32Bit, wBinCode, 13);
//			}

//			ncComplete += qrx->m_nBlockLength[i];
//		}
	} 
	return (qrx->m_ncDataCodeu32Bit != -1); 
}
//��ȡbit����
int GetBitLength(u8 nMode, int ncData, int nVerGroup)
{
	int ncBits = 0; 
	switch (nMode)
	{
		case QR_MODE_NUMERAL:
			ncBits = 4 + nIndicatorLenNumeral[nVerGroup] + (10 * (ncData / 3));
			switch (ncData % 3)
			{
				case 1:
					ncBits += 4;
					break;
				case 2:
					ncBits += 7;
					break;
				default:
					break;
			} 
			break; 
		case QR_MODE_ALPHABET:
			ncBits = 4 + nIndicatorLenAlphabet[nVerGroup] + (11 * (ncData / 2)) + (6 * (ncData % 2));
			break; 
		case QR_MODE_8BIT:
			ncBits = 4 + nIndicatorLen8Bit[nVerGroup] + (8 * ncData);
			break; 
		default:
			//ncBits = 4 + nIndicatorLenKanji[nVerGroup] + (13 * (ncData / 2));//������д����
			ncBits = 4 + nIndicatorLenKanji[nVerGroup] + (13 * ncData);//�ĵ��㷨
			break;
	} 
	return ncBits;
}
//����bit��
int SetBitStream(int nIndex, u32 wData, int ncData)
{
	int i;
 	if (nIndex == -1 || nIndex + ncData > MAX_DATACODEu32 * 8) return -1;
 	for (i = 0; i < ncData; ++i)
	{
		if (wData & (1 << (ncData - i - 1)))
		{
			qrx->m_byDataCodeu32[(nIndex + i) / 8] |= 1 << (7 - ((nIndex + i) % 8));
		}
	} 
	return nIndex + ncData;
}
//�ǲ������ֱ�����Ϣ
//�Ƿ���QR_TRUE���񷵻�QR_FALSE
u8 IsNumeralData(unsigned char c)
{
	if (c >= '0' && c <= '9') return QR_TRUE; 
	return QR_FALSE;
}
//�Ƿ����ַ�������Ϣ
//�Ƿ���QR_TRUE���񷵻�QR_FALSE
u8 IsAlphabetData(unsigned char c)
{
	if (c >= '0' && c <= '9') return QR_TRUE;
 	if (c >= 'A' && c <= 'Z') return QR_TRUE;
 	if (c == ' ' || c == '$' || c == '%' || c == '*' || c == '+' || c == '-' || c == '.' || c == '/' || c == ':')return QR_TRUE;
 	return QR_FALSE;
}
//�Ƿ����ĺ�����Ϣ
//�Ƿ���QR_TRUE���񷵻�QR_FALSE
u8 IsKanjiData(unsigned char c1, unsigned char c2)
{

	if (((c1 >= 0x81 && c1 <= 0x9f) || (c1 >= 0xe0 && c1 <= 0xeb)) && (c2 >= 0x40))
	{
		if ((c1 == 0x9f && c2 > 0xfc) || (c1 == 0xeb && c2 > 0xbf)) return QR_FALSE; 
		return QR_TRUE;
	}
	return QR_FALSE;
}
u8 IsChineseData(unsigned char c1, unsigned char c2)
{
	//����ģʽ
	if((c1 >= 0xa1 && c1 < 0xaa) || (c1 >= 0xb0 && c1 <= 0xfa))
	{
		if(c2 >= 0xa1 && c2 <= 0xfe)return QR_TRUE;
	}
	return QR_FALSE;
} 
//��ĸ����Ϣת��
u8 AlphabetToBinaly(unsigned char c)
{
	if (c >= '0' && c <= '9') return (unsigned char)(c - '0');
 	if (c >= 'A' && c <= 'Z') return (unsigned char)(c - 'A' + 10);
 	if (c == ' ') return 36;
 	if (c == '$') return 37;
 	if (c == '%') return 38;
 	if (c == '*') return 39;
 	if (c == '+') return 40;
 	if (c == '-') return 41;
 	if (c == '.') return 42;
 	if (c == '/') return 43;
 	return 44;
}
//����������Ϣת��
//���ĺͺ��ֵı�����ȥһ��ֵ��
//�磺��0X8140 to 0X9FFC�е��ַ����ȥ8140��
//��0XE040��0XEBBF�е��ַ�Ҫ��ȥ0XC140��
//Ȼ��ѽ��ǰ����16����λ�ó�������0XC0��
//Ȼ���ټ��Ϻ�����16����λ�����ת��13bit�ı��롣
u32 KanjiToBinaly(u32 wc)
{
	if (wc >= 0x8140 && wc <= 0x9ffc)wc -= 0x8140;
	else wc -= 0xc140; 
	return (u32)(((wc >> 8) * 0xc0) + (wc & 0x00ff));
} 
//�������ĺ��ֵ������ĺ���ת���������ƣ�
//1�����ڵ�һ�ֽ�Ϊ0xA1~0xAA֮��,�ڶ��ֽ���0xA1~0xFE֮���ַ���
//a)��һ�ֽڼ�ȥ0xA1��
//b)��һ���������0x60;
//c���ڶ��ֽڼ�ȥ0xA1;
//d)��b)����Ľ������c����Ľ��;
//e)�����ת��Ϊ13λ�����ƴ���
//1�����ڵ�һ�ֽ�Ϊ0xB0~0xFA֮��,�ڶ��ֽ���0xA1~0xFE֮���ַ���
//a)��һ�ֽڼ�ȥ0xA6��
//b)��һ���������0x60;
//c���ڶ��ֽڼ�ȥ0xA1;
//d)��b)����Ľ������c����Ľ��;
//e)�����ת��Ϊ13λ�����ƴ� 
u32 ChineseToBinaly(u32 wc)
{
	if(wc >= 0xa1a1 && wc <= 0xa9fe)
	{
		return (u32)((((wc >> 8)-0xa1) * 0x60) + ((wc & 0x00ff)-0xa1));
	}
	if(wc >= 0xb0a1 && wc <= 0xf9fe)
	{
		return (u32)((((wc >> 8)-0xa6) * 0x60) + ((wc & 0x00ff)-0xa1));
	}
	return (u32)((((wc >> 8)-0xa6) * 0x60) + ((wc & 0x00ff)-0xa1));
}
//������
void GetRSCodeu32(u8 *lpbyRSWork, int ncDataCodeu32, int ncRSCodeu32)
{
	int i, j; 
	for (i = 0; i < ncDataCodeu32 ; ++i)
	{
		if (lpbyRSWork[0] != 0)
		{
			u8 nExpFirst = byIntToExp[lpbyRSWork[0]];  
			for (j = 0; j < ncRSCodeu32; ++j)
			{
				u8 nExpElement = (u8)(((int)(byRSExp[ncRSCodeu32][j] + nExpFirst)) % 255);
 				lpbyRSWork[j] = (u8)(lpbyRSWork[j + 1] ^ byExpToInt[nExpElement]);
			}
 			for (j = ncRSCodeu32; j < ncDataCodeu32 + ncRSCodeu32 - 1; ++j) lpbyRSWork[j] = lpbyRSWork[j + 1];
		}else
		{
			for (j = 0; j < ncDataCodeu32 + ncRSCodeu32 - 1; ++j) lpbyRSWork[j] = lpbyRSWork[j + 1];
		}
	}
}
//��ʽ�����ݴ�ţ����������еĳߴ��У�
//���http://coolshell.cn/articles/10590.html#jtss-tsina
void FormatModule(void)
{
	int i, j;
	int nMinPenalty, nPenalty;
	memset(qrx->m_byModuleData, 0, sizeof(qrx->m_byModuleData));
	SetFunctionModule();	//���Ȱѻ���ͼ��������������
	SetCodeu32Pattern();	//
	if (qrx->m_nMaskingNo == -1)
	{
		qrx->m_nMaskingNo = 0;
		SetMaskingPattern(qrx->m_nMaskingNo); 
		SetFormatInfoPattern(qrx->m_nMaskingNo); 
		nMinPenalty = CountPenalty();
		for (i = 1; i <= 7; ++i)
		{
			SetMaskingPattern(i); 
			SetFormatInfoPattern(i); 
			nPenalty = CountPenalty();
			if (nPenalty < nMinPenalty)
			{
				nMinPenalty = nPenalty;
				qrx->m_nMaskingNo = i;
			}
		}
	}
	SetMaskingPattern(qrx->m_nMaskingNo); 
	SetFormatInfoPattern(qrx->m_nMaskingNo); 
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize; ++j)
		{
			qrx->m_byModuleData[i][j] = (u8)((qrx->m_byModuleData[i][j] & 0x11) != 0);
		}
	}
}
//���ù�����ģ����Ϣ
//Position Detection Pattern	���ڱ�Ƕ�ά����д�С
//Separators for Postion Detection Patterns	�����Ϳ��Ա��һ������
//Timing PatternsҲ�����ڶ�λ�ġ�ԭ���Ƕ�ά����40�ֳߴ磬�ߴ�����˺���Ҫ�и���׼�ߣ���Ȼɨ���ʱ����ܻ�ɨ����
//Alignment Patterns ֻ��Version 2���ϣ�����Version2���Ķ�ά����Ҫ���������ͬ����Ϊ�˶�λ�õġ�
void SetFunctionModule(void)
{
	int i, j;

	SetFinderPattern(0, 0);					//������������
	SetFinderPattern(qrx->m_nSymbleSize - 7, 0);	
	SetFinderPattern(0, qrx->m_nSymbleSize - 7);	
	for (i = 0; i < 8; ++i)
	{
		qrx->m_byModuleData[i][7] = qrx->m_byModuleData[7][i] = '\x20';
		qrx->m_byModuleData[qrx->m_nSymbleSize - 8][i] = qrx->m_byModuleData[qrx->m_nSymbleSize - 8 + i][7] = '\x20';
		qrx->m_byModuleData[i][qrx->m_nSymbleSize - 8] = qrx->m_byModuleData[7][qrx->m_nSymbleSize - 8 + i] = '\x20';
	}
	for (i = 0; i < 9; ++i)
	{
		qrx->m_byModuleData[i][8] = qrx->m_byModuleData[8][i] = '\x20';
	}
	for (i = 0; i < 8; ++i)
	{
		qrx->m_byModuleData[qrx->m_nSymbleSize - 8 + i][8] = qrx->m_byModuleData[8][qrx->m_nSymbleSize - 8 + i] = '\x20';
	}
	SetVersionPattern();	//�汾��Ϣ
	for (i = 0; i < QR_VersonInfo[qrx->m_nVersion].ncAlignPoint; ++i)
	{
		SetAlignmentPattern(QR_VersonInfo[qrx->m_nVersion].nAlignPoint[i], 6);//С����
		SetAlignmentPattern(6, QR_VersonInfo[qrx->m_nVersion].nAlignPoint[i]);
		for (j = 0; j < QR_VersonInfo[qrx->m_nVersion].ncAlignPoint; ++j)
		{
			SetAlignmentPattern(QR_VersonInfo[qrx->m_nVersion].nAlignPoint[i], QR_VersonInfo[qrx->m_nVersion].nAlignPoint[j]);
		}
	}
	for (i = 8; i <= qrx->m_nSymbleSize - 9; ++i)
	{
		qrx->m_byModuleData[i][6] = (i % 2) == 0 ? '\x30' : '\x20';
		qrx->m_byModuleData[6][i] = (i % 2) == 0 ? '\x30' : '\x20';
	}
}
//���ô������Ϣ
// O X X X X X X X
// O X O O O O O X
// O X O X X X O X
// O X O X X X O X
// O X O X X X O X
// O X O O O O O X
// O X X X X X X X
void SetFinderPattern(int x, int y)
{
	const u8 byPattern[] = {0x7f,0x41,0x5d,0x5d,0x5d,0x41,0x7f}; 
	int i, j; 
	for (i = 0; i < 7; ++i)
	{
		for (j = 0; j < 7; ++j)
		{
			qrx->m_byModuleData[x + j][y + i] = (byPattern[i] & (1 << (6 - j))) ? '\x30' : '\x20'; 
		}
	}
}
//����С������Ϣ
//Alignment Patterns ֻ��Version 2���ϣ�����Version2���Ķ�ά����Ҫ���������ͬ����Ϊ�˶�λ�õġ�
//Alignment Patterns�ǳ���3����Ļ��֣���С�Ļ���
// O O O X X X X X
// O O O X O O O X
// O O O X O X O X
// O O O X O O O X
// O O O X X X X X
void SetAlignmentPattern(int x, int y)
{
	const u8 byPattern[] = {0x1f,0x11,0x15,0x11,0x1f}; 
	int i, j;
	if (qrx->m_byModuleData[x][y] & 0x20)return; 
	x -= 2; y -= 2; 
	for (i = 0; i < 5; ++i)
	{
		for (j = 0; j < 5; ++j)
		{
			qrx->m_byModuleData[x + j][y + i] = (byPattern[i] & (1 << (4 - j))) ? '\x30' : '\x20'; 
		}
	}
}
//���ð汾���� >= Version 7���ϣ���ҪԤ������3 x 6��������һЩ�汾��Ϣ��
void SetVersionPattern(void)
{
	int i, j;
	int nVerData;
	if (qrx->m_nVersion <= 6)return;//�� >= Version 7���ϣ���ҪԤ������3 x 6��������һЩ�汾��Ϣ��
		
	nVerData = qrx->m_nVersion << 12;
	for (i = 0; i < 6; ++i)
	{
		if (nVerData & (1 << (17 - i)))
		{
			nVerData ^= (0x1f25 << (5 - i));
		}
	}
	nVerData += qrx->m_nVersion << 12;
	for (i = 0; i < 6; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			qrx->m_byModuleData[qrx->m_nSymbleSize - 11 + j][i] = qrx->m_byModuleData[i][qrx->m_nSymbleSize - 11 + j] =
			(nVerData & (1 << (i * 3 + j))) ? '\x30' : '\x20';
		}
	}
}
//�������Ϊ���ǿ��Կ�ʼ��ͼ����ʹ��ˡ�
//��ά��Ļ��Ҽ�����û�����꣬����Ҫ��������;�����ĸ���codeu32s�������һ��
//��ν����أ��������£�
//���������룺��ÿ����ĵ�һ��codeu32s���ó�����˳�����кã�
//Ȼ����ȡ��һ��ĵڶ�����������ơ��磺����ʾ���е�Data Codeu32s����
void SetCodeu32Pattern(void)
{
	int x = qrx->m_nSymbleSize;
	int y = qrx->m_nSymbleSize - 1;
	int nCoef_x = 1; 
	int nCoef_y = 1; 
	int i, j;
	for (i = 0; i < qrx->m_ncAllCodeu32; ++i)
	{
		for (j = 0; j < 8; ++j)
		{
			do
			{
				x += nCoef_x;
				nCoef_x *= -1; 
				if (nCoef_x < 0)
				{
					y += nCoef_y;
					if (y < 0 || y == qrx->m_nSymbleSize)
					{
						y = (y < 0) ? 0 : qrx->m_nSymbleSize - 1;
						nCoef_y *= -1;
						x -= 2;
						if (x == 6) --x; 
					}
				}
			}
			while (qrx->m_byModuleData[x][y] & 0x20); 
			qrx->m_byModuleData[x][y] = (qrx->m_byAllCodeu32[i] & (1 << (7 - j))) ? '\x02' : '\x00';
		}
	}
}
//�����ڱ���Ϣ
void SetMaskingPattern(int nPatternNo)
{
	int i, j;
	u8 bMask;
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize; ++j)
		{
			if (! (qrx->m_byModuleData[j][i] & 0x20)) 
			{
				switch (nPatternNo)
				{
					case 0:
						bMask = ((i + j) % 2 == 0)?QR_TRUE:QR_FALSE;
						break;
					case 1:
						bMask = (i % 2 == 0)?QR_TRUE:QR_FALSE;
						break;
					case 2:
						bMask = (j % 3 == 0)?QR_TRUE:QR_FALSE;
						break;
					case 3:
						bMask = ((i + j) % 3 == 0)?QR_TRUE:QR_FALSE;
						break;
					case 4:
						bMask = (((i / 2) + (j / 3)) % 2 == 0)?QR_TRUE:QR_FALSE;
						break;
					case 5:
						bMask = (((i * j) % 2) + ((i * j) % 3) == 0)?QR_TRUE:QR_FALSE;
						break;
					case 6:
						bMask = ((((i * j) % 2) + ((i * j) % 3)) % 2 == 0)?QR_TRUE:QR_FALSE;
						break;
					default: 
						bMask = ((((i * j) % 3) + ((i + j) % 2)) % 2 == 0)?QR_TRUE:QR_FALSE;
						break;
				}
				qrx->m_byModuleData[j][i] = (u8)((qrx->m_byModuleData[j][i] & 0xfe) | (((qrx->m_byModuleData[j][i] & 0x02) > 1) ^ bMask));
			}
		}
	}
}
//���ø�ʽ����Ϣ 
void SetFormatInfoPattern(int nPatternNo)
{
	int nFormatInfo;
	int i;
	int nFormatData;
	switch (qrx->m_nLevel)
	{
		case QR_LEVEL_M:
			nFormatInfo = 0x00; 
			break;
		case QR_LEVEL_L:
			nFormatInfo = 0x08; 
			break;
		case QR_LEVEL_Q:
			nFormatInfo = 0x18; 
			break;
		default: 
			nFormatInfo = 0x10; 
			break;
	}
	nFormatInfo += nPatternNo;
	nFormatData = nFormatInfo << 10;
	for (i = 0; i < 5; ++i)
	{
		if (nFormatData & (1 << (14 - i)))
		{
			nFormatData ^= (0x0537 << (4 - i)); 
		}
	}
	nFormatData += nFormatInfo << 10;
	nFormatData ^= 0x5412;
	for (i = 0; i <= 5; ++i)qrx->m_byModuleData[8][i] = (nFormatData & (1 << i)) ? '\x30' : '\x20';
	qrx->m_byModuleData[8][7] = (nFormatData & (1 << 6)) ? '\x30' : '\x20';
	qrx->m_byModuleData[8][8] = (nFormatData & (1 << 7)) ? '\x30' : '\x20';
	qrx->m_byModuleData[7][8] = (nFormatData & (1 << 8)) ? '\x30' : '\x20';
	for (i = 9; i <= 14; ++i)qrx->m_byModuleData[14 - i][8] = (nFormatData & (1 << i)) ? '\x30' : '\x20';
	for (i = 0; i <= 7; ++i)qrx->m_byModuleData[qrx->m_nSymbleSize - 1 - i][8] = (nFormatData & (1 << i)) ? '\x30' : '\x20';
	qrx->m_byModuleData[8][qrx->m_nSymbleSize - 8] = '\x30';
	for (i = 8; i <= 14; ++i)qrx->m_byModuleData[8][qrx->m_nSymbleSize - 15 + i] = (nFormatData & (1 << i)) ? '\x30' : '\x20';
} 
int CountPenalty(void)
{
	int nPenalty = 0;
	int i, j, k;
	int nCount = 0, s_nCount;
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize - 4; ++j)
		{
			int nCount = 1;
			for (k = j + 1; k < qrx->m_nSymbleSize; k++)
			{
				if (((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i][k] & 0x11) == 0))
					++nCount;
				else break;
			} 
			if (nCount >= 5)nPenalty += 3 + (nCount - 5); 
			j = k - 1;
		}
	} 
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize - 4; ++j)
		{
			int nCount = 1;

			for (k = j + 1; k < qrx->m_nSymbleSize; k++)
			{
				if (((qrx->m_byModuleData[j][i] & 0x11) == 0) == ((qrx->m_byModuleData[k][i] & 0x11) == 0))
					++nCount;
				else break;
			} 
			if (nCount >= 5)nPenalty += 3 + (nCount - 5); 
			j = k - 1;
		}
	} 
	for (i = 0; i < qrx->m_nSymbleSize - 1; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize - 1; ++j)
		{
			if ((((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i + 1][j]		& 0x11) == 0)) &&
				(((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i]	[j + 1] & 0x11) == 0)) &&
				(((qrx->m_byModuleData[i][j] & 0x11) == 0) == ((qrx->m_byModuleData[i + 1][j + 1] & 0x11) == 0)))
			{
				nPenalty += 3;
			}
		}
	} 
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize - 6; ++j)
		{
			if (((j == 0) ||				 (! (qrx->m_byModuleData[i][j - 1] & 0x11))) && // �� �܂��� �V���{���O
											 (   qrx->m_byModuleData[i][j]     & 0x11)   && // �� - 1
											 (! (qrx->m_byModuleData[i][j + 1] & 0x11))  && // �� - 1
											 (   qrx->m_byModuleData[i][j + 2] & 0x11)   && // �� ��
											 (   qrx->m_byModuleData[i][j + 3] & 0x11)   && // �� ��3
											 (   qrx->m_byModuleData[i][j + 4] & 0x11)   && // �� ��
											 (! (qrx->m_byModuleData[i][j + 5] & 0x11))  && // �� - 1
											 (   qrx->m_byModuleData[i][j + 6] & 0x11)   && // �� - 1
				((j == qrx->m_nSymbleSize - 7) || (! (qrx->m_byModuleData[i][j + 7] & 0x11))))   // �� �܂��� �V���{���O
			{
				if (((j < 2 || ! (qrx->m_byModuleData[i][j - 2] & 0x11)) && 
					 (j < 3 || ! (qrx->m_byModuleData[i][j - 3] & 0x11)) &&
					 (j < 4 || ! (qrx->m_byModuleData[i][j - 4] & 0x11))) ||
					((j >= qrx->m_nSymbleSize - 8  || ! (qrx->m_byModuleData[i][j + 8]  & 0x11)) &&
					 (j >= qrx->m_nSymbleSize - 9  || ! (qrx->m_byModuleData[i][j + 9]  & 0x11)) &&
					 (j >= qrx->m_nSymbleSize - 10 || ! (qrx->m_byModuleData[i][j + 10] & 0x11))))
				{
					nPenalty += 40;
				}
			}
		}
	} 
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize - 6; ++j)
		{
			if (((j == 0) ||				 (! (qrx->m_byModuleData[j - 1][i] & 0x11))) && // �� �܂��� �V���{���O
											 (   qrx->m_byModuleData[j]    [i] & 0x11)   && // �� - 1
											 (! (qrx->m_byModuleData[j + 1][i] & 0x11))  && // �� - 1
											 (   qrx->m_byModuleData[j + 2][i] & 0x11)   && // �� ��
											 (   qrx->m_byModuleData[j + 3][i] & 0x11)   && // �� ��3
											 (   qrx->m_byModuleData[j + 4][i] & 0x11)   && // �� ��
											 (! (qrx->m_byModuleData[j + 5][i] & 0x11))  && // �� - 1
											 (   qrx->m_byModuleData[j + 6][i] & 0x11)   && // �� - 1
				((j == qrx->m_nSymbleSize - 7) || (! (qrx->m_byModuleData[j + 7][i] & 0x11))))   // �� �܂��� �V���{���O
			{
				if (((j < 2 || ! (qrx->m_byModuleData[j - 2][i] & 0x11)) && 
					 (j < 3 || ! (qrx->m_byModuleData[j - 3][i] & 0x11)) &&
					 (j < 4 || ! (qrx->m_byModuleData[j - 4][i] & 0x11))) ||
					((j >= qrx->m_nSymbleSize - 8  || ! (qrx->m_byModuleData[j + 8][i]  & 0x11)) &&
					 (j >= qrx->m_nSymbleSize - 9  || ! (qrx->m_byModuleData[j + 9][i]  & 0x11)) &&
					 (j >= qrx->m_nSymbleSize - 10 || ! (qrx->m_byModuleData[j + 10][i] & 0x11))))
				{
					nPenalty += 40;
				}
			}
		}
	} 
	for (i = 0; i < qrx->m_nSymbleSize; ++i)
	{
		for (j = 0; j < qrx->m_nSymbleSize; ++j)
		{
			if (! (qrx->m_byModuleData[i][j] & 0x11))
			{
				++nCount;
			}
		}
	} 
	if( (50 - ((nCount * 100) / (qrx->m_nSymbleSize * qrx->m_nSymbleSize))) > 0) s_nCount = 50 - ((nCount * 100) / (qrx->m_nSymbleSize * qrx->m_nSymbleSize));
	else s_nCount = 0 - (50 - ((nCount * 100) / (qrx->m_nSymbleSize * qrx->m_nSymbleSize)));
	nPenalty += (s_nCount / 5) * 10; 
	return nPenalty;
}  















