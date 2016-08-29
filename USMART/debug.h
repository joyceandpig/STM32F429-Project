#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup debug_config_macro
  * @{
  */
//#ifndef DEBUG	
//	#define DEBUG					//���Ϊ���а�
//	#warning "���Ե��Ա���"
//#endif
	
#define VERSION "0.0.1"
	
#include "stdlib.h"
#include "usmart.h"


#if 1
	extern char aTxBuffer[1024];
	extern void UART_Printf(char *pData);	
	#define MyPrintf(...)		do{sprintf(aTxBuffer,__VA_ARGS__);UART_Printf(aTxBuffer);}while(0)/*ָ�����,Ĭ��ʹ�ô�����Ϊ�������ʹ������ͨ�Žӿ�,����ʵ��MyPrintf����*/
#else
	#define MyPrintf(...)		printf(__VA_ARGS__)/*Ĭ�������MCU���printf �����ض��򣡣���*/
#endif
extern unsigned long int os_time_get(void); /*��ȡϵͳ����ʱ��,��λms.���û�ʵ��*/
extern unsigned long int sys_time_get(void); /*��ȡϵͳ����ʱ��,��λms.���û�ʵ��*/
	
#ifdef	OS_UCOS
	#define GET_SYSTEM_TIME			os_time_get() /*os_time_get()*/ /*��ȡϵͳ����ʱ�亯��*/	
#else
	#define GET_SYSTEM_TIME			sys_time_get()					/*ϵͳʱ��*/
//	#warning	"Not use uc/OS Operating System!"
	#define Sleep(x)	 
#endif

/**
  * @}
  */
/** @addtogroup �����ƺ�
* @{
*/ 
/*
ANSI������ �ο�:http://my.oschina.net/u/1587819/blog/227164
*/

//����ɫ: 40--49								ǰ��ɫ: 30--39	
															//����ɫ		  ǰ��ɫ
#define CONTROL_BLACK		0			//40: �� 		30: ��	
#define CONTROL_RED			1     //41: �� 		31: ��  
#define CONTROL_GREEN		2     //42: ��		32: ��  
#define	CONTROL_YELLOW	3     //43: �� 		33: ��  
#define CONTROL_BLUE		4     //44: �� 		34: ��  
#define CONTROL_PURPLE	5     //45: �� 		35: ��  
#define CONTROL_NAVY		6     //46: ���� 	36: ����  
#define	CONTROL_GRAY		7     //47: ��ɫ 	37: ��ɫ  
//#define CONTROL_BLACK		8	            //48: ��ɫ 	38: ��ɫ  
#define	CONTROL_WHITE		9     //49: ��ɫ 	39: ��ɫ  

#define LINUX_CONTROL

#ifdef	LINUX_CONTROL
	#define	Telnet_Control(...)				MyPrintf(__VA_ARGS__)/**/
#else
	#define	Telnet_Control(...)				/*��Linux����̨ģʽ �������ʽ���Ʒ�*/
#endif

#define TELNET_RESET_CURSOR() 		Telnet_Control("\033[H")			 	/*��긴λ*/

#define TELNET_UNDER_LINE()				Telnet_Control("\03[4m")				/*�»���*/
#define TELNET_CURSOR_BLINK()			Telnet_Control("\033[5m") 			/*�����˸*/
#define TELNET_HIGHT_LIGHT() 			Telnet_Control("\033[7m")				/*����*/ 
#define TELNET_UN_HIGHT_LIGHT()		Telnet_Control("\033[8m")				/*ȡ������*/

#define TELNET_COLOR(x,y)					do{Telnet_Control("\033[%dm\033[%dm",x+30,y+40);}while(0)	/*033[30m -- \033[37m ����ǰ��ɫ*/	
																																				/*\033[40m -- \033[47m ���ñ���ɫ */
#define TELNET_MOVEUP(x) 					Telnet_Control("\033[%dA", (x)) /*���ƹ��*/  
#define TELNET_MOVEDOWN(x) 				Telnet_Control("\033[%dB", (x)) /*���ƹ��*/   
#define TELNET_MOVERIGHT(y)				Telnet_Control("\033[%dC",(y)) 	/*���ƹ��*/ 
#define TELNET_MOVELEFT(y) 				Telnet_Control("\033[%dD", (y)) /*���ƹ��*/ 
#define TELNET_MOVETO(x,y) 				Telnet_Control("\033[%d;%dH", (x), (y)) /*��λ���*/  
#define TELNET_CLEAR_SCREEN()	 		Telnet_Control("\033[2J") 			/*�����Ļ*/ 
#define TELNET_CLEAR_LINE()				Telnet_Control("\033[K")				/*\033[K ����ӹ�굽��β������ */

#define TELNET_SAVE_CURSOR()			Telnet_Control("\033[s")				/*������λ��*/
#define	TELNET_RESTORE_CURSOR()		Telnet_Control("\033[u")				/*�ָ����λ��*/
#define TELNET_HIDE_CURSOR() 			Telnet_Control("\033[?25l") 		/*���ع��*/  
#define TELNET_SHOW_CURSOR() 			Telnet_Control("\033[?25h") 		/*��ʾ���*/  

/**
  * @}
  */
	
/** @addtogroup Exported_macro
  * @{
  */

#define CLC_CMD	do{	\
		if(USMART_StatusCheck(USMART_RECV_FLAG_ENTER))	\
		{	\
			u8 i =0;	\
			for( ;i< sizeof("[root@Terminal]") ;i++)	\
			{\
				Telnet_Control("\x8\x1b[\xff\x50");	\
			}\
			USMART_StatusClear(USMART_RECV_FLAG_ENTER);/*����س�״̬*/	\
		}	\
	}while(0)/*���[sunsheen@Terminal]*/
	
