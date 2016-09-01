#include "smsplay.h"
#include "usart.h"	 
#include "spb.h"	  
#include "calendar.h"
#include "t9input.h"
//#include "sai.h"
//#include "camera.h" 
//#include "audioplay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//APP-���Ų��� ����	   
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

//��ʾ��Ϣ
u8*const sms_remind_msg_tbl[9][GUI_LANGUAGE_NUM]=
{
"��ܰ��ʾ:û�ж���Ϣ","��ܰ��ʾ:�]�ж���Ϣ","Remind:No SMS",
"ѡ�����:","�]�ж���Ϣ","Choose option:", 
"���Ŷ�ȡ��...","�����xȡ��...","SMS Reading...", 
"ɾ���ɹ�!","�h���ɹ�!","Delete OK!",
"ɾ��ʧ��!","�h��ʧ��!","Delete Failed", 
"����:","�l��:","Send:",  
"�յ�:","�յ�:","Receive:",   
"���������ռ��˺���!","Ո��ݔ���ռ���̖�a!","Please input Addressee NUM first!", 
"����ʧ��!","�l��ʧ��!","Send failed!", 
};
u8*const sms_option_tbl[GUI_LANGUAGE_NUM][3]=
{
{"�½���Ϣ","�Ķ���Ϣ","ɾ����Ϣ"},
{"�½���Ϣ","��x��Ϣ","�h����Ϣ"},		 
{"New SMS","Read SMS","Delete SMS"},
}; 
u8*const sms_send_tbl[GUI_LANGUAGE_NUM]={"����","�l��","SEND"};
u8*const sms_contact_tbl[2][GUI_LANGUAGE_NUM]=
{
"��ϵ��:","�M��:","Contacts:",
"�ռ���:","�ռ���:","Addressee:",
};
 
////////////////////////////////////////////////////////////////////////////////////////
//������������

