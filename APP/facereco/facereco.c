#include "facereco.h" 
#include "camera.h"
#include "key.h"
#include "timer.h" 
#include "audioplay.h" 
#include "ltdc.h"  
#include "t9input.h"
#include "appplay.h" 
#include "atk_frec.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-����ʶ�� ����	   
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

//��setings.c���涨��
extern u8*const sysset_remindmsg_tbl[2][GUI_LANGUAGE_NUM];
//��camera.c���涨��
extern u8*const camera_remind_tbl[4][GUI_LANGUAGE_NUM];

u16 reg_time=0; 				//ʶ��һ����������ʱ��
u16 frec_curline=0;				//����ͷ�������,��ǰ�б��
u32 *frec_dcmi_line_buf[2];		//RGB��ʱ,����ͷ����һ��һ�ж�ȡ,�����л��� 
_frec_obj frec_dev;				//����ʶ����ƽṹ��

//��ʾ��Ϣ
u8*const frec_remind_msg_tbl[14][GUI_LANGUAGE_NUM]=
{
"��ʾ:�޿���ģ��,�������ģ��","��ʾ:�o����ģ��,Ո�����ģ��","Remind:No Model,Pls Add Model",
"ѡ�����:","�x�����:","Choose option:",
"�����б�","��Ę�б�","Face List",    
"�༭����","��݋��Ę","Edit Face",    
"�������","�����Ę","Add Face",      
"ʶ����","�R�e�Y��","Result",  
"���ڱ���...","���ڱ���...","Saving...",
"����ɹ�!","����ɹ�!","Save OK!", 
"����ʧ��!","����ʧ��!","Save Failed!", 
"ɾ���ɹ�!","�h���ɹ�!","Delete OK!", 
"ɾ��ʧ��!","�h��ʧ��!","Delete Failed!",  
"����ʶ��...","�����R�e...","Recognizing...",  
"���Ȱ�KEY_UP���ģ��!","Ո�Ȱ�KEY_UP���ģ��!","Press KEY_UP add face!", 
"�޷�ʶ��!","�o���R�e!","Recognize Failed!", 
};
//ѡ�����Ŀ
u8*const frec_option_tbl[GUI_LANGUAGE_NUM][2]=
{
{"�༭","ɾ��"},
{"��݋","�h��"},		 
{"Edit","Delete"},
}; 
//����/ȡ����ť
u8*const frec_sc_btn_tbl[GUI_LANGUAGE_NUM][2]=
{
{"����","ȡ��"},
{"����","ȡ��"},		 
{"Save","Cancel"},
}; 
//��Ƭ��Ϣ
u8*const frec_card_info_tbl[GUI_LANGUAGE_NUM][4]=
{
{" ����:"," �绰:"," �Ա�:"," ��ʱ:"},
{" ����:"," �Ԓ:"," �Ԅe:"," �ĕr:"},
{" Name:","Phone:"," Sex :"," Time:"}, 
};
////////////////////////////////////////////////////////////////////////////////////////
//����ʶ����������

//����FREC�ڵ�
//����ֵ:�ڵ��׵�ַ
frec_node* frec_node_creat(void)
{
	frec_node *pnode;
	pnode=gui_memex_malloc(sizeof(frec_node));
	if(pnode)
	{
		pnode->pname=0;	//����ָ������
		pnode->pnum=0;	//����ָ������
		pnode->sex=0;	//�Ա�ָ������
		pnode->index=0;	//Ĭ�ϱ��Ϊ0 
		pnode->next=0; 
	}
	return pnode; 
}
//�ͷ�һ���ڵ���ڴ�
//pnode:�ڵ��ַ
//mode:0,�����ͷŽڵ�����(ֻɾ������,�ڵ�ͷ��ַ����.)
//     1,�����ڵ�ͷҲ�ͷŵ�(�����ڵ�ɾ��)
void frec_node_free(frec_node * pnode,u8 mode)
{
	if(pnode==NULL)return;	//�սڵ�
	if(pnode->pname)gui_memex_free(pnode->pname);	//�ͷ�pname�ڴ�
	if(pnode->pnum)gui_memex_free(pnode->pnum);		//�ͷ�pnum�ڴ�  
	if(pnode->sex)gui_memex_free(pnode->sex);		//�ͷ�sex�ڴ�  
	if(mode==1)gui_memex_free(pnode);				//�ͷ�pnode�ڴ� 
}
//�ڵ�i���ڵ�����һ���ڵ�
//head:��ͷ
//pnew:�½ڵ�
//i:����ڵڼ����ڵ�֮��,0XFFFF,��ĩβ���
//����ֵ:0,��ӳɹ�
//    ����,���ʧ��.
u8 frec_node_insert(frec_node* head,frec_node *pnew,u16 i)
{
	u16 j;
	frec_node *p,*p1;
	p=p1=head;
	for(j=0;j<i&&p1!=NULL;j++)
	{
		p1=p1->next;
		if(p1!=NULL)
		{
			p=p1;
		}
	}
	if(p1==NULL&&(i!=0XFFFF))return 1;//�޷������i���ڵ�
	pnew->next=p->next;//����ڵ��nextָ������ڵ�
	p->next=pnew;//�½ڵ�����i���ڵ�֮��
	return 0;//����ɹ�	
}
//ɾ��һ��SMS�ڵ�
//head:��ͷ
//i:Ҫɾ���Ľڵ��,0,��ʾ��ͷ(��ͷ�����øú���ɾ��)
//����ֵ:0,ɾ���ɹ�
//    ����,ɾ��ʧ��/����
u8 frec_node_delete(frec_node* head,u16 i)
{
	u16 j;
	frec_node *p,*p1=0;
	if(i==0)return 1;
	p=head;
	for(j=0;j<i&&p->next!=NULL;j++)
	{
		p1=p;
		p=p->next;
	}
	if(p==NULL||p1==NULL)return 2;//�����ڵĽڵ� 
	p1->next=p->next;	//����ǰһ���ڵ��ָ��
	frec_node_free(p,1);	//�ͷŽڵ��ڴ� 
	return 0;//ɾ���ɹ�	
}	
//�õ�һ���ڵ���Ϣ
//head:��ͷ
//i:Ҫ�õ���Ϣ�Ľڵ��,0,��ʾ��ͷ
//����ֵ:0,�õ���Ϣʧ��
//    ����,�ڵ��ַ
frec_node* frec_node_getnode(frec_node* head,u16 i)
{
	u16 j;
	frec_node *p; 
	p=head;
	for(j=0;j<i&&p!=NULL;j++)
	{
		p=p->next;
	}
	return p;//���ؽڵ��ַ
}
//�õ�һ���ڵ���Ϣ
//head:��ͷ
//index:Ҫ�õ��Ľڵ��index���
//����ֵ:0,�õ���Ϣʧ��
//    ����,�ڵ��ַ
frec_node* frec_node_getnode_index(frec_node* head,u8 index)
{
	frec_node *p; 
	p=head;
	while(p!=NULL)
	{
		if(p->index==index)break;
		p=p->next;
		
	}
	return p;//���ؽڵ��ַ
}
//�õ����������С
//head:��ͷ 
//����ֵ:0,�����С(�ڵ����)
u16 frec_node_getsize(frec_node* head)
{
	u16 j;
	frec_node *p; 
	p=head;
	for(j=0;p!=NULL;j++)
	{
		p=p->next;
	}
	return j;//���ؽڵ��ַ
}	
//ɾ����������
//head:��ͷ 
//����ֵ:0,�����С(�ڵ����)
void frec_node_destroy(frec_node* head)
{ 
	frec_node *p,*p1; 
	p=head;
	while(p->next!=NULL)
	{
		p1=p->next;
		p->next=p1->next;
		frec_node_free(p1,1);	//ɾ��һ���ڵ�
	} 
	frec_node_free(p,1);		//ɾ�����һ���ڵ�
}	
////////////////////////////////////////////////////////////////////////////////////////
#define ATK_FREC_INFO_PNAME 		"2:ATK_FREC/INFO_%03d.bin"		//����ģ�ʹ洢�ļ���+�ļ���,�洢��NAND FLASH