#define system_sec		GET_SYSTEM_TIME/1000 /*����ʱ��s*/
#define system_msec		GET_SYSTEM_TIME%1000 /*����ʱ��ms*/

#ifdef OS_UCOS
	#define ERR_PRINT_TIME()	do{CLC_CMD;TELNET_COLOR(CONTROL_RED,CONTROL_WHITE);		MyPrintf("[E:%2d.%03d] ",system_sec, system_msec);}while(0)
	#define WARN_PRINT_TIME()	do{CLC_CMD;TELNET_COLOR(CONTROL_YELLOW,CONTROL_WHITE);		MyPrintf("[W:%2d.%03d] ",system_sec, system_msec);}while(0)
	#define DBG_PRINT_TIME()	do{CLC_CMD;TELNET_COLOR(CONTROL_BLACK,CONTROL_WHITE);	MyPrintf("[D:%2d.%03d] ",system_sec, system_msec);}while(0)
	#define INFO_PRINT_TIME()	do{CLC_CMD;TELNET_COLOR(CONTROL_BLACK,CONTROL_WHITE);MyPrintf("[I:%2d.%03d] ",system_sec, system_msec);}while(0)
#else
	#define ERR_PRINT_TIME()	
	#define WARN_PRINT_TIME()	
	#define DBG_PRINT_TIME()	
	#define INFO_PRINT_TIME()	
#endif

#define P_INFO(...) 			do{INFO_PRINT_TIME();MyPrintf(__VA_ARGS__); MyPrintf("\r\n");}while(0)	/*��ӡ��Ϣ*/

#define P_ERR(...) 				do{ERR_PRINT_TIME();MyPrintf(__VA_ARGS__);MyPrintf("\r\n");}while(0)/*��ӡ����*/
#define P_ERR_MISS(...) 	do{ERR_PRINT_TIME();MyPrintf("%s miss\n",__FUNCTION__); MyPrintf("\r\n");}while(0)/*��ӡmiss��Ϣ*/
#define P_ERR_MEM(...) 		do{ERR_PRINT_TIME();MyPrintf("%s mem err\n",__FUNCTION__); MyPrintf("\r\n");}while(0)/*��ӡ�ڴ������Ϣ*/
#define P_ERR_FUN(...) 		do{\
															ERR_PRINT_TIME();	\
															MyPrintf("%s (%d): \"%s\" function is error!\n",__FILE__,__LINE__,__FUNCTION__); \
															MyPrintf("\r\n");	\
														}while(0)/*��ӡ����������Ϣ*/

#define P_WARN(...) 		  do{\
															WARN_PRINT_TIME();	\
															MyPrintf("Warning! %s (%d): \"%s\" \n",__FILE__,__LINE__,__FUNCTION__); \
															MyPrintf(__VA_ARGS__);\
															MyPrintf("\r\n");	\
														}while(0)/*��ӡ����������Ϣ*/

