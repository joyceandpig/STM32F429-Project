#ifndef __FACEFRECO_H
#define __FACEFRECO_H 	
#include "common.h"  



//��������ڵ�
typedef  struct __frec_node 
{	
	u8* pname;	//����ָ��
	u8* pnum;	//�ֻ�����ָ��
	u8* sex;	//�Ա��ַ���ָ��
	u8 index;	//������,�����λ��,Ҳ����ʶ��ID. 
	struct __frec_node *next;//ָ����һ���ڵ�
}frec_node; 

//����ʶ����ƽṹ��
__packed typedef struct 
{ 
	u16 xoff;					//����ͷ��LCD������ʾ,x�����ƫ��
	u16 yoff;					//����ͷ��LCD������ʾ,y�����ƫ��
	u16 width;					//����ͷ��LCD������ʾ�Ŀ��
	u16 height;					//����ͷ��LCD������ʾ�ĸ߶�
	u16 *databuf;				//ͼ�����ݻ�����
	frec_node *face;			//������Ϣ
}_frec_obj;

extern _frec_obj frec_dev;		//����ʶ����ƽṹ��
extern u16 frec_curline;		//����ͷ�������,��ǰ�б��,����ʶ����
extern vu8 frec_running;		//����ʶ�������б�־

frec_node* frec_node_creat(void);

void frec_node_free(frec_node * pnode,u8 mode);
u8 frec_node_insert(frec_node* head,frec_node *pnew,u16 i);
u8 frec_node_delete(frec_node* head,u16 i);
frec_node* frec_node_getnode(frec_node* head,u16 i);
u16 frec_node_getsize(frec_node* head);
void frec_node_destroy(frec_node* head);


void frec_set_image_center(_frec_obj *frecdev);
void frec_get_image_data(u16 *dbuf,u16 xoff,u16 yoff,u16 xsize,u16 width);

u8 frec_add_a_face_info(_frec_obj *frecdev);

u8 appplay_frec_read_a_face(frec_node *pnode,_frec_obj *frecdev,u8 index);

u8 appplay_frec_add_a_face(frec_node *pnode,_frec_obj *frecdev,u8 mode);

void frec_show_picture(u16 x, u16 y, u16 w, u16 h, u16 *data);
u8 frec_play(void); 
#endif