//����һ��������Ϣ����
//ʵ���Ͼ��Ǳ���һ���ļ�.
//�ļ���ʽ:
//������ͼƬ����,ͼƬ�ߴ�(90*120),Ȼ��30���ֽ�,�洢����.����20���ֽ�,�洢����;���1���ֽڴ洢�Ա�.
//��: ͼƬ����(90*120*2�ֽ�)+����(30�ֽ�)+����(20�ֽ�)+�Ա�(20���ֽ�) 
//�ļ��ܳ���: 90*120*2+30+20+20= 21670�ֽ�
//frecdev:����ʶ����ƽṹ�� 
//����ֵ:0,����
//    ����,�������
u8 appplay_frec_save_a_faceinfo(_frec_obj *frecdev)
{  	
	u8* path;
	FIL *fp; 
	u32 fw;
	u8 res=0;
	u8 *p=(u8*)frecdev->databuf;
	p+=90*120*2;	//ƫ�Ƶ�ͼ������ĩβ
 	memset(p,0,70);	//����
 	if(frecdev->face->pname)sprintf((char*)p,"%s",frecdev->face->pname);//�洢����
	p+=30;
 	if(frecdev->face->pnum)sprintf((char*)p,"%s",frecdev->face->pnum);	//�洢����
	p+=20;
 	if(frecdev->face->sex)sprintf((char*)p,"%s",frecdev->face->sex);	//�洢�Ա� 
	path=gui_memin_malloc(30);			//�����ڴ�
	fp=gui_memin_malloc(sizeof(FIL));	//�����ڴ�
	if(!fp)
	{
		gui_memin_free(path);
		return 1;
	}
	sprintf((char*)path,ATK_FREC_INFO_PNAME,frecdev->face->index); 
	res=f_open(fp,(char*)path,FA_WRITE|FA_CREATE_ALWAYS);
	if(res==FR_OK)
	{
		res=f_write(fp,(u8*)frecdev->databuf,90*120*2+70,&fw);	//д���ļ�		
	}
	f_close(fp);
	if(res)res=2;
	gui_memin_free(path);
	gui_memin_free(fp);
	return res;	 
}
//ɾ��һ��������Ϣ����
//index:Ҫ���������λ��(һ����ռһ��λ��),��Χ:0~MAX_LEBEL_NUM-1  
//����ֵ:0,����
//    ����,�������
u8 appplay_frec_delete_a_faceinfo(u8 index)
{
	u8* path;  
	u8 res;
	path=gui_memin_malloc(30);			//�����ڴ� 
	if(!path)
	{ 
		return ATK_FREC_MEMORY_ERR;
	}
	sprintf((char*)path,ATK_FREC_INFO_PNAME,index); 
	res=f_unlink((char*)path);  
	gui_memin_free(path);
	return res;	
}



