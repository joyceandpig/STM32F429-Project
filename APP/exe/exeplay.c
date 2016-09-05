#include "exeplay.h"
#include "stm32f4xx_hal_rtc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-������ ����	   
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

//�ٺ���,��PCָ���ܵ��µ�main����ȥ
dummyfun jump2app;		
//�����ŵ�ַ
extern u8 mem2base[MEM2_MAX_SIZE];	
RTC_HandleTypeDef RTC_BKP_Handler;

//ִ��BIN�ļ�ǰ����ʾ
u8*const exeplay_remindmsg_tbl[GUI_LANGUAGE_NUM]=
{
"  ִ��BIN�ļ���,ϵͳ���޷���������,ֻ�ܸ�λ����.��ȷ��ִ��ô?",
"  ����BIN�ļ���,ϵ�y���o���^�m�\��,ֻ�܏�λ�؆�.���_�J����ô?",
"  After run the BIN file,SYSTEM will unable to continue.Confirm?",
};
//����
u8*const exeplay_warning_tbl[GUI_LANGUAGE_NUM]=
{
"����:",
"����:",
"Warning:",
};
//APP����Ƿ���ʾ��
u8*const exeplay_apperrmsg_tbl[GUI_LANGUAGE_NUM]=
{					   							    
"�Ƿ���SRAM APP����","�Ƿ���SRAM APP����","Illegal SRAM APP File!",	 
};	  
  
