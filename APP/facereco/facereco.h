#ifndef __FACEFRECO_H
#define __FACEFRECO_H 	
#include "common.h"  



//人脸链表节点
typedef  struct __frec_node 
{	
	u8* pname;	//姓名指针
	u8* pnum;	//手机号码指针
	u8* sex;	//性别字符串指针
	u8 index;	//该人脸,保存的位置,也就是识别ID. 
	struct __frec_node *next;//指向下一个节点
}frec_node; 

//人脸识别控制结构体
__packed typedef struct 
{ 
	u16 xoff;					//摄像头在LCD上面显示,x方向的偏移
	u16 yoff;					//摄像头在LCD上面显示,y方向的偏移
	u16 width;					//摄像头在LCD上面显示的宽度
	u16 height;					//摄像头在LCD上面显示的高度
	u16 *databuf;				//图像数据缓存区
	frec_node *face;			//人脸信息
}_frec_obj;

extern _frec_obj frec_dev;		//人脸识别控制结构体
extern u16 frec_curline;		//摄像头输出数据,当前行编号,人脸识别用
extern vu8 frec_running;		//人脸识别运行中标志

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













