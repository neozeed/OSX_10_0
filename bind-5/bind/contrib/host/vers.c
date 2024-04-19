#ifndef lint
static char Version[] = "@(#)vers.c	e07@nikhef.nl (Eric Wassenaar) 991331";
#endif

char *version = "host version 991331";

#if defined(apollo)
int h_errno = 0;		/* prevent shared library conflicts */
#endif
