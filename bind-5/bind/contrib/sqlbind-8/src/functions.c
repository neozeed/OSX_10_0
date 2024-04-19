/*
 * SQLBind 8 functions
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/errno.h>
#include <memory.h>
#include <sys/param.h>
#include <stdarg.h>

#include "sqlbind.h"

/* Some defs from bind */
#define NS_INT16SZ      2       /* #/bytes of data in a u_int16_t */

/* #define NS_PUT16(s, cp) { \
        register u_int16_t t_s = (u_int16_t)(s); \
        register u_char *t_cp = (u_char *)(cp); \
        *t_cp++ = t_s >> 8; \
        *t_cp   = t_s; \
        (cp) += NS_INT16SZ; \
}*/

char buf[255]; 
extern char *cp;
char tmpbuf[MAXD]; 
extern int logging;

char *newstr(size_t len, int needpanic);

/* General warning logged to syslog */
int warning(int level, const char *msg, ...){
   va_list args;
   char *out;

   if(level<=logging || logging==LOG_STOP || logging==LOG_FATAL){
      if((out=malloc(255))==NULL)
         return(-1);
      va_start(args, msg);
      vsprintf(out, msg, args);
      va_end(args);
      openlog("sqlbind", LOG_CONS, LOG_DAEMON);
      syslog(LOG_INFO, out);
      closelog();
   }
   return 0;
}

/* Usage to print when daemon is called with -? */
void usage(char *name){
   fprintf(stderr, 
	"%s
Usage: %s [-f filename]
	filename is the name of the configureation file
	to be loaded.  Default: /etc/sqlbind.conf\n",
	VERSION, name);
   exit(1);
}

/* 
 * Return a table name to be used to load a zone from the db 
 * This changes . to _ and - to __ to make the db server happy
 */
char * 
fixname(char c[MAXD]){
   int i=0, y=0;

   while(c[i]!='\0'){
      if(c[i]=='.'){
         tmpbuf[y] = '_';
      }else if(c[i]=='-'){
         tmpbuf[y]='_';
         tmpbuf[y+1]='_';
         ++y;
      }else{
         tmpbuf[y] = c[i];
      }
      ++i;
      ++y;
   }
   tmpbuf[y]='\0';

   return tmpbuf;      
}

/* Copied from bind and cleaned up a bit */
char *
savestr(const char *str, int needpanic) {
        char *buf;
        size_t len;

        len = strlen(str);
        if (len > 65536) {
                if (needpanic)
                        warning(LOG_WARN, "savestr: string too long");
                else
                        return (NULL);
        }
        buf = newstr(len, needpanic);
        memcpy(buf, str, len + 1);
        return (buf);
}

/* Another copy from bind */
char *
newstr(size_t len, int needpanic){
   u_char *buf, *bp;

   buf = (u_char *)malloc(2 + len + 1);
   if(buf == NULL){
      if(needpanic)
	  warning(LOG_WARN, "savestr: malloc failed");
      else
	  return (NULL);
   }
   bp = buf;
   NS_PUT16(len, bp);
   return ((char *)bp);
}

/* Return a line from the file */
char *
get_line(FILE *fp){
   return(fgets(buf, sizeof buf, fp));
}

/* Hackup the line and fill in the head pointers */
void
parse_line(struct sql_rec_head *rec, char *line){
   int c, i;
   int blank=0;
   char buf[100]="", key[30]="", val[30]="";

   for(i=0; i<strlen(line); ++i){
     if(comment(line[i])){
	 return;
     }else{
       if(whitespace(line[i])){
	  blank=0;
	  if(strcmp(buf, "zone")==0){
		sprintf(key, "%s", buf); 
		buf[0]='\0';
	  }else if(strcmp(buf, "dbhost")==0){
		sprintf(key, "%s", buf); 
		buf[0]='\0';
	  }else if(strcmp(buf, "dbuser")==0){
		sprintf(key, "%s", buf); 
		buf[0]='\0';
	  }else if(strcmp(buf, "dbpass")==0){
		sprintf(key, "%s", buf); 
		buf[0]='\0';
	  }else if(strlen(buf)>0){
              if(strcmp(key, "zone")==0){
		rec->dbzone=savestr(buf, 1);
		return;
	      }else if(strcmp(key, "dbhost")==0){
		rec->dbhost=savestr(buf, 1);
		return;
              }else if(strcmp(key, "dbuser")==0){
		rec->dbuser=savestr(buf, 1);
		return;
	      }else if(strcmp(key, "dbpass")==0){
		rec->dbpass=savestr(buf, 1);
		return;
              }
	  }
       }else{
	  buf[blank]=line[i];
	  buf[blank+1]='\0'; 
	  ++blank;
       }  
      }
    }
}

