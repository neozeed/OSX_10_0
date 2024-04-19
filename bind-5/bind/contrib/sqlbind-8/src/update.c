/*
 * Dynamic update for SQLBind 8
 * Partly borrowed from nsupdate in bind
 */

#include "config.h"
#include "sqlbind.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Map class and type names to number
 */
struct map {
	char    token[10];
	int     val;
};

struct map class_strs[] = {
	{ "in",         C_IN },
	{ "chaos",      C_CHAOS },
	{ "hs",         C_HS },
};
#define M_CLASS_CNT (sizeof(class_strs) / sizeof(struct map))

struct map type_strs[] = {
	{ "a",          T_A },
	{ "ns",         T_NS },
	{ "cname",      T_CNAME },
	{ "soa",        T_SOA },
	{ "mb",         T_MB },
	{ "mg",         T_MG },
	{ "mr",         T_MR },
	{ "null",       T_NULL },
	{ "wks",        T_WKS },
	{ "ptr",        T_PTR },
	{ "hinfo",      T_HINFO },
	{ "minfo",      T_MINFO },
	{ "mx",         T_MX },
	{ "txt",        T_TXT },
	{ "rp",         T_RP },
	{ "afsdb",      T_AFSDB },
	{ "x25",        T_X25 },
	{ "isdn",       T_ISDN },
	{ "rt",         T_RT },
	{ "nsap",       T_NSAP },
	{ "nsap_ptr",   T_NSAP_PTR },
	{ "px",         T_PX },
	{ "loc",        T_LOC },
};
#define M_TYPE_CNT (sizeof(type_strs) / sizeof(struct map))

struct map section_strs[] = {
	{ "zone",	S_ZONE },
	{ "prereq",	S_PREREQ },
	{ "update", 	S_UPDATE },
	{ "reserved",	S_ADDT },
};
#define M_SECTION_CNT (sizeof(section_strs) / sizeof(struct map))

struct map opcode_strs[] = {
	{ "nxdomain",	NXDOMAIN },
	{ "yxdomain",	YXDOMAIN },
	{ "nxrrset", 	NXRRSET },
	{ "yxrrset",	YXRRSET },
	{ "delete",	DELETE },
	{ "add",	ADD },
};
#define M_OPCODE_CNT (sizeof(opcode_strs) / sizeof(struct map))

/* 
 * Pass a sql record and an operation in
 * and return a ns_updrec struct to be 
 * to be attached to the linked list.
 */
ns_updrec *
new_rec(struct SQL_REC rec, int r_opcode){
   char *r_dname;
   char section[15], opcode[10];
   int i, c, n, n1, r_size, r_section;
   int16_t r_class, r_type=-1;
   u_int32_t r_ttl=0;
   struct map *mp;
   ns_updrec *rrecp;
   char tmp_dname[MAXD];
   char tmp_data[MAXD];

   /* Build up the owner */
   if(rec.sqlOwner[strlen(rec.sqlOwner)]=='.'){
      r_dname = savestr(rec.sqlOwner, 1);
   }else{
      if(rec.sqlOwner[0]=='\0')
	 sprintf(tmp_dname, "%s.", rec.sqlOrigin);
      else
	 sprintf(tmp_dname, "%s.%s.", rec.sqlOwner, rec.sqlOrigin);
	 r_dname = savestr(tmp_dname, 1);
   }

   /* Default to class type IN */
   r_class = C_IN; 

   /* Set the ttl */
   r_ttl=rec.sqlTTL;

   /* loop through and pick the right class out of the map struct */
   for (mp = class_strs; mp < class_strs+M_CLASS_CNT; mp++) {
      if (!strcasecmp(rec.sqlClass, mp->token)) {
	 r_class = mp->val;
	 break;
      }
   }

   /*
    * type and rdata field may or may not be required depending
    * on the section and operation
    */
   switch (r_opcode) {
      case DELETE:
	 r_ttl = 0;
	 r_type = T_ANY;
	 /* read type, if specified */
	 for (mp = type_strs; mp < type_strs+M_TYPE_CNT; mp++){
	    if (!strcasecmp(rec.sqlType, mp->token)) {
		r_type = mp->val;
	    }
	 }
	 break;

      case ADD:
	 if (r_ttl == 0) {
	    r_ttl=300;
	    warning(LOG_WARN, "ttl == 0;  setting ttl to %d\n",r_ttl);
	 }
	 /* read type */
	 for (mp = type_strs; mp < type_strs+M_TYPE_CNT; mp++){
	    if (!strcasecmp(rec.sqlType, mp->token)) {
	        r_type = mp->val;
	  	break;
	    }
	 }
	 if (r_type == -1) {
	    warning(LOG_WARN, "Invalid type for record to be added\n");
	 }
	 break;

      default:
	 warning(LOG_WARN, "Unknown operation in update section\n");
   } /* Close the switch */

   /* Make a ns_updrec from the above info */
   rrecp=res_mkupdrec(2, r_dname, r_class, r_type, r_ttl);

   /* If the record is a MX, set the preferance for it */
   if(strcasecmp(rec.sqlType, "MX")==0){
      sprintf(tmp_data, "%s %s", rec.sqlPref, rec.sqlData);
   }else{
      strcpy(tmp_data, rec.sqlData);
   }

   /* Fill in some of the fields in the record */
   rrecp->r_id = rec.sqlID;
   rrecp->r_time = rec.sqlTime; 
   rrecp->r_opcode = r_opcode;
   rrecp->r_size = strlen(tmp_data);
   rrecp->r_data = savestr(tmp_data, 1);

   /* return the record to add to the linked list */
   return(rrecp);
}

/*
 * Pass in a head pointer to a linked list and
 * start handing out updates to the dns server.
 * Return back the number of updated records.
 */
int
update_named(struct sql_rec_head *rec_head){
   struct sql_rec_head *cur_rec;
   int n;
   static struct __res_state res;

   /* Initialize the structure */
   (void) res_ninit(&res);

   /* Loop through the list passing the record of to res_nupdate */
   cur_rec=rec_head;
   while(cur_rec){
      if((n = res_nupdate(&res, cur_rec->r_head))<0){
         warning(LOG_WARN, "Update failed for zone: %s\n", cur_rec->dbzone);
      }
      cur_rec=cur_rec->pNext;
   }
   return(n);
}


