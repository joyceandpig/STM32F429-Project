#ifndef __NETPLAY_H
#define __NETPLAY_H
#include "common.h"  
#include "lwip_comm.h"  
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "httpd.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

#define NET_RMEMO_MAXLEN		10000		//RMEMO�����յ��ַ���.����ܳ���65535
#define NET_SMEMO_MAXLEN		400			//RMEMO�����������400���ַ�.



#define NET_IP_BACK_COLOR		0X42F0		//IP���򱳾���ɫ
#define NET_COM_RIM_COLOR		0X7BCF		//�ָ�����ɫ
#define NET_MSG_FONT_COLOR		0X4A49		//��ʾ��Ϣ������ɫ
#define NET_MSG_BACK_COLOR		0XBE3B		//��ʾ��Ϣ������ɫ
#define NET_MEMO_BACK_COLOR		0XA599		//2��memo���򱳾���ɫ






void net_load_ui(void);
void net_msg_show(u16 y,u16 height,u8 fsize,u32 tx,u32 rx,u8 prot,u8 flag); 
void net_edit_colorset(_edit_obj *ipx,_edit_obj *portx,u8 prot,u8 connsta); 
u16 net_get_port(u8 *str);
u32 net_get_ip(u8 *str); 
void net_tcpserver_remove_timewait(void); 
void net_disconnect(struct netconn *netconn1,struct netconn *netconn2);
u8 net_test(void);
u8 net_play(void);
#endif















 



















