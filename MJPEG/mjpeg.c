#include "mjpeg.h" 
#include "malloc.h"
#include "ff.h"  
#include "lcd.h" 
#include "ltdc.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//MJPEG��Ƶ���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   


struct jpeg_decompress_struct *cinfo;
struct my_error_mgr *jerr;
u8 *jpegbuf;			//jpeg���ݻ���ָ��
u16 *outlinebuf;		//����л���,��ͼ����*2�ֽ�
u16 outlinecnt;			//����м�����
u32 jbufsize;			//jpeg buf��С
u16 imgoffx,imgoffy;	//ͼ����x,y�����ƫ����


  
////////////////////////////////////////////////////////////////////////////////
//�򵥿��ٵ��ڴ����,������ٶ�
#define MJPEG_MAX_MALLOC_SIZE 		38*1024			//�����Է���38K�ֽ�


u8 *jmembuf;			//mjpeg����� �ڴ��
u32 jmempos;			//�ڴ��ָ��

//mjpeg�����ڴ�
void* mjpeg_malloc(u32 num)
{
	u32 curpos=jmempos; //�˴η������ʼ��ַ
 	jmempos+=num;		//��һ�η������ʼ��ַ 
	if(jmempos>38*1024) 
	{
		printf("mem error:%d,%d",curpos,num);
	}
	return (void *)&jmembuf[curpos];	//�������뵽���ڴ��׵�ַ
}  
////////////////////////////////////////////////////////////////////////////////
//�����˳�
static void my_error_exit(j_common_ptr cinfo)
{ 
	my_error_ptr myerr=(my_error_ptr) cinfo->err; 
	(*cinfo->err->output_message) (cinfo);	 
	longjmp(myerr->setjmp_buffer, 1);	  
} 

METHODDEF(void) my_emit_message(j_common_ptr cinfo, int msg_level)
{
	my_error_ptr myerr=(my_error_ptr) cinfo->err;  
    if(msg_level<0)
	{
		printf("emit msg:%d\r\n",msg_level); 
		longjmp(myerr->setjmp_buffer, 1);		
	}
}

//��ʼ����Դ,��ִ���κβ���
static void init_source(j_decompress_ptr cinfo)
{
    //����Ҫ���κ�����.
    return;
} 
//������뻺����,һ���Զ�ȡ��֡����
static boolean fill_input_buffer(j_decompress_ptr cinfo)
{  
	if(jbufsize==0)//������
	{
		printf("jd read off\r\n");
        //��������
        jpegbuf[0] = (u8) 0xFF;
        jpegbuf[1] = (u8) JPEG_EOI;
  		cinfo->src->next_input_byte =jpegbuf;
		cinfo->src->bytes_in_buffer = 2; 
	}else
	{
		cinfo->src->next_input_byte =jpegbuf;
		cinfo->src->bytes_in_buffer = jbufsize;
		jbufsize-=jbufsize;
	}
    return TRUE;
}
//���ļ�����,����num_bytes������
static void skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{ 
    /* Just a dumb implementation for now.  Could use fseek() except
    * it doesn't work on pipes.  Not clear that being smart is worth
    * any trouble anyway --- large skips are infrequent.
    */
    if (num_bytes > 0)
    {
        while(num_bytes>(long) cinfo->src->bytes_in_buffer)
        {
            num_bytes-=(long)cinfo->src->bytes_in_buffer;
            (void)cinfo->src->fill_input_buffer(cinfo);
            /* note we assume that fill_input_buffer will never
            * return FALSE, so suspension need not be handled.
            */
        }
        cinfo->src->next_input_byte += (size_t) num_bytes;
        cinfo->src->bytes_in_buffer -= (size_t) num_bytes;
    }
} 
//�ڽ��������,��jpeg_finish_decompress��������
static void term_source(j_decompress_ptr cinfo)
{
    //�����κδ���
    return;
}
//��ʼ��jpeg��������Դ
static void jpeg_filerw_src_init(j_decompress_ptr cinfo)
{ 
    if (cinfo->src == NULL)     /* first time for this JPEG object? */
    {
        cinfo->src = (struct jpeg_source_mgr *)
                     (*cinfo->mem->alloc_small)((j_common_ptr) cinfo, JPOOL_PERMANENT,
                                              sizeof(struct jpeg_source_mgr)); 
    } 
    cinfo->src->init_source = init_source;
    cinfo->src->fill_input_buffer = fill_input_buffer;
    cinfo->src->skip_input_data = skip_input_data;
    cinfo->src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
    cinfo->src->term_source = term_source;
    cinfo->src->bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
    cinfo->src->next_input_byte = NULL; /* until buffer loaded */
} 


