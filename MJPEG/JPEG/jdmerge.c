/*
 * jdmerge.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains code for merged upsampling/color conversion.
 *
 * This file combines functions from jdsample.c and jdcolor.c;
 * read those files first to understand what's going on.
 *
 * When the chroma components are to be upsampled by simple replication
 * (ie, box filtering), we can save some work in color conversion by
 * calculating all the output pixels corresponding to a pair of chroma
 * samples at one time.  In the conversion equations
 *	R = Y           + K1 * Cr
 *	G = Y + K2 * Cb + K3 * Cr
 *	B = Y + K4 * Cb
 * only the Y term varies among the group of pixels corresponding to a pair
 * of chroma samples, so the rest of the terms can be calculated just once.
 * At typical sampling ratios, this eliminates half or three-quarters of the
 * multiplications needed for color conversion.
 *
 * This file currently provides implementations for the following cases:
 *	YCbCr => RGB color conversion only.
 *	Sampling ratios of 2h1v or 2h2v.
 *	No scaling needed at upsample time.
 *	Corner-aligned (non-CCIR601) sampling alignment.
 * Other special cases could be added, but in most applications these are
 * the only common cases.  (For uncommon cases we fall back on the more
 * general code in jdsample.c and jdcolor.c.)
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"

#include "lcd.h"


#ifdef UPSAMPLE_MERGING_SUPPORTED


/* Private subobject */

typedef struct {
  struct jpeg_upsampler pub;	/* public fields */

  /* Pointer to routine to do actual upsampling/conversion of one row group */
  JMETHOD(void, upmethod, (j_decompress_ptr cinfo,
			   JSAMPIMAGE input_buf, JDIMENSION in_row_group_ctr,
			   JSAMPARRAY output_buf));

  /* Private state for YCC->RGB conversion */
  int * Cr_r_tab;		/* => table for Cr to R conversion */
  int * Cb_b_tab;		/* => table for Cb to B conversion */
  INT32 * Cr_g_tab;		/* => table for Cr to G conversion */
  INT32 * Cb_g_tab;		/* => table for Cb to G conversion */

  /* For 2:1 vertical sampling, we produce two output rows at a time.
   * We need a "spare" row buffer to hold the second output row if the
   * application provides just a one-row buffer; we also use the spare
   * to discard the dummy last row if the image height is odd.
   */
  JSAMPROW spare_row;
  boolean spare_full;		/* T if spare buffer is occupied */

  JDIMENSION out_row_width;	/* samples per output row */
  JDIMENSION rows_to_go;	/* counts rows remaining in image */
} my_upsampler;

typedef my_upsampler * my_upsample_ptr;

#define SCALEBITS	16	/* speediest right-shift on some machines */
#define ONE_HALF	((INT32) 1 << (SCALEBITS-1))
#define FIX(x)		((INT32) ((x) * (1L<<SCALEBITS) + 0.5))


/*
 * Initialize tables for YCC->RGB colorspace conversion.
 * This is taken directly from jdcolor.c; see that file for more info.
 */

LOCAL(void)
build_ycc_rgb_table (j_decompress_ptr cinfo)
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample;
  int i;
  INT32 x;
  SHIFT_TEMPS

  upsample->Cr_r_tab = (int *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(int));
  upsample->Cb_b_tab = (int *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(int));
  upsample->Cr_g_tab = (INT32 *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(INT32));
  upsample->Cb_g_tab = (INT32 *)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				(MAXJSAMPLE+1) * SIZEOF(INT32));

  for (i = 0, x = -CENTERJSAMPLE; i <= MAXJSAMPLE; i++, x++) {
    /* i is the actual input pixel value, in the range 0..MAXJSAMPLE */
    /* The Cb or Cr value we are thinking of is x = i - CENTERJSAMPLE */
    /* Cr=>R value is nearest int to 1.40200 * x */
    upsample->Cr_r_tab[i] = (int)
		    RIGHT_SHIFT(FIX(1.40200) * x + ONE_HALF, SCALEBITS);
    /* Cb=>B value is nearest int to 1.77200 * x */
    upsample->Cb_b_tab[i] = (int)
		    RIGHT_SHIFT(FIX(1.77200) * x + ONE_HALF, SCALEBITS);
    /* Cr=>G value is scaled-up -0.71414 * x */
    upsample->Cr_g_tab[i] = (- FIX(0.71414)) * x;
    /* Cb=>G value is scaled-up -0.34414 * x */
    /* We also add in ONE_HALF so that need not do it in inner loop */
    upsample->Cb_g_tab[i] = (- FIX(0.34414)) * x + ONE_HALF;
  }
}


