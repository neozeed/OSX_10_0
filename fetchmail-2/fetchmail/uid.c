/*
 * uid.c -- UIDL handling for POP3 servers without LAST
 *
 * For license terms, see the file COPYING in this directory.
 */

#include "config.h"

#include <stdio.h>
#include <limits.h>
#if defined(STDC_HEADERS)
#include <stdlib.h>
#include <string.h>
#endif
#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "fetchmail.h"

/*
 * Machinery for handling UID lists live here.  This is mainly to support
 * RFC1725-conformant POP3 servers without a LAST command, but may also be
 * useful for making the IMAP4 querying logic UID-oriented, if a future
 * revision of IMAP forces me to.
 *
 * These functions are also used by the rest of the code to maintain
 * string lists.
 *
 * Here's the theory:
 *
 * At start of a query, we have a (possibly empty) list of UIDs to be
 * considered seen in `oldsaved'.  These are messages that were left in
 * the mailbox and *not deleted* on previous queries (we don't need to
 * remember the UIDs of deleted messages because ... well, they're gone!)
 * This list is initially set up by initialize_saved_list() from the
 * .fetchids file.
 *
 * Early in the query, during the execution of the protocol-specific 
 * getrange code, the driver expects that the host's `newsaved' member
 * will be filled with a list of UIDs and message numbers representing
 * the mailbox state.  If this list is empty, the server did
 * not respond to the request for a UID listing.
 *
 * Each time a message is fetched, we can check its UID against the
 * `oldsaved' list to see if it is old.
 *
 * Each time a message-id is seen, we mark it with MARK_SEEN.
 *
 * Each time a message is deleted, we mark its id UID_DELETED in the
 * `newsaved' member.  When we want to assert that an expunge has been
 * done on the server, we call expunge_uid() to register that all
 * deleted messages are gone by marking them UID_EXPUNGED.
 *
 * At the end of the query, the `newsaved' member becomes the
 * `oldsaved' list.  The old `oldsaved' list is freed.
 *
 * At the end of the fetchmail run, seen and non-EXPUNGED members of all
 * current `oldsaved' lists are flushed out to the .fetchids file to
 * be picked up by the next run.  If there are no un-expunged
 * messages, the file is deleted.
 *
 * Note: some comparisons (those used for DNS address lists) are caseblind!  
 */

/* UIDs associated with un-queried hosts */
static struct idlist *scratchlist;

#ifdef POP3_ENABLE
void initialize_saved_lists(struct query *hostlist, const char *idfile)
/* read file of saved IDs and attach to each host */
{
    int	st;
    FILE	*tmpfp;
    struct query *ctl;

    /* make sure lists are initially empty */
    for (ctl = hostlist; ctl; ctl = ctl->next)
	ctl->skipped = ctl->oldsaved = ctl->newsaved = (struct idlist *)NULL;

    /* let's get stored message UIDs from previous queries */
    if ((tmpfp = fopen(idfile, "r")) != (FILE *)NULL)
    {
	char buf[POPBUFSIZE+1];
	char *host;
	char *user;
	char *id;
	char *atsign;	/* temp pointer used in parsing user and host */
	char *delimp1;
	char saveddelim1;
	char *delimp2;
	char saveddelim2;

	while (fgets(buf, POPBUFSIZE, tmpfp) != (char *)NULL)
	{
	  /*
	   * At this point, we assume the bug has two fields -- a user@host 
	   * part, and an ID part. Either field may contain spurious @ signs. 
           * The previous version of this code presumed one could split at 
           * the rightmost '@'.  This is not correct, as InterMail puts an 
           * '@' in the UIDL.
	   */
	  
	  /* very first, skip leading spaces */
	  user = buf + strspn(buf, " \t");

	  /* First, we split the buf into a userhost part and an id part */
	  if ( (delimp1 = strpbrk(user, " \t")) != NULL ) {
		id = delimp1 + strspn(delimp1, " \t");	/* set pointer to id */
	  	saveddelim1 = *delimp1;	/* save char after token */
		*delimp1 = '\0';		/* delimit token with \0 */
		if (id != NULL) {
			/* now remove trainling white space chars from id */
	  		if ( (delimp2 = strpbrk(id, " \t\n")) != NULL ) {
				saveddelim2 = *delimp2;
				*delimp2 = '\0';
			}
	      		atsign = strrchr(user, '@');
	      		if (atsign) {
				*atsign = '\0';
				host = atsign + 1;

			}
			for (ctl = hostlist; ctl; ctl = ctl->next) {
				if (ctl->server.truename &&
			 	    strcasecmp(host, ctl->server.truename) == 0
			 	    && strcasecmp(user, ctl->remotename) == 0) {
	
					save_str(&ctl->oldsaved, id, UID_SEEN);
					break;
				}
			}
			/* if it's not in a host we're querying,
			** save it anyway */
			if (ctl == (struct query *)NULL) {
				/* restore string */
				*delimp1 = saveddelim1;
				*atsign = '@';
	  			if (delimp2 != NULL) {
					*delimp2 = saveddelim2;
				}
				save_str(&scratchlist, buf, UID_SEEN);
			}
		}
	    }
	}
	fclose(tmpfp);
    }

    if (outlevel >= O_DEBUG)
    {
	struct idlist	*idp;
	int uidlcount = 0;

	for (ctl = hostlist; ctl; ctl = ctl->next)
	    if (ctl->server.uidl)
	    {
		report_build(stdout, "Old UID list from %s:",ctl->server.pollname);
		for (idp = ctl->oldsaved; idp; idp = idp->next)
		    report_build(stdout, " %s", idp->id);
		if (!idp)
		    report_build(stdout, " <empty>");
		report_complete(stdout, "\n");
		uidlcount++;
	    }

	if (uidlcount)
	{
	    report_build(stdout, "Scratch list of UIDs:");
	    for (idp = scratchlist; idp; idp = idp->next)
		report_build(stdout, " %s", idp->id);
	    if (!idp)
		report_build(stdout, " <empty>");
	    report_complete(stdout, "\n");
	}
    }
}
#endif /* POP3_ENABLE */