//д���־ֵ
//val:��־ֵ
void exeplay_write_appmask(u16 val)
{	
	__HAL_RCC_PWR_CLK_ENABLE();//ʹ�ܵ�Դʱ��PWR
	HAL_PWR_EnableBkUpAccess();//ȡ����������д����
	
	HAL_RTCEx_BKUPWrite(&RTC_BKP_Handler,RTC_BKP_DR0,val);//����Ѿ���ʼ������
}
//���������ʼ��ʱ�򱻵���.
//����Ƿ���app������Ҫִ��.�����,��ֱ��ִ��.
void exeplay_app_check(void)
{
	if(HAL_RTCEx_BKUPRead(&RTC_BKP_Handler,RTC_BKP_DR0)!=0X5050)//���BKP1,���Ϊ0X5050,��˵����Ҫִ��app����
	{
		exeplay_write_appmask(0X0000);	//д��0,��ֹ��λ���ٴ�ִ��app����.
		
		SDRAM_Init();					//��ʼ��SDRAM,��Ϊ��Ҫ���ⲿsram�������ݵ��ڲ�sram
		mymemcpy((u8*)EXEPLAY_APP_BASE,(u8*)EXEPLAY_SRC_BASE,EXEPLAY_APP_SIZE);//����EXEPLAY_APP_SIZE�ֽ� 
		jump2app=(dummyfun)*(vu32*)(EXEPLAY_APP_BASE+4);	//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)EXEPLAY_APP_BASE);			 		//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ�Ŷ�ջ��ַ)
		jump2app();	  										//ִ��app����.
	}
}  
//������   
u8 exe_play(void)
{
  	FIL* f_exe;		 
	DIR exeplaydir;		//exeplaydirר��	  
	FILINFO *exeplayinfo;	 	   
	u8 res;
	u8 rval=0;			//����ֵ	  
	u8 *pname=0;
	u8 *fn;
  
 	_btn_obj* rbtn;		//���ذ�ť�ؼ�
   	_filelistbox_obj * flistbox;
	_filelistbox_list * filelistx; 	//�ļ�

	app_filebrower((u8*)APP_MFUNS_CAPTION_TBL[8][gui_phy.language],0X07);//��ʾ��Ϣ
   	flistbox=filelistbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.listfsize);//����һ��filelistbox
 	if(flistbox==NULL)rval=1;							//�����ڴ�ʧ��.
	else  
	{
		flistbox->fliter=FLBOX_FLT_BIN;	//����BIN�ļ�
		filelistbox_add_disk(flistbox);	//��Ӵ���·��
 		filelistbox_draw_listbox(flistbox);
	} 	
	f_exe=(FIL *)gui_memin_malloc(sizeof(FIL));	//����FIL�ֽڵ��ڴ����� 
	if(f_exe==NULL)rval=1;//����ʧ��

	rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
	exeplayinfo=(FILINFO*)gui_memin_malloc(sizeof(FILINFO));//����FILENFO�ڴ�
	if(!exeplayinfo||!rbtn)rval=1;	//û���㹻�ڴ湻���� 
	else
	{																				
	 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];//���� 
	 	rbtn->font=gui_phy.tbfsize;//�����µ������С	 	 
		rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
		rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
		btn_draw(rbtn);//����ť
	}
	
   	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(5);
 		if(system_task_return)break;			//TPAD����
		filelistbox_check(flistbox,&in_obj);	//ɨ���ļ�
		res=btn_check(rbtn,&in_obj);
		if(res)
		{
			if(((rbtn->sta&0X80)==0))//��ť״̬�ı���
			{
				if(flistbox->dbclick!=0X81)
				{
 					filelistx=filelist_search(flistbox->list,flistbox->selindex);//�õ���ʱѡ�е�list����Ϣ
					if(filelistx->type==FICO_DISK)//�Ѿ�������������
					{				 
						break;
					}else filelistbox_back(flistbox);//�˻���һ��Ŀ¼	 
				} 
 			}	 
		}
		if(flistbox->dbclick==0X81)//˫���ļ���
		{	    
     		rval=f_opendir(&exeplaydir,(const TCHAR*)flistbox->path); //��ѡ�е�Ŀ¼
			if(rval)break;	 			   
			dir_sdi(&exeplaydir,flistbox->findextbl[flistbox->selindex-flistbox->foldercnt]);
	 		rval=f_readdir(&exeplaydir,exeplayinfo);//��ȡ�ļ���Ϣ
			if(rval)break;//�򿪳ɹ�    
			fn=(u8*)(exeplayinfo->fname);
			pname=gui_memin_malloc(strlen((const char*)fn)+strlen((const char*)flistbox->path)+2);//�����ڴ�
			if(pname==NULL)rval=1;//����ʧ��
			{
				pname=gui_path_name(pname,flistbox->path,fn);	//�ļ�������·��  
				rval=f_open(f_exe,(const TCHAR*)pname,FA_READ);	//ֻ����ʽ���ļ�
				if(rval)break;	//��ʧ��   
				if(f_exe->obj.objsize<MEM2_MAX_SIZE)//���Էŵ���,�����û�ȷ��ִ��
				{  
  					gui_memset(mem2base,0,f_exe->obj.objsize);	//�����Ƭ�ڴ�
 	 				rval=f_read(f_exe,mem2base,f_exe->obj.objsize,(UINT*)&br);//����BIN����������       
  					if(rval)break;										//��ʧ��,ֱ���˳�   
					if(((*(vu32*)(mem2base+4))&0xFF000000)==0x20000000)//�ж��Ƿ�Ϊ0X20XXXXXX.���APP�ĺϷ��ԡ�
					{
	  					res=window_msg_box((lcddev.width-200)/2,(lcddev.height-160)/2,200,160,(u8*)exeplay_remindmsg_tbl[gui_phy.language],(u8*)exeplay_warning_tbl[gui_phy.language],16,0,0X03,0);//��ʾ������Ϣ
						if(res==1)//������ȷ�ϼ�
						{
							exeplay_write_appmask(0X5050);	//д���־��,��־��app��Ҫ����
//							Sys_Soft_Reset();				//����һ����λ
							sys_reset();
						}
					}else//�Ƿ�APP�ļ� 
					{
						window_msg_box((lcddev.width-160)/2,(lcddev.height-80)/2,160,80,(u8*)exeplay_apperrmsg_tbl[gui_phy.language],(u8*)exeplay_warning_tbl[gui_phy.language],12,0,0,0);//��ʾ�Ƿ���APP�ļ�
						delay_ms(1500);//��ʱ1.5��
					}
					filelistbox_rebuild_filelist(flistbox);//�ؽ�flistbox
				} 
			} 
			flistbox->dbclick=0;	//���÷��ļ����״̬
 			gui_memin_free(pname);	//�ͷ��ڴ�		  
		}
	}	
	filelistbox_delete(flistbox);	//ɾ��filelist
	btn_delete(rbtn);				//ɾ����ť	 	  
 	gui_memin_free(pname);			//�ͷ��ڴ�	
 	gui_memin_free(exeplayinfo);	//�ͷ��ڴ�	
	gui_memin_free(f_exe);					   
	return rval;   
}