//����SMS�ڵ�
//����ֵ:�ڵ��׵�ַ
sms_node* sms_node_creat(void)
{
	sms_node *pnode;
	pnode=gui_memex_malloc(sizeof(sms_node));
	if(pnode)
	{
		pnode->pnum=0;
		pnode->msg=0;
		pnode->time=0;
		pnode->item=0;
		pnode->index=0;
		pnode->readed=0;
		pnode->next=0; 
	}
	return pnode; 
}
//�ͷ�һ���ڵ���ڴ�
//pnode:�ڵ��ַ
//mode:0,�����ͷŽڵ�����(ֻɾ������,�ڵ�ͷ��ַ����.)
//     1,�����ڵ�ͷҲ�ͷŵ�(�����ڵ�ɾ��)
void sms_node_free(sms_node * pnode,u8 mode)
{
	if(pnode==NULL)return;	//�սڵ�
	if(pnode->pnum)gui_memex_free(pnode->pnum);	//�ͷ�pnum�ڴ�
	if(pnode->msg)gui_memex_free(pnode->msg);	//�ͷ�msg�ڴ�
	if(pnode->time)gui_memex_free(pnode->time);	//�ͷ�time�ڴ� 
	if(pnode->item)gui_memex_free(pnode->item);	//�ͷ�item�ڴ� 
	if(mode==1)gui_memex_free(pnode);			//�ͷ�pnode�ڴ� 
}
//�ڵ�i���ڵ�����һ���ڵ�
//head:��ͷ
//pnew:�½ڵ�
//i:����ڵڼ����ڵ�֮��,0XFFFF,��ĩβ���
//����ֵ:0,��ӳɹ�
//    ����,���ʧ��.
u8 sms_node_insert(sms_node* head,sms_node *pnew,u16 i)
{
	u16 j;
	sms_node *p,*p1;
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
u8 sms_node_delete(sms_node* head,u16 i)
{
	u16 j;
	sms_node *p,*p1=0;
	if(i==0)return 1;
	p=head;
	for(j=0;j<i&&p->next!=NULL;j++)
	{
		p1=p;
		p=p->next;
	}
	if(p==NULL||p1==NULL)return 2;//�����ڵĽڵ� 
	p1->next=p->next;	//����ǰһ���ڵ��ָ��
	sms_node_free(p,1);	//�ͷŽڵ��ڴ� 
	return 0;//ɾ���ɹ�	
}	
//�õ�һ���ڵ���Ϣ
//head:��ͷ
//i:Ҫ�õ���Ϣ�Ľڵ��,0,��ʾ��ͷ
//����ֵ:0,�õ���Ϣʧ��
//    ����,�ڵ��ַ
sms_node* sms_node_getnode(sms_node* head,u16 i)
{
	u16 j;
	sms_node *p; 
	p=head;
	for(j=0;j<i&&p!=NULL;j++)
	{
		p=p->next;
	}
	return p;//���ؽڵ��ַ
}	
//�õ����������С
//head:��ͷ 
//����ֵ:0,�����С(�ڵ����)
u16 sms_node_getsize(sms_node* head)
{
	u16 j;
	sms_node *p; 
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
void sms_node_destroy(sms_node* head)
{ 
	sms_node *p,*p1; 
	p=head;
	while(p->next!=NULL)
	{
		p1=p->next;
		p->next=p1->next;
		sms_node_free(p1,1);//ɾ��һ���ڵ�
	} 
	sms_node_free(p,1);//ɾ�����һ���ڵ�
}	
////////////////////////////////////////////////////////////////////////////////////////
//��ʾ��Ϣ����/����״̬
//mode:0,�յ�����Ϣ
//     1,���Ͷ��ųɹ�
//��gsm.c�������
void sms_remind_msg(u8 mode)
{
	u8 i;
	if(mode==0)//�յ�����Ϣ
	{
		for(i=0;i<2;i++)//2����ʾ
		{
//			PCF8574_WriteBit(BEEP_IO,0);
			delay_ms(30);
//			PCF8574_WriteBit(BEEP_IO,1);
			delay_ms(70); 
		}
	}else //���Ͷ��ųɹ�
	{
//		PCF8574_WriteBit(BEEP_IO,0);
		delay_ms(60);
//		PCF8574_WriteBit(BEEP_IO,1);
		delay_ms(140);
	}
	delay_ms(1300);
} 

//��ȡһ������
//pnode:Ҫ�洢�������ŵĽڵ�
//index:������SIM�����λ��(1~xxxx)
//mode:1,���ı���ŵ�״̬;0,�ı����״̬.
//����ֵ:0,��ȡ�ɹ�
//       1,��λ��Ϊ��(δ�洢����)
//    ����,��ȡʧ��
u8 sms_read_sms(sms_node *pnode,u16 index,u8 mode)
{
	u8 *p,*p1,*p2; 
	u8 res,t=0; 
	p=gui_memin_malloc(200);//����200���ֽڵ��ڴ�
	if(p==0)return 2;//�ڴ�����ʧ��
	for(t=0;t<3;t++)//����3��
	{
		sprintf((char*)p,"AT+CMGR=%d,%d",index,mode);//���Ͷ�����ָ��(����mode����,�����ǲ��Ǹı����״̬)
 		res=gsm_send_cmd(p,"+CMGR:",100);  
		if(res==0)//��ȡ���ųɹ�
		{
			pnode->index=index;	//��¼����������SIM����λ��
			if(strstr((const char*)(USART3_RX_BUF),"OK")==0)//û���յ�OKӦ��,��ȴ�100ms,��OK���͹���
			{ 
				continue;//������һ��
			}
			if(strstr((const char*)(USART3_RX_BUF),"UNREAD")==0)pnode->readed=1;
			else pnode->readed=0;
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");
			if(!p1)continue;							//�����ˣ���������
			p2=(u8*)strstr((const char*)(p1+2),"\"");
			if(!p2)continue;							//�����ˣ���������
			p2[0]=0;//���������
			gsm_unigbk_exchange(p1+2,p,0);			//��unicode�ַ�ת��Ϊgbk�� 
			pnode->pnum=gui_memex_malloc(20);			//����20�ֽ��ڴ�,���ڴ洢�绰����
			if(pnode->pnum)strcpy((char*)pnode->pnum,(char*)p);//��������
			p1=(u8*)strstr((const char*)(p2+1),"/");
			if(!p1)										//��������,��ֹ
			{
				gui_memex_free(pnode->pnum);			//�ͷ��ڴ�
				continue;
			}
			p2=(u8*)strstr((const char*)(p1),"+");
			if(!p2)										//��������,��ֹ
			{
				gui_memex_free(pnode->pnum);			//�ͷ��ڴ�
				continue;
			}
			p2[0]=0;//���������
			pnode->time=gui_memex_malloc(20);			//����20�ֽ��ڴ�,���ڴ洢���Ž���ʱ��
			if(pnode->time)sprintf((char*)pnode->time,"20%s",(char*)(p1-2));//��������ʱ��,���20����  
			p1=(u8*)strstr((const char*)(p2+1),"\r");	//Ѱ�һس���
			if(!p1)
			{
				gui_memex_free(pnode->pnum);			//�ͷ��ڴ�
				gui_memex_free(pnode->time);			//�ͷ��ڴ�
				continue;
			}
			gsm_unigbk_exchange(p1+2,p,0);			//��unicode�ַ�ת��Ϊgbk�� 
			pnode->msg=gui_memex_malloc(strlen((char*)p));//�����ڴ�,���ڴ洢��������
			if(pnode->msg)strcpy((char*)pnode->msg,(char*)p);//������������ 
			pnode->item=gui_memex_malloc(strlen((char*)pnode->pnum)+strlen((char*)pnode->msg)+strlen((char*)pnode->time));
			if(pnode->item)
			{ 
				strncpy((char*)p,(char*)(pnode->time+5),5);//�����·�/���ڵ�����p
				p[5]=0;//���������
				sprintf((char*)pnode->item,"%s%s(%s):%s",pnode->readed?"��":"��",pnode->pnum,p,pnode->msg);
			}
			break;
		}else if(res==1)
		{
			//printf("check sta:%d\r\n",res);
			if(gsm_check_cmd("OK"))break;//���յ�OK,����û����,˵������ط������ڶ���
		}
	}
	gsm_cmd_over();	//�������
	gui_memin_free(p);	//�ͷ��ڴ� 
	return res;
} 
//ɾ��1����Ϣ
//head:ָ����Žڵ�ͷָ���ָ��
//index:�ڵ���.
//����ֵ:0,ɾ���ɹ�
//    ����,ɾ��ʧ��
u8 sms_delete_sms(sms_node **head,u16 index)
{
	sms_node*ptemp;
	u8 *p;	
	u8 res,t=0; 
	p=gui_memin_malloc(200);//����200���ֽڵ��ڴ�
	ptemp=sms_node_getnode(*head,index);//��ȡ�ڵ��
	if(p==0||ptemp==NULL)return 1;//�ڴ�����ʧ��/�õ��ڵ���Ϣʧ�� 
	res=window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,"",(u8*)APP_DELETE_CAPTION_TBL[gui_phy.language],12,0,0X03,0);
	if(res==1)//ȷ��ɾ��
	{
		for(t=0;t<3;t++)//����3��
		{	
			sprintf((char*)p,"AT+CMGD=%d",ptemp->index);//����ɾ������ָ��
			res=gsm_send_cmd(p,"OK",100);//�ȴ�ɾ���ɹ�
			if(res==0)//ɾ�����ųɹ�
			{ 
				if(index==0)//ɾ���ڵ�ͷ
				{
					if((*head)->next)//������һ���ڵ�
					{
						ptemp=*head;
						*head=(*head)->next;
						sms_node_free(ptemp,1);
					}else
					{
						sms_node_free(*head,0);	//���ͷŽڵ����� 
						(*head)->pnum=gui_memex_malloc(30);	//����30�ֽ�
						(*head)->msg=gui_memex_malloc(30);	//����30�ֽ�
						(*head)->time=gui_memex_malloc(30);	//����30�ֽ�
						(*head)->item=gui_memex_malloc(30);	//����30�ֽ�
						if((*head)->item)
						{
							strcpy((char*)(*head)->pnum,"ALIENTEK");//����
							strcpy((char*)(*head)->msg,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//����:�޶���Ϣ��item����
							calendar_get_time(&calendar);
							sprintf((char*)(*head)->pnum,"%d/%d/%d,%d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//����ʱ��			
							strcpy((char*)(*head)->item,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//����:�޶���Ϣ��item����
						} 
					}
				}else
				{
					res=sms_node_delete(*head,index);//ɾ������ڵ�
					if(res)res=3;//ɾ���ڵ�ʧ��
				}
				break;   
			}else res=2;//ɾ������ʧ��
		} 
		if(res==0)window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sms_remind_msg_tbl[3][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		else window_msg_box((lcddev.width-200)/2,(lcddev.height-100)/2,200,100,(u8*)sms_remind_msg_tbl[4][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,0,0);
		delay_ms(800);
	}
	gsm_send_cmd("AT+CPMS?","+CPMS:",50);
	gsm_cmd_over();//�������
	gui_memin_free(p);//�ͷ��ڴ�
	return res;
}
//��ȡ��������
//����ֵ:0,��ȡ�ɹ�
//    ����,ʧ��
u8 sms_get_smsnum(u16 *num,u16 *max)
{
	u8 *p1,*p2;
	u8 res,i;
//	if(audiodev.status&(1<<4))
//	{ 	 
//		SAI_Play_Stop();//ֹͣ��Ƶ����
//	} 
	for(i=0;i<3;i++)//����3��
	{
		res=gsm_send_cmd("AT+CPMS?","+CPMS:",50);
		if(res==0)	//��ѯ��ѡ��Ϣ�洢��
		{ 
			p1=(u8*)strstr((const char*)(USART3_RX_BUF),",");   
			p2=(u8*)strstr((const char*)(p1+1),",");
			if((p2-p1)==2)*num=p1[1]-'0';//1λ
			else if((p2-p1)==3)*num=(p1[1]-'0')*10+p1[2]-'0';//2λ
			else if((p2-p1)==4)*num=(p1[1]-'0')*100+(p1[2]-'0')*10+p1[2]-'0';//3λ 
			if(p2[3]==',')//С��64K SIM�������洢��ʮ������
			{
				*max=(p2[1]-'0')*10+p2[2]-'0';//��ȡ���洢��������
				p2[3]=0;
			}else //�����64K SIM�������ܴ洢100�����ϵ���Ϣ
			{
				*max=(p2[1]-'0')*100+(p2[2]-'0')*10+p2[3]-'0';//��ȡ���洢��������
				p2[4]=0;
			}
			break;
		}
	}
//	if(audiodev.status&(1<<4))
//	{
//		SAI_Play_Start();//������Ƶ����  
//	} 
	gsm_cmd_over();//�������
	return res;
}

//�½�ѡ���
//sel:��ǰѡ�е���Ŀ
//top:��ǰ�������Ŀ
//caption:ѡ�������
//head:��������ͷ 
//����ֵ:[7]:0,���µ��Ƿ��ذ���,���߷����˴���;1,������ѡ���,����˫����ѡ�е���Ŀ.
//       [6]:0,˫���¼�;1,ѡ�ť����.
//     [5:0]:0,��ʾ�޴���;1,��ʾҪ������Ϣ;��������ֵ,�������.
u8 sms_msg_select(u16 *sel,u16 *top,u8 * caption,sms_node* head)
{
	u8  res;
	u8 rval=0;			//����ֵ	  
  	u16 i;	
	u16 itemsize;		//����Ŀ��
	sms_node *pnode;
	_btn_obj* rbtn=0;			//���ذ�ť�ؼ�
 	_btn_obj* okbtn=0;			//ȷ�ϰ�ť�ؼ�
	_listbox_obj * tlistbox;	//listbox  
	itemsize=sms_node_getsize(head);			//���ٵ���һ���ڵ�
	if(itemsize==0)return 2;//һ���ڵ㶼û��,����!!
	if(*sel>=itemsize)*sel=itemsize-1;//���Ʒ�Χ
	if(*top>=itemsize)*top=0;//���Ʒ�Χ
	app_filebrower(caption,0X07);				//��ʾ����  
   	tlistbox=listbox_creat(0,gui_phy.tbheight,lcddev.width,lcddev.height-gui_phy.tbheight*2,1,gui_phy.tbfsize);	//����һ��filelistbox
	if(tlistbox==NULL)rval=1;					//�����ڴ�ʧ��.
	else	//�����Ŀ
	{
		for(i=0;i<itemsize;i++)
		{
			pnode=sms_node_getnode(head,i);
			res=listbox_addlist(tlistbox,pnode->item);
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
		if(gsmdev.newmsg)	//������Ϣ,Ҫ�������
		{
			rval=0X01;
			break;	
		}
	}	
	listbox_delete(tlistbox);		//ɾ��listbox
	btn_delete(okbtn);				//ɾ����ť	  	 
	btn_delete(rbtn);				//ɾ����ť 
	return rval; 
}   
//��ȡ���ж���Ϣ
//head:���Žڵ�ͷ
//maxnum:����������
//����ֵ:��ȡ�Ķ���Ϣ����,0,һ����û�ж���
u16 sms_read_all_sms(sms_node *head,u16 *maxnum)
{	
	_window_obj* twin=0;			//����
	_progressbar_obj * curprog=0;	//��ǰ����
	sms_node* pnew;
	u8 res;
	u16 i=0,j=0;
	u16 msgnum=0,msgmax=0;  
	u16 sx,sy;
	sx=(lcddev.width-200)/2;
	sy=(lcddev.height-100)/2; 
	twin=window_creat(sx,sy,200,32+20+20+28,0,1,16);//��������
	curprog=progressbar_creat(sx+10,sy+32+20,180,20,0X61);		//����������
	gsmdev.newmsg=0;//����Ϣ��������  
	sms_get_smsnum(&msgnum,&msgmax);
	*maxnum=msgmax;//��¼����������
	if(msgnum&&twin&&curprog)
	{
		twin->caption=sms_remind_msg_tbl[2][gui_phy.language];
		twin->windowbkc=APP_WIN_BACK_COLOR;	
 		window_draw(twin);						//��������
		progressbar_draw_progressbar(curprog);	//��ʾ������
		curprog->totallen=msgnum; 				//�����msgnum
		pnew=head;
		for(i=1;i<msgmax+1;i++)
		{
			res=sms_read_sms(pnew,i,1);//��ȡһ������
			if(res==0)//�ɹ���ȡ��һ��
			{
				if(pnew!=head)//���Ǳ�ͷ
				{
					res=sms_node_insert(head,pnew,0XFFFF);//��ĩβ׷���½ڵ�
					if(res)break;//׷��ʧ��
				} 
				j++;
				curprog->curpos=j;
				progressbar_draw_progressbar(curprog);//��ʾ������
				if(j==msgnum)break;
				pnew=sms_node_creat();//�����½ڵ�
				if(pnew==0)break;//����ʧ�� 
			}else if(res==2)//��ȡʧ��
			{
				if(pnew!=head)sms_node_free(pnew,1);//ɾ����󴴽��Ľڵ�
				break;
			} 
		}
		msgnum=j;//��¼�ܹ���ȡ�˶���������
		delay_ms(200);
	}  
	window_delete(twin);//ɾ������
	progressbar_delete(curprog);//ɾ��������	
	system_task_return=0;		//����˳���־
	return msgnum;
} 

 
//����һ������
//pnode:��ǰ���Žڵ�
//mode:ģʽ
//     0,�Ķ�/�ظ���ǰ����
//     1,�½�һ������
void sms_send_sms(sms_node* pnode,u8 mode)
{
	_edit_obj* tedit; 
  	_btn_obj* sbtn=0;				//���Ͷ��Ű�ť
	_t9_obj * t9=0;					//���뷨  
	_memo_obj * rmemo=0,* smemo=0;	//memo�ؼ�
	sms_node *pnew;
	
	u8 pnum_height,pnum_fsize;
	u8 rmsg_height,rmsg_fsize;
	u16 rmemo_height,smemo_width;
	u8 smemo_height,smemo_offx,sarea_height;
	u8 sbtn_width,sbtn_height,sbtn_fize;
	u8 smsg_height,smsg_fsize; 
	u16 t9height; 
	u16 res; 
	u8 rval=0;
	u8 ccnt=70;		//�ַ�������,һ������,���70���ַ�
	u8 editflag=0;	//0,�༭����smemo
					//1,�༭�����ռ���(tedit)
	u8 *str=sms_contact_tbl[mode][gui_phy.language];
	u8 *p;
	u8 curmsg=0; 
	
	LCD_Clear(SMS_RMEMO_BACK_COLOR);//���� 
	if(lcddev.width==240)
	{
		pnum_height=24;pnum_fsize=16;
		rmsg_height=16;rmsg_fsize=12;
		rmemo_height=80;
		smemo_height=48;smemo_width=176;smemo_offx=4;sarea_height=66;
		sbtn_width=40;sbtn_height=30;sbtn_fize=16;
		smsg_height=16;smsg_fsize=12;
		t9height=134;
	}else if(lcddev.width==272)
	{
		pnum_height=24;pnum_fsize=16;
		rmsg_height=20;rmsg_fsize=12;
		rmemo_height=160;
		smemo_height=80;smemo_width=196;smemo_offx=4;sarea_height=100;
		sbtn_width=60;sbtn_height=40;sbtn_fize=24;
		smsg_height=20;smsg_fsize=16;	
		t9height=176;
	}else if(lcddev.width==320)
	{
		pnum_height=44;pnum_fsize=24;
		rmsg_height=26;rmsg_fsize=16;
		rmemo_height=136;
		smemo_height=80;smemo_width=208;smemo_offx=12;sarea_height=100;
		sbtn_width=60;sbtn_height=40;sbtn_fize=24;
		smsg_height=20;smsg_fsize=16;	
		t9height=176;
	}else if(lcddev.width==480)
	{
		pnum_height=54;pnum_fsize=24;
		rmsg_height=36;rmsg_fsize=16;
		rmemo_height=304;
		smemo_height=112;smemo_width=304;smemo_offx=26;sarea_height=140;
		sbtn_width=90;sbtn_height=60;sbtn_fize=24;
		smsg_height=20;smsg_fsize=16;	
		t9height=266;
	}else if(lcddev.width==600)
	{
		pnum_height=60;pnum_fsize=32;
		rmsg_height=36;rmsg_fsize=16;
		rmemo_height=400;
		smemo_height=144;smemo_width=360;smemo_offx=26;sarea_height=160;
		sbtn_width=120;sbtn_height=80;sbtn_fize=32;
		smsg_height=36;smsg_fsize=24;	
		t9height=368;
	} 
	gui_fill_rectangle(0,0,lcddev.width,pnum_height,SMS_PNUM_BACK_COLOR);//�����ϵ�����򱳾�
	gui_fill_rectangle(0,pnum_height,lcddev.width,rmsg_height,SMS_RMSG_BACK_COLOR);//����ʱ�����򱳾�ɫ
	gui_draw_hline(0,pnum_height+rmsg_height-1,lcddev.width,SMS_COM_RIM_COLOR);//�ָ���
	gui_fill_rectangle(0,pnum_height+rmsg_height+rmemo_height,lcddev.width,sarea_height,SMS_SAREA_BACK_COLOR);//�������򱳾�ɫ
	gui_draw_hline(0,pnum_height+rmsg_height+rmemo_height,lcddev.width,SMS_COM_RIM_COLOR);//�ָ���
	gui_show_string(str,10,(pnum_height-pnum_fsize)/2,lcddev.width,pnum_fsize,pnum_fsize,SMS_PNUM_NAME_COLOR);//��ʾ��ϵ��/�ռ���
	rmemo=memo_creat(0,pnum_height+rmsg_height,lcddev.width,rmemo_height,0,0,16,SMS_RMEMO_MAXLEN);//����memo�ؼ�,���SMS_RMEMO_MAXLEN���ַ�	
 	tedit=edit_creat(strlen((char*)str)*pnum_fsize/2+10,(pnum_height-pnum_fsize-6)/2,lcddev.width-strlen((char*)str)*pnum_fsize/2-10,pnum_fsize+6,0,4,pnum_fsize);//�����༭��
	smemo=memo_creat(smemo_offx,pnum_height+rmsg_height+rmemo_height+(sarea_height-smemo_height)/2,smemo_width,smemo_height,0,1,16,140);//���140���ַ�	
	t9=t9_creat((lcddev.width%5)/2,lcddev.height-t9height,lcddev.width-(lcddev.width%5),t9height,0); 
	sbtn=btn_creat(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2,sbtn_width,sbtn_height,0,2);	//�����߽ǰ�ť
	str=gui_memin_malloc(300);	//����300�ֽ��ڴ�
	p=gui_memin_malloc(300);	//����300�ֽ��ڴ�
	if(!tedit||!rmemo||!smemo||!t9||!sbtn||!str||!p)rval=1;//����ʧ��. 
	if(rval==0)//�����ɹ�
	{ 
		sbtn->bkctbl[0]=0X6BF6;	//�߿���ɫ
		sbtn->bkctbl[1]=0X545E;	//0X8C3F.��һ�е���ɫ				
		sbtn->bkctbl[2]=0X5C7E;	//0X545E,�ϰ벿����ɫ
		sbtn->bkctbl[3]=0X2ADC;	//�°벿����ɫ	 
		sbtn->bcfucolor=WHITE;	//�ɿ�ʱΪ��ɫ
		sbtn->bcfdcolor=BLACK;	//����ʱΪ��ɫ 
		sbtn->caption=sms_send_tbl[gui_phy.language];
		sbtn->font=sbtn_fize;
		tedit->textbkcolor=SMS_PNUM_BACK_COLOR;
		tedit->textcolor=SMS_PNUM_NUM_COLOR;	
		rmemo->textbkcolor=SMS_RMEMO_BACK_COLOR;
		rmemo->textcolor=SMS_RMEMO_FONT_COLOR;
		smemo->textbkcolor=SMS_SMEMO_BACK_COLOR;
		smemo->textcolor=SMS_SMEMO_FONT_COLOR;
		if(mode==0)
		{
			gui_show_string(pnode->time,10,pnum_height+(rmsg_height-rmsg_fsize)/2,lcddev.width-10,rmsg_fsize,rmsg_fsize,SMS_RMSG_FONT_COLOR);
			strcpy((char*)tedit->text,(const char *)pnode->pnum);//�����绰����
			strcpy((char*)rmemo->text,(const char *)pnode->msg);//������Ϣ
			memo_draw_memo(rmemo,0);
		}else
		{
			editflag=1;			//Ĭ�ϱ༭��ϵ��
			tedit->type=0X07;	//�����˸ 
			smemo->type=0X00;	//memo���ɱ༭,��겻��˸
		} 
		sprintf((char*)str,"(%02d)",ccnt);
 		gui_show_strmid(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,BLACK,smsg_fsize,str);//��ʾ�ַ�
		edit_draw(tedit);//���༭��
		memo_draw_memo(smemo,0);//��memo�ؼ�
		t9_draw(t9);	
		btn_draw(sbtn);
	} 
	while(rval==0)
	{
		tp_dev.scan(0);    
		in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);	//�õ�������ֵ   
		delay_ms(1000/OS_TICKS_PER_SEC);		//��ʱһ��ʱ�ӽ���
		if(system_task_return)break;			//TPAD����
		if(mode==1)//�½����ŵ�ʱ��,Ҫ�л�tedit/rmemo��ѡ��
		{
			if(tedit->top<in_obj.y&&in_obj.y<(tedit->top+tedit->height)&&(tedit->left<in_obj.x)&&in_obj.x<(tedit->left+tedit->width))//��edit���ڲ� 
			{
				editflag=1;			//Ĭ�ϱ༭��ϵ��
				memo_show_cursor(smemo,0);//�ر�smemo�Ĺ��
				tedit->type=0X06;	//�����˸ 
				smemo->type=0X00;	//memo���ɱ༭,��겻��˸
			}
			if(smemo->top<in_obj.y&&in_obj.y<(smemo->top+smemo->height)&&(smemo->left<in_obj.x)&&in_obj.x<(smemo->left+smemo->width))//��smemo�ڲ� 
			{ 
				editflag=0;			//Ĭ�ϱ༭��ϵ��
				edit_show_cursor(tedit,0);//�ر�edit�Ĺ��
				tedit->type=0X04;	//��겻��˸ 
				smemo->type=0X01;	//memo�ɱ༭,��˸���  
			}
		}		
		edit_check(tedit,&in_obj);
		t9_check(t9,&in_obj);		   
		memo_check(smemo,&in_obj);
		memo_check(rmemo,&in_obj);//���rmemo
		if(t9->outstr[0]!=NULL)//����ַ�
		{
			if(editflag)
			{
				if((t9->outstr[0]<='9'&&t9->outstr[0]>='0')||t9->outstr[0]==0X08)edit_add_text(tedit,t9->outstr);//����/�˸��
			}else if(t9->outstr[0]==0X08||ccnt)//�˸��,����Ч
			{ 
				if(t9->outstr[0]==0X08){if(ccnt<70)ccnt++;}
				else ccnt--;
				if(t9->outstr[0]==0X0D)	//��ӻس�/���е�ʱ��,ȥ���س�,������������.
				{
					t9->outstr[0]=0X0A;	//���з�
					t9->outstr[1]=0;	//������
				}
				sprintf((char*)str,"(%02d)",ccnt);
				gui_fill_rectangle(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,SMS_SAREA_BACK_COLOR);
				gui_show_strmid(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,BLACK,smsg_fsize,str);//��ʾ�ַ�
				memo_add_text(smemo,t9->outstr);
			}
			t9->outstr[0]=NULL;	 			//�������ַ� 
		}
		res=btn_check(sbtn,&in_obj);   
		if(ccnt!=70&&res&&((sbtn->sta&(1<<7))==0)&&(sbtn->sta&(1<<6)))//������,�а����������ɿ�,����TP�ɿ���
		{ 
			if(strlen((char*)tedit->text)>0)
			{
				//��ʼ���Ͷ���
				gsm_unigbk_exchange(tedit->text,str,1); //���绰����ת��Ϊunicode�ַ���
				sprintf((char*)p,"AT+CMGS=\"%s\"",str); 
				if(gsm_send_cmd(p,">",200)==0)			//���Ͷ�������+�绰����
				{ 		 	
					gsm_unigbk_exchange(smemo->text,str,1);	//����������ת��Ϊunicode�ַ���.
					u3_printf("%s",str);	 
					gsm_send_cmd((u8*)0X1A,"+CMGS:",0);		//���ͽ�����,��������,���ȴ�
					calendar_get_time(&calendar);
					sprintf((char*)(rmemo->text+strlen((char*)rmemo->text)),"\n\n[%d/%d/%d %d:%d]%s\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,(char*)sms_remind_msg_tbl[5][gui_phy.language]);//���2�����з�
					strcat(((char*)rmemo->text),(char*)smemo->text);//��ӻظ�����			
					smemo->text[0]=0;		//smemo,text��������
					memo_draw_memo(smemo,0);//�ػ�smemo
					memo_draw_memo(rmemo,1);//�ػ�rmemo 
					ccnt=70;				//�����ַ�������
					sprintf((char*)str,"(%02d)",ccnt);
					gui_fill_rectangle(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,SMS_SAREA_BACK_COLOR);
					gui_show_strmid(smemo_offx+smemo_width+(lcddev.width-smemo_offx-smemo_width-sbtn_width)/2,pnum_height+rmsg_height+rmemo_height+(sarea_height-sbtn_height)/2+sbtn_height,sbtn_width,smsg_height,BLACK,smsg_fsize,str);//��ʾ�ַ�
				}else//����ӦAT+CMGSָ��,����ʧ��
				{ 
					window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)sms_remind_msg_tbl[8][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);	
 				}
				gsm_cmd_over();//�˳�ָ��ģʽ
			}else//�]����ϵ��?��ʾҪ��������ϵ��
			{  
				window_msg_box((lcddev.width-200)/2,(lcddev.height-80)/2,200,80,(u8*)sms_remind_msg_tbl[7][gui_phy.language],(u8*)APP_REMIND_CAPTION_TBL[gui_phy.language],12,0,1<<6,1200);	
 			}
		}
		if(curmsg<gsmdev.newmsg)	//��δ��������Ϣ
		{
			res=1;
			pnew=sms_node_creat();	//�����½ڵ�
			if(pnew)//�����ɹ�
			{
				res=sms_read_sms(pnew,gsmdev.newmsgindex[curmsg],1);//��ȡ����
				if(res==0)//��ȡ�ɹ�
				{ 
					if(strcmp((char*)pnew->pnum,(const char *)tedit->text)==0)//�Ա���������,���
					{ 
						calendar_get_time(&calendar);
						sprintf((char*)(rmemo->text+strlen((char*)rmemo->text)),"\n\n[%d/%d/%d %d:%d]%s\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,(char*)sms_remind_msg_tbl[6][gui_phy.language]);//���2�����з�
						strcat(((char*)rmemo->text),(char*)pnew->msg);//��ӻظ�����		
						memo_draw_memo(rmemo,1);//�ػ�rmemo 		
					} 
					curmsg++;
				}
			}
			if(res)curmsg=gsmdev.newmsg;
			sms_node_free(pnew,1);
		}
	}
	gui_memin_free(str);
	gui_memin_free(p);
	memo_delete(rmemo);
	memo_delete(smemo);
	t9_delete(t9);
	btn_delete(sbtn);
 	edit_delete(tedit); 
	system_task_return=0;
}
//���Ź���
u8 sms_play(void)
{
	u8 res;
	u16 msgnum=0,msgmax=0;
	sms_node *head,*ptemp;
	u16 selitem=0;
	u16 topitem=0; 
	u8 fsel=0;		//����ѡ��
	u8 readnewsms=0;//0,����ȡ����
					//1,��ȡ����
					//2,�½�����	
	
	head=sms_node_creat();//������ͷ
	msgnum=sms_read_all_sms(head,&msgmax);//��ȡ���ж���
	if(msgnum==0)//һ�����Ŷ�û��
	{ 
		head->pnum=gui_memex_malloc(30);//����30�ֽ�
		head->msg=gui_memex_malloc(30);	//����30�ֽ�
		head->time=gui_memex_malloc(30);//����30�ֽ�
		head->item=gui_memex_malloc(30);//����30�ֽ�
		if(head->item)
		{
			strcpy((char*)head->pnum,"ALIENTEK");//����
			strcpy((char*)head->msg,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//����:�޶���Ϣ��item����
			calendar_get_time(&calendar);
			sprintf((char*)head->pnum,"%d/%d/%d,%d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//����ʱ��			
			strcpy((char*)head->item,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//����:�޶���Ϣ��item����
		}
	} 
	if(head->item)//item������
	{
		LCD_Clear(BLACK);  
		while(1)
		{
			res=sms_msg_select(&selitem,&topitem,APP_MFUNS_CAPTION_TBL[26][gui_phy.language],head);
 			if(system_task_return)break;//TPAD����
			if(res&0X80)
			{
				readnewsms=0;
				if(res&1<<6)
				{
					fsel=0;
					if(msgnum)res=app_items_sel((lcddev.width-180)/2,(lcddev.height-192)/2,180,72+40*3,(u8**)sms_option_tbl[gui_phy.language],3,(u8*)&fsel,0XD0,(u8*)sms_remind_msg_tbl[1][gui_phy.language]);//3��ѡ��
					else res=app_items_sel((lcddev.width-180)/2,(lcddev.height-152)/2,180,72+40*2,(u8**)sms_option_tbl[gui_phy.language],2,(u8*)&fsel,0X90,(u8*)sms_remind_msg_tbl[1][gui_phy.language]);//2��ѡ��
 					if(res==0)//���óɹ�
					{
						switch(fsel)
						{
							case 0://�½����� 
								readnewsms=2;
								break;
							case 1://��ȡ���� 
								readnewsms=1;
								break;
							case 2:
								//ɾ������
								res=sms_delete_sms(&head,selitem);//ɾ����������
								if(res==0)
								{
									msgnum--;//����������1 
								} 
								break;
						}  
					}  	
				}else readnewsms=1;//˫��,��ȡ���� 
				if(readnewsms==2)sms_send_sms(ptemp,1);//�½����� 
				else if(readnewsms==1&&msgnum)//�ж��ŵ�ʱ��,�Ķ�/�ظ���ǰ����
				{
					ptemp=sms_node_getnode(head,selitem);//��ȡ�ڵ���Ϣ
					if(ptemp)//��ȡ�ɹ�
					{  
						sms_node_free(ptemp,0);	//�ͷŽڵ�����
						sms_read_sms(ptemp,ptemp->index,0);	//���¶�ȡ��������,���ı����״̬ 
						sms_node_free(ptemp,0);	//�ͷŽڵ����� 
						res=sms_read_sms(ptemp,ptemp->index,0);	//�����¶�ȡ����,���ı����״̬
						if(res==0)sms_send_sms(ptemp,0);//�Ķ�/�ظ���ǰ���� 					
					}
				} 				
			}else if(res==0)break;	//���ذ�ť������,�˳�
			while(gsmdev.newmsg)//�յ�����Ϣ��
			{
				if(msgnum==0)
				{
					sms_node_free(head,0);//���ͷ�֮ǰ�ı�ͷ������ 
					res=sms_read_sms(head,gsmdev.newmsgindex[gsmdev.newmsg-1],1);//��ȡ����
				}else
				{
					ptemp=sms_node_creat();//�����½ڵ�
					res=sms_read_sms(ptemp,gsmdev.newmsgindex[gsmdev.newmsg-1],1);//��ȡ����
					if(res==0)//��ȡ�ɹ�
					{
						res=sms_node_insert(head,ptemp,0XFFFF);//��ĩβ׷���½ڵ� 
					}
				}
				if(res)
				{ 
					gsmdev.newmsg=0;//ʣ�µĲ��ٶ�ȡ��
					sms_node_free(ptemp,1);//���ʧ��,�ͷŽڵ��ڴ�
					if(msgnum==0)//ֻ�б�ͷ�����,�Ҷ�����ʧ��,�������Ĭ����Ϣ
					{		
						head->pnum=gui_memex_malloc(30);//����30�ֽ�
						head->msg=gui_memex_malloc(30);	//����30�ֽ�
						head->time=gui_memex_malloc(30);//����30�ֽ�
						head->item=gui_memex_malloc(30);//����30�ֽ�
						if(head->item)
						{
							strcpy((char*)head->pnum,"ALIENTEK");//����
							strcpy((char*)head->msg,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//����:�޶���Ϣ��item����
							calendar_get_time(&calendar);
							sprintf((char*)head->pnum,"%d/%d/%d,%d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//����ʱ��			
							strcpy((char*)head->item,(char*)sms_remind_msg_tbl[0][gui_phy.language]);//����:�޶���Ϣ��item����
						}
					}
				}else 
				{
					msgnum++;//������Ŀ��1
					gsmdev.newmsg--;
				}
			}			
		} 
	}
	sms_node_destroy(head);//ɾ����������
	return 0;
	
}






















