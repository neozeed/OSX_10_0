#define WANT_IRS_NIS
#define WANT_IRS_PW
#define WANT_IRS_GR
#define SIG_FN void
#if defined(HAS_PTHREADS) && defined(_REENTRANT)
#define DO_PTHREADS
#endif

#define SETGRENT_VOID
#define SETPWENT_VOID
