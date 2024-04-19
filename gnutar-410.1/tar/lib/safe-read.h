#ifndef PARAMS
# if defined PROTOTYPES || (defined __STDC__ && __STDC__)
#  define PARAMS(Args) Args
# else
#  define PARAMS(Args) ()
# endif
#endif

ssize_t full_write PARAMS ((int desc, const char *ptr, size_t len));
ssize_t safe_read PARAMS ((int desc, void *ptr, size_t len));
