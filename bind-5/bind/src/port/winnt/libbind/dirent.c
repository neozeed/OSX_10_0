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

#include <stdlib.h>
#include <io.h>
#include <dirent.h>
#include <string.h>

/*
 * Chicanery to imitate opendir() readdir() closedir()\
 * These UNIX calls have ANSI near equivalents _findfirst(), _findnext() and _findclose()
 */


DIR *opendir(const char *name)
{
DIR *dir;

char *filespec;

	/* Need to make a wildcard search string from the directory name */
	filespec = (char *)malloc(strlen(name) + 3);
	strcpy(filespec, name);
	strcat(filespec, "\\*");

	dir = (DIR *)malloc(sizeof(DIR));

	dir->fhandle = _findfirst(filespec, &dir->fd);
	dir->sstatus = 0;
	if(dir->fhandle == -1) /* We failed to find anything */
	{
		free(dir);
		free(filespec);
		return NULL;
	}

	free(filespec);
	return dir;
}	

struct dirent* readdir(DIR *dir)
{
	if(dir->sstatus == -1) /* We have no valid file to return */
	{
		return NULL;
	}
	
	/* Fill in the dirent struct */
	dir->de.d_namlen = strlen(dir->fd.name);
	strcpy(dir->de.d_name, dir->fd.name);
	dir->de.d_type = dir->fd.attrib;

	/* ready the next file to return */
	dir->sstatus = _findnext(dir->fhandle, &dir->fd);

	return (&dir->de);
}

void closedir(DIR *dir)
{
	_findclose(dir->fhandle);
	if(dir)
		free(dir);
	dir = NULL;
}
