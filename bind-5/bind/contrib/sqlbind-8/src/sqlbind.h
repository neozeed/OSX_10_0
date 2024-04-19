/*
 * SQLBind 8 header
 */

#include "config.h"
#include <sys/types.h>

#define VERSION "SQLBind8 0.9.0"

#ifdef HAVE_BIND
#include <port_before.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <resolv.h>
#include <ctype.h>

#include <port_after.h>
#endif

#include<mysql.h>

#define MAXD 255 

#define DBHOST 0
#define DBUSER 1
#define DBPASS 2
#define DBMBOX 3
#define DBREFRESH 4
#define DBRETRY 5
#define DBEXPIRE 6
#define DBMIN 7
#define DBSERIAL 8
#define DBTYPE 9

#define LOG_INF 1 /* info */
#define LOG_WARN 2 /* warning */
#define LOG_DB 3 /* db errors */
#define LOG_STOP 9 /* bad stuff */
#define LOG_FATAL 10 /* fatal stuff */

/* This will return depending on good/bad chars */
#define whitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '"' || (c) == ';' || (c) == '{' || (c) == '}')
#define comment(c) ((c) == '#' || (c) == '/')


struct SQL_REC {
   char *sqlOrigin;
   char *sqlOwner;
   char *sqlClass;
   char *sqlType;
   char *sqlData;
   u_int32_t sqlTime;
   int sqlTTL;
   char *sqlPref;
   int sqlID;
};

struct sql_rec_head {
   char *dbhost;
   char *dbuser;
   char *dbpass;
   char *dbzone;
   MYSQL sql_sock;
   ns_updrec *r_head;
   struct sql_rec_head *pNext;
};

extern struct sql_rec_head *pHead; 

void usage(char *name);
char *savestr(const char *str, int needpanic);
char *fixname(char c[MAXD]);

char *get_line(FILE *fp);
void parse_line(struct sql_rec_head *rec, char *line);
int check_fields(struct sql_rec_head *rec);

struct sql_rec_head *new_zone();


#ifdef HAVE_BIND
extern int sql_update(struct SQL_REC *);
#endif

#define DB_HOST 1
#define DB_USER 2
#define DB_PASS 3
#define DB_TABLE 4