//���ؿ�Ƭ����
//����ֵ:0,�ɹ�;����,ʧ��;
u8 appplay_frec_card_ui(u16 xoff,u16 yoff,u16 width,u16 height) 
{ 
 	_window_obj* twin=0;	//���� 
	twin=window_creat(xoff,yoff,width,height,0,0X01,16);//��������
	if(twin)
	{
		twin->captionbkcu=APPPLAY_IN_BACKCOLOR;		//Ĭ��caption�ϲ��ֱ���ɫ
		twin->captionbkcd=APPPLAY_IN_BACKCOLOR;		//Ĭ��caption�²��ֱ���ɫ
		twin->captioncolor=APPPLAY_IN_BACKCOLOR;	//Ĭ��caption����ɫ
	 	twin->windowbkc=APPPLAY_IN_BACKCOLOR;	 	//Ĭ�ϱ���ɫ
	 
		window_draw(twin);
	}else  return 1;
	window_delete(twin);
	return 0;
}
//��ʾʶ����
//pnode:Ҫ�洢�������Ľڵ�
//frecdev:����ʶ����ƽṹ��  
//time:��ʱ.(��λ:ms)
//����ֵ:0,����
//    ����,�������
u8 appplay_frec_show_result(frec_node *pnode,_frec_obj *frecdev,u16 time)
{
	u8 fontsize;					//����ߴ�
	u16 cxoff,cyoff,cwidth,cheight;	//��Ƭ�ߴ�
	u8 cixoff,ciyoff;				//��Щƫ�ƶ�����card�����ƫ��(��lcd��ƫ��,��Ҫ���Ͽ�Ƭ��ƫ��)
	u16 review_width;			 	//Ԥ�����
	u8 review_yoff;
	
	u8 res;
	u8 *pstr=0;
	if(lcddev.width==240)
	{
		fontsize=12;
		cyoff=20+3+140+3,cwidth=230;cheight=150;
		cixoff=5,ciyoff=10; 
		review_width=105,review_yoff=20+3;
	}else if(lcddev.width==272)
	{ 
		fontsize=16;
		cxoff=6,cyoff=24+18+200+18,cwidth=260;cheight=200;
		cixoff=8,ciyoff=30;
		review_width=110,review_yoff=24+18; 		
	}else if(lcddev.width==320)
	{ 
		fontsize=16;
		cyoff=24+18+200+18,cwidth=300;cheight=200;
		cixoff=20,ciyoff=30;
		review_width=150,review_yoff=24+18; 		
	}else if(lcddev.width==480)
	{
		fontsize=24;
		cyoff=32+49+320+49,cwidth=450;cheight=300;
		cixoff=40,ciyoff=50;
		review_width=240,review_yoff=32+49; 		
	}else if(lcddev.width==600)
	{
		fontsize=24;
		cyoff=32+60+400+60,cwidth=450;cheight=300;
		cixoff=40,ciyoff=50;
		review_width=300,review_yoff=32+49; 		
	}
	cxoff=(lcddev.width-cwidth)/2;
	
	pstr=gui_memin_malloc(100);
	if(pstr)
	{
		frec_get_image_data(frecdev->databuf,frecdev->xoff,frecdev->yoff,frecdev->width,review_width);//��ȡԤ��ͼƬ
		
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,APPPLAY_EX_BACKCOLOR);//��䱳��ɫ
		app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);	//�·ֽ���	 
		gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)frec_remind_msg_tbl[5][gui_phy.language]);//��ʾ����-ʶ����  
		
		frec_show_picture((lcddev.width-review_width)/2,review_yoff,review_width,review_width*4/3,frec_dev.databuf);//��ʾԤ��ͼƬ
		appplay_frec_card_ui(cxoff,cyoff,cwidth,cheight);		//��ʾ��Ƭ���
		
		res=appplay_frec_read_a_face(pnode,frecdev,pnode->index);//��ȡ������Ϣ����
		if(res==0)
		{
			
			frec_show_picture(cxoff+cwidth-cixoff-90,cyoff+ciyoff,90,120,frec_dev.databuf);		//��ʾԤ��ͼƬ
			sprintf((char*)pstr,"%s%s",frec_card_info_tbl[gui_phy.language][0],pnode->pname); 
			gui_show_string(pstr,cxoff+cixoff,cyoff+ciyoff,cwidth,fontsize,fontsize,BLACK);		//��ʾ����
			sprintf((char*)pstr,"%s%s",frec_card_info_tbl[gui_phy.language][1],pnode->pnum); 
			gui_show_string(pstr,cxoff+cixoff,cyoff+ciyoff+fontsize*2,cwidth,fontsize,fontsize,BLACK);	//��ʾ�Ա�
			sprintf((char*)pstr,"%s%s",frec_card_info_tbl[gui_phy.language][2],pnode->sex); 
			gui_show_string(pstr,cxoff+cixoff,cyoff+ciyoff+fontsize*4,cwidth,fontsize,fontsize,BLACK);	//��ʾ�绰 
			sprintf((char*)pstr,"%s%dms",frec_card_info_tbl[gui_phy.language][3],time); 
			gui_show_string(pstr,cxoff+cixoff,cyoff+ciyoff+fontsize*6,cwidth,fontsize,fontsize,RED);	//��ʾ��ʱ
			system_task_return=0;
			while(1)
			{
				delay_ms(1000/OS_TICKS_PER_SEC);	//��ʱһ��ʱ�ӽ���
				if(system_task_return)break;		//TPAD����  
			}				
		}
		
	}else res=0XFF;
	gui_memin_free(pstr);
	return res;
}