struct idlist *save_str(struct idlist **idl, const char *str, flag status)
/* save a number/UID pair on the given UID list */
{
    struct idlist **end;

    /* do it nonrecursively so the list is in the right order */
    for (end = idl; *end; end = &(*end)->next)
	continue;

    *end = (struct idlist *)xmalloc(sizeof(struct idlist));
    (*end)->val.status.mark = status;
    (*end)->id = str ? xstrdup(str) : (char *)NULL;
    (*end)->next = NULL;

    return(*end);
}

void free_str_list(struct idlist **idl)
/* free the given UID list */
{
    if (*idl == (struct idlist *)NULL)
	return;

    free_str_list(&(*idl)->next);
    free ((*idl)->id);
    free(*idl);
    *idl = (struct idlist *)NULL;
}

void save_str_pair(struct idlist **idl, const char *str1, const char *str2)
/* save an ID pair on the given list */
{
    struct idlist **end;

    /* do it nonrecursively so the list is in the right order */
    for (end = idl; *end; end = &(*end)->next)
	continue;

    *end = (struct idlist *)xmalloc(sizeof(struct idlist));
    (*end)->id = str1 ? xstrdup(str1) : (char *)NULL;
    if (str2)
	(*end)->val.id2 = xstrdup(str2);
    else
	(*end)->val.id2 = (char *)NULL;
    (*end)->next = (struct idlist *)NULL;
}

#ifdef __UNUSED__
void free_str_pair_list(struct idlist **idl)
/* free the given ID pair list */
{
    if (*idl == (struct idlist *)NULL)
	return;

    free_idpair_list(&(*idl)->next);
    free ((*idl)->id);
    free ((*idl)->val.id2);
    free(*idl);
    *idl = (struct idlist *)NULL;
}
#endif

int str_in_list(struct idlist **idl, const char *str, const flag caseblind)
/* is a given ID in the given list? (comparison may be caseblind) */
{
    if (*idl == (struct idlist *)NULL || str == (char *) NULL)
	return(0);
    else if (!caseblind && strcmp(str, (*idl)->id) == 0)
	return(1);
    else if (caseblind && strcasecmp(str, (*idl)->id) == 0)
	return(1);
    else
	return(str_in_list(&(*idl)->next, str, caseblind));
}

int str_nr_in_list( struct idlist **idl, const char *str )
  /* return the position of str in idl */
{
    int nr;
    struct idlist *walk;
    if ( !str )
        return -1;
    for( walk = *idl, nr = 0; walk; nr ++, walk = walk->next )
        if( strcmp( str, walk->id) == 0 )
	    return nr;
    return -1;
}

int str_nr_last_in_list( struct idlist **idl, const char *str)
/* return the last position of str in idl */
{
    int nr, ret = -1;
    struct idlist *walk;
    if ( !str )
        return -1;
    for( walk = *idl, nr = 0; walk; nr ++, walk = walk->next )
        if( strcmp( str, walk->id) == 0 )
	    ret = nr;
    return ret;
}

