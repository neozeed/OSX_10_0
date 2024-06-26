/************************************************************************
 *	Some routines common to procmail and formail			*
 *									*
 *	Copyright (c) 1990-1997, S.R. van den Berg, The Netherlands	*
 *	#include "../README"						*
 ************************************************************************/
#ifdef RCS
static /*const*/char rcsid[]=
 "$Id: common.c,v 1.1.1.1 1999/09/23 17:30:07 wsanchez Exp $";
#endif
#include "procmail.h"
#include "sublib.h"
#include "robust.h"
#include "shell.h"
#include "misc.h"
#include "common.h"

void shexec(argv)const char*const*argv;
{ int i;char**newargv;const char**p;
#ifdef SIGXCPU
  signal(SIGXCPU,SIG_DFL);signal(SIGXFSZ,SIG_DFL);
#endif
#ifdef SIGLOST
  signal(SIGLOST,SIG_DFL);
#endif						/* or is it a shell script ? */
  signal(SIGPIPE,SIG_DFL);execvp(*argv,(char*const*)argv);
  for(p=(const char**)argv,i=1;i++,*p++;);	      /* count the arguments */
  newargv=malloc(i*sizeof*p);		      /* no shell script? -> trouble */
  for(*(p=(const char**)newargv)=binsh;*++p= *argv++;);
  execv(*newargv,newargv);free(newargv);nlog("Failed to execute");
  logqnl(*argv);
  exit(EX_UNAVAILABLE);
}

void detab(p)char*p;
{ while(p=strpbrk(p,"\t\n\v\f\r"))
     *p=' ';			     /* take out all tabs and other specials */
}

char*skpspace(chp)const char*chp;
{ for(;;chp++)
     switch(*chp)
      { case ' ':case '\t':
	   continue;
	default:
	   return (char*)chp;
      }
}

#ifdef NOstrcspn
int strcspn(whole,sub)const char*const whole,*const sub;
{ const register char*p;
  p=whole;
  while(*p&&!strchr(sub,*p))
     p++;
  return p-whole;
}
#endif

void ultstr(minwidth,val,dest)unsigned long val;char*dest;
{ int i;unsigned long j;
  j=val;i=0;					   /* a beauty, isn't it :-) */
  do i++;					   /* determine needed width */
  while(j/=10);
  while(--minwidth>=i)				 /* fill up any excess width */
     *dest++=' ';
  *(dest+=i)='\0';
  do *--dest='0'+val%10;			  /* display value backwards */
  while(val/=10);
}

int waitfor(pid)const pid_t pid;	      /* wait for a specific process */
{ int i;pid_t j;
  while(pid!=(j=wait(&i))||WIFSTOPPED(i))
     if(-1==j)
	return NO_PROCESS;
     else if(!pid)
	break;
  return lexitcode=WIFEXITED(i)?WEXITSTATUS(i):-WTERMSIG(i);
}

int strnIcmp(a,b,l)register const char*a,*b;register size_t l;
{ unsigned i,j;
  if(l)						 /* case insensitive strncmp */
     do
      { while(*a&&*a==*b&&--l)
	   a++,b++;
	if(!l)
	   break;
	if((i= *a++)-'A'<='Z'-'A')
	   i+='a'-'A';
	if((j= *b++)-'A'<='Z'-'A')
	   j+='a'-'A';
	if(j!=i)
	   return i>j?1:-1;
      }
     while(i&&j&&--l);
  return 0;
}