//���/�༭����
//pnode:Ҫ�洢�������Ľڵ�
//frecdev:����ʶ����ƽṹ�� 
//mode:0,�༭����;1,�������.
//����ֵ:0,����
//    ����,�������
u8 appplay_frec_add_a_face(frec_node *pnode,_frec_obj *frecdev,u8 mode)
{  		
  	_btn_obj* okbtn=0,*cbtn=0;				//��ɺ�ȡ����ť.
	_t9_obj * t9=0;							//���뷨
	_edit_obj* tname=0,*tphone=0,*tsex=0; 
	u16 *pimage=0;
	
	u8 fontsize;					//����ߴ�
	u16 cxoff,cyoff,cwidth,cheight;	//��Ƭ�ߴ�
	u8 cixoff,ciyoff;				//��Щƫ�ƶ�����card�����ƫ��(��lcd��ƫ��,��Ҫ���Ͽ�Ƭ��ƫ��)
	u8 btn_height;					//�����߶�(��ȹ̶�Ϊ�߶ȵ�3��)
	u8 btnpit;						//�������
	u16 t9height;	
	
	u8 res=0,rval=0;
	u8 person;
	u8 editflag=0;					//Ĭ�ϱ༭����
									//0,�༭����
									//1,�༭�绰
									//2,�༭�Ա�

	if(lcddev.width==240)
	{
		fontsize=12;
		cyoff=5+20,cwidth=230;cheight=156;//cheight=3*ciyoff+btn_height+120
		cixoff=5,
		btn_height=20; 
		t9height=134; 
	}else if(lcddev.width==272)
	{ 
		fontsize=12;
		cyoff=10+24,cwidth=260;cheight=200;
		cixoff=12,
		btn_height=25; 
		t9height=176;  
	}else if(lcddev.width==320)
	{ 
		fontsize=16;
		cyoff=40+24,cwidth=300;cheight=200;
		cixoff=20,
		btn_height=25; 
		t9height=176;  
	}else if(lcddev.width==480)
	{
		fontsize=24;
		cyoff=101+32,cwidth=450;cheight=300;
		cixoff=40,
		btn_height=40; 
		t9height=266; 
	}else if(lcddev.width==600)
	{
		fontsize=24;
		cyoff=120+32,cwidth=550;cheight=340;
		cixoff=40,
		btn_height=50; 
		t9height=368; 
	}
	btnpit=(cwidth-2*3*btn_height)*3/7;
	cxoff=(lcddev.width-cwidth)/2;
	ciyoff=(cheight-btn_height-120)/3;
	if(mode==1)frec_get_image_data(frecdev->databuf,frecdev->xoff,frecdev->yoff,frecdev->width,90);//�������,��ȡ90*120��Ԥ��ͼƬ
	else	//�༭����,��ֱ�Ӷ�ȡSD����������Ϣ�ļ�.
	{
		res=appplay_frec_read_a_face(pnode,frecdev,pnode->index);//��ȡ������Ϣ����
	}
	
	gui_fill_rectangle(0,0,lcddev.width,lcddev.height,APPPLAY_EX_BACKCOLOR);//��䱳��ɫ
	app_gui_tcbar(0,0,lcddev.width,gui_phy.tbheight,0x02);	//�·ֽ���	 
	gui_show_strmid(0,0,lcddev.width,gui_phy.tbheight,WHITE,gui_phy.tbfsize,(u8*)frec_remind_msg_tbl[3+mode][gui_phy.language]);//��ʾ����  
	appplay_frec_card_ui(cxoff,cyoff,cwidth,cheight);	//��ʾ��Ƭ���
	
	okbtn=btn_creat(cxoff+btnpit,cyoff+cheight-ciyoff-btn_height,btn_height*3,btn_height,0,0);		//������׼��ť
	cbtn=btn_creat(cxoff+cwidth-btnpit-3*btn_height,cyoff+cheight-ciyoff-btn_height,btn_height*3,btn_height,0,0);	//������׼��ť
	tname=edit_creat(cxoff+cixoff+fontsize*3,cyoff+ciyoff-3,fontsize*7+6,fontsize+6,0,4,fontsize);	//�����༭��
	tphone=edit_creat(cxoff+cixoff+fontsize*3,cyoff+ciyoff+fontsize*2-3,fontsize*7+6,fontsize+6,0,4,fontsize);//�����༭��
	tsex=edit_creat(cxoff+cixoff+fontsize*3,cyoff+ciyoff+fontsize*4-3,fontsize*7+6,fontsize+6,0,4,fontsize);//�����༭��
 
	if(mode==1)//���������,��Ҫ�����ڴ�
	{
		pnode->pname=gui_memex_malloc(30);	//����30�ֽ��ڴ�,���ڴ洢����
		pnode->pnum=gui_memex_malloc(20);	//����20�ֽ��ڴ�,���ڴ洢�绰����
		pnode->sex=gui_memex_malloc(20);	//����20�ֽ��ڴ�,���ڴ洢�Ա���Ϣ
		pimage=gui_memin_malloc(30*40*2);	//����30*40ͼƬ����
	}
	t9=t9_creat((lcddev.width%5)/2,lcddev.height-t9height,lcddev.width-(lcddev.width%5),t9height,0); 	
	if(t9)//����ɹ�
	{ 
		okbtn->caption=frec_sc_btn_tbl[gui_phy.language][0];//���水ť
		okbtn->font=fontsize;
		cbtn->caption=frec_sc_btn_tbl[gui_phy.language][1]; //ȡ����ť
		cbtn->font=fontsize; 	
		tname->textbkcolor=APPPLAY_IN_BACKCOLOR;			//������ɫ
		tname->textcolor=BLUE;
		tphone->textbkcolor=APPPLAY_IN_BACKCOLOR;			//������ɫ
		tphone->textcolor=BLUE;	
		tsex->textbkcolor=APPPLAY_IN_BACKCOLOR;				//������ɫ
		tsex->textcolor=BLUE;		
		tname->type=0X06;			//�����˸  
		if(mode==0)//�༭����
		{
			strcpy((char*)tname->text,(char*)pnode->pname);	//��������
			strcpy((char*)tphone->text,(char*)pnode->pnum);	//�����绰
			strcpy((char*)tsex->text,(char*)pnode->sex);	//�����Ա� 	
		}
		gui_show_string(frec_card_info_tbl[gui_phy.language][0],cxoff+cixoff,cyoff+ciyoff,fontsize*3,fontsize,fontsize,BLACK);
		gui_show_string(frec_card_info_tbl[gui_phy.language][1],cxoff+cixoff,cyoff+ciyoff+fontsize*2,fontsize*3,fontsize,fontsize,BLACK);
		gui_show_string(frec_card_info_tbl[gui_phy.language][2],cxoff+cixoff,cyoff+ciyoff+fontsize*4,fontsize*3,fontsize,fontsize,BLACK);
	
		frec_show_picture(cxoff+cwidth-cixoff-90,cyoff+ciyoff,90,120,frec_dev.databuf);//��ʾԤ��ͼƬ
		
		edit_draw(tname);	//�����༭��
 		edit_draw(tphone);	//�绰�༭��
 		edit_draw(tsex);	//�Ա�༭�� 
		btn_draw(okbtn);
		btn_draw(cbtn);
		t9_draw(t9);	
		rval=0;	
	}else rval=1;
	system_task_return=0;
	if(res)rval=0XFF;		//ǰ�������,���ٽ���ѭ��.
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
		if(system_task_return){rval=0XFF;break;}//TPAD���� 
		if(tname->top<in_obj.y&&in_obj.y<(tname->top+tname->height)&&(tname->left<in_obj.x)&&in_obj.x<(tname->left+tname->width))//��tname���ڲ� 
		{
			editflag=0;					//�༭���� 
			edit_show_cursor(tphone,0);	//�ر�tphone�Ĺ��
			tphone->type=0X04;			//��겻��˸ 
			edit_show_cursor(tsex,0);	//�ر�tsex�Ĺ��
			tsex->type=0X04;			//��겻��˸  
			tname->type=0X06;			//�����˸  
		} 	
		if(tphone->top<in_obj.y&&in_obj.y<(tphone->top+tphone->height)&&(tphone->left<in_obj.x)&&in_obj.x<(tphone->left+tphone->width))//��tphone���ڲ� 
		{
			editflag=1;					//�༭���� 
			edit_show_cursor(tname,0);	//�ر�tname�Ĺ��
			tname->type=0X04;			//��겻��˸ 
			edit_show_cursor(tsex,0);	//�ر�tsex�Ĺ��
			tsex->type=0X04;			//��겻��˸  
			tphone->type=0X06;			//�����˸  
		} 
		if(tsex->top<in_obj.y&&in_obj.y<(tsex->top+tsex->height)&&(tsex->left<in_obj.x)&&in_obj.x<(tsex->left+tsex->width))//��tsex���ڲ� 
		{
			editflag=2;					//�༭���� 
			edit_show_cursor(tphone,0);	//�ر�tphone�Ĺ��
			tphone->type=0X04;			//��겻��˸ 
			edit_show_cursor(tname,0);	//�ر�tname�Ĺ��
			tname->type=0X04;			//��겻��˸  
			tsex->type=0X06;			//�����˸  
		} 
		edit_check(tname,&in_obj);
		edit_check(tphone,&in_obj);
		edit_check(tsex,&in_obj);
		t9_check(t9,&in_obj);		    
		if(t9->outstr[0]!=NULL)//����ַ�
		{
			if(editflag==0)//����
			{
				if(t9->outstr[0]!=0X0D)edit_add_text(tname,t9->outstr);//��֧�ֻس�
			}else if(editflag==1)//�绰
			{
				if((t9->outstr[0]<='9'&&t9->outstr[0]>='0')||t9->outstr[0]==0X08)edit_add_text(tphone,t9->outstr);//����/�˸��
			}else if(editflag==2)//�Ա�
			{
				if(t9->outstr[0]!=0X0D)edit_add_text(tsex,t9->outstr);//��֧�ֻس�				
			} 
			t9->outstr[0]=NULL;	 			//�������ַ� 
		}
		res=btn_check(okbtn,&in_obj);   
		if(res&&((okbtn->sta&(1<<7))==0)&&(okbtn->sta&(1<<6)))	//������,�а����������ɿ�,����TP�ɿ���
		{  
			if(mode==1)frec_get_image_data(pimage,cxoff+cwidth-cixoff-90,cyoff+ciyoff,90,30);//���������,��ȡ30*40��ͼƬ
 			window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)frec_remind_msg_tbl[6][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],16,0,0,0);//��ʾ��������	
			res=0; 
			if(mode==1)//���һ������ 
			{
				res=atk_frec_add_a_face(pimage,&person);			
				pnode->index=person;
			}
			if(res==0)
			{ 
				strcpy((char*)pnode->pname,(char*)tname->text);	//��������
				strcpy((char*)pnode->pnum,(char*)tphone->text);	//�����绰
				strcpy((char*)pnode->sex,(char*)tsex->text);	//�����Ա� 
 				frecdev->face=pnode;
				res=appplay_frec_save_a_faceinfo(frecdev);
				if(res)rval=2;	//���������Ϣʧ�� 
				rval=0;			//����
				break;
			}else rval=3;		//�������ģ��ʧ��
		} 
		res=btn_check(cbtn,&in_obj);   
		if(res&&((cbtn->sta&(1<<7))==0)&&(cbtn->sta&(1<<6)))	//������,�а����������ɿ�,����TP�ɿ���
		{  
			rval=0XFF;//ȡ��
 			break;
		} 
	}	
	btn_delete(okbtn);
	btn_delete(cbtn);
	edit_delete(tname);
	edit_delete(tphone);
	edit_delete(tsex);
	t9_delete(t9);  
	gui_memin_free(pimage);
	if(rval==2||rval==3||rval==0)//��ʾ������
	{
		if(rval==0)res=7;
		else res=8;
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)frec_remind_msg_tbl[res][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],16,0,0,0);//��ʾʧ��
		if(rval==0)atk_frec_load_data_model();//���¼�������ʶ��ģ��
		delay_ms(600);
	}
	return rval;
}  
//��ȡһ������
//pnode:Ҫ�洢�����Ľڵ�
//frecdev:����ʶ����ƽṹ�� 
//index:ʶ��ID
//����ֵ:0,����
//    ����,�������
u8 appplay_frec_read_a_face(frec_node *pnode,_frec_obj *frecdev,u8 index)
{  	
	u8* path;
	FIL *fp; 
	u32 fr;
	u8 res;
	u8 *p;
	path=gui_memin_malloc(30);			//�����ڴ�
	fp=gui_memin_malloc(sizeof(FIL));	//�����ڴ�
	pnode->pname=gui_memex_malloc(30);	//����30�ֽ��ڴ�,���ڴ洢����
	pnode->pnum=gui_memex_malloc(20);	//����20�ֽ��ڴ�,���ڴ洢�绰����
	pnode->sex=gui_memex_malloc(20);	//����20�ֽ��ڴ�,���ڴ洢�Ա�
	if(!fp)
	{
		gui_memin_free(path);
		return ATK_FREC_MEMORY_ERR;
	}
	pnode->index=index;
	sprintf((char*)path,ATK_FREC_INFO_PNAME,pnode->index); 
	res=f_open(fp,(char*)path,FA_READ);
	if(res==FR_OK)
	{
		res=f_read(fp,(char*)frecdev->databuf,90*120*2+70,&fr);	//��ȡ�ļ�	
		if(fr==90*120*2+70)
		{ 
			p=(u8*)frecdev->databuf+90*120*2;
			strcpy((char*)pnode->pname,(char*)p);
			p+=30; 
			strcpy((char*)pnode->pnum,(char*)p);
			p+=20;
			strcpy((char*)pnode->sex,(char*)p); 
			res=0;			
		}else res=ATK_FREC_READ_WRITE_ERR; 
	} 
	f_close(fp);
	if(res)res=ATK_FREC_READ_WRITE_ERR;
	gui_memin_free(path);
	gui_memin_free(fp);
	return res;		 
} 
//��ȡ��������
//head:Ҫ�洢�����Ľڵ�ͷ
//frecdev:����ʶ����ƽṹ�� 
//fnum:���õ�������
//����ֵ:��ȡ��������,0,һ�Ŷ�û�ж���
u8 appplay_frec_read_all_face(frec_node *head,_frec_obj *frecdev,u8 *fnum)
{  	
	frec_node* pnew;
	u8 i;
	u8 res;
	u8 face_num=0;
	pnew=head;
	for(i=0;i<MAX_LEBEL_NUM;i++)
	{
		if(model.validSample[i])	//������ģ����Ч
		{
			res=appplay_frec_read_a_face(pnew,frecdev,i);//��ȡ������Ϣ����
			if(res==0)
			{
				if(pnew!=head)//���Ǳ�ͷ
				{
					res=frec_node_insert(head,pnew,0XFFFF);//��ĩβ׷���½ڵ�
					if(res)break;		//׷��ʧ��
				}  
				face_num++;
				printf("���һ������%02d,���:%02d\r\n",face_num,pnew->index);
				pnew=frec_node_creat();	//�����½ڵ�
				if(pnew==0)break;		//����ʧ�� 
			}else
			{
				model.validSample[i]=0;	//��Ǹ�����ģ�岻����(ȱ��������Ϣ����)
				frec_node_free(pnew,0);	//ɾ���ڵ���Ϣ
			}
		}
	}
	if(pnew!=head)frec_node_free(pnew,1);//ɾ���ڵ���Ϣ(�����ͷ,�Ͳ�ɾ)  
	*fnum=face_num;
	return face_num;
}

