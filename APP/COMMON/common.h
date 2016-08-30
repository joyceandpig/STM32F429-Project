#ifndef __COMMON_H
#define __COMMON_H 	
#include "sys.h"
#include "touch.h"	 
#include "includes.h"
#include "gui.h"
//#include "wm8978.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APPͨ�� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/2/16
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//V1.1 20140216
//�����Ը��ֱַ���LCD��֧��.
//V1.2 20140727
//�޸�app_show_float������һ��bug
////////////////////////////////////////////////////////////////////////////////// 	   

//Ӳ��ƽ̨��Ӳ���汾����	   	
#define HARDWARE_VERSION	   		14		//Ӳ���汾,�Ŵ�10��,��1.0��ʾΪ10
#define SOFTWARE_VERSION	    	210		//����汾,�Ŵ�100��,��1.00,��ʾΪ100
//Ӳ��V2.0
//1������TVS��Դ������
//2�������Դ����DCDC��Դ��������������ѹ������
//Ӳ��V2.2
//1��˿ӡλ����΢�仯.
//2��������ά��.
//V2.25  20121027
//����3.5���ֱ���LCD֧��
//V2.26  20131124
//1,���Ӷ�NT35310��������֧��
//2,�������µ�SYSTEM�ļ���,֧��MDK3~MDK4.
//3,ȫ�����V3.5��ͷ�ļ�.
//4,USMART�������µ�V3.1�汾,֧�ֺ���ִ��ʱ��鿴.
//V2.30 20140216
//1,���Ӷ�NT35510��������֧��
//2,���ӶԵ��ݴ�������֧��
//3,��������Ӧ��ͬ�ֱ���LCD����.
//4,�������µ�SYSTEM�ļ���,֧��MDK3~MDK5 




//ϵͳ���ݱ����ַ			  
#define SYSTEM_PARA_SAVE_BASE 		100		//ϵͳ��Ϣ�����׵�ַ.��100��ʼ.
			    

////////////////////////////////////////////////////////////////////////////////////////////
//��ͼ��/ͼƬ·��
extern u8*const APP_OK_PIC;			//ȷ��ͼ��
extern u8*const APP_CANCEL_PIC;		//ȡ��ͼ��
extern u8*const APP_UNSELECT_PIC;	//δѡ��ͼ��
extern u8*const APP_SELECT_PIC;		//ѡ��ͼ��
extern u8*const APP_VOL_PIC;		//����ͼƬ·��

extern u8*const APP_ASCII_S14472;	//���������144*72����������·�� 
extern u8*const APP_ASCII_S8844;	//���������88*44����������·�� 
extern u8*const APP_ASCII_S7236;	//���������72*36����������·�� 
extern u8*const APP_ASCII_S6030;	//����ܴ�����·��
extern u8*const APP_ASCII_5427;		//��ͨ������·��
extern u8*const APP_ASCII_3618;		//��ͨ������·��
extern u8*const APP_ASCII_2814;		//��ͨ������·��

extern u8* asc2_14472;				//��ͨ144*72���������
extern u8* asc2_8844;				//��ͨ����88*44���������
extern u8* asc2_7236;				//��ͨ����72*36���������
extern u8* asc2_s6030;				//���������60*30���������
extern u8* asc2_5427;				//��ͨ����54*27���������
extern u8* asc2_3618;				//��ͨ����36*18���������
extern u8* asc2_2814;				//��ͨ����28*14���������

extern const u8 APP_ALIENTEK_ICO1824[];	//��������ͼ��,�����flash
extern const u8 APP_ALIENTEK_ICO2432[];	//��������ͼ��,�����flash
extern const u8 APP_ALIENTEK_ICO3648[];	//��������ͼ��,�����flash
////////////////////////////////////////////////////////////////////////////////////////////
//APP���ܹ�����Ŀ
#define APP_FUNS_NUM	27

//app��Ҫ���ܽ������
extern u8*const APP_MFUNS_CAPTION_TBL[APP_FUNS_NUM][GUI_LANGUAGE_NUM];
extern u8*const APP_DISK_NAME_TBL[3][GUI_LANGUAGE_NUM];

extern u8*const APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_DELETE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
//ƽ���ߵ���ֹ��ɫ����
#define WIN_SMOOTH_LINE_SEC	0XB1FFC4	//��ֹ��ɫ
#define WIN_SMOOTH_LINE_MC	0X1600B1	//�м���ɫ

//��������ѡ����Ŀ��������Ϣ
#define APP_ITEM_BTN1_WIDTH		60	  		//��2������ʱ�Ŀ��
#define APP_ITEM_BTN2_WIDTH		100			//ֻ��1������ʱ�Ŀ��
#define APP_ITEM_BTN_HEIGHT		30			//�����߶�
#define APP_ITEM_ICO_SIZE		32			//ICOͼ��ĳߴ�

#define APP_ITEM_SEL_BKCOLOR	0X0EC3		//ѡ��ʱ�ı���ɫ
#define APP_WIN_BACK_COLOR	 	0XC618		//���屳��ɫ


#define APP_FB_TOPBAR_HEIGHT	20			//�ļ��������,���������ĸ߶�
#define APP_FB_BTMBAR_HEIGHT	20			//�ļ��������/���Խ���,�ײ������ĸ߶�
#define APP_TEST_TOPBAR_HEIGHT	20			//���Խ���,���������߶�

//��ֵ����
#define	app_pi	3.1415926535897932384626433832795 
/////////////////////////////////////////////////////////////////////////
																			    
u32  app_get_rand(u32 max);
void app_srand(u32 seed);
void app_set_lcdsize(u8 mode);
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 app_get_numlen(long long num,u8 dir);
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor);
void app_filebrower(u8 *topname,u8 mode);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb);

u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height);
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor);
u8 * app_get_icopath(u8 mode,u8 *selpath,u8 *unselpath,u8 selx,u8 index);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *selx,u8 mode,u8*caption);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

u8 app_system_file_check(u8* diskx);//ϵͳ�ļ����
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode);
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize);
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8* src);
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2);
void app_get_version(u8*buf,u32 ver,u8 len);//�õ��汾��

void app_usmart_getsn(void);		//USMARTר��.
u8 app_system_parameter_init(void);	//ϵͳ��Ϣ��ʼ��
void app_lcd_auto_bklight(void);	//LCD�����Զ����ƺ���
//void app_wm8978_volset(u8 vol);		//WM8978��������
//void app_wm8978_eqset(_wm8978_obj *wmset,u8 eqx);//eq����
//void app_wm8978_setall(void);		//��������WM8978����
void app_set_default(void);			//�ָ�Ĭ������
#endif




























