#define dump_hex(tag, buff, size) do{	\
		int dump_hex_i;	\
		DBG_PRINT_TIME();	\
		MyPrintf("%s[", tag);\
		for (dump_hex_i = 0; dump_hex_i < size; dump_hex_i++)\
		{\
			MyPrintf("%02x ", ((char*)buff)[dump_hex_i]);\
		}\
		MyPrintf("]\r\n");\
}while(0)

#ifdef DEBUG
	#define P_DBG(...)  			do{DBG_PRINT_TIME();MyPrintf(__VA_ARGS__); MyPrintf("\r\n");}while(0)	/*��ӡ��Ϣ*/
	#define P_TRACK(...) 			do{DBG_PRINT_TIME();MyPrintf("%s,line:%d",__FUNCTION__, __LINE__);MyPrintf("\r\n");}while(0)/*λ��׷��*/
	#define P_ENTER(...) 			do{DBG_PRINT_TIME();MyPrintf("enter %s\n",__FUNCTION__);MyPrintf("\r\n");}while(0)/*��ӡ������Ϣ�����뺯��*/
	#define P_EXIT(...) 			do{DBG_PRINT_TIME();MyPrintf("exit %s\n",__FUNCTION__);MyPrintf("\r\n");}while(0)/*��ӡ������Ϣ���˳�����*/
	#define P_HEX(X,Y)				dump_hex("",(unsigned char*)X, Y)
#else	
	#define P_DBG(...) 			
	
	#define P_TRACK(...) 			
	#define P_ENTER(...)  		
	#define P_EXIT(...)  	
	#define P_HEX(X,Y)		
#endif/* DEBUG */
		
#define u_printf(X,...)	P_##X(__VA_ARGS__)
	
/**
  * @}
  */

/**
  * @}
  */

	
#define assert(x)	\
do{                                                                     \
	if (!(x))                                                             \
	{                                                                     \
		u_printf(ERR, "%s:%d assert " #x "failed\r\n", __FILE__, __LINE__);	\
		while(1);										\
	}                                                                     \
}while(0)

typedef enum
{
	PRINT_TYPE_D, PRINT_TYPE_H, PRINT_TYPE_P_D, PRINT_TYPE_P_H,
} STAT_PRINT_TYPE;

#if SYS_MONITOR
	#define INC_MONITER_ERR_VALUE(x)    			(x->err_cnt)++
	#define ADD_MONITER_READ_VALUE(x, v)    	(x->read_total_size) += (v)
	#define ADD_MONITER_WRITE_VALUE(x, v)    	(x->write_total_size) += (v)

	#define INC_MONITOR_VALUE(X) 							X.value++
	#define ADD_MONITOR_VALUE(X, V) 					X.value += V
	#define SET_MONITER_VALUE(X, V)  					X.value = V

	#define DECLARE_MONITOR_ITEM(TAG, NAME)	 struct dev_monitor_item NAME  \
	__attribute__((section("monitor_item_table"))) = {TAG, 0}

	#define EXTERN_MONITOR_ITEM(NAME)	 extern struct dev_monitor_item NAME;


	#define INC_MONITOR_ITEM_VALUE(X)  				X.value++
	#define DEC_MONITOR_ITEM_VALUE(X)  				X.value--
	#define SET_MONITOR_ITEM_VALUE(X, V)  		X.value = V
	#define ADD_MONITOR_ITEM_VALUE(X, V) 			X.value += V
	#define SUB_MONITOR_ITEM_VALUE(X, V) 			X.value -= V
	#define GET_MONITOR_ITEM_VALUE(X) 				X.value
#else
	#define DECLARE_MONITOR_ITEM(TAG, NAME)
	#define INC_MONITOR_ITEM_VALUE(X)
	#define DEC_MONITOR_ITEM_VALUE(X)
	#define SET_MONITOR_ITEM_VALUE(X, V)
	#define ADD_MONITOR_ITEM_VALUE(X, V)
	#define SUB_MONITOR_ITEM_VALUE(X, V)

	#define INC_MONITER_ERR_VALUE(x)
	#define ADD_MONITER_READ_VALUE(x, v)
	#define ADD_MONITER_WRITE_VALUE(x, v)

	#define INC_MONITOR_VALUE(X)
	#define ADD_MONITOR_VALUE(X, V)
	#define SET_MONITER_VALUE(X, V)
	#define GET_MONITOR_ITEM_VALUE(X)
	#define EXTERN_MONITOR_ITEM(NAME)
#endif

#ifdef __cplusplus
}
#endif

#endif