//ɾ��1����Ϣ
//head:ָ����Žڵ�ͷָ���ָ��
//index:�ڵ���.
//����ֵ:0,ɾ���ɹ�
//    ����,ɾ��ʧ��
u8 appplay_frec_delete_face(frec_node **head,u16 index)
{
	frec_node*ptemp;
	u8 res;
	ptemp=frec_node_getnode(*head,index);		//��ȡ�ڵ��
	if(ptemp==NULL)return 1;			//�ڴ�����ʧ��/�õ��ڵ���Ϣʧ�� 
	res=atk_frec_delete_data(ptemp->index);	//ɾ������ģ������
	if(ptemp->index<MAX_LEBEL_NUM)model.validSample[ptemp->index]=0;//�������ģ����Ч��
	if(res==0)
	{
		res=appplay_frec_delete_a_faceinfo(ptemp->index);//ɾ��������Ϣ����
	} 
	if(res==0)//ɾ�����ųɹ�
	{ 
		if(index==0)//ɾ���ڵ�ͷ
		{
			if((*head)->next)//������һ���ڵ�
			{
				ptemp=*head;
				*head=(*head)->next;
				frec_node_free(ptemp,1);
			}else
			{
				frec_node_free(*head,0);			//���ͷŽڵ����� 
				(*head)->pname=gui_memex_malloc(30);//����30�ֽ�
				(*head)->pnum=gui_memex_malloc(20);	//����20�ֽ� 
				if((*head)->pnum)
				{
					strcpy((char*)(*head)->pnum,"0");	//����
					strcpy((char*)(*head)->pname,(char*)frec_remind_msg_tbl[0][gui_phy.language]);//����:��ģ����Ϣ��item����  
				} 
			}
		}else
		{
			res=frec_node_delete(*head,index);//ɾ������ڵ�
			if(res)res=3;	//ɾ���ڵ�ʧ��
		}  
	}else res=2;			//ɾ��ʧ��  
	return res;
}

