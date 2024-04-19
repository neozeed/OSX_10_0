/*
Copyright (C) 2000  Danny Mayer.

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

Portions Copyright (C) 2000  Internet Software Consortium.

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef _DIRENT_H
#define _DIRENT_H

/* NOTE: This was copied from io.h and placed here because we have
 * defined the _POSIX_ macro and as a result io.h skips these
 * structures even though they are needed here.
 */

#ifdef _POSIX_
#ifndef _TIME_T_DEFINED
typedef long time_t;            /* time value */
#define _TIME_T_DEFINED         /* avoid multiple def's of time_t */
#endif

#ifndef _FSIZE_T_DEFINED
typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */
#define _FSIZE_T_DEFINED
#endif

#ifndef _FINDDATA_T_DEFINED

struct _finddata_t {
    unsigned    attrib;
    time_t      time_create;    /* -1 for FAT file systems */
    time_t      time_access;    /* -1 for FAT file systems */
    time_t      time_write;
    _fsize_t    size;
    char        name[260];
};
#endif //_FINDDATA_T_DEFINED
#endif //_POSIX_

/* Emulating UNIX calls opendir() readdir() and closedir() */


struct dirent {
	unsigned int	d_type; 			/* file type, see below */
	unsigned long	d_namlen;			/* length of string in d_name */
	char			d_name[_MAX_PATH];	/* name must be no longer than this */
};

struct dirlist {
	long fhandle;			/* The search handle */
	int  sstatus;			/* Search status */
	struct dirent de;		/* The file entry Structure being returned */
	struct _finddata_t fd;	/* File Info being returned */
};

typedef struct dirlist DIR;

DIR *opendir(const char *name);
struct dirent* readdir(DIR *dir);
void closedir(DIR *dir);

#endif
