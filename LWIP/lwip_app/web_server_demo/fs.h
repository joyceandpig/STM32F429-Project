#ifndef __FS_H__
#define __FS_H__
#include "lwip/opt.h"
#include "ff.h"
#include "malloc.h"
/////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������ 
//lwip fs��������(����lwip��fs.c�޸�,��֧�ִ�SD����ȡ��ҳԴ��͸�����Դ)	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/3/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


/** Set this to 1 to include an application state argument per file
 * that is opened. This allows to keep a state per connection/file.
 */
#ifndef LWIP_HTTPD_FILE_STATE
#define LWIP_HTTPD_FILE_STATE         0
#endif

/** HTTPD_PRECALCULATED_CHECKSUM==1: include precompiled checksums for
 * predefined (MSS-sized) chunks of the files to prevent having to calculate
 * the checksums at runtime. */
#ifndef HTTPD_PRECALCULATED_CHECKSUM
#define HTTPD_PRECALCULATED_CHECKSUM  0
#endif

#if HTTPD_PRECALCULATED_CHECKSUM
struct fsdata_chksum 
{
	u32_t offset;
	u16_t chksum;
	u16_t len;
};
#endif /* HTTPD_PRECALCULATED_CHECKSUM */

//fs_file�ṹ��
struct fs_file 
{
	FIL *flwip;				//�ļ�ָ��
	char *data;   			//���ݻ�����
	int len;  				//ʣ�����ݳ���  
	u32 dataleft;			//���ݻ�����ʣ���ֽ���
	u32 dataptr;			//���ݻ�������ָ��
	u32 fleft;				//�ļ�ʣ���ֽ���
	void *pextension;
#if HTTPD_PRECALCULATED_CHECKSUM			//HTTPD_PRECALCULATED_CHECKSUM�Ƿ���?
	const struct fsdata_chksum *chksum;
	u16_t chksum_count;
#endif  
	u8_t http_header_included;				//�Ƿ����httpͷ,������������:LWIP_HTTPD_DYNAMIC_HEADERSΪ1,��lwip�Զ����httpͷ
#if LWIP_HTTPD_CUSTOM_FILES					//LWIP_HTTPD_CUSTOM_FILES�Ƿ���?
	u8_t is_custom_file;
#endif  
#if LWIP_HTTPD_FILE_STATE					//LWIP_HTTPD_FILE_STATE�Ƿ���?
	void *state;
#endif  
};

struct fs_file *fs_open(const char *name);
void fs_close(struct fs_file *file);
int fs_read(struct fs_file *file, char *buffer, int count);
int fs_bytes_left(struct fs_file *file);

#if LWIP_HTTPD_FILE_STATE
/** This user-defined function is called when a file is opened. */
void *fs_state_init(struct fs_file *file, const char *name);
/** This user-defined function is called when a file is closed. */
void fs_state_free(struct fs_file *file, void *state);
#endif /* #if LWIP_HTTPD_FILE_STATE */

#endif /* __FS_H__ */