//�������
//sel:��ǰѡ�е���Ŀ
//top:��ǰ�������Ŀ
//caption:ѡ�������
//head:����ͷ 
//����ֵ:[7]:0,���µ��Ƿ��ذ���,���߷����˴���;1,������ѡ���,����˫����ѡ�е���Ŀ.
//       [6]:0,˫���¼�;1,ѡ�ť����.
//     [5:0]:0,��ʾ�޴���;1,��ʾҪ������Ϣ;��������ֵ,�������.
u8 frec_face_browser(u8 *sel,u8 *top,u8 * caption,frec_node* head)
{
	u8  res;
	u8 rval=0;			//����ֵ	  
  	u16 i;	
	u16 itemsize;		//����Ŀ��
	frec_node *pnode;
	_btn_obj* rbtn=0;			//���ذ�ť�ؼ�
 	_btn_obj* okbtn=0;			//ȷ�ϰ�ť�ؼ�
	_listbox_obj * tlistbox;	//listbox  
	itemsize=frec_node_getsize(head);			//���ٵ���һ���ڵ�
	if(itemsize==0)return 2;	//һ���ڵ㶼û��,����!!
	if(*sel>=itemsize)*sel=itemsize-1;//���Ʒ�Χ
	if(*top>=itemsize)*top=0;	//���Ʒ�Χ
	app_filebrower(caption,0X07);				//��ʾ����  
   	tlistbox=listbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.tbfsize);	//����һ��filelistbox
	if(tlistbox==NULL)rval=1;	//�����ڴ�ʧ��.
	else						//�����Ŀ
	{
		for(i=0;i<itemsize;i++)
		{
			pnode=frec_node_getnode(head,i);
			res=listbox_addlist(tlistbox,pnode->pname);
			if(res)
			{
				rval=1;
				break;
			}
		}
	}
	if(rval==0)//�ɹ��������Ŀ
	{
		tlistbox->scbv->topitem=*top;
		tlistbox->selindex=*sel;
		listbox_draw_listbox(tlistbox);
		rbtn=btn_creat(lcddev.width-2*gui_phy.tbfsize-8-1,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//�������ְ�ť
 		okbtn=btn_creat(0,lcddev.height-gui_phy.tbheight,2*gui_phy.tbfsize+8,gui_phy.tbheight-1,0,0x03);//����ȷ�����ְ�ť
		if(rbtn==NULL||okbtn==NULL)rval=1;		//û���㹻�ڴ湻����
		else
		{
		 	rbtn->caption=(u8*)GUI_BACK_CAPTION_TBL[gui_phy.language];	//����
		 	rbtn->font=gui_phy.tbfsize;//�����µ������С	 
			rbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
			rbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
			btn_draw(rbtn);//����ť
		 	okbtn->caption=(u8*)GUI_OPTION_CAPTION_TBL[gui_phy.language];//����
		 	okbtn->font=gui_phy.tbfsize;//�����µ������С	 
			okbtn->bcfdcolor=WHITE;	//����ʱ����ɫ
			okbtn->bcfucolor=WHITE;	//�ɿ�ʱ����ɫ
			btn_draw(okbtn);//����ť
		}	   
 	}  
	system_task_return=0;
   	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
 		if(system_task_return)break;			//TPAD����
		res=btn_check(rbtn,&in_obj);		    //���ذ�ť���
		if(res)if(((rbtn->sta&0X80)==0))break;	//�˳� 
		res=btn_check(okbtn,&in_obj);//ȷ�ϰ�ť���
		if(res)
		{
			if(((okbtn->sta&0X80)==0))//��ť״̬�ı���
			{ 
				*top=tlistbox->scbv->topitem;//��¼�˳�ʱѡ�����Ŀ
				*sel=tlistbox->selindex;
				rval=3<<6;//���ѡ�ť����							 				   			   
  			}	 
		}   
		listbox_check(tlistbox,&in_obj);	//ɨ�� 
		if(tlistbox->dbclick==0X80)//˫����
		{	  
			*top=tlistbox->scbv->topitem;//��¼�˳�ʱѡ�����Ŀ
			*sel=tlistbox->selindex;
			rval=2<<6;//���˫��������								 				   			   
		}
	}	
	listbox_delete(tlistbox);		//ɾ��listbox
	btn_delete(okbtn);				//ɾ����ť	  	 
	btn_delete(rbtn);				//ɾ����ť 
	system_task_return=0;
	return rval; 
}   
////////////////////////////////////////////////////////////////////////////////////////

//����ͼ����Ļ������(������ͷ�������,��λ����Ļ������,�ұ�֤��߱�Ϊ3:4).
//frecdev:����ʶ����ƽṹ��
void frec_set_image_center(_frec_obj *frecdev)
{ 
	frecdev->xoff=0;
	frecdev->yoff=0;
	frecdev->width=lcddev.width;
	frecdev->height=lcddev.height;
	if(lcddev.id==0X1963||lcddev.id==0X5510||lcddev.id==0X7084)
	{   
		frecdev->height=640; 
	}else if(lcddev.id==0X5310)
	{ 
		frecdev->width=300;
		frecdev->height=400;  
	}else if(lcddev.id==0X7016)		//1024*600��RGB��
	{
		frecdev->height=640; 
		frecdev->width=480;  
	}else if(lcddev.id==0X4342)		//480*272��RGB��
	{
		frecdev->height=360; 
		frecdev->width=270;  
	}	
	frecdev->yoff=(lcddev.height-frecdev->height)/2;
	frecdev->xoff=(lcddev.width-frecdev->width)/2;
	frec_curline=frec_dev.yoff;		//��λ��ƫ��
}