void str_set_mark( struct idlist **idl, const char *str, const flag val)
/* update the mark on an of an id to given value */
{
    int nr;
    struct idlist *walk;
    if (!str)
        return;
    for(walk = *idl, nr = 0; walk; nr ++, walk = walk->next)
        if (strcmp(str, walk->id) == 0)
	    walk->val.status.mark = val;
}

int count_list( struct idlist **idl)
/* count the number of elements in the list */
{
  if( !*idl )
    return 0;
  return 1 + count_list( &(*idl)->next );
}

char *str_from_nr_list(struct idlist **idl, int number)
/* return the number'th string in idl */
{
    if( !*idl  || number < 0)
        return 0;
    if( number == 0 )
        return (*idl)->id;
    return str_from_nr_list(&(*idl)->next, number-1);
}

    
char *str_find(struct idlist **idl, int number)
/* return the id of the given number in the given list. */
{
    if (*idl == (struct idlist *) 0)
	return((char *) 0);
    else if (number == (*idl)->val.status.num)
	return((*idl)->id);
    else
	return(str_find(&(*idl)->next, number));
}

char *idpair_find(struct idlist **idl, const char *id)
/* return the id of the given id in the given list (caseblind comparison) */
{
    if (*idl == (struct idlist *) 0)
	return((char *) 0);
    else if (strcasecmp(id, (*idl)->id) == 0)
	return((*idl)->val.id2 ? (*idl)->val.id2 : (*idl)->id);
    else
	return(idpair_find(&(*idl)->next, id));
}

int delete_str(struct idlist **idl, int num)
/* delete given message from given list */
{
    struct idlist	*idp;

    for (idp = *idl; idp; idp = idp->next)
	if (idp->val.status.num == num)
	{
	    idp->val.status.mark = UID_DELETED;
	    return(1);
	}
    return(0);
}

void append_str_list(struct idlist **idl, struct idlist **nidl)
/* append nidl to idl (does not copy *) */
{
    if ((*nidl) == (struct idlist *)NULL || *nidl == *idl)
	return;
    else if ((*idl) == (struct idlist *)NULL)
	*idl = *nidl;
    else if ((*idl)->next == (struct idlist *)NULL)
	(*idl)->next = *nidl;
    else if ((*idl)->next != *nidl)
	append_str_list(&(*idl)->next, nidl);
}

#ifdef POP3_ENABLE
void expunge_uids(struct query *ctl)
/* assert that all UIDs marked deleted have actually been expunged */
{
    struct idlist *idl;

    for (idl = ctl->newsaved; idl; idl = idl->next)
	if (idl->val.status.mark == UID_DELETED)
	    idl->val.status.mark = UID_EXPUNGED;
}

void update_str_lists(struct query *ctl)
/* perform end-of-query actions on UID lists */
{
    free_str_list(&ctl->oldsaved);
    free_str_list(&scratchlist);
    ctl->oldsaved = ctl->newsaved;
    ctl->newsaved = (struct idlist *) NULL;

    if (ctl->server.uidl && outlevel >= O_DEBUG)
    {
	struct idlist *idp;

	report_build(stdout, "New UID list from %s:", ctl->server.pollname);
	for (idp = ctl->oldsaved; idp; idp = idp->next)
	    report_build(stdout, " %s = %d", idp->id, idp->val.status.mark);
	if (!idp)
	    report_build(stdout, " <empty>");
	report_complete(stdout, "\n");
    }
}

void write_saved_lists(struct query *hostlist, const char *idfile)
/* perform end-of-run write of seen-messages list */
{
    int		idcount;
    FILE	*tmpfp;
    struct query *ctl;
    struct idlist *idp;

    /* if all lists are empty, nuke the file */
    idcount = 0;
    for (ctl = hostlist; ctl; ctl = ctl->next) {
	if (ctl->oldsaved)
	    idcount++;
    }

    /* either nuke the file or write updated last-seen IDs */
    if (!idcount && !scratchlist)
    {
	if (outlevel >= O_DEBUG)
	    report(stdout, "Deleting fetchids file.\n");
	unlink(idfile);
    }
    else
	if ((tmpfp = fopen(idfile, "w")) != (FILE *)NULL) {
	    for (ctl = hostlist; ctl; ctl = ctl->next) {
		for (idp = ctl->oldsaved; idp; idp = idp->next)
		    if (idp->val.status.mark == UID_SEEN
				|| idp->val.status.mark == UID_DELETED)
			fprintf(tmpfp, "%s@%s %s\n", 
			    ctl->remotename, ctl->server.truename, idp->id);
	    }
	    for (idp = scratchlist; idp; idp = idp->next)
		fputs(idp->id, tmpfp);
	    fclose(tmpfp);
	}
}
#endif /* POP3_ENABLE */

/* uid.c ends here */