/* Make a new zone and fill in some defaults */
struct sql_rec_head *
new_zone(){
   struct sql_rec_head *newzone=(struct sql_rec_head *)calloc(1, sizeof(struct sql_rec_head));
   
   newzone->dbhost=savestr("localhost", 1);
   newzone->dbuser=savestr("bind", 1);
   newzone->dbpass=savestr("bindpass", 1);
   newzone->dbzone=savestr("", 1);   
   newzone->pNext=0;
   newzone->r_head=0;

   return(newzone);
} 

/*********************************************************************
 * Group of functions that work on individual records in a zone list *
 *********************************************************************/

/*
 * Return depending on if the record needs to
 * be deleted or not.
 */
int
need_del_rec(ns_updrec *list_head, struct SQL_REC rec){
   if(list_head->r_id==rec.sqlID){
        if(list_head->r_time<rec.sqlTime){
           return -1;
        }
   }
   return 0;
}

/*
 * Return depending on if the record
 * exists in the list or not.
 */
int
rec_exists(ns_updrec *list_head, struct SQL_REC rec){
   ns_updrec *next_list;

   next_list=list_head;
   while(next_list){
      if(next_list->r_id==rec.sqlID){
         return -1;
      }
      next_list=next_list->r_next;
   }

   return 0;
}

/********************************************************************
 * Group of functions that work on individual linked lists of zones *
 ********************************************************************/

/* Remove any delete records from the zone */
int
clean_zone(struct sql_rec_head *rec_head){
   ns_updrec *next_list, *tmp_list;

   next_list=rec_head->r_head;
   if(next_list->r_opcode==0){
        tmp_list=next_list->r_next;
        free(next_list);
        next_list=tmp_list;
   }
   while(next_list->r_next){
      if(next_list->r_next->r_opcode==0){
         tmp_list=next_list->r_next->r_next;
         free(next_list->r_next);
         next_list->r_next=tmp_list;
      }
      next_list=next_list->r_next;
   }
   return 0;
}

/* Free the mem taken by a zone */
int
free_zone(struct sql_rec_head *rec_head){
   ns_updrec *tmp_rec, *next_rec;
   
   next_rec=rec_head->r_head;
   while(next_rec){
      tmp_rec=next_rec;
      next_rec=next_rec->r_next;
      res_freeupdrec(tmp_rec);
   }
   rec_head->r_head='\0';
   return 0;
}

/********************************************************************
 * Group of functions that work on all the zones.		    *
 ********************************************************************/

/* Free all the zones */
int 
free_all(struct sql_rec_head *rec_head){
   struct sql_rec_head *tmp_head, *next_head;

   next_head=rec_head;
   while(next_head){
      tmp_head=next_head;
      next_head=next_head->pNext;
      if(free_zone(tmp_head)){
	warning(LOG_INF, "Done with free() on zone: %s\n", tmp_head->dbzone);
      }
   } 

   return 0;
}

/* schedule loading all the zones */
int
sched_all(struct sql_rec_head *rec_head){
   struct sql_rec_head *tmp_rec;

   tmp_rec=rec_head;
   while(tmp_rec){
      if(update_zone_mysql(tmp_rec)){ 
	 warning(LOG_INF, "Done updating record list for zone: %s\n",
		tmp_rec->dbzone);
      }
      tmp_rec=tmp_rec->pNext;
   }
   return 0;
}

/* Do a cleanup on all the zones */
int
clean_all(struct sql_rec_head *rec_head){
   struct sql_rec_head *tmp_rec;

   tmp_rec=rec_head;
   while(tmp_rec){
      if(clean_zone(tmp_rec)){
	 warning(LOG_INF, "Done cleaning zone: %s\n",tmp_rec->dbzone);
      }
      tmp_rec=tmp_rec->pNext;
   }
   return 0;
}


