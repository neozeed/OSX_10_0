#define ZEND_API
#define ZEND_DLEXPORT

@TOP@

#undef uint
#undef ulong

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

@BOTTOM@

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if ZEND_BROKEN_SPRINTF
int zend_sprintf(char *buffer, const char *format, ...);
#else
# define zend_sprintf sprintf
#endif

#include <math.h>

#ifdef HAVE_ISNAN
#define zend_isnan(a) isnan(a)
#elif defined(NAN)
#define zend_isnan(a) (((a)==NAN)?1:0)
#else
#define zend_isnan(a) 0
#endif

#ifdef HAVE_ISINF
#define zend_isinf(a) isinf(a)
#elif defined(INFINITY)
/* Might not work, but is required by ISO C99 */
#define zend_isinf(a) (((a)==INFINITY)?1:0)
#else
#define zend_isinf(a) 0
#endif

#ifdef HAVE_FINITE
#define zend_finite(a) finite(a)
#elif defined(HAVE_ISFINITE) || defined(isfinite)
#define zend_finite(a) isfinite(a)
#elif defined(fpclassify)
#define zend_finite(a) ((fpclassify((a))!=FP_INFINITE&&fpclassify((a))!=FP_NAN)?1:0)
#else
#define zend_finite(a) (zend_isnan(a) ? 0 : zend_isinf(a) ? 0 : 1)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
