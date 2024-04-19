/*
 * SQLBind 8 functions
 */

#include "config.h"
#include "sqlbind.h"

#ifdef HAVE_MYSQL
#include <mysql.h>
#include <errmsg.h>
#endif

#include <string.h>

#define dbname "bind"

/* 
 * Load a zone from the db server. If need be, update an
 * existing zone.  This is the start of the real sql code.
 */
int 
update_zone_mysql(struct sql_rec_head *rec_head){
   MYSQL_RES *sql_res;
   MYSQL_ROW sql_row;
   char q1[]="select sqlOrigin,sqlOwner,sqlClass,sqlType,sqlData,sqlTTL,sqlPref,sqlID,unix_timestamp(sqlTime) from";
   char q2[]="order by sqlID";
   char sql_query[255];

   struct SQL_REC tmp_sql_rec;
   ns_updrec *cur_ns_rec, *tmp_ns_rec;

   /* build up the query string for the zone */
   sprintf(sql_query, "%s %s %s", q1, fixname(rec_head->dbzone), q2); 

   /* 
   * Connect to the db and query it.
   * Complain about any errors and return.
   */
   if(db_connect(rec_head)){
      warning(LOG_DB, "Error connecting to db server\n");
   }

   /* Query the db. Complain on error. */
   if(mysql_real_query(&rec_head->sql_sock, sql_query, strlen(sql_query)) < 0 || !(sql_res=mysql_store_result(&rec_head->sql_sock))){
      warning(LOG_DB, "Query failed\n");
      return 1;
   }

   while (sql_row=mysql_fetch_row(sql_res)) {
      tmp_sql_rec.sqlType = savestr(sql_row[3], 1);

      /* don't mess with soa's just yet */
      if(strcmp(tmp_sql_rec.sqlType, "SOA")!=0){

         tmp_sql_rec.sqlOrigin = savestr(sql_row[0], 1); 
         tmp_sql_rec.sqlOwner = savestr(sql_row[1], 1);
         tmp_sql_rec.sqlClass = savestr(sql_row[2], 1);
         tmp_sql_rec.sqlData = savestr(sql_row[4], 1);
         tmp_sql_rec.sqlTTL = atoi(sql_row[5]);
         tmp_sql_rec.sqlPref = savestr(sql_row[6], 1);
         tmp_sql_rec.sqlID = atoi(sql_row[7]);
         tmp_sql_rec.sqlTime = atoi(sql_row[8]);

	 /* start looping through the linked list to place the record */
         if(rec_head->r_head == NULL){
            rec_head->r_head=(ns_updrec *)new_rec(tmp_sql_rec, ADD);
         }else{
	    for(cur_ns_rec=rec_head->r_head;cur_ns_rec!=NULL;cur_ns_rec=cur_ns_rec->r_next){
	       /* Handle deletes and adding new records */
	       if(cur_ns_rec->r_next!=NULL){
	          if(need_del_rec(cur_ns_rec->r_next, tmp_sql_rec)){
	  	     /* put in the delete record */
	             tmp_ns_rec=cur_ns_rec->r_next->r_next;
		     cur_ns_rec->r_next->r_ttl=0;
		     cur_ns_rec->r_next->r_opcode=0;
		     cur_ns_rec->r_next->r_next=(ns_updrec *)new_rec(tmp_sql_rec, ADD);
		     cur_ns_rec->r_next->r_next->r_next=tmp_ns_rec;
		     break;
	          }
	       }else{
	  	  if(!rec_exists(rec_head->r_head, tmp_sql_rec))
		     cur_ns_rec->r_next=(ns_updrec *)new_rec(tmp_sql_rec, ADD);
	          break;
	       }
	   } /* close the for loop used to place records */
       } /* close the null head if test */
    } /* close the soa if test */
   } /* close the while that loops through records */

   /* Clean up after our self */
   mysql_free_result(sql_res);

   return 0;
}

/*
 * Connect to the database.  If the connection goes
 * away for some reasone, reconnect.
 */
int
db_connect(struct sql_rec_head *rec_head){
   /* 
    * if we have never connected to the db, 
    * initialize the struct and connect.
    */ 
   if(rec_head->sql_sock.host=='\0'){
      mysql_init(&rec_head->sql_sock);
      if (!mysql_real_connect(&rec_head->sql_sock, rec_head->dbhost, rec_head->dbuser, rec_head->dbpass, dbname, 0, NULL, 0)){
         warning(LOG_DB, "Couldn't connect to database server: %s", 
		mysql_error(&rec_head->sql_sock));
         return 1;
      }
   }
   /*
    * If we have connected and lost the connection
    * for some reason, reconnect to the server.
    */
   if(mysql_ping(&rec_head->sql_sock)){
      if(mysql_errno(&rec_head->sql_sock)==CR_SERVER_GONE_ERROR){
          rec_head->sql_sock.reconnect=1;
          if(!mysql_ping(&rec_head->sql_sock))
             warning(LOG_DB, "Reconnecting to db server\n");
      }else{
          warning(LOG_DB, "Lost connection to db server\n");
          return 1;
      }
   }
   return 0;
}

