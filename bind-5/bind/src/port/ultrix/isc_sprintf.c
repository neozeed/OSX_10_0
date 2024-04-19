#include <stdio.h>
#include <stdarg.h>

/*
 * An implementation of sprintf() that always returns an int regardless
 * of the compile environment.  The Ultrix vsprintf() _always_ returns
 * an int.  Code from Mark Andrews.
 */
int
__isc_sprintf(char *str, const char *format, ...)
{
	va_list ap;
	int len;

	va_start(ap, format);
	len = vsprintf(str, format, ap);
	va_end(va_list ap);
	return(len);
}
