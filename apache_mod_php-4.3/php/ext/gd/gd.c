/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Bakken <ssb@guardian.no>                               |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id: gd.c,v 1.1.1.3 2001/01/25 04:59:18 wsanchez Exp $ */

/* gd 1.2 is copyright 1994, 1995, Quest Protein Database Center, 
   Cold Spring Harbor Labs. */

/* Note that there is no code from the gd package in this file */

#include "php.h"
#include "ext/standard/head.h"
#include <math.h>
#include "SAPI.h"
#include "php_gd.h"
#include "ext/standard/fsock.h"
#include "ext/standard/info.h"

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_LIBGD

static int le_gd, le_gd_font;
#if HAVE_LIBT1
static int le_ps_font, le_ps_enc;
#endif

#include <gd.h>
#include <gdfontt.h>  /* 1 Tiny font */
#include <gdfonts.h>  /* 2 Small font */
#include <gdfontmb.h> /* 3 Medium bold font */
#include <gdfontl.h>  /* 4 Large font */
#include <gdfontg.h>  /* 5 Giant font */
#ifdef ENABLE_GD_TTF
# include "gdttf.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef ENABLE_GD_TTF
static void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int);
#endif

#ifdef GD2_VERS
/* it's >= 1.5, i.e. has IOCtx */
#define USE_GD_IOCTX 1
#else
#undef USE_GD_IOCTX
#endif

#ifndef USE_GD_IOCTX
#define gdImageCreateFromJpegCtx NULL
#define gdImageCreateFromPngCtx  NULL
#define gdImageCreateFromWBMPCtx NULL
#endif

#ifdef THREAD_SAFE
DWORD GDlibTls;
static int numthreads=0;
void *gdlib_mutex=NULL;

/*
typedef struct gdlib_global_struct{
	int le_gd;
	int le_gd_font;
#if HAVE_LIBT1
	int le_ps_font;
	int le_ps_enc;
#endif
} gdlib_global_struct;

# define GD_GLOBAL(a) gdlib_globals->a
# define GD_TLS_VARS gdlib_global_struct *gdlib_globals = TlsGetValue(GDlibTls);

#else
#  define GD_GLOBAL(a) a
#  define GD_TLS_VARS
int le_gd;
int le_gd_font;
#if HAVE_LIBT1
int le_ps_font;
int le_ps_enc;
#endif
*/
#endif

function_entry gd_functions[] = {
	PHP_FE(imagearc,								NULL)
	PHP_FE(imagechar,								NULL)
	PHP_FE(imagecharup,								NULL)
	PHP_FE(imagecolorallocate,						NULL)
	PHP_FE(imagepalettecopy,						NULL)
	PHP_FE(imagecolorat,							NULL)
	PHP_FE(imagecolorclosest,						NULL)
	PHP_FE(imagecolorclosesthwb,					NULL)
	PHP_FE(imagecolordeallocate,					NULL)
	PHP_FE(imagecolorresolve,						NULL)
	PHP_FE(imagecolorexact,							NULL)
	PHP_FE(imagecolorset,							NULL)
	PHP_FE(imagecolortransparent,					NULL)
	PHP_FE(imagecolorstotal,						NULL)
	PHP_FE(imagecolorsforindex,						NULL)
	PHP_FE(imagecopy,								NULL)
	PHP_FE(imagecopymerge,							NULL)
	PHP_FE(imagecopyresized,						NULL)
	PHP_FE(imagecreate,								NULL)
	PHP_FE(imagecreatefromstring,					NULL)
	PHP_FE(imagecreatefrompng,						NULL)
	PHP_FE(imagepng,								NULL)
	PHP_FE(imagecreatefromgif,						NULL)
	PHP_FE(imagegif,								NULL)
	PHP_FE(imagecreatefromjpeg,						NULL)
	PHP_FE(imagejpeg,								NULL)
	PHP_FE(imagecreatefromwbmp,                     NULL)
	PHP_FE(imagewbmp,                               NULL)
	PHP_FE(imagecreatefromxbm,						NULL)
	PHP_FE(imagecreatefromxpm,						NULL)
	PHP_FE(imagedestroy,							NULL)
	PHP_FE(imagegammacorrect,						NULL)
	PHP_FE(imagefill,								NULL)
	PHP_FE(imagefilledpolygon,						NULL)
	PHP_FE(imagefilledrectangle,					NULL)
	PHP_FE(imagefilltoborder,						NULL)
	PHP_FE(imagefontwidth,							NULL)
	PHP_FE(imagefontheight,							NULL)
	PHP_FE(imageinterlace,							NULL)
	PHP_FE(imageline,								NULL)
	PHP_FE(imageloadfont,							NULL)
	PHP_FE(imagepolygon,							NULL)
	PHP_FE(imagerectangle,							NULL)
	PHP_FE(imagesetpixel,							NULL)
	PHP_FE(imagestring,								NULL)
	PHP_FE(imagestringup,							NULL)
	PHP_FE(imagesx,									NULL)
	PHP_FE(imagesy,									NULL)
	PHP_FE(imagedashedline,							NULL)
	PHP_FE(imagettfbbox,							NULL)
	PHP_FE(imagettftext,							NULL)
	PHP_FE(imagepsloadfont,							NULL)
	/*
	PHP_FE(imagepscopyfont,							NULL)
	*/
	PHP_FE(imagepsfreefont,							NULL)
	PHP_FE(imagepsencodefont,						NULL)
	PHP_FE(imagepsextendfont,						NULL)
	PHP_FE(imagepsslantfont,						NULL)
	PHP_FE(imagepstext,								NULL)
	PHP_FE(imagepsbbox,								NULL)
	PHP_FE(imagetypes,								NULL)
	{NULL, NULL, NULL}
};

zend_module_entry gd_module_entry = {
	"gd", gd_functions, PHP_MINIT(gd), NULL, NULL, NULL, PHP_MINFO(gd), STANDARD_MODULE_PROPERTIES
};

#ifdef ZTS
int gd_globals_id;
#else
static php_gd_globals gd_globals;
#endif

#ifdef COMPILE_DL_GD
ZEND_GET_MODULE(gd)
#endif


static void php_free_gd_image(zend_rsrc_list_entry *rsrc)
{
	gdImageDestroy((gdImagePtr)rsrc->ptr);
}


static void php_free_gd_font(zend_rsrc_list_entry *rsrc)
{
	gdFontPtr fp = (gdFontPtr)rsrc->ptr;
	if (fp->data) {
		efree(fp->data);
	}
	efree(fp);
}


PHP_MINIT_FUNCTION(gd)
{
	GDLS_FETCH();

#if defined(THREAD_SAFE)
	gdlib_global_struct *gdlib_globals;
	PHP_MUTEX_ALLOC(gdlib_mutex);
	PHP_MUTEX_LOCK(gdlib_mutex);
	numthreads++;
	if (numthreads==1){
		if (!PHP3_TLS_PROC_STARTUP(GDlibTls)){
			PHP_MUTEX_UNLOCK(gdlib_mutex);
			PHP_MUTEX_FREE(gdlib_mutex);
			return FAILURE;
		}
	}
	PHP_MUTEX_UNLOCK(gdlib_mutex);
	if(!PHP3_TLS_THREAD_INIT(GDlibTls,gdlib_globals,gdlib_global_struct)){
		PHP_MUTEX_FREE(gdlib_mutex);
		return FAILURE;
	}
#endif
	le_gd = zend_register_list_destructors_ex(php_free_gd_image, NULL, "gd", module_number);
	le_gd_font = zend_register_list_destructors_ex(php_free_gd_font, NULL, "gd font", module_number);
#if HAVE_LIBT1
	T1_SetBitmapPad(8);
	T1_InitLib(NO_LOGFILE|IGNORE_CONFIGFILE|IGNORE_FONTDATABASE);
	T1_SetLogLevel(T1LOG_DEBUG);
	le_ps_font = zend_register_list_destructors_ex(php_free_ps_font, NULL, "gd PS font", module_number);
	le_ps_enc = zend_register_list_destructors_ex(php_free_ps_enc, NULL, "gd PS encoding", module_number);
#endif
	REGISTER_LONG_CONSTANT("IMG_GIF", 1, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_JPG", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_JPEG", 2, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_PNG", 4, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IMG_WBMP", 8, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

PHP_MINFO_FUNCTION(gd)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "GD Support", "enabled");

	/* need to use a PHPAPI function here because it is external module in windows */

#if HAVE_GDIMAGECOLORRESOLVE
	php_info_print_table_row(2, "GD Version", "1.6.2 or higher");
#elif HAVE_LIBGD13
	php_info_print_table_row(2, "GD Version", "between 1.3 and 1.6.1");
#else
	php_info_print_table_row(2, "GD Version", "1.2");
#endif

#ifdef ENABLE_GD_TTF
	php_info_print_table_row(2, "FreeType Support", "enabled");
#if HAVE_LIBFREETYPE
	php_info_print_table_row(2, "FreeType Linkage", "with freetype");
#elif HAVE_LIBTTF
	php_info_print_table_row(2, "FreeType Linkage", "with TTF library");
#else
	php_info_print_table_row(2, "FreeType Linkage", "with unknown library");
#endif
#endif

#ifdef HAVE_LIBT1
	php_info_print_table_row(2, "T1Lib Support", "enabled");
#endif         

/* this next part is stupid ... if I knew better, I'd put them all on one row (cmv) */

#ifdef HAVE_GD_GIF
	php_info_print_table_row(2, "GIF Support", "enabled");
#endif
#ifdef HAVE_GD_JPG
	php_info_print_table_row(2, "JPG Support", "enabled");
#endif
#ifdef HAVE_GD_PNG
	php_info_print_table_row(2, "PNG Support", "enabled");
#endif
#ifdef HAVE_GD_WBMP
	php_info_print_table_row(2, "WBMP Support", "enabled");
#endif
#ifdef HAVE_GD_XBM
	php_info_print_table_row(2, "XBM Support", "enabled");
#endif
	php_info_print_table_end();
}