/*
 * Initialize for an upsampling pass.
 */

METHODDEF(void)
start_pass_merged_upsample (j_decompress_ptr cinfo)
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample;

  /* Mark the spare buffer empty */
  upsample->spare_full = FALSE;
  /* Initialize total-height counter for detecting bottom of image */
  upsample->rows_to_go = cinfo->output_height;
}


/*
 * Control routine to do upsampling (and color conversion).
 *
 * The control routine just handles the row buffering considerations.
 */

METHODDEF(void)
merged_2v_upsample (j_decompress_ptr cinfo,
		    JSAMPIMAGE input_buf, JDIMENSION *in_row_group_ctr,
		    JDIMENSION in_row_groups_avail,
		    JSAMPARRAY output_buf, JDIMENSION *out_row_ctr,
		    JDIMENSION out_rows_avail)
/* 2:1 vertical sampling case: may need a spare row. */
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample;
  JSAMPROW work_ptrs[2];
  JDIMENSION num_rows;		/* number of rows returned to caller */

  if (upsample->spare_full) {
    /* If we have a spare row saved from a previous cycle, just return it. */
//    jcopy_sample_rows(& upsample->spare_row, 0, output_buf + *out_row_ctr, 0,
//		      1, upsample->out_row_width);
    num_rows = 1;
    upsample->spare_full = FALSE;
  } else {
    /* Figure number of rows to return to caller. */
    num_rows = 2;
    /* Not more than the distance to the end of the image. */
    if (num_rows > upsample->rows_to_go)
      num_rows = upsample->rows_to_go;
    /* And not more than what the client can accept: */
    out_rows_avail -= *out_row_ctr;
    if (num_rows > out_rows_avail)
      num_rows = out_rows_avail;
    /* Create output pointer array for upsampler. */
//    work_ptrs[0] = output_buf[*out_row_ctr];
    if (num_rows > 1) {
//      work_ptrs[1] = output_buf[*out_row_ctr + 1];
    } else {
//      work_ptrs[1] = upsample->spare_row;
      upsample->spare_full = TRUE;
    }
    /* Now do the upsampling. */
    (*upsample->upmethod) (cinfo, input_buf, *in_row_group_ctr, work_ptrs);
  }

  /* Adjust counts */
  *out_row_ctr += num_rows;
  upsample->rows_to_go -= num_rows;
  /* When the buffer is emptied, declare this input row group consumed */
  if (! upsample->spare_full)
    (*in_row_group_ctr)++;
}


METHODDEF(void)
merged_1v_upsample (j_decompress_ptr cinfo,
		    JSAMPIMAGE input_buf, JDIMENSION *in_row_group_ctr,
		    JDIMENSION in_row_groups_avail,
		    JSAMPARRAY output_buf, JDIMENSION *out_row_ctr,
		    JDIMENSION out_rows_avail)
/* 1:1 vertical sampling case: much easier, never need a spare row. */
{
  my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample;

  /* Just do the upsampling. */
  (*upsample->upmethod) (cinfo, input_buf, *in_row_group_ctr,
			 output_buf + *out_row_ctr);
  /* Adjust counts */
  (*out_row_ctr)++;
  (*in_row_group_ctr)++;
}


/*
 * These are the routines invoked by the control routines to do
 * the actual upsampling/conversion.  One row group is processed per call.
 *
 * Note: since we may be writing directly into application-supplied buffers,
 * we have to be honest about the output width; we can't assume the buffer
 * has been rounded up to an even width.
 */


/*
 * Upsample and color convert for the case of 2:1 horizontal and 1:1 vertical.
 */
#include "lcd.h"
#include "ltdc.h"

//��mjpeg.c���涨��
extern u16 imgoffx,imgoffy;		//ͼ����x,y�����ƫ����
extern u16 outlinecnt;			//����м�����
extern u16 *outlinebuf;			//����л���,��ͼ����*2�ֽ�

//��ltdc.c���涨��
extern u32 *ltdc_framebuf[2];	//LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����


//��ָ�����������ָ����ɫ��,DMA2D���	
//�˺�����֧��u16,RGB565��ʽ����ɫ�������.
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)  
//ע��:sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
//color:Ҫ������ɫ�����׵�ַ
void LTDC_Color_Fill2(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u32 psx,psy,pex,pey;	//��LCD���Ϊ��׼������ϵ,����������仯���仯
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	//����ϵת��
	if(lcdltdc.dir)	//����
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//����
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	DMA2D->OOR=offline;				//������ƫ�� 
	DMA2D->CR&=~(1<<0);				//��ֹͣDMA2D
	DMA2D->FGMAR=(u32)color;		//Դ��ַ
	DMA2D->OMAR=addr;				//����洢����ַ
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//�趨�����Ĵ��� 
	DMA2D->CR|=1<<0;				//����DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//�ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//��ʱ�˳�
	} 
	DMA2D->IFCR|=1<<1;				//���������ɱ�־  	
}  
 
METHODDEF(void)
h2v1_merged_upsample (j_decompress_ptr cinfo,
		      JSAMPIMAGE input_buf, JDIMENSION in_row_group_ctr,
		      JSAMPARRAY output_buf)
{
	my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample;
	JDIMENSION col;
	int y, cred, cgreen, cblue;
	int cb, cr;
	//  JSAMPROW outptr;
	JSAMPROW inptr0, inptr1, inptr2;


	/* copy these pointers into registers if possible */
	JSAMPLE * range_limit = cinfo->sample_range_limit;
	int * Crrtab = upsample->Cr_r_tab;
	int * Cbbtab = upsample->Cb_b_tab;
	INT32 * Crgtab = upsample->Cr_g_tab;
	INT32 * Cbgtab = upsample->Cb_g_tab;
	SHIFT_TEMPS

	inptr0 = input_buf[0][in_row_group_ctr];
	inptr1 = input_buf[1][in_row_group_ctr];
	inptr2 = input_buf[2][in_row_group_ctr];
	// outptr = output_buf[0];
	/* Loop for each pair of output pixels */
	if(lcdltdc.width!=0)//RGB��
	{
		for (col = cinfo->output_width >> 1; col > 0; col--) 
		{
			/* Do the chroma part of the calculation */
			cb = GETJSAMPLE(*inptr1++);
			cr = GETJSAMPLE(*inptr2++);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			/* Fetch 2 Y values and emit 2 pixels */
			y  = GETJSAMPLE(*inptr0++);
			//outptr[RGB_RED] =   range_limit[y + cred];
			//outptr[RGB_GREEN] = range_limit[y + cgreen];
			//outptr[RGB_BLUE] =  range_limit[y + cblue];
			//outptr += RGB_PIXELSIZE; 
			outlinebuf[(col<<1)-1]=(range_limit[y + cblue] >> 3)<<11 | (range_limit[y + cgreen] >> 2) << 5 | range_limit[y + cred] >> 3;
			
			y  = GETJSAMPLE(*inptr0++);
			//outptr[RGB_RED] =   range_limit[y + cred];
			//outptr[RGB_GREEN] = range_limit[y + cgreen];
			//outptr[RGB_BLUE] =  range_limit[y + cblue];
			//outptr += RGB_PIXELSIZE;
			outlinebuf[(col<<1)-2] = (range_limit[y + cblue] >> 3)<<11 | (range_limit[y + cgreen] >> 2) << 5 | range_limit[y + cred] >> 3;

		} 
		LTDC_Color_Fill2(imgoffx,outlinecnt,cinfo->output_width+imgoffx-1,outlinecnt,outlinebuf);//DM2D��� 
		outlinecnt++;
		/* If image width is odd, do the last output column separately */
		//if (cinfo->output_width & 1) //RGB��,��������.
		
	}else	//MCU��
	{
		for (col = cinfo->output_width >> 1; col > 0; col--) 
		{
			/* Do the chroma part of the calculation */
			cb = GETJSAMPLE(*inptr1++);
			cr = GETJSAMPLE(*inptr2++);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			/* Fetch 2 Y values and emit 2 pixels */
			y  = GETJSAMPLE(*inptr0++);
			//outptr[RGB_RED] =   range_limit[y + cred];
			//outptr[RGB_GREEN] = range_limit[y + cgreen];
			//outptr[RGB_BLUE] =  range_limit[y + cblue];
			//outptr += RGB_PIXELSIZE; 
			LCD->LCD_RAM = (range_limit[y + cblue] >> 3)<<11 | (range_limit[y + cgreen] >> 2) << 5 | range_limit[y + cred] >> 3;

			y  = GETJSAMPLE(*inptr0++);
			//outptr[RGB_RED] =   range_limit[y + cred];
			//outptr[RGB_GREEN] = range_limit[y + cgreen];
			//outptr[RGB_BLUE] =  range_limit[y + cblue];
			//outptr += RGB_PIXELSIZE;
			LCD->LCD_RAM = (range_limit[y + cblue] >> 3)<<11 | (range_limit[y + cgreen] >> 2) << 5 | range_limit[y + cred] >> 3;

		} 
		/* If image width is odd, do the last output column separately */
		if (cinfo->output_width & 1) 
		{
			cb = GETJSAMPLE(*inptr1);
			cr = GETJSAMPLE(*inptr2);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			y  = GETJSAMPLE(*inptr0);
			//outptr[RGB_RED] =   range_limit[y + cred];
			//outptr[RGB_GREEN] = range_limit[y + cgreen];
			//outptr[RGB_BLUE] =  range_limit[y + cblue];
			LCD->LCD_RAM = (range_limit[y + cblue] >> 3)<<11 | (range_limit[y + cgreen] >> 2) << 5 | range_limit[y + cred] >> 3;
		}
	}
} 