//mjpeg �����ʼ��
//offx,offy:x,y�����ƫ��
//����ֵ:0,�ɹ�;
//       1,ʧ��
u8 mjpegdec_init(u16 offx,u16 offy)
{
	cinfo=mymalloc(SRAMCCM,sizeof(struct jpeg_decompress_struct));
	jerr=mymalloc(SRAMCCM,sizeof(struct my_error_mgr));
	jmembuf=mymalloc(SRAMCCM,MJPEG_MAX_MALLOC_SIZE);//MJPEG�����ڴ������
	outlinebuf=mymalloc(SRAMIN,lcddev.width*2);		//��������л���
	if(cinfo==0||jerr==0||jmembuf==0||outlinebuf==0)
	{
		mjpegdec_free();
		return 1;
	}
	//����ͼ����x,y�����ƫ����
	imgoffx=offx;
	imgoffy=offy; 
	return 0;
}
//mjpeg����,�ͷ��ڴ�
void mjpegdec_free(void)
{    
	myfree(SRAMCCM,cinfo);
	myfree(SRAMCCM,jerr);
	myfree(SRAMCCM,jmembuf); 
	myfree(SRAMIN,outlinebuf);
}

//����һ��JPEGͼƬ
//buf:jpeg����������
//bsize:�����С
//����ֵ:0,�ɹ�
//    ����,����
u8 mjpegdec_decode(u8* buf,u32 bsize)
{
    JSAMPARRAY buffer;		
	if(bsize==0)return 1;
	jpegbuf=buf;
	jbufsize=bsize;	   
	jmempos=0;//MJEPG����,���´�0��ʼ�����ڴ�
	
	cinfo->err=jpeg_std_error(&jerr->pub); 
	jerr->pub.error_exit = my_error_exit; 
	jerr->pub.emit_message = my_emit_message; 
	//if(bsize>20*1024)printf("s:%d\r\n",bsize); 
	if (setjmp(jerr->setjmp_buffer)) //������
	{ 
 		jpeg_abort_decompress(cinfo);
		jpeg_destroy_decompress(cinfo); 
		return 2;
	} 
	jpeg_create_decompress(cinfo); 
	jpeg_filerw_src_init(cinfo);  
	jpeg_read_header(cinfo, TRUE); 
	cinfo->dct_method = JDCT_IFAST;
	cinfo->do_fancy_upsampling = 0;  
	jpeg_start_decompress(cinfo); 
	if(lcdltdc.width!=0)//RGB��
	{
		outlinecnt=imgoffy;				//������λ�� 
		RCC->AHB1ENR|=1<<23;			//ʹ��DM2Dʱ��
		DMA2D->CR=0<<16;				//�洢�����洢��ģʽ
		DMA2D->FGPFCCR=LCD_PIXFORMAT;	//������ɫ��ʽ
		DMA2D->FGOR=0;					//ǰ������ƫ��Ϊ0
	}else//MCU��
	{
		LCD_Set_Window(imgoffx,imgoffy,cinfo->output_width,cinfo->output_height);
		LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	
	}
	while (cinfo->output_scanline < cinfo->output_height) 
	{ 
		jpeg_read_scanlines(cinfo, buffer, 1);
	} 
	if(lcdltdc.width==0)//��RGB��
	{   
		LCD_Set_Window(0,0,lcddev.width,lcddev.height);//�ָ�����
	}
	jpeg_finish_decompress(cinfo); 
	jpeg_destroy_decompress(cinfo);  
	return 0;
}


 













