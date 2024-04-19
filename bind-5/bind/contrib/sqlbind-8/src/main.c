/*
 * SQLBind 8 daemon
 */

#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>

#include "sqlbind.h"

unsigned int interval=0;
struct sql_rec_head *pHead;
char *cp;
int logging;

/* The default config file name */
char *filename="/etc/sqlbind.conf";
FILE *filestream;

/*
 * OPEN_MAX is POSIX.1
 */
#ifndef OPEN_MAX
# define OPEN_MAX 256		/* anybody's guess */
#endif

#define ARGS "?fl:i:"

static void
catch_sigterm(int sig){
   warning(LOG_INF, "Caught term signal.  Exiting now\n");
   free_all(pHead);
   exit(0);
}

int main(int argc, char **argv){
   int f, c, n;
   struct sigaction sa_new;

   /* some pointers to use when loading the file */
   struct sql_rec_head *cur_rec, *rec_tmp;

   sa_new.sa_handler=catch_sigterm;
   sigemptyset(&sa_new.sa_mask);
   sa_new.sa_flags = 0;
   sigaction(SIGTERM,&sa_new,NULL);

   /* Loop through the command line options */
   while ((c = getopt(argc, argv, ARGS)) != -1){
     switch (c){
	case 'f':
	   filename = optarg;
	   break;
	case 'i':
	   interval = atoi(optarg);
	   break;
	case 'l':
	   logging = atoi(optarg);
	   if(logging >0 && logging <= LOG_DB){
	      warning(LOG_INF, "Using logging level %d\n", logging);
	   }else{
	      printf("Invalid logging level: %d
Valid levels are 1-3\n", logging);
	      return(-1);
	   } 
	   break;
        case '?': 
	   usage(argv[0]);
	   return(-1);
        default: abort();
     } 
   }

   if (optind < argc){
      usage(argv[0]);
      return(-1);
   }

   /* Open the config file */
   if((filestream=fopen(filename, "r"))==NULL){
      printf("Can't open configuration file: %s\n", filename);
      warning(LOG_FATAL, "Can't open configuration file: %s\n", filename);
      return(-1);
   }

   /* loop through the config file filling the head pointer list */
   while(cp=get_line(filestream)){
      if(strstr(cp, "zone")){
	 rec_tmp=new_zone();
      }
      parse_line(rec_tmp, cp);

      if(strstr(cp, "};")){
         if(pHead==NULL){
	    pHead=rec_tmp;
	 }else{
	    cur_rec=pHead;
	    while(cur_rec->pNext!=NULL){
	       cur_rec=cur_rec->pNext;
	    }
	    cur_rec->pNext=rec_tmp;
	 }
      } /* Close the if test */
   } /* Close the while loop */

  /* Close the file */
  fclose(filestream);

  /* Set the sleep timmer */
  if(interval<=0){
     interval=120;
     warning(LOG_WARN, "Using default interval %dsec\n", interval);
  }


  /*
   * Ignore a few signals for good measure.
   */
/*  signal(SIGTERM, SIG_IGN);
  signal(SIGINT, SIG_IGN); */

   /* Fork to the background */
   if (fork() > 0)
      exit(0);


   /* Become session leader (get rid of controlling terminal). */
   setsid();

   /* Make sure we are on a file system that stays mounted. */
   chdir("/");

   for (f = 0; f < OPEN_MAX; f++)
      close(f);

   /* Run through the list proccessing each zone */
   for(;;){
      sched_all(pHead);
      update_named(pHead);
      n=clean_all(pHead);
      warning(LOG_INF, "Finished update. %d records updated\n", n);
      sleep(interval);
   }

   return 0;
}