/*
 * Upsample and color convert for the case of 2:1 horizontal and 2:1 vertical.
 */  
METHODDEF(void)
h2v2_merged_upsample (j_decompress_ptr cinfo,
		      JSAMPIMAGE input_buf, JDIMENSION in_row_group_ctr,
		      JSAMPARRAY output_buf)
{
	int y, cred, cgreen, cblue;
	int col;
	JSAMPLE * range_limit = cinfo->sample_range_limit;

	my_upsample_ptr upsample = (my_upsample_ptr) cinfo->upsample;

	int cb, cr;
	//  JSAMPROW outptr0, outptr1;
	//  uint16_t rgb565;

	JSAMPROW inptr00, inptr01, inptr1, inptr2;
	//  JDIMENSION col;
	/* copy these pointers into registers if possible */
	int * Crrtab = upsample->Cr_r_tab;
	int * Cbbtab = upsample->Cb_b_tab;
	INT32 * Crgtab = upsample->Cr_g_tab;
	INT32 * Cbgtab = upsample->Cb_g_tab;
	SHIFT_TEMPS

	inptr00 = input_buf[0][in_row_group_ctr*2];
	//  inptr01 = input_buf[0][in_row_group_ctr*2 + 1];
	inptr1 = input_buf[1][in_row_group_ctr];
	inptr2 = input_buf[2][in_row_group_ctr];
	
	if(lcdltdc.width!=0)//RGB��
	{

		/* Loop for each group of output pixels */
		for (col = cinfo->output_width >> 1; col > 0; col--) 
		{
			/* Do the chroma part of the calculation */
			cb = GETJSAMPLE(*inptr1++);
			cr = GETJSAMPLE(*inptr2++);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			/* Fetch 4 Y values and emit 4 pixels */
			y  = GETJSAMPLE(*inptr00++);
			outlinebuf[(col<<1)-1] = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;
 
			y  = GETJSAMPLE(*inptr00++);
			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;
			outlinebuf[(col<<1)-2] = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;
		}
		LTDC_Color_Fill2(imgoffx,outlinecnt,cinfo->output_width+imgoffx-1,outlinecnt,outlinebuf);//DM2D��� 
		outlinecnt++; 
		//inptr00 = input_buf[0][in_row_group_ctr*2];
		inptr01 = input_buf[0][in_row_group_ctr*2 + 1];
		inptr1 = input_buf[1][in_row_group_ctr];
		inptr2 = input_buf[2][in_row_group_ctr];

		/* Loop for each group of output pixels */
		for (col = cinfo->output_width >> 1; col > 0; col--) 
		{ 
			/* Do the chroma part of the calculation */
			cb = GETJSAMPLE(*inptr1++);
			cr = GETJSAMPLE(*inptr2++);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			/* Fetch 4 Y values and emit 4 pixels */
			y  = GETJSAMPLE(*inptr01++);
			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;

			outlinebuf[(col<<1)-1] = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;

			y  = GETJSAMPLE(*inptr01++);
			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;

			outlinebuf[(col<<1)-2] = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;
		}
		LTDC_Color_Fill2(imgoffx,outlinecnt,cinfo->output_width+imgoffx-1,outlinecnt,outlinebuf);//DM2D��� 
		outlinecnt++; 
		/* If image width is odd, do the last output column separately */
		//if (cinfo->output_width & 1)  //RGB��,��������. 
	}else
	{
		/* Loop for each group of output pixels */
		for (col = cinfo->output_width >> 1; col > 0; col--) 
		{
			/* Do the chroma part of the calculation */
			cb = GETJSAMPLE(*inptr1++);
			cr = GETJSAMPLE(*inptr2++);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			/* Fetch 4 Y values and emit 4 pixels */
			y  = GETJSAMPLE(*inptr00++);
			LCD->LCD_RAM = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;

			y  = GETJSAMPLE(*inptr00++);
			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;
			//LCD->LCD_RAM=pixel.color.d16;
			LCD->LCD_RAM = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;
		}
		//  inptr00 = input_buf[0][in_row_group_ctr*2];
		inptr01 = input_buf[0][in_row_group_ctr*2 + 1];
		inptr1 = input_buf[1][in_row_group_ctr];
		inptr2 = input_buf[2][in_row_group_ctr];

		/* Loop for each group of output pixels */
		//  for (col = cinfo->output_width >> 1; col > 0; col--) {
		for (col = cinfo->output_width >> 1; col > 0; col--) 
		{ 
			/* Do the chroma part of the calculation */
			cb = GETJSAMPLE(*inptr1++);
			cr = GETJSAMPLE(*inptr2++);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			/* Fetch 4 Y values and emit 4 pixels */
			y  = GETJSAMPLE(*inptr01++);
			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;
			//LCD->RAM=pixel.color.d16;

			LCD->LCD_RAM = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;

			y  = GETJSAMPLE(*inptr01++);
			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;
			//LCD->LCD_RAM = pixel.color.d16;
			LCD->LCD_RAM = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;
		}	
		/* If image width is odd, do the last output column separately */
		if (cinfo->output_width & 1) 
		{
			cb = GETJSAMPLE(*inptr1);
			cr = GETJSAMPLE(*inptr2);
			cred = Crrtab[cr];
			cgreen = (int) RIGHT_SHIFT(Cbgtab[cb] + Crgtab[cr], SCALEBITS);
			cblue = Cbbtab[cb];
			y  = GETJSAMPLE(*inptr00);
			//outptr0[RGB_RED] =   range_limit[y + cred];
			//outptr0[RGB_GREEN] = range_limit[y + cgreen];
			//outptr0[RGB_BLUE] =  range_limit[y + cblue];

			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;
			//LCD->LCD_RAM = pixel.color.d16;
			LCD->LCD_RAM = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;


			y  = GETJSAMPLE(*inptr01);
			//outptr1[RGB_RED] =   range_limit[y + cred];
			//outptr1[RGB_GREEN] = range_limit[y + cgreen];
			//outptr1[RGB_BLUE] =  range_limit[y + cblue];


			//pixel.color.B = range_limit[y + cblue] >> 3;
			//pixel.color.G = range_limit[y + cgreen] >> 2;
			//pixel.color.R = range_limit[y + cred] >> 3;
			//LCD->RAM = pixel.color.d16;
			LCD->LCD_RAM = range_limit[y + cblue] >> 3 | (range_limit[y + cgreen] >> 2) << 5 | (range_limit[y + cred] >> 3) << 11;
		}		
	}  
}