//��ȡԭʼͼƬ����
//dbuf:���ݻ�����
//xoff,yoff:Ҫ��ȡ��ͼ��������ʼ����
//xsize:Ҫ��ȡ��ͼ��������
//width:Ҫ��ȡ�Ŀ��(��߱Ⱥ�Ϊ3:4)  
void frec_get_image_data(u16 *dbuf,u16 xoff,u16 yoff,u16 xsize,u16 width)
{
	int w, h; 
	u16 height=width*4/3;
	float scale=(float)xsize/width;
	for(h=0;h<height;h++)
	{
		for(w=0;w<width;w++)
		{
			dbuf[h*width+w]=LCD_ReadPoint(xoff+w*scale,yoff+h*scale); 
 		}
	}
}
//��ʾͼƬ
//x,y,w,h:ͼƬ��ʾ����
//data:ͼƬ���ݻ�����
void frec_show_picture(u16 x, u16 y, u16 w, u16 h, u16 *data)
{
	u16 i, j; 
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{ 
			LCD_Fast_DrawPoint(x + j, y + i, *data);
			data ++;
		}
	}
} 
//FREC RGB�����ݽ��ջص�����
void frec_rgblcd_dcmi_rx_callback(void)
{  
	u16 *pbuf;
	if(DMA2_Stream1->CR&(1<<19))//DMAʹ��buf1,��ȡbuf0
	{ 
		pbuf=(u16*)frec_dcmi_line_buf[0]; 
	}else 						//DMAʹ��buf0,��ȡbuf1
	{
		pbuf=(u16*)frec_dcmi_line_buf[1]; 
	} 	
	LTDC_Color_Fill(frec_dev.xoff,frec_curline,frec_dev.xoff+frec_dev.width-1,frec_curline,pbuf);//DM2D��� 
	if(frec_curline<(frec_dev.height+frec_dev.yoff))frec_curline++;
}

vu8 frec_running=0;		//����ʶ��������
u8 ex_sw=0;