/* Need this for cpdf. See also comment in file.c php3i_get_le_fp() */
PHPAPI int phpi_get_le_gd(void)
{
	GDLS_FETCH();

	return le_gd;
}

#ifndef HAVE_GDIMAGECOLORRESOLVE

/********************************************************************/
/* gdImageColorResolve is a replacement for the old fragment:       */
/*                                                                  */
/*      if ((color=gdImageColorExact(im,R,G,B)) < 0)                */
/*        if ((color=gdImageColorAllocate(im,R,G,B)) < 0)           */
/*          color=gdImageColorClosest(im,R,G,B);                    */
/*                                                                  */
/* in a single function                                             */

int
gdImageColorResolve(gdImagePtr im, int r, int g, int b)
{
	int c;
	int ct = -1;
	int op = -1;
	long rd, gd, bd, dist;
	long mindist = 3*255*255;  /* init to max poss dist */

	for (c = 0; c < im->colorsTotal; c++) {
		if (im->open[c]) {
			op = c;             /* Save open slot */
			continue;           /* Color not in use */
		}
		rd = (long)(im->red  [c] - r);
		gd = (long)(im->green[c] - g);
		bd = (long)(im->blue [c] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if (dist < mindist) {
			if (dist == 0) {
				return c;       /* Return exact match color */
			}
			mindist = dist;
			ct = c;
		}
	}
	/* no exact match.  We now know closest, but first try to allocate exact */
	if (op == -1) {
		op = im->colorsTotal;
		if (op == gdMaxColors) {    /* No room for more colors */
			return ct;          /* Return closest available color */
		}
		im->colorsTotal++;
	}
	im->red  [op] = r;
	im->green[op] = g;
	im->blue [op] = b;
	im->open [op] = 0;
	return op;                  /* Return newly allocated color */
}

#endif

/* {{{ proto int imageloadfont(string filename)
   Load a new font */
PHP_FUNCTION(imageloadfont) 
{
	zval **file;
	int hdr_size = sizeof(gdFont) - sizeof(char *);
	int ind, body_size, n=0, b;
	gdFontPtr font;
	FILE *fp;
	int issock=0, socketd=0;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

#ifdef PHP_WIN32
	fp = V_FOPEN(Z_STRVAL_PP(file), "rb");
#else
	fp = php_fopen_wrapper(Z_STRVAL_PP(file), "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd, NULL);
#endif
	if (fp == NULL) {
		php_error(E_WARNING, "ImageFontLoad: unable to open file");
		RETURN_FALSE;
	}

	/* Only supports a architecture-dependent binary dump format
	 * at the moment.
	 * The file format is like this on machines with 32-byte integers:
	 *
	 * byte 0-3:   (int) number of characters in the font
	 * byte 4-7:   (int) value of first character in the font (often 32, space)
	 * byte 8-11:  (int) pixel width of each character
	 * byte 12-15: (int) pixel height of each character
	 * bytes 16-:  (char) array with character data, one byte per pixel
	 *                    in each character, for a total of 
	 *                    (nchars*width*height) bytes.
	 */
	font = (gdFontPtr)emalloc(sizeof(gdFont));
	b = 0;
	while (b < hdr_size && (n = fread(&font[b], 1, hdr_size - b, fp)))
		b += n;
	if (!n) {
		fclose(fp);
		efree(font);
		if (feof(fp)) {
			php_error(E_WARNING, "ImageFontLoad: end of file while reading header");
		} else {
			php_error(E_WARNING, "ImageFontLoad: error while reading header");
		}
		RETURN_FALSE;
	}
	body_size = font->w * font->h * font->nchars;
	font->data = emalloc(body_size);
	b = 0;
	while (b < body_size && (n = fread(&font->data[b], 1, body_size - b, fp)))
		b += n;
	if (!n) {
		fclose(fp);
		efree(font->data);
		efree(font);
		if (feof(fp)) {
			php_error(E_WARNING, "ImageFontLoad: end of file while reading body");
		} else {
			php_error(E_WARNING, "ImageFontLoad: error while reading body");
		}
		RETURN_FALSE;
	}
	fclose(fp);

	/* Adding 5 to the font index so we will never have font indices
	 * that overlap with the old fonts (with indices 1-5).  The first
	 * list index given out is always 1.
	 */
	ind = 5 + zend_list_insert(font, le_gd_font);

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imagecreate(int x_size, int y_size)
   Create a new image */
PHP_FUNCTION(imagecreate)
{
	zval **x_size, **y_size;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &x_size, &y_size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(x_size);
	convert_to_long_ex(y_size);

	im = gdImageCreate(Z_LVAL_PP(x_size), Z_LVAL_PP(y_size));

	ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
}
/* }}} */

/* {{{ proto int imagetypes(void)
   Return the types of images supported in a bitfield - 1=gif, 2=jpeg, 4=png, 8=wbmp */
PHP_FUNCTION(imagetypes)
{
	int ret=0;	
#ifdef HAVE_GD_GIF
	ret = 1;
#endif
#ifdef HAVE_GD_JPG
	ret |= 2;
#endif
#ifdef HAVE_GD_PNG
	ret |= 4;
#endif
#ifdef HAVE_GD_WBMP
	ret |= 8;
#endif
	RETURN_LONG(ret);
}

static int _php_image_type (char data[8])
{
#ifdef HAVE_LIBGD15
	/* Based on ext/standard/images.c */

	if (data == NULL)
		return -1;

	if (!memcmp(data, php_sig_jpg, 3))
		return PHP_GDIMG_TYPE_JPG;
	else if (!memcmp(data, php_sig_png, 3)) {
		if (!memcmp(data, php_sig_png, 8))
			return PHP_GDIMG_TYPE_PNG;
	}
	else if (!memcmp(data, php_sig_gif, 3))
		return PHP_GDIMG_TYPE_GIF;
#ifdef HAVE_GD_WMBP
	else {
		gdIOCtx *io_ctx;
		io_ctx = gdNewDynamicCtx (8, data);
		if (io_ctx) {
			if (getmbi(gdGetC, io_ctx) == 0 && skipheader(gdGetC, io_ctx) == 0 ) {
				io_ctx->free(io_ctx);
				return PHP_GDIMG_TYPE_WBM;
			} else
				io_ctx->free(io_ctx);
		}
	}
#endif
	return -1;
#endif
}

gdImagePtr _php_image_create_from_string (zval **data, char *tn, gdImagePtr (*ioctx_func_p)())
{
#ifdef HAVE_LIBGD15 
	gdImagePtr im;
	gdIOCtx *io_ctx;
        
	io_ctx = gdNewDynamicCtx (Z_STRLEN_PP(data),Z_STRVAL_PP(data));
        
	if(!io_ctx) {
		return NULL;
	}
        
	im = (*ioctx_func_p)(io_ctx);
	if (!im) {
		php_error(E_WARNING, "%s: Passed data is not in '%s' format", get_active_function_name(), tn);
		return NULL;
	}

	return im;
#else
	return NULL;
#endif
}

/* {{{ proto int imagecreatefromstring(string image)
   Create a new image from the image stream in the string */
PHP_FUNCTION (imagecreatefromstring)
{
#ifdef HAVE_LIBGD15
	zval **data;
	gdImagePtr im;
	int imtype;
	char sig[8];
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &data) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(data);
	memcpy(sig, Z_STRVAL_PP(data), 8);

	imtype = _php_image_type (sig);

	switch (imtype) {
		case PHP_GDIMG_TYPE_JPG:
#ifdef HAVE_GD_JPG
			im = _php_image_create_from_string (data, "JPEG", gdImageCreateFromJpegCtx);
#else
			php_error(E_WARNING, "No JPEG support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_PNG:
#ifdef HAVE_GD_PNG
			im = _php_image_create_from_string (data, "PNG", gdImageCreateFromPngCtx);
#else
			php_error(E_WARNING, "No PNG support in this PHP build");
			RETURN_FALSE;
#endif
			break;
			
		case PHP_GDIMG_TYPE_GIF:
#ifdef HAVE_GD_GIF
			im = _php_image_create_from_string (data, "GIF", gdImageCreateFromGifCtx);
#else
			php_error(E_WARNING, "No GIF support in this PHP build");
			RETURN_FALSE;
#endif
			break;

		case PHP_GDIMG_TYPE_WBM:
#ifdef HAVE_GD_WBMP
			im = _php_image_create_from_string (data, "WBMP",gdImageCreateFromWBMPCtx );
#else
			php_error(E_WARNING, "No WBMP support in this PHP build");
			RETURN_FALSE;
#endif
			 break;

		default:
			php_error(E_WARNING, "Data is not in a recognized format.");
            RETURN_FALSE;

	}
	if (!im) {
		php_error(E_WARNING, "Couldn't create GD Image Stream out of Data");
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
#else
	php_error(E_ERROR, "ImageCreateFromString: Only available with GD 1.5+");
#endif
}
/* }}} */

static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(), gdImagePtr (*ioctx_func_p)()) 
{
	zval **file;
	gdImagePtr im;
	char *fn=NULL;
	FILE *fp;
	int issock=0, socketd=0;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

	fn = Z_STRVAL_PP(file);

#ifdef PHP_WIN32
	fp = V_FOPEN(fn, "rb");
#else
	fp = php_fopen_wrapper(fn, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd, NULL);
#endif
	if (!fp && !socketd) {
		php_strip_url_passwd(fn);
		php_error(E_WARNING, "%s: Unable to open '%s' for reading", get_active_function_name(), fn);
		RETURN_FALSE;
	}

#ifndef USE_GD_IOCTX
	ioctx_func_p = NULL; /* don't allow sockets without IOCtx */
#endif

	if(issock && !ioctx_func_p) {
		php_error(E_WARNING, "%s: Sockets are not supported for image type '%s'",get_active_function_name(),tn);
		RETURN_FALSE;
	}

	if(issock && socketd) {
#ifdef USE_GD_IOCTX
		gdIOCtx* io_ctx;
#define  CHUNK_SIZE 8192
		int read_len=0,buff_len=0,buff_size=5*CHUNK_SIZE;
		char *buff,*buff_cur;

		buff = malloc(buff_size); /* Should be malloc! GD uses free */
		buff_cur = buff;

		do {
			if(buff_len > buff_size - CHUNK_SIZE) {
				buff_size += CHUNK_SIZE;
				buff = realloc(buff, buff_size);
			}
			read_len = SOCK_FREAD(buff_cur, CHUNK_SIZE, socketd);
			buff_len += read_len;
			buff_cur += read_len;
		} while(read_len>0);

		io_ctx = gdNewDynamicCtx(buff_len,buff);
		if(!io_ctx) {
			php_error(E_WARNING,"%s: Cannot allocate GD IO context",get_active_function_name());
		}
		im = (*ioctx_func_p)(io_ctx);
		io_ctx->free(io_ctx);
#endif
	} else {
		im = (*func_p)(fp);

		fflush(fp);
		fclose(fp);
	}

	if (!im) {
		php_error(E_WARNING,"%s: '%s' is not a valid %s file", get_active_function_name(), fn, tn);
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, im, le_gd);
}

/* {{{ proto int imagecreatefromgif(string filename)
   Create a new image from GIF file or URL */
PHP_FUNCTION(imagecreatefromgif)
{
#ifdef HAVE_GD_GIF
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF", gdImageCreateFromGif,NULL);
#else /* HAVE_GD_GIF */
	php_error(E_WARNING, "ImageCreateFromGif: No GIF support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_GIF */
}
/* }}} */

/* {{{ proto int imagecreatefromjpeg(string filename)
   Create a new image from JPEG file or URL */
PHP_FUNCTION(imagecreatefromjpeg)
{
#ifdef HAVE_GD_JPG
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageCreateFromJpeg,gdImageCreateFromJpegCtx);
#else /* HAVE_GD_JPG */
	php_error(E_WARNING, "ImageCreateFromJpeg: No JPEG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_JPG */
}
/* }}} */

/* {{{ proto int imagecreatefrompng(string filename)
   Create a new image from PNG file or URL */
PHP_FUNCTION(imagecreatefrompng)
{
#ifdef HAVE_GD_PNG
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImageCreateFromPng,gdImageCreateFromPngCtx);
#else /* HAVE_GD_PNG */
	php_error(E_WARNING, "ImageCreateFromPng: No PNG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_PNG */
}
/* }}} */

/* {{{ proto int imagecreatefromxbm(string filename)
   Create a new image from XBM file or URL */
PHP_FUNCTION(imagecreatefromxbm)
{
#ifdef HAVE_GD_XBM
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XBM, "XBM", gdImageCreateFromXbm,NULL);
#else /* HAVE_GD_XBM */
	php_error(E_WARNING, "ImageCreateFromXbm: No XBM support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_XBM */
}
/* }}} */

/* {{{ todo int imagecreatefromxpm(string filename)
   Create a new image from XPM file or URL */
PHP_FUNCTION(imagecreatefromxpm)
{
	/*
#ifdef HAVE_GD_XPM
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XPM, "XPM", gdImageCreateFromXpm,NULL);
#else
	*/
	php_error(E_WARNING, "ImageCreateFromXpm: No XPM support in this PHP build");
	RETURN_FALSE;
	/*
#endif
	*/
}
/* }}} */

/* {{{ proto int imagecreatefromwbmp(string filename)
   Create a new image from WBMP file or URL */
PHP_FUNCTION(imagecreatefromwbmp)
{
#ifdef HAVE_GD_WBMP
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WBM, "WBMP", gdImageCreateFromWBMP,gdImageCreateFromWBMPCtx);
#else /* HAVE_GD_WBMP */
	php_error(E_WARNING, "ImageCreateFromWBMP: No WBMP support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_WBMP */
}
/* }}} */

static void _php_image_output(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, void (*func_p)()) 
{
	zval **imgind, **file, **quality;
	gdImagePtr im;
	char *fn = NULL;
	FILE *fp;
	int argc = ZEND_NUM_ARGS();
	int output = 1, q = -1;
	GDLS_FETCH();

	if (argc < 1 || argc > 3 || zend_get_parameters_ex(argc, &imgind, &file, &quality) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, imgind, -1, "Image", le_gd);

	if (argc > 1) {
		convert_to_string_ex(file);
		fn = Z_STRVAL_PP(file);
		if (argc == 3) {
			convert_to_long_ex(quality);
			q = Z_LVAL_PP(quality);
		}
	}

	if ((argc == 2) || (argc == 3 && Z_STRLEN_PP(file))) {
		if (!fn || fn == empty_string || php_check_open_basedir(fn)) {
			php_error(E_WARNING, "%s: invalid filename '%s'", get_active_function_name(), fn);
			RETURN_FALSE;
		}

		fp = V_FOPEN(fn, "wb");
		if (!fp) {
			php_error(E_WARNING, "%s: unable to open '%s' for writing", get_active_function_name(), fn);
			RETURN_FALSE;
		}
		if (image_type == PHP_GDIMG_TYPE_JPG) {
			(*func_p)(im, fp, q);
		} else {
			(*func_p)(im, fp);
		}
		fflush(fp);
		fclose(fp);
	} else {
		int   b;
		FILE *tmp;
		char  buf[4096];

		tmp = tmpfile();
		if (tmp == NULL) {
			php_error(E_WARNING, "%s: unable to open temporary file", get_active_function_name());
			RETURN_FALSE;
		}
		output = php_header();
		if (output) {
			if (image_type == PHP_GDIMG_TYPE_JPG) {
				(*func_p)(im, tmp, q);
			} else {
				(*func_p)(im, tmp);
			}
            fseek(tmp, 0, SEEK_SET);
#if APACHE && defined(CHARSET_EBCDIC)
			SLS_FETCH();
            /* This is a binary file already: avoid EBCDIC->ASCII conversion */
            ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
            while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
                php_write(buf, b);
            }
        }
        fclose(tmp);
        /* the temporary file is automatically deleted */
	}
    RETURN_TRUE;
}

/* {{{ proto int imagegif(int im [, string filename])
   Output GIF image to browser or file */
PHP_FUNCTION(imagegif)
{
#ifdef HAVE_GD_GIF
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GIF, "GIF", gdImageGif);
#else /* HAVE_GD_GIF */
	php_error(E_WARNING, "ImageGif: No GIF support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_GIF */
}
/* }}} */

/* {{{ proto int imagepng(int im [, string filename])
   Output PNG image to browser or file */
PHP_FUNCTION(imagepng)
{
#ifdef HAVE_GD_PNG
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImagePng);
#else /* HAVE_GD_PNG */
	php_error(E_WARNING, "ImagePng: No PNG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_PNG */
}
/* }}} */


/* {{{ proto int imagejpeg(int im [, string filename [, int quality]])
   Output JPEG image to browser or file */
PHP_FUNCTION(imagejpeg)
{
#ifdef HAVE_GD_JPG
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageJpeg);
#else /* HAVE_GD_JPG */
	php_error(E_WARNING, "ImageJpeg: No JPG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_JPG */
}
/* }}} */

/* {{{ proto int imagewbmp(int im [, string filename])
   Output WBMP image to browser or file */
PHP_FUNCTION(imagewbmp)
{
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WBM, "WBMP", _php_image_output_wbmp);
}
/* }}} */

/* {{{ _php_image_output_wbmp(gdImagePtr im, FILE *fp)
 */
static void _php_image_output_wbmp(gdImagePtr im, FILE *fp)
{
	int x, y;
	int c, p, width, height;

	/* WBMP header, black and white, no compression */
	fputc(0,fp); fputc(0,fp);
		
	/* Width and height of image */
	c = 1; width = im->sx;
	while(width & 0x7f << 7*c) c++;
	while(c > 1) fputc(0x80 | ((width >> 7*--c) & 0xff), fp);
	fputc(width & 0x7f,fp);
	c = 1; height = im->sy;
	while(height & 0x7f << 7*c) c++;
	while(c > 1) fputc(0x80 | ((height >> 7*--c) & 0xff), fp);
	fputc(height & 0x7f,fp);
		
	/* Actual image data */
	for(y = 0; y < im->sy; y++) {
		p = c = 0;
		for(x = 0; x < im->sx; x++) {
#if HAVE_GD_ANCIENT
			if(im->pixels[x][y] == 0) c = c | (1 << (7-p));
#else
			if(im->pixels[y][x] == 0) c = c | (1 << (7-p));
#endif
			if(++p == 8) {
				fputc(c,fp);
				p = c = 0;
			}
		}
		if(p) fputc(c,fp);
	}
}
/* }}} */

/* {{{ proto int imagedestroy(int im)
   Destroy an image */
PHP_FUNCTION(imagedestroy)
{
	zval **IM;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	zend_list_delete(Z_LVAL_PP(IM));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecolorallocate(int im, int red, int green, int blue)
   Allocate a color for an image */
PHP_FUNCTION(imagecolorallocate)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorAllocate(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

/* {{{ proto int imagepalettecopy(int dst, int src)
   Copy the palette from the src image onto the dst image */
PHP_FUNCTION(imagepalettecopy)
{
#if HAVE_LIBGD15
	zval **dstim, **srcim;
	gdImagePtr dst, src;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &dstim, &srcim) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(dst, gdImagePtr, dstim, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(src, gdImagePtr, srcim, -1, "Image", le_gd);

	gdImagePaletteCopy(dst, src);
#else
	php_error(E_WARNING, "Sorry ImagePaletteCopy was introduced in GD version 1.5");
	RETURN_FALSE;
#endif
}
/* }}} */

/* im, x, y */
/* {{{ proto int imagecolorat(int im, int x, int y)
   Get the index of the color of a pixel */
PHP_FUNCTION(imagecolorat)
{
	zval **IM, **x, **y;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &IM, &x, &y) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);

	if (gdImageBoundsSafe(im, Z_LVAL_PP(x), Z_LVAL_PP(y))) {
#if HAVE_LIBGD13
		RETURN_LONG(im->pixels[Z_LVAL_PP(y)][Z_LVAL_PP(x)]);
#else
		RETURN_LONG(im->pixels[Z_LVAL_PP(x)][Z_LVAL_PP(y)]);
#endif
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagecolorclosest(int im, int red, int green, int blue)
   Get the index of the closest color to the specified color */
PHP_FUNCTION(imagecolorclosest)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorClosest(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

/* {{{ proto int imagecolorclosesthwb(int im, int red, int green, int blue)
   Get the index of the color which has the hue, white and blackness nearest to the given color */
PHP_FUNCTION(imagecolorclosesthwb)
{
#if HAVE_COLORCLOSESTHWB
	zval **IM, **red, **green, **blue;
	gdImagePtr im;
	GDLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	
	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);

	RETURN_LONG(gdImageColorClosestHWB(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
#else
	php_error(E_WARNING, "ImageColorClosestHWB functionality was added in GD 1.8");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto int imagecolordeallocate(int im, int index)
   De-allocate a color for an image */
PHP_FUNCTION(imagecolordeallocate)
{
	zval **IM, **index;
	int col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM, &index) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(index);
	col = Z_LVAL_PP(index);

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		gdImageColorDeallocate(im, col);
		RETURN_TRUE;
	} else {
		php_error(E_WARNING, "Color index out of range");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagecolorresolve(int im, int red, int green, int blue)
   Get the index of the specified color or its closest possible alternative */
PHP_FUNCTION(imagecolorresolve)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorResolve(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

/* {{{ proto int imagecolorexact(int im, int red, int green, int blue)
   Get the index of the specified color */
PHP_FUNCTION(imagecolorexact)
{
	zval **IM, **red, **green, **blue;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &IM, &red, &green, &blue) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	RETURN_LONG(gdImageColorExact(im, Z_LVAL_PP(red), Z_LVAL_PP(green), Z_LVAL_PP(blue)));
}
/* }}} */

/* {{{ proto int imagecolorset(int im, int col, int red, int green, int blue)
   Set the color for the specified palette index */
PHP_FUNCTION(imagecolorset)
{
	zval **IM, **color, **red, **green, **blue;
	int col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &IM, &color, &red, &green, &blue) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(color);
	convert_to_long_ex(red);
	convert_to_long_ex(green);
	convert_to_long_ex(blue);
	
	col = Z_LVAL_PP(color);
	
	if (col >= 0 && col < gdImageColorsTotal(im)) {
		im->red[col]   = Z_LVAL_PP(red);
		im->green[col] = Z_LVAL_PP(green);
		im->blue[col]  = Z_LVAL_PP(blue);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imagecolorsforindex(int im, int col)
   Get the colors for an index */
PHP_FUNCTION(imagecolorsforindex)
{
	zval **IM, **index;
	int col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &IM, &index) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(index);
	col = Z_LVAL_PP(index);
	
	if (col >= 0 && col < gdImageColorsTotal(im)) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value,"red",  im->red[col]);
		add_assoc_long(return_value,"green",im->green[col]);
		add_assoc_long(return_value,"blue", im->blue[col]);
	} else {
		php_error(E_WARNING, "Color index out of range");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagegammacorrect(int im, double inputgamma, double outputgamma)
   Apply a gamma correction to a GD image */
PHP_FUNCTION(imagegammacorrect)
{
	zval **IM, **inputgamma, **outputgamma;
	gdImagePtr im;
	int i;
	double input, output;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &IM, &inputgamma, &outputgamma) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(inputgamma);
	convert_to_double_ex(outputgamma);

	input = Z_DVAL_PP(inputgamma);
	output = Z_DVAL_PP(outputgamma);

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	for (i = 0; i < gdImageColorsTotal(im); i++) {
		im->red[i]   = (int)((pow((pow((im->red[i]   / 255.0), input)), 1.0 / output) * 255)+.5);
		im->green[i] = (int)((pow((pow((im->green[i] / 255.0), input)), 1.0 / output) * 255)+.5);
		im->blue[i]  = (int)((pow((pow((im->blue[i]  / 255.0), input)), 1.0 / output) * 255)+.5);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagesetpixel(int im, int x, int y, int col)
   Set a single pixel */
PHP_FUNCTION(imagesetpixel)
{
	zval **IM, **x, **y, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &x, &y, &col) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(col);

	gdImageSetPixel(im,Z_LVAL_PP(x),Z_LVAL_PP(y),Z_LVAL_PP(col));

	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imageline(int im, int x1, int y1, int x2, int y2, int col)
   Draw a line */
PHP_FUNCTION(imageline)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageLine(im,Z_LVAL_PP(x1),Z_LVAL_PP(y1),Z_LVAL_PP(x2),Z_LVAL_PP(y2),Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagedashedline(int im, int x1, int y1, int x2, int y2, int col)
   Draw a dashed line */
PHP_FUNCTION(imagedashedline)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageDashedLine(im,Z_LVAL_PP(x1),Z_LVAL_PP(y1),Z_LVAL_PP(x2),Z_LVAL_PP(y2),Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */

/* im, x1, y1, x2, y2, col */
/* {{{ proto int imagerectangle(int im, int x1, int y1, int x2, int y2, int col)
   Draw a rectangle */
PHP_FUNCTION(imagerectangle)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageRectangle(im,Z_LVAL_PP(x1),Z_LVAL_PP(y1),Z_LVAL_PP(x2),Z_LVAL_PP(y2),Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* im, x1, y1, x2, y2, col */
/* {{{ proto int imagefilledrectangle(int im, int x1, int y1, int x2, int y2, int col)
   Draw a filled rectangle */
PHP_FUNCTION(imagefilledrectangle)
{
	zval **IM, **x1, **y1, **x2, **y2, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &x1, &y1, &x2, &y2, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x1);
	convert_to_long_ex(y1);
	convert_to_long_ex(x2);
	convert_to_long_ex(y2);
	convert_to_long_ex(col);

	gdImageFilledRectangle(im,Z_LVAL_PP(x1),Z_LVAL_PP(y1),Z_LVAL_PP(x2),Z_LVAL_PP(y2),Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagearc(int im, int cx, int cy, int w, int h, int s, int e, int col)
   Draw a partial ellipse */
PHP_FUNCTION(imagearc)
{
	zval **IM, **cx, **cy, **w, **h, **ST, **E, **col;
	gdImagePtr im;
	int e,st;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 8 ||	zend_get_parameters_ex(8, &IM, &cx, &cy, &w, &h, &ST, &E, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(cx);
	convert_to_long_ex(cy);
	convert_to_long_ex(w);
	convert_to_long_ex(h);
	convert_to_long_ex(ST);
	convert_to_long_ex(E);
	convert_to_long_ex(col);

	e = Z_LVAL_PP(E);
	st = Z_LVAL_PP(ST);

	if (e < 0)   e %= 360;
	if (st < 0) st %= 360;

	gdImageArc(im,Z_LVAL_PP(cx),Z_LVAL_PP(cy),Z_LVAL_PP(w),Z_LVAL_PP(h),st,e,Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* im, x, y, border, col */
/* {{{ proto int imagefilltoborder(int im, int x, int y, int border, int col)
   Flood fill to specific color */
PHP_FUNCTION(imagefilltoborder)
{
	zval **IM, **x, **y, **border, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 5 ||	zend_get_parameters_ex(5, &IM, &x, &y, &border, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(border);
	convert_to_long_ex(col);

	gdImageFillToBorder(im,Z_LVAL_PP(x),Z_LVAL_PP(y),Z_LVAL_PP(border),Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* im, x, y, col */
/* {{{ proto int imagefill(int im, int x, int y, int col)
   Flood fill */
PHP_FUNCTION(imagefill)
{
	zval **IM, **x, **y, **col;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &x, &y, &col) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(x);
	convert_to_long_ex(y);
	convert_to_long_ex(col);

	gdImageFill(im,Z_LVAL_PP(x),Z_LVAL_PP(y),Z_LVAL_PP(col));
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagecolorstotal(int im)
   Find out the number of colors in an image's palette */
PHP_FUNCTION(imagecolorstotal)
{
	zval **IM;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	RETURN_LONG(gdImageColorsTotal(im));
}
/* }}} */	

/* im, col */
/* {{{ proto int imagecolortransparent(int im [, int col])
   Define a color as transparent */
PHP_FUNCTION(imagecolortransparent)
{
	zval **IM, **COL;
	gdImagePtr im;
	GDLS_FETCH();

	switch(ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &IM) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &IM, &COL) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(COL);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	if (ZEND_NUM_ARGS() > 1) {
		gdImageColorTransparent(im, Z_LVAL_PP(COL));
	}

	RETURN_LONG(gdImageGetTransparent(im));
}
/* }}} */	

/* im, interlace */
/* {{{ proto int imageinterlace(int im [, int interlace])
   Enable or disable interlace */
PHP_FUNCTION(imageinterlace)
{
	zval **IM, **INT;
	gdImagePtr im;
	GDLS_FETCH();

	switch(ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &IM) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &IM, &INT) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(INT);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	if (ZEND_NUM_ARGS() > 1) {
		gdImageInterlace(im,Z_LVAL_PP(INT));
	}

	RETURN_LONG(gdImageGetInterlaced(im));
}
/* }}} */	

/* arg = 0  normal polygon
   arg = 1  filled polygon */
/* im, points, num_points, col */
static void php_imagepolygon(INTERNAL_FUNCTION_PARAMETERS, int filled) {
	zval **IM, **POINTS, **NPOINTS, **COL;
	pval **var = NULL;
	gdImagePtr im;
	gdPointPtr points;	
	int npoints, col, nelem, i;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 4 ||	zend_get_parameters_ex(4, &IM, &POINTS, &NPOINTS, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(NPOINTS);
	convert_to_long_ex(COL);

	npoints = Z_LVAL_PP(NPOINTS);
	col = Z_LVAL_PP(COL);

	if ((*POINTS)->type != IS_ARRAY) {
		php_error(E_WARNING, "2nd argument to imagepolygon not an array");
		RETURN_FALSE;
	}

	nelem = zend_hash_num_elements((*POINTS)->value.ht);
	if (nelem < 6) {
		php_error(E_WARNING, "you must have at least 3 points in your array");
		RETURN_FALSE;
	}

	if (nelem < npoints * 2) {
		php_error(E_WARNING, "trying to use %d points in array with only %d points", npoints, nelem/2);
		RETURN_FALSE;
	}

	points = (gdPointPtr) emalloc(npoints * sizeof(gdPoint));

	for (i = 0; i < npoints; i++) {
		if (zend_hash_index_find((*POINTS)->value.ht, (i * 2), (void **) &var) == SUCCESS) {
			SEPARATE_ZVAL((var));
			convert_to_long(*var);
			points[i].x = Z_LVAL_PP(var);
		}
		if (zend_hash_index_find((*POINTS)->value.ht, (i * 2) + 1, (void **) &var) == SUCCESS) {
			SEPARATE_ZVAL(var);
			convert_to_long(*var);
			points[i].y = Z_LVAL_PP(var);
		}
	}

	if (filled) {
		gdImageFilledPolygon(im, points, npoints, col);
	} else {
		gdImagePolygon(im, points, npoints, col);
	}

	efree(points);
	RETURN_TRUE;
}


/* {{{ proto int imagepolygon(int im, array point, int num_points, int col)
   Draw a polygon */
PHP_FUNCTION(imagepolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefilledpolygon(int im, array point, int num_points, int col)
   Draw a filled polygon */
PHP_FUNCTION(imagefilledpolygon)
{
	php_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


static gdFontPtr php_find_gd_font(int size)
{
	gdFontPtr font;
	int ind_type;
	GDLS_FETCH();

	switch (size) {
    	case 1:
			 font = gdFontTiny;
			 break;
    	case 2:
			 font = gdFontSmall;
			 break;
    	case 3:
			 font = gdFontMediumBold;
			 break;
    	case 4:
			 font = gdFontLarge;
			 break;
    	case 5:
			 font = gdFontGiant;
			 break;
	    default:
			font = zend_list_find(size - 5, &ind_type);
			 if (!font || ind_type != le_gd_font) {
				  if (size < 1) {
					   font = gdFontTiny;
				  } else {
					   font = gdFontGiant;
				  }
			 }
			 break;
	}

	return font;
}


/*
 * arg = 0  ImageFontWidth
 * arg = 1  ImageFontHeight
 */
static void php_imagefontsize(INTERNAL_FUNCTION_PARAMETERS, int arg)
{
	zval **SIZE;
	gdFontPtr font;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &SIZE) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long_ex(SIZE);

	font = php_find_gd_font(Z_LVAL_PP(SIZE));
	RETURN_LONG(arg ? font->h : font->w);
}


/* {{{ proto int imagefontwidth(int font)
   Get font width */
PHP_FUNCTION(imagefontwidth)
{
	php_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefontheight(int font)
   Get font height */
PHP_FUNCTION(imagefontheight)
{
	php_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* workaround for a bug in gd 1.2 */
void php_gdimagecharup(gdImagePtr im, gdFontPtr f, int x, int y, int c,
						 int color)
{
	int cx, cy, px, py, fline;
	cx = 0;
	cy = 0;
	if ((c < f->offset) || (c >= (f->offset + f->nchars))) {
		return;
	}
	fline = (c - f->offset) * f->h * f->w;
	for (py = y; (py > (y - f->w)); py--) {
		for (px = x; (px < (x + f->h)); px++) {
			if (f->data[fline + cy * f->w + cx]) {
				gdImageSetPixel(im, px, py, color);	
			}
			cy++;
		}
		cy = 0;
		cx++;
	}
}

/*
 * arg = 0  ImageChar
 * arg = 1  ImageCharUp
 * arg = 2  ImageString
 * arg = 3  ImageStringUp
 */
static void php_imagechar(INTERNAL_FUNCTION_PARAMETERS, int mode) 
{
	zval **IM, **SIZE, **X, **Y, **C, **COL;
	gdImagePtr im;
	int ch = 0, col, x, y, size, i, l = 0;
	unsigned char *str = NULL;
	gdFontPtr font;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 6 ||	zend_get_parameters_ex(6, &IM, &SIZE, &X, &Y, &C, &COL) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	convert_to_long_ex(SIZE);
	convert_to_long_ex(X);
	convert_to_long_ex(Y);
	convert_to_string_ex(C);
	convert_to_long_ex(COL);

	col = Z_LVAL_PP(COL);

	if (mode < 2) {
		ch = (int)((unsigned char)*(Z_STRVAL_PP(C)));
	} else {
		str = (unsigned char *) estrndup(Z_STRVAL_PP(C), Z_STRLEN_PP(C));
		l = strlen(str);
	}

	y = Z_LVAL_PP(Y);
	x = Z_LVAL_PP(X);
	size = Z_LVAL_PP(SIZE);

	font = php_find_gd_font(size);

	switch(mode) {
    	case 0:
			gdImageChar(im, font, x, y, ch, col);
			break;
    	case 1:
			php_gdimagecharup(im, font, x, y, ch, col);
			break;
    	case 2:
			for (i = 0; (i < l); i++) {
				gdImageChar(im, font, x, y, (int)((unsigned char)str[i]),
							col);
				x += font->w;
			}
			break;
    	case 3: {
			for (i = 0; (i < l); i++) {
				/* php_gdimagecharup(im, font, x, y, (int)str[i], col); */
				gdImageCharUp(im, font, x, y, (int)str[i], col);
				y -= font->w;
			}
			break;
		}
	}
	if (str) {
		efree(str);
	}
	RETURN_TRUE;
}	

/* {{{ proto int imagechar(int im, int font, int x, int y, string c, int col)
   Draw a character */ 
PHP_FUNCTION(imagechar) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagecharup(int im, int font, int x, int y, string c, int col)
   Draw a character rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagecharup) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int imagestring(int im, int font, int x, int y, string str, int col)
   Draw a string horizontally */
PHP_FUNCTION(imagestring) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto int imagestringup(int im, int font, int x, int y, string str, int col)
   Draw a string vertically - rotated 90 degrees counter-clockwise */
PHP_FUNCTION(imagestringup) 
{
	php_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto int imagecopy(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h)
   Copy part of an image */ 
PHP_FUNCTION(imagecopy)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 8 ||	
		zend_get_parameters_ex(8, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);

	gdImageCopy(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecopymerge(int src_im, int dst_im, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h, int pct)
   Merge one part of an image with another */
PHP_FUNCTION(imagecopymerge)
{
#if HAVE_LIBGD15
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY, **PCT;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX, pct;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 9 ||
		zend_get_parameters_ex(9, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH, &PCT) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);
	convert_to_long_ex(PCT);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);
	pct  = Z_LVAL_PP(PCT);

	gdImageCopyMerge(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH, pct);
	RETURN_TRUE;
#else
	php_error(E_WARNING, "ImageCopyMerge was introduced in GD version 1.5");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto int imagecopyresized(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int dst_w, int dst_h, int src_w, int src_h)
   Copy and resize part of an image */
PHP_FUNCTION(imagecopyresized)
{
	zval **SIM, **DIM, **SX, **SY, **SW, **SH, **DX, **DY, **DW, **DH;
	gdImagePtr im_dst, im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 10 ||
		zend_get_parameters_ex(10, &DIM, &SIM, &DX, &DY, &SX, &SY, &DW, &DH, &SW, &SH) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im_dst, gdImagePtr, DIM, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(im_src, gdImagePtr, SIM, -1, "Image", le_gd);

	convert_to_long_ex(SX);
	convert_to_long_ex(SY);
	convert_to_long_ex(SW);
	convert_to_long_ex(SH);
	convert_to_long_ex(DX);
	convert_to_long_ex(DY);
	convert_to_long_ex(DW);
	convert_to_long_ex(DH);

	srcX = Z_LVAL_PP(SX);
	srcY = Z_LVAL_PP(SY);
	srcH = Z_LVAL_PP(SH);
	srcW = Z_LVAL_PP(SW);
	dstX = Z_LVAL_PP(DX);
	dstY = Z_LVAL_PP(DY);
	dstH = Z_LVAL_PP(DH);
	dstW = Z_LVAL_PP(DW);

	gdImageCopyResized(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagesx(int im)
   Get image width */
PHP_FUNCTION(imagesx)
{
	zval **IM;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	RETURN_LONG(gdImageSX(im));
}
/* }}} */

/* {{{ proto int imagesy(int im)
   Get image height */
PHP_FUNCTION(imagesy)
{
	zval **IM;
	gdImagePtr im;
	GDLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);

	RETURN_LONG(gdImageSY(im));
}
/* }}} */

#ifdef ENABLE_GD_TTF
#define TTFTEXT_DRAW 0
#define TTFTEXT_BBOX 1
#endif

/* {{{ proto array imagettfbbox(int size, int angle, string font_file, string text)
   Give the bounding box of a text using TrueType fonts */
PHP_FUNCTION(imagettfbbox)
{
#ifdef ENABLE_GD_TTF
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX);
#else 
	php_error(E_WARNING, "ImageTtfBBox: No TTF support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ proto array imagettftext(int im, int size, int angle, int x, int y, int col, string font_file, string text)
   Write text to the image using a TrueType font */
PHP_FUNCTION(imagettftext)
{
#ifdef ENABLE_GD_TTF
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW);
#else 
	php_error(E_WARNING, "ImageTtfText: No TTF support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

#ifdef ENABLE_GD_TTF
static
void php_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval **IM, **PTSIZE, **ANGLE, **X, **Y, **C, **FONTNAME, **COL;
	gdImagePtr im=NULL;
	int col, x, y, l=0, i, brect[8];
	double ptsize, angle;
	unsigned char *str = NULL, *fontname = NULL;
	char *error;
	GDLS_FETCH();

	if (mode == TTFTEXT_BBOX) {
		if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &PTSIZE, &ANGLE, &FONTNAME, &C) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		if (ZEND_NUM_ARGS() != 8 || zend_get_parameters_ex(8, &IM, &PTSIZE, &ANGLE, &X, &Y, &COL, &FONTNAME, &C) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		ZEND_FETCH_RESOURCE(im, gdImagePtr, IM, -1, "Image", le_gd);
	}

	convert_to_double_ex(PTSIZE);
	convert_to_double_ex(ANGLE);
	convert_to_string_ex(FONTNAME);
	convert_to_string_ex(C);

	if (mode == TTFTEXT_BBOX) {
		im = NULL;
		col = x = y = -1;
	} else {
		convert_to_long_ex(X);
		convert_to_long_ex(Y);
		convert_to_long_ex(COL);
		col = Z_LVAL_PP(COL);
		y = Z_LVAL_PP(Y);
		x = Z_LVAL_PP(X);
	}

	ptsize = Z_DVAL_PP(PTSIZE);
	angle = Z_DVAL_PP(ANGLE) * (M_PI/180); /* convert to radians */

	str = (unsigned char *) Z_STRVAL_PP(C);
	l = strlen(str);
	fontname = (unsigned char *) Z_STRVAL_PP(FONTNAME);

#ifdef USE_GD_IMGSTRTTF
	error = gdImageStringTTF(im, brect, col, fontname, ptsize, angle, x, y, str);
#else
	error = gdttf(im, brect, col, fontname, ptsize, angle, x, y, str);
#endif

	if (error) {
		php_error(E_WARNING, "%s", error);
		RETURN_FALSE;
	}
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* return array with the text's bounding box */
	for (i = 0; i < 8; i++) {
		add_next_index_long(return_value, brect[i]);
	}
}
#endif	/* ENABLE_GD_TTF */

#if HAVE_LIBT1

void php_free_ps_font(zend_rsrc_list_entry *rsrc)
{
	int *font = (int *)rsrc->ptr;
	T1_DeleteFont(*font);
	efree(font);
}

void php_free_ps_enc(zend_rsrc_list_entry *rsrc)
{
	char **enc = (char **)rsrc->ptr;
	T1_DeleteEncoding(enc);
}

#endif

/* {{{ proto int imagepsloadfont(string pathname)
   Load a new font from specified file */
PHP_FUNCTION(imagepsloadfont)
{
#if HAVE_LIBT1
	zval **file;
	int f_ind, *font;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &file) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(file);

	f_ind = T1_AddFont(Z_STRVAL_PP(file));

	if (f_ind < 0) {
		switch (f_ind) {
		case -1:
			php_error(E_WARNING, "Couldn't find the font file");
			RETURN_FALSE;
			break;
		case -2:
		case -3:
			php_error(E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php_error(E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	if (T1_LoadFont(f_ind)) {
		php_error(E_WARNING, "Couldn't load the font");
		RETURN_FALSE;
	}

	font = (int *) emalloc(sizeof(int));
	*font = f_ind;
	ZEND_REGISTER_RESOURCE(return_value, font, le_ps_font);
#else 
	php_error(E_WARNING, "ImagePsLoadFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ The function in t1lib which this function uses seem to be buggy...
proto int imagepscopyfont(int font_index)
Make a copy of a font for purposes like extending or reenconding */
/*
PHP_FUNCTION(imagepscopyfont)
{
#if HAVE_LIBT1
	zval **fnt;
	int l_ind, type;
	gd_ps_font *nf_ind, *of_ind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fnt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(fnt);

	of_ind = zend_list_find(Z_LVAL_PP(fnt), &type);

	if (type != le_ps_font) {
		php_error(E_WARNING, "%d is not a Type 1 font index", Z_LVAL_PP(fnt));
		RETURN_FALSE;
	}

	nf_ind = emalloc(sizeof(gd_ps_font));
	nf_ind->font_id = T1_CopyFont(of_ind->font_id);

	if (nf_ind->font_id < 0) {
		l_ind = nf_ind->font_id;
		efree(nf_ind);
		switch (l_ind) {
		case -1:
			php_error(E_WARNING, "FontID %d is not loaded in memory", l_ind);
			RETURN_FALSE;
			break;
		case -2:
			php_error(E_WARNING, "Tried to copy a logical font");
			RETURN_FALSE;
			break;
		case -3:
			php_error(E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php_error(E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	nf_ind->extend = 1;
	l_ind = zend_list_insert(nf_ind, le_ps_font);
	RETURN_LONG(l_ind);
#else 
	php_error(E_WARNING, "ImagePsCopyFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
*/
/* }}} */

/* {{{ proto bool imagepsfreefont(int font_index)
   Free memory used by a font */
PHP_FUNCTION(imagepsfreefont)
{
#if HAVE_LIBT1
	zval **fnt;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &fnt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	zend_list_delete(Z_LVAL_PP(fnt));
	RETURN_TRUE;
#else 
	php_error(E_WARNING, "ImagePsFreeFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool imagepsencodefont(int font_index, string filename)
   To change a fonts character encoding vector */
PHP_FUNCTION(imagepsencodefont)
{
#if HAVE_LIBT1
	zval **fnt, **enc;
	char **enc_vector;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &enc) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(enc);

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	if ((enc_vector = T1_LoadEncoding(Z_STRVAL_PP(enc))) == NULL) {
		php_error(E_WARNING, "Couldn't load encoding vector from %s", Z_STRVAL_PP(enc));
		RETURN_FALSE;
	}

	T1_DeleteAllSizes(*f_ind);
	if (T1_ReencodeFont(*f_ind, enc_vector)) {
		T1_DeleteEncoding(enc_vector);
		php_error(E_WARNING, "Couldn't reencode font");
		RETURN_FALSE;
	}
	zend_list_insert(enc_vector, le_ps_enc);
	RETURN_TRUE;
#else 
	php_error(E_WARNING, "ImagePsEncodeFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ proto bool imagepsextendfont(int font_index, double extend)
   Extend or or condense (if extend < 1) a font */
PHP_FUNCTION(imagepsextendfont)
{
#if HAVE_LIBT1
	zval **fnt, **ext;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &ext) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(ext);

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	if (T1_ExtendFont(*f_ind, Z_DVAL_PP(ext)) != 0) RETURN_FALSE;

	RETURN_TRUE;
#else 
	php_error(E_WARNING, "ImagePsExtendFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ proto bool imagepsslantfont(int font_index, double slant)
   Slant a font */
PHP_FUNCTION(imagepsslantfont)
{
#if HAVE_LIBT1
	zval **fnt, **slt;
	int *f_ind;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &fnt, &slt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double_ex(slt);

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	if (T1_SlantFont(*f_ind, Z_DVAL_PP(slt)) != 0) RETURN_FALSE;
	RETURN_TRUE;
#else 
	php_error(E_WARNING, "ImagePsSlantFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ proto array imagepstext(int image, string text, int font, int size, int xcoord, int ycoord [, int space, int tightness, double angle, int antialias])
   Rasterize a string over an image */
PHP_FUNCTION(imagepstext)
{
#if HAVE_LIBT1
	zval **img, **str, **fnt, **sz, **fg, **bg, **sp, **px, **py, **aas, **wd, **ang;
	int i, j, x, y;
	int space;
	int *f_ind;
	int h_lines, v_lines, c_ind;
	int rd, gr, bl, fg_rd, fg_gr, fg_bl, bg_rd, bg_gr, bg_bl,_fg,_bg;
	int aa[16], aa_steps;
	int width, amount_kern, add_width;
	double angle, extend;
	unsigned long aa_greys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	gdImagePtr bg_img;
	GLYPH *str_img;
	T1_OUTLINE *char_path, *str_path;
	T1_TMATRIX *transform = NULL;
	char *_str;
	
	switch(ZEND_NUM_ARGS()) {
	case 8:
		if (zend_get_parameters_ex(8, &img, &str, &fnt, &sz, &fg, &bg, &px, &py) == FAILURE) {
			RETURN_FALSE;
		}
		space = 0;
		aa_steps = 4;
		width = 0;
		angle = 0;
		break;
	case 12:
		if (zend_get_parameters_ex(12, &img, &str, &fnt, &sz, &fg, &bg, &px, &py, &sp, &wd, &ang, &aas) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_long_ex(sp);
		convert_to_long_ex(aas);
		convert_to_long_ex(wd);
		convert_to_double_ex(ang);
		space = Z_LVAL_PP(sp);
		aa_steps = Z_LVAL_PP(aas);
		width = Z_LVAL_PP(wd);
		angle = Z_DVAL_PP(ang);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(bg_img, gdImagePtr, img, -1, "Image", le_gd);
	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	x = Z_LVAL_PP(px);
	y = Z_LVAL_PP(py);
	_fg = Z_LVAL_PP(fg);
	_bg = Z_LVAL_PP(bg);
	
	fg_rd = gdImageRed  (bg_img, _fg);
	fg_gr = gdImageGreen(bg_img, _fg);
	fg_bl = gdImageBlue (bg_img, _fg);

	bg_rd = gdImageRed  (bg_img, _bg);
	bg_gr = gdImageGreen(bg_img, _bg);
	bg_bl = gdImageBlue (bg_img, _bg);

	for (i = 0; i < aa_steps; i++) {
		rd = bg_rd+(double)(fg_rd-bg_rd)/aa_steps*(i+1);
		gr = bg_gr+(double)(fg_gr-bg_gr)/aa_steps*(i+1);
		bl = bg_bl+(double)(fg_bl-bg_bl)/aa_steps*(i+1);
		aa[i] = gdImageColorResolve(bg_img, rd, gr, bl);
	}

	T1_AASetBitsPerPixel(8);

	switch (aa_steps) {
	case 4:
		T1_AASetGrayValues(0, 1, 2, 3, 4);
		T1_AASetLevel(T1_AA_LOW);
		break;
	case 16:
		T1_AAHSetGrayValues(aa_greys);
		T1_AASetLevel(T1_AA_HIGH);
		break;
	default:
		php_error(E_WARNING, "Invalid value %d as number of steps for antialiasing", aa_steps);
		RETURN_FALSE;
	}

	if (angle) {
		transform = T1_RotateMatrix(NULL, angle);
	}

	_str = Z_STRVAL_PP(str);

	if (width) {
		extend = T1_GetExtend(*f_ind);
		str_path = T1_GetCharOutline(*f_ind, _str[0], Z_LVAL_PP(sz), transform);

		for (i = 1; i < Z_STRLEN_PP(str); i++) {
			amount_kern = (int) T1_GetKerning(*f_ind, _str[i-1], _str[i]);
			amount_kern += _str[i-1] == ' ' ? space : 0;
			add_width = (int) (amount_kern+width)/extend;

			char_path = T1_GetMoveOutline(*f_ind, add_width, 0, 0, Z_LVAL_PP(sz), transform);
			str_path = T1_ConcatOutlines(str_path, char_path);

			char_path = T1_GetCharOutline(*f_ind, _str[i], Z_LVAL_PP(sz), transform);
			str_path = T1_ConcatOutlines(str_path, char_path);
		}
		str_img = T1_AAFillOutline(str_path, 0);
	} else {
		str_img = T1_AASetString(*f_ind, _str,  Z_STRLEN_PP(str), space, T1_KERNING, Z_LVAL_PP(sz), transform);
	}

	if (T1_errno) {
		php_error(E_WARNING, "libt1 returned error %d", T1_errno);
		RETURN_FALSE;
	}

	h_lines = str_img->metrics.ascent -  str_img->metrics.descent;
	v_lines = str_img->metrics.rightSideBearing - str_img->metrics.leftSideBearing;

	for (i = 0; i < v_lines; i++) {
		for (j = 0; j < h_lines; j++) {
			switch (str_img->bits[j*v_lines+i]) {
			case 0:
				break;
			default:
				c_ind = aa[str_img->bits[j*v_lines+i]-1];
				gdImageSetPixel(bg_img, x+str_img->metrics.leftSideBearing+i, y-str_img->metrics.ascent+j, c_ind);
			}
		}
	}

	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING, "Couldn't initialize array for returning bounding box");
		RETURN_FALSE;
	}

	add_next_index_long(return_value, str_img->metrics.leftSideBearing);
	add_next_index_long(return_value, str_img->metrics.descent);
	add_next_index_long(return_value, str_img->metrics.rightSideBearing);
	add_next_index_long(return_value, str_img->metrics.ascent);

#else 
	php_error(E_WARNING, "ImagePsText: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ proto array imagepsbbox(string text, int font, int size [, int space, int tightness, int angle])
   Return the bounding box needed by a string if rasterized */
PHP_FUNCTION(imagepsbbox)
{
#if HAVE_LIBT1
	zval **str, **fnt, **sz, **sp, **wd, **ang;
	int i, space, add_width, char_width, amount_kern;
	int cur_x, cur_y, dx, dy;
	int x1, y1, x2, y2, x3, y3, x4, y4;
	int *f_ind;
	int per_char = 0;
	double angle, sin_a = 0, cos_a = 0;
	BBox char_bbox, str_bbox = {0, 0, 0, 0};

	switch(ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &str, &fnt, &sz) == FAILURE) {
			RETURN_FALSE;
		}
		space = 0;
		break;
	case 6:
		if (zend_get_parameters_ex(6, &str, &fnt, &sz, &sp, &wd, &ang) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_long_ex(sp);
		convert_to_long_ex(wd);
		convert_to_double_ex(ang);
		space = Z_LVAL_PP(sp);
		add_width = Z_LVAL_PP(wd);
		angle = Z_DVAL_PP(ang) * M_PI / 180;
		sin_a = sin(angle);
		cos_a = cos(angle);
		per_char =  add_width || angle ? 1 : 0;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(f_ind, int *, fnt, -1, "Type 1 font", le_ps_font);

	convert_to_string_ex(str);
	convert_to_long_ex(sz);

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
#define new_x(a, b) (int) ((a) * cos_a - (b) * sin_a)
#define new_y(a, b) (int) ((a) * sin_a + (b) * cos_a)

	if (per_char) {
		space += T1_GetCharWidth(*f_ind, ' ');
		cur_x = cur_y = 0;

		for (i = 0; i < (*str)->value.str.len; i++) {
			if ((*str)->value.str.val[i] == ' ') {
				char_bbox.llx = char_bbox.lly = char_bbox.ury = 0;
				char_bbox.urx = char_width = space;
			} else {
				char_bbox = T1_GetCharBBox(*f_ind, (*str)->value.str.val[i]);
				char_width = T1_GetCharWidth(*f_ind, (*str)->value.str.val[i]);
			}
			amount_kern = i ? T1_GetKerning(*f_ind, (*str)->value.str.val[i-1], (*str)->value.str.val[i]) : 0;

			/* Transfer character bounding box to right place */
			x1 = new_x(char_bbox.llx, char_bbox.lly) + cur_x;
			y1 = new_y(char_bbox.llx, char_bbox.lly) + cur_y;
			x2 = new_x(char_bbox.llx, char_bbox.ury) + cur_x;
			y2 = new_y(char_bbox.llx, char_bbox.ury) + cur_y;
			x3 = new_x(char_bbox.urx, char_bbox.ury) + cur_x;
			y3 = new_y(char_bbox.urx, char_bbox.ury) + cur_y;
			x4 = new_x(char_bbox.urx, char_bbox.lly) + cur_x;
			y4 = new_y(char_bbox.urx, char_bbox.lly) + cur_y;

			/* Find min & max values and compare them with current bounding box */
			str_bbox.llx = min(str_bbox.llx, min(x1, min(x2, min(x3, x4))));
			str_bbox.lly = min(str_bbox.lly, min(y1, min(y2, min(y3, y4))));
			str_bbox.urx = max(str_bbox.urx, max(x1, max(x2, max(x3, x4))));
			str_bbox.ury = max(str_bbox.ury, max(y1, max(y2, max(y3, y4))));

			/* Move to the next base point */
			dx = new_x(char_width + add_width + amount_kern, 0);
			dy = new_y(char_width + add_width + amount_kern, 0);
			cur_x += dx;
			cur_y += dy;
			/*
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", x1, y1, x2, y2, x3, y3, x4, y4, char_bbox.llx, char_bbox.lly, char_bbox.urx, char_bbox.ury, char_width, amount_kern, cur_x, cur_y, dx, dy);
			*/
		}

	} else {
		str_bbox = T1_GetStringBBox(*f_ind, (*str)->value.str.val, (*str)->value.str.len, space, T1_KERNING);
	}
	if (T1_errno) RETURN_FALSE;
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	/*
	printf("%d %d %d %d\n", str_bbox.llx, str_bbox.lly, str_bbox.urx, str_bbox.ury);
	*/
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.llx)*(*sz)->value.lval/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.lly)*(*sz)->value.lval/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.urx)*(*sz)->value.lval/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.ury)*(*sz)->value.lval/1000));
#else 
	php_error(E_WARNING, "ImagePsBBox: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

#endif	/* HAVE_LIBGD */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