/*
 * Module initialization routine for merged upsampling/color conversion.
 *
 * NB: this is called under the conditions determined by use_merged_upsample()
 * in jdmaster.c.  That routine MUST correspond to the actual capabilities
 * of this module; no safety checks are made here.
 */

GLOBAL(void)
jinit_merged_upsampler (j_decompress_ptr cinfo)
{
	my_upsample_ptr upsample;

	upsample = (my_upsample_ptr)
	(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
	SIZEOF(my_upsampler));
	cinfo->upsample = (struct jpeg_upsampler *) upsample;
	upsample->pub.start_pass = start_pass_merged_upsample;
	upsample->pub.need_context_rows = FALSE;

	upsample->out_row_width = cinfo->output_width * cinfo->out_color_components;

	if (cinfo->max_v_samp_factor == 2) 
	{
		upsample->pub.upsample = merged_2v_upsample;
		upsample->upmethod = h2v2_merged_upsample;
		/* Allocate a spare row buffer */
		//    upsample->spare_row = (JSAMPROW)
		//      (*cinfo->mem->alloc_large) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		//		(size_t) (upsample->out_row_width * SIZEOF(JSAMPLE)));
	} else 
	{
		upsample->pub.upsample = merged_1v_upsample;
		upsample->upmethod = h2v1_merged_upsample;
		/* No spare row needed */
		upsample->spare_row = NULL;
	} 
	build_ycc_rgb_table(cinfo);
}

#endif /* UPSAMPLE_MERGING_SUPPORTED */