//����ʶ��
u8 frec_play(void)
{ 
	u8 res,fac;							  
	u8 key;					//��ֵ		 
 	u8 *msgbuf;				//��Ϣ������ 
	u8 person;
	u8 rval=0;  
	
	frec_node *head=0,*ptemp=0; 
	
	u8 fnum=0; 
	u8 fsel;
	u8 selitem=0;
	u8 topitem=0; 
	u16 *pbkcolor;
	 
	if(audiodev.status&(1<<7))		//��ǰ�ڷŸ�??����ֹͣ
	{
		audio_stop_req(&audiodev);	//ֹͣ��Ƶ����
		audio_task_delete();		//ɾ�����ֲ�������.
	}
	//��ʾ��ʼ���OV5640
 	window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[0][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
	if(OV5640_Init())//��ʼ��OV5640
	{
		window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)camera_remind_tbl[1][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(500);  
		rval=1;
	} 
	frec_set_image_center(&frec_dev);	//���õ���Ļ������
	msgbuf=gui_memin_malloc(40);		//����40���ֽ��ڴ�,���ڴ���ļ���
	frec_dev.databuf=gui_memex_malloc(frec_dev.width*frec_dev.height/2);//�����ڴ�	
	frec_dcmi_line_buf[0]=gui_memin_malloc(frec_dev.width*2);//Ϊdma���������ڴ�	
	frec_dcmi_line_buf[1]=gui_memin_malloc(frec_dev.width*2);//Ϊdma���������ڴ�	
 	if(!frec_dcmi_line_buf[1]||!msgbuf||!frec_dev.databuf)rval=1;//����ʧ��
	if(rval==0)
	{ 
		frec_running=1; 
		sw_sdcard_mode();								//SD��ģʽ  
		rval=atk_frec_initialization();					//��ʼ������ʶ��
 		head=frec_node_creat();							//������ͷ
		fnum=appplay_frec_read_all_face(head,&frec_dev,&fnum);//��ȡ��������
		if(fnum==0)//һ��������û��
		{  
			head->pname=gui_memex_malloc(30);	//����30�ֽ�
			head->pnum=gui_memex_malloc(20);	//����20�ֽ� 
			if(head->pnum)
			{
				strcpy((char*)head->pnum,"0");	//����
 				strcpy((char*)head->pname,(char*)frec_remind_msg_tbl[0][gui_phy.language]);//����:��ģ����Ϣ��item����  
  			}
		} 
		if(head->pnum)
		{
			sw_ov5640_mode();			//5640ģʽ 
			OV5640_RGB565_Mode();		//RGB565ģʽ 
			OV5640_Focus_Init();  
			OV5640_Focus_Constant();	//���������Խ� 
			OV5640_Light_Mode(0);		//�Զ�ģʽ
			OV5640_Brightness(4);		//��������
			OV5640_Color_Saturation(3);	//ɫ������
			OV5640_Contrast(3);			//�Աȶ����� 
			OV5640_Exposure(3);			//�ع�ȼ�
			OV5640_Sharpness(33);		//�Զ����
			DCMI_Init();				//DCMI���� 
			if(lcdltdc.pwidth!=0)		//RGB��
			{
				dcmi_rx_callback=frec_rgblcd_dcmi_rx_callback;//RGB���������ݻص�����
				DCMI_DMA_Init((u32)frec_dcmi_line_buf[0],(u32)frec_dcmi_line_buf[1],frec_dev.width/2,1,1);//DCMI DMA����  
			}else						//MCU ��
			{
				DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);	//DCMI DMA����,MCU��,����
			} 
			LCD_Clear(BLACK);   
			fac=800/frec_dev.height;	//�õ���������
			OV5640_OutSize_Set((1280-fac*frec_dev.width)/2,(800-fac*frec_dev.height)/2,frec_dev.width,frec_dev.height); 	 
			LCD_Set_Window(frec_dev.xoff,frec_dev.yoff,frec_dev.width,frec_dev.height);//���ÿ�����.  
 			DCMI_Start();				//�������� 
			TIM6_Int_Init(100-1,9600-1);//10Khz����Ƶ��,��100��10ms,����ͳ������ʶ��ʱ��
 			system_task_return=0; 
 			while(rval==0)
			{	 
				if(system_task_return)break;	//TPAD����	  
				key=KEY_Scan(0);//��֧������
				tp_dev.scan(0);
				if(tp_dev.sta&TP_PRES_DOWN)		//������Ļ,ִ�жԽ�
				{
					OV5640_Focus_Single();//ִ���Զ��Խ�
					tp_dev.scan(0);
					tp_dev.sta=0;
				}
				if(key==WKUP_PRES||key==KEY0_PRES||key==KEY2_PRES)
				{
					DCMI_Stop();		//ֹͣ���� 
					LCD_Set_Window(0,0,lcddev.width,lcddev.height);//���ÿ�����.  
					sw_sdcard_mode();	//SD��ģʽ  
					switch(key)
					{
						case KEY2_PRES: //����ģ���б�,����ɾ��/�༭������Ϣ
							while(1)
							{
								res=frec_face_browser(&selitem,&topitem,frec_remind_msg_tbl[2][gui_phy.language],head);
								if(system_task_return)break;//TPAD����
								if(res&0X80)
								{
									fsel=0;
									if(res&1<<6)
									{ 
										if(fnum==0)res=1;
										else res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)frec_option_tbl[gui_phy.language],2,(u8*)&fsel,0X90,(u8*)frec_remind_msg_tbl[1][gui_phy.language]);//2��ѡ��
										if(res==0)//���óɹ�
										{
											switch(fsel)
											{
												case 0://�༭���� 
													fsel=0XFF;//���Ҫ�༭����
													break; 
												case 1://ɾ������
													res=appplay_frec_delete_face(&head,selitem);//ɾ����������
													if(res==0)
													{
														fnum--;//��������һ
													}else res=1; 
													window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)frec_remind_msg_tbl[9+res][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],16,0,0,0);//ɾ���ɹ�/ʧ��
													delay_ms(600);
													break;
											}  
										}  	
									}else fsel=0XFF;	//˫��,���Ҫ�༭����
									if(fsel==0XFF&&fnum)	//��������ʱ��,�༭����
									{
										ptemp=frec_node_getnode(head,selitem);			//�õ�ѡ�нڵ����Ϣ.
										if(ptemp)
										{
											frec_node_free(ptemp,0);					//�ͷŽڵ�����   
											appplay_frec_add_a_face(ptemp,&frec_dev,0);	//�༭�������� 
										} 
									}
								}else break;//���ؼ�/����
							}
 							break;
						case KEY0_PRES:		//ʶ������
							pbkcolor=gui_memex_malloc(200*80*2);//���뱳��ɫ����
							if(pbkcolor==0)continue;			//�ڴ�����ʧ��
							frec_get_image_data(frec_dev.databuf,frec_dev.xoff,frec_dev.yoff,frec_dev.width,30); 
							app_read_bkcolor((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,pbkcolor);//��ȡ����ɫ
							window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)frec_remind_msg_tbl[11][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],16,0,0,0);//��ʾʧ��
  							reg_time=0; 
 							res=atk_frec_recognition_face(frec_dev.databuf,&person);
							if(res==ATK_FREC_MODEL_DATA_ERR)
							{	
								window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)frec_remind_msg_tbl[12][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],16,0,0,0);//��ʾʧ��
								delay_ms(600);
 							}else if(res==ATK_FREC_UNREC_FACE_ERR)
							{	
								window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)frec_remind_msg_tbl[13][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],16,0,0,0);//��ʾʧ��
								delay_ms(600);
							}else 
							{  
								ptemp=frec_node_getnode_index(head,person);//�õ��ڵ���Ϣ
								if(ptemp)
								{
									frec_node_free(ptemp,0);								//�ͷŽڵ�����   
									app_recover_bkcolor((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,pbkcolor);//�ָ�����ɫ
									appplay_frec_show_result(ptemp,&frec_dev,reg_time*10);	//��ʾʶ����
								}    
							} 
							gui_memex_free(pbkcolor);
							break;
						case WKUP_PRES://KEY UP����,���һ������������ݿ� 
							if(fnum==0)
							{
								frec_node_free(head,0);							//���ͷ�֮ǰ�ı�ͷ������ 
								res=appplay_frec_add_a_face(head,&frec_dev,1);	//���һ������
							}else
							{
								ptemp=frec_node_creat();//�����½ڵ�
								res=appplay_frec_add_a_face(ptemp,&frec_dev,1);	//���һ������
								if(res==0)//��ȡ�ɹ�
								{
									res=frec_node_insert(head,ptemp,0XFFFF);	//��ĩβ׷���½ڵ� 
								}
							}							 
							if(res==0)//��ӳɹ�
							{
								fnum++; 
							}else 
							{
								frec_node_free(ptemp,1);//���ʧ��,�ͷŽڵ��ڴ�(���ﲻ�ͷű�ͷ!)
								if(fnum==0)	//ֻ�б�ͷ�����,�Ҷ�ģ��ʧ��,�������Ĭ����Ϣ
								{		  
									strcpy((char*)head->pnum,"0");	//����
									strcpy((char*)head->pname,(char*)frec_remind_msg_tbl[0][gui_phy.language]);//����:��ģ����Ϣ��item����  
 								}								 
							} 
							break;   
						default :
							break;
					} 
					LCD_Clear(BLACK); 
					sw_ov5640_mode();	//5640ģʽ
					LCD_Set_Window(frec_dev.xoff,frec_dev.yoff,frec_dev.width,frec_dev.height);//���ÿ�����.  
					DCMI_Start(); 				//��������  
					system_task_return=0;
				} 
				if(hsync_int)//�ող���֡�ж�,������ʱ
				{ 
					delay_ms(1);
					hsync_int=0;
				}  
			}
			DCMI_Stop();
			sw_sdcard_mode();	//�л�ΪSD��ģʽ 
			atk_frec_destroy();	//��������ʶ��
			TIM6->CR1&=~(1<<0); //�رն�ʱ��6 
			LCD_Set_Window(0,0,lcddev.width,lcddev.height);	//�ָ�Ĭ�ϴ���
			frec_running=0; 
		}
	}
	if(head)frec_node_destroy(head);//ɾ����������
 	gui_memin_free(frec_dcmi_line_buf[0]);			
 	gui_memin_free(frec_dcmi_line_buf[1]);			
 	gui_memin_free(msgbuf);				
	gui_memex_free(frec_dev.databuf); 
	//�ָ�ƫ����
	frec_dev.xoff=0;
	frec_dev.yoff=0;
	return 0;
}





















