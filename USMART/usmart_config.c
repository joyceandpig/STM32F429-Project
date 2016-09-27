#include "usmart.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#ifdef	DEBUG
	#include "24cxx.h"
	#include "w25qxx.h"
#endif
#include "videoplay.h"

#include "fontupd.h"//�ֿ�
extern void MPU_Test(void);
extern void sys_reset(void);
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	
	(void*)sys_reset,"void sys_reset(void)",		
#endif		
	(void*)font_init,"u8 font_init(u8 cmd)",

#ifdef	DEBUG
	(void*) AT24CXX_Test,"void AT24CXX_Test(u8 n)",
	(void*) W25QXX_Test,"void W25QXX_Test(u8 n)",
#endif	
	(void*) font_init,"u8 font_init(u8 cmd)",
	(void*) video_play,"void video_play(void)",
};	

#include "usart.h"
void USMART_Send_Data(u8 data)
{//��ֲʹ��
	if(HAL_UART_Transmit(&USART1_Handler, &data, 1,2000) != HAL_OK)
	{
		
	}
}

const u16 USMART_RECV_LEN_MAX = 1024;
u8 USMART_RX_BUF[USMART_RECV_LEN_MAX];     //���ջ���,���USART_REC_LEN���ֽ�.


///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   

