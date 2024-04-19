/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include <NetInfo/dsstatus.h>
#include "dsstore.h"
#include "nistore.h"
#include "dsindex.h"
#include "dsdata.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>

#define DataStoreAccessMode 0700
#define ConfigFileName "Config"
#define IndexFileName "Index"
#define TempIndexFileName "TempIndex"
#define StoreFileBase "Store"
#define CreateFileName "Create"
#define TempFileName "Temp"
#define DeleteFileName "Delete"
#define CleanFileName "Clean"

#define NAME_INDEX_KEY "index_key"
#define NAME_NAME "name"

#define ReservedFileNameLength 64

#define DataQuantum 32

static char zero[DataQuantum];

#define MIN_CACHE_SIZE 100
#define MAX_CACHE_SIZE 100

#define forever for(;;)

#define Quantize(X) ((((X) + DataQuantum - 1) / DataQuantum) * DataQuantum)
static dsstatus store_save_data(dsstore *, dsrecord *, dsdata *);

/*
 * Index file contains these entries
 *     dsid = record id
 *     vers = record version
 *     size = quantized size
 *     where = index into Store.<size> file for this record
 *
 * Update INDEX_ENTRY_SIZE if you change this structure.
 */

#define INDEX_ENTRY_SIZE 16

typedef struct {
	u_int32_t dsid;
	u_int32_t vers;
	u_int32_t size;
	u_int32_t where;
} store_index_entry_t;

/*
 * Insert a new entry into a store's memory index.
 */
static void
dsstore_index_insert(dsstore *s, store_index_entry_t *x, int replace)
{
	unsigned int i, top, bot, mid, range;
	store_index_entry_t *e, *t, *b, *m;

	if (s == NULL) return;
	if (x == NULL) return;
	if (x->dsid == IndexNull) return;
	if (x->size == 0) return;

	e = (store_index_entry_t *)malloc(INDEX_ENTRY_SIZE);
	memmove(e, x, INDEX_ENTRY_SIZE);

	if (s->index_count == 0)
	{
		s->index = (void **)malloc(sizeof(void *));
		s->index[s->index_count] = e;
		s->index_count++;
		return;
	}

	top = s->index_count - 1;
	bot = 0;
	mid = top / 2;

	range = top - bot;
	while (range > 1)
	{
		m = s->index[mid];
		if (e->dsid == m->dsid)
		{
			if (replace)
			{
				s->index[mid] = e;
				free(m);
			}
			else
			{
				free(e);
			}
			return;
		}
		else if (e->dsid < m->dsid) top = mid;
		else bot = mid;

		range = top - bot;
		mid = bot + (range / 2);
	}

	t = s->index[top];
	if (e->dsid == t->dsid)
	{
		if (replace)
		{
			s->index[top] = e;
			free(t);
		}
		else
		{
			free(e);
		}
		return;
	}

	b = s->index[bot];
	if (e->dsid == b->dsid)
	{
		if (replace)
		{
			s->index[bot] = e;
			free(b);
		}
		else
		{
			free(e);
		}
		return;
	}

	if (e->dsid < b->dsid) mid = bot;
	else if (e->dsid > t->dsid) mid = top + 1;
	else mid = top;

	s->index_count++;
	s->index = (void **)realloc(s->index, sizeof(void *) * s->index_count);
	for (i = s->index_count - 1; i > mid; i--) s->index[i] = s->index[i - 1];
	s->index[mid] = e;
}

/*
 * Find an entry from a store's memory index.
 */
static u_int32_t
dsstore_index_lookup(dsstore *s, u_int32_t dsid)
{
	unsigned int top, bot, mid, range;
	store_index_entry_t *t, *b, *m;

	if (s == NULL) return IndexNull;
	if (s->index_count == 0) return IndexNull;

	top = s->index_count - 1;
	bot = 0;
	mid = top / 2;

	range = top - bot;
	while (range > 1)
	{
		m = s->index[mid];
		if (dsid == m->dsid) return mid;
		else if (dsid < m->dsid) top = mid;
		else bot = mid;

		range = top - bot;
		mid = bot + (range / 2);
	}

	t = s->index[top];
	if (dsid == t->dsid) return top;

	b = s->index[bot];
	if (dsid == b->dsid) return bot;
	return IndexNull;
}

/*
 * Delete an entry from a store's memory index.
 */
static void
dsstore_index_delete(dsstore *s, u_int32_t dsid)
{
	u_int32_t where, i;

	if (s == NULL) return;
	where = dsstore_index_lookup(s, dsid);
	if (where == IndexNull) return;

	free(s->index[where]);

	if (s->index_count == 1)
	{
		s->index_count = 0;
		free(s->index);
		s->index = NULL;
		return;
	}

	for (i = where + 1; i < s->index_count; i++)
		s->index[i - 1] = s->index[i];

	s->index_count--;
	s->index = (void **)realloc(s->index, s->index_count * sizeof(void *));	
}

#ifdef NOTDEF
/*
 * Find an entry with a specific version in the store's memory index.
 */
static u_int32_t
dsstore_version_lookup(dsstore *s, u_int32_t vers)
{
	u_int32_t i;
	store_index_entry_t *e;

	for (i = 0; i < s->index_count; i++)
	{
		e = s->index[i];
		if (e->vers == vers) return i;
	}

	return IndexNull;
}
#endif

/*
 * Find a record with a specific version.
 */
u_int32_t
dsstore_version_record(dsstore *s, u_int32_t vers)
{
	u_int32_t i;
	store_index_entry_t *e;

	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_version_record(s, vers);

	for (i = 0; i < s->index_count; i++)
	{
		e = s->index[i];
		if (e->vers == vers) return e->dsid;
	}

	return IndexNull;
}

/*
 * Debugging utility to print the index.
 */
void
dsstore_print_index(dsstore *s, FILE *f)
{
	store_index_entry_t *e;
	int i;

	if (s == NULL) return;
	if (f == NULL) return;


	fprintf(f, "     #      ID   Vers   Size  Index\n");

	for (i = 0; i < s->index_count; i++)
	{
		e = (store_index_entry_t *)s->index[i];
		fprintf(f, "%6d: %6u %6u %6u %6u\n",
			i, e->dsid, e->vers, e->size, e->where);
	}
}

/*
 * Open a file in the store.
 */
static FILE *
dsstore_fopen(dsstore *s, char *name, char *mode)
{
	char path[MAXPATHLEN + 1];

	if (s == NULL) return fopen(name, mode);

	if ((strlen(s->dsname) + strlen(name) + 1) > MAXPATHLEN) return NULL;
	sprintf(path, "%s/%s", s->dsname, name);
	return fopen(path, mode);
}

/*
 * Stat a file in the store.
 */
static int
dsstore_stat(dsstore *s, char *name, struct stat *sb)
{
	char path[MAXPATHLEN + 1];

	if (s == NULL) return stat(name, sb);
	
	if ((strlen(s->dsname) + strlen(name) + 1) > MAXPATHLEN)
		return DSStatusWriteFailed;
	sprintf(path, "%s/%s", s->dsname, name);
	return stat(path, sb);
}

/*
 * Unlink a file in the store.
 */
static int
dsstore_unlink(dsstore *s, char *name)
{
	char path[MAXPATHLEN + 1];

	if (s == NULL) return unlink(name);
	
	if ((strlen(s->dsname) + strlen(name) + 1) > MAXPATHLEN)
		return DSStatusWriteFailed;
	sprintf(path, "%s/%s", s->dsname, name);
	return unlink(path);
}

/*
 * Rename a file in the store.
 */
static int
dsstore_rename(dsstore *s, char *old, char *new)
{
	char pold[MAXPATHLEN + 1];
	char pnew[MAXPATHLEN + 1];

	if (s == NULL) return rename(old, new);
	
	if ((strlen(s->dsname) + strlen(old) + 1) > MAXPATHLEN)
		return DSStatusWriteFailed;

	if ((strlen(s->dsname) + strlen(new) + 1) > MAXPATHLEN)
		return DSStatusWriteFailed;

	sprintf(pold, "%s/%s", s->dsname, old);
	sprintf(pnew, "%s/%s", s->dsname, new);

	return rename(pold, pnew);
}

/*
 * Open a Store.<size> file.
 */
static FILE *
dsstore_store_fopen(dsstore *s, u_int32_t size)
{
	char path[MAXPATHLEN + 1];
	char str[40];
	struct stat sb;

	sprintf(str, "%u", size);
	if ((strlen(s->dsname) + strlen(StoreFileBase) + strlen(str) + 1) > MAXPATHLEN)
		return NULL;

	sprintf(path, "%s/%s.%u", s->dsname, StoreFileBase, size);

	if (0 != stat(path, &sb)) return fopen(path, "a+");
	return fopen(path, "r+");
}

static dsstatus
dsstore_write_index(dsstore *s)
{
	store_index_entry_t *e, x;
	int i, y;
	FILE *f;

	if (s == NULL) return DSStatusInvalidStore;

	dsstore_unlink(s, IndexFileName);
	dsstore_unlink(s, TempIndexFileName);

	f = dsstore_fopen(s, TempIndexFileName, "w");
	if (f == NULL) return DSStatusWriteFailed;

	y = IndexNull;

	for (i = 0; i < s->index_count; i++)
	{
		e = (store_index_entry_t *)s->index[i];
		
		/* More paraniod integrity checking */
		if ((i > 0) && (y >= e->dsid)) return DSStatusInvalidStore;
		if (e->size == 0) return DSStatusInvalidStore;
		y = e->dsid;

		x.dsid = htonl(e->dsid);
		x.vers = htonl(e->vers);
		x.size = htonl(e->size);
		x.where = htonl(e->where);

		if (fwrite(&x, INDEX_ENTRY_SIZE, 1, f) != 1) return DSStatusWriteFailed;
	}
	fclose(f);
	
	return dsstore_rename(s, TempIndexFileName, IndexFileName);
}

/*
 * Utilitys for checking flags.
 */
static int
dsstore_access_readwrite(dsstore *s)
{
	if ((s->flags & DSSTORE_FLAGS_ACCESS_MASK) == DSSTORE_FLAGS_ACCESS_READWRITE)
		return 1;
	return 0;
}

/*
 * Open a Data Store
 * Will creates the store if necessary.
 */
dsstatus
dsstore_new(dsstore **s, char *dirname, u_int32_t flags)
{
	int r;
	u_int32_t i, newstore;
	FILE *f;
	struct stat sb;
	char path[MAXPATHLEN + 1];
	
	newstore = 0;

	/*
	 * Make sure there's enough path space to create files in the store.
	 * ReservedFileNameLength is long enough to fit "Store." plus an
	 * integer string big enough for 128bit integer.  I.E. There is
	 * no possible chance of overflowing MAXPATHLEN, even though we
	 * explicitly check all path lengths everywhere they appear.
	 */
	if ((strlen(dirname) + ReservedFileNameLength + 1) > MAXPATHLEN)
		return DSStatusWriteFailed;

	if (0 != dsstore_stat(NULL, dirname, &sb))
	{
		newstore = 1;
		r = mkdir(dirname, DataStoreAccessMode);
		if (r != 0) return DSStatusWriteFailed;
	}

	sprintf(path, "%s/%s", dirname, IndexFileName);

	/*
	 * Don't create an empty Index if the store exists!
	 * Crash-recovery will re-create it.
	 */
	if ((newstore == 1) && (0 != dsstore_stat(NULL, path, &sb)))
	{
		f = fopen(path, "w");
		if (f == NULL) return DSStatusWriteFailed;
		fclose(f);
	}

	sprintf(path, "%s/%s", dirname, ConfigFileName);

	if (0 != dsstore_stat(NULL, path, &sb))
	{
		f = fopen(path, "w");
		if (f == NULL) return DSStatusWriteFailed;
		i = htonl(DSSTORE_VERSION);
		r = fwrite(&i, 4, 1, f);
		fclose(f);
		if (r != 1) return DSStatusWriteFailed;
	}

	return dsstore_open(s, dirname, flags);
}

/*
 * Reads a Store file and adds index entries.
 * Removes the store file if it is empty.
 */
static dsstatus
index_recovery_read(dsstore *s, u_int32_t size)
{
	FILE *f;
	u_int32_t  where;
	off_t offset;
	store_index_entry_t e;
	dsrecord *r;
	int count, is_empty;
	struct stat sb;
	char store[64];

	if (s == NULL) return DSStatusInvalidStore;
	if (size == 0) return DSStatusOK;

	sprintf(store, "%s.%d", StoreFileBase, size);
	if (dsstore_stat(s, store, &sb) != 0) return DSStatusReadFailed;

	e.size = size;
	count = sb.st_size / size;

	f = dsstore_fopen(s, store, "r");
	if (f == NULL) return DSStatusReadFailed;

	is_empty = 1;
	r = NULL;

	offset = 0;
	for (e.where = 0; e.where < count; e.where++)
	{
		if (fseek(f, offset, SEEK_SET) != 0) break;
		offset += size;

		r = dsrecord_fread(f);
		if (r == NULL) continue;

		if (r->dsid != IndexNull) is_empty = 0;

		where = dsstore_index_lookup(s, r->dsid);
		if (where != IndexNull)
		{
			dsrecord_release(r);
			return DSStatusDuplicateRecord;
		}

		e.dsid = r->dsid;
		e.vers = r->vers;
		dsstore_index_insert(s, &e, 0);

		s->nichecksum += ((r->dsid + 1) * r->serial);

		if (e.vers > s->max_vers) s->max_vers = e.vers;

		dsrecord_release(r);
	}

	fclose(f);

	if (is_empty) return DSStatusNoData;
	return DSStatusOK;
}

/*
 * Recreates the Index file by reading all the store files.
 * Also calculates the nichecksum.
 */
static dsstatus
index_recovery(dsstore *s)
{
	DIR *dp;
	struct direct *d;
	int i, len;
	u_int32_t size;
	dsstatus status;
	int *unlink_list, unlink_count;
	char store[64], *p;

	if (s == NULL) return DSStatusInvalidStore;

	dp = opendir(s->dsname);
	if (dp == NULL) return DSStatusReadFailed;

	unlink_count = 0;
	unlink_list = NULL;

	len = strlen(StoreFileBase);

	while ((d = readdir(dp)))
	{
		if (!strncmp(d->d_name, StoreFileBase, len))
		{
			p = strrchr(d->d_name, '.');
			if (p == NULL) continue;
			size = atoi(p+1);
			if (size == 0) continue;

			status = index_recovery_read(s, size);
			if (status == DSStatusNoData)
			{
				if (unlink_count == 0)
				{
					unlink_list = (int *)malloc(sizeof(int));
				}
				else
				{
					unlink_list = (int *)realloc(unlink_list, (unlink_count + 1) * sizeof(int));
				}
				unlink_list[unlink_count] = size;
				unlink_count++;
			}
			else if (status != DSStatusOK)
			{
				closedir(dp);
				if (unlink_count > 0) free(unlink_list);
				return status;
			}
		}
	}

	closedir(dp);
	
	for (i = 0; i < unlink_count; i++)
	{
		sprintf(store, "%s.%d", StoreFileBase, unlink_list[i]);
		dsstore_unlink(s, store);
	}

	if (unlink_count > 0) free(unlink_list);

	return DSStatusOK;
}

/*
 * Writes to the data store are done in four steps:
 * 1: A copy of the new record is written to the Temp file.
 * 2: Temp is renamed as Create.
 * 3: The record is written into the Store file.
 * 4: The Create file is deleted.
 *
 * If a write failed while the record was being written to the Temp
 * file, or before the Temp file was renamed, we recover by removing
 * the Temp file. The data store reverts to the old version of the record.
 * 
 * If the Create file exists, we can be sure that its contents were correctly
 * written.  The write to the store may have failed, so the old or possibly
 * corrupted store record is deleted and the record is copied from the Create
 * file to the store.
 */
static dsstatus
create_recovery(dsstore *s)
{
	dsrecord *r;
	u_int32_t dsid, pdsid, i, len, parent_ok;
	dsdata *d;
	FILE *f;
	dsstatus status;
	
	/* Read record from Create file */
	f = dsstore_fopen(s, CreateFileName, "r");
	if (f == NULL)
	{
		/* Can't recover record */
		dsstore_unlink(s, CreateFileName);
		return DSStatusWriteFailed;
	}

	r = dsrecord_fread(f);
	fclose(f);
	if (r == NULL)
	{
		/* Can't recover record */
		dsstore_unlink(s, CreateFileName);
		return DSStatusWriteFailed;
	}

	/* Delete record from store */
	status = dsstore_remove(s, r->dsid);
	if (status == DSStatusInvalidPath)
	{
	}
	else if (status != DSStatusOK)
	{
		/* This should never happen! */
		dsstore_unlink(s, CreateFileName);
		return DSStatusWriteFailed;
	}

	dsid = r->dsid;
	pdsid = r->super;
	
	/* Write data to store */	
	d = dsrecord_to_dsdata(r);
	status = store_save_data(s, r, d);
	dsdata_release(d);
	if (status != DSStatusOK) return DSStatusWriteFailed;
	
	r = dsstore_fetch(s, pdsid);
	if (r == NULL) return DSStatusWriteFailed;

	parent_ok = 0;
	len = r->sub_count;
	for (i = 0; i < len; i++) if (r->sub[i] == dsid) parent_ok = 1;

	/* Update parent */
	if (parent_ok == 0)
	{
		dsrecord_append_sub(r, dsid);
		status = dsstore_save(s, r);
		dsrecord_release(r);
		if (status != DSStatusOK) return DSStatusWriteFailed;
	}
	else
	{
		dsrecord_release(r);
	}

	dsstore_unlink(s, CreateFileName);

	return DSStatusOK;
}

/*
 * Deletions from the data store are done in two steps.  First the
 * record ID is written to the Delete file, then the record is
 * deleted from the Store file.  If the deletion failed before the ID
 * is written to the Delete file, the data store reverts to the old
 * version of the record.  If the Delete file was written and the
 * deletion from the store failed, the store record is deleted.
 */
static dsstatus
delete_recovery(dsstore *s)
{
	int ret;
	u_int32_t dsid;
	dsstatus status;
	FILE *f;

	f = dsstore_fopen(s, DeleteFileName, "r");
	if (f == NULL)
	{
		dsstore_unlink(s, DeleteFileName);
		return DSStatusWriteFailed;
	}

	ret = fread(&dsid, sizeof(u_int32_t), 1, f);
	fclose(f);
	if (ret != 1)
	{
		dsstore_unlink(s, DeleteFileName);
		return DSStatusWriteFailed;
	}

	status = dsstore_remove(s, dsid);
	if (status == DSStatusInvalidPath) status = DSStatusOK;

	dsstore_unlink(s, DeleteFileName);
	return status;
}

/*
 * Recursively check the tree starting at the root directory, 
 * checking "super" back pointers.
 * If the tree is repaired at any point, the check re-starts at the root.
 */
#define TreeCheckOK 0
#define TreeCheckUpdateFailed 1
#define TreeCheckOrphanedChild 2
#define TreeCheckKidnappedChild 3

static u_int32_t
tree_check_child(dsstore *s, dsrecord *parent, dsrecord *child, int write_allowed)
{
	dsrecord *r;
	dsstatus status, status1;

	if (child->super == parent->dsid) return TreeCheckOK;
	if (write_allowed == 0) return TreeCheckUpdateFailed;

	/*
	 * Child doesn't know this parent. The child's link to it's parent
	 * overrides the parent's link to a child.
	 */
	r = dsstore_fetch(s, child->super);
	if (r == NULL)
	{
		/*
		 * "Real" parent doesn't exist.  We link the child to the
		 * parent that claimed it.
		 */
		child->super = parent->dsid;
		status = dsstore_save(s, child);
		if (status == DSStatusOK) return TreeCheckOrphanedChild;
		return TreeCheckUpdateFailed;
	}

	/* Remove child from "false" parent. */
	dsrecord_remove_sub(parent, child->dsid);
	status = dsstore_save(s, parent);

	/* Make sure child is attached to real parent. */
	dsrecord_append_sub(r, child->dsid);
	status1 = dsstore_save(s, r);

	dsrecord_release(r);

	if ((status == DSStatusOK) && (status1 == DSStatusOK))
		return TreeCheckKidnappedChild;
	return TreeCheckUpdateFailed;
}

static u_int32_t
tree_check_record(dsstore *s, dsrecord *r, int write_allowed)
{
	u_int32_t i, tc;
	dsrecord *c;
	dsstatus status;

	for (i = 0; i < r->sub_count; i++)
	{
		c = dsstore_fetch(s, r->sub[i]);
		if (c == NULL)
		{
			if (write_allowed == 0) return TreeCheckUpdateFailed;
			dsrecord_remove_sub(r, r->sub[i]);
			status = dsstore_save(s, r);
			if (status == DSStatusOK) 
			{
				/* Backup by one since r->sub[i] was removed. */
				i -= 1;
				continue;
			}
			return TreeCheckUpdateFailed;
		}

		tc = tree_check_child(s, r, c, write_allowed);
		/* If the child was an orphan, we can continue checking. */
		if (tc == TreeCheckOrphanedChild) tc = TreeCheckOK;
		if (tc != TreeCheckOK)
		{
			dsrecord_release(c);
			return tc;
		}
		
		tc = tree_check_record(s, c, write_allowed);
		dsrecord_release(c);
		/* Must fail back to the root dir, since the tree changed */
		if (tc != TreeCheckOK)
		return tc;
	}

	return TreeCheckOK;
}
			
static dsstatus
tree_check(dsstore *s, int write_allowed)
{
	dsrecord *r;
	u_int32_t tc;

	if (s->index_count == 0) return DSStatusOK;

	r = dsstore_fetch(s, 0);
	if (r == NULL) return DSStatusInvalidStore;

	tc = -1;
	while (tc != TreeCheckOK)
	{
		tc = tree_check_record(s, r, write_allowed);
		if (tc == TreeCheckUpdateFailed) return DSStatusInvalidStore;
	}

	dsrecord_release(r);

	return DSStatusOK;
}

static dsstatus
connection_check_record(dsstore *s, u_int32_t dsid, char *test)
{
	dsrecord *r, *p;
	u_int32_t c, n, i;
	dsstatus status;

	if (dsid == 0) return DSStatusOK;

	r = dsstore_fetch(s, dsid);
	if (r == NULL) return DSStatusInvalidRecordID;

	c = r->dsid;
	n = r->super;
	
	forever
	{
		if (n == dsid)
		{
			/* A cycle - break it and attach to root */
			n = 0;
			r->super = 0;
			status = dsstore_save(s, r);
		}

		p = dsstore_fetch(s, n);
		if (p == NULL)
		{
			/* Parent doesn't exist - attach to root */
			p = dsstore_fetch(s, 0);
		}

		if (dsrecord_has_sub(p, c) == 0)
		{
			dsrecord_append_sub(p, c);
			status = dsstore_save(s, p);
		}
		
		i = dsstore_index_lookup(s, n);
		if (i >= s->index_count) i = 0;
		c = n;
		n = p->super;

		dsrecord_release(r);
		r = p;

		if ((c == 0) || (test[i] != 0))
		{
			dsrecord_release(p);
			return DSStatusOK;
		}

		test[i] = 1;
	}

	return DSStatusOK;
}

static dsstatus
connection_check(dsstore *s)
{
	u_int32_t i;
	char *test;
	store_index_entry_t *e;
	dsstatus x, status;

	test = NULL;
	if (s->index_count > 0)
	{
		test = malloc(s->index_count);
		memset(test, 0, s->index_count);
	}

	x = DSStatusOK;
	for (i = 0; i < s->index_count; i++)
	{
		if (test[i] != 0) continue; 
		e = s->index[i];
		test[i] = 1;
		status = connection_check_record(s, e->dsid, test);
		if (status != DSStatusOK) x = status;
	}

	free(test);

	return x;
}

static dsstatus
dsstore_read_index(dsstore *s)
{
	struct stat sb;
	u_int32_t i, count, where, x;
	store_index_entry_t e;
	FILE *ifp;

	if (dsstore_stat(s, IndexFileName,  &sb) != 0)
	{
		/* This should never happen! */
		return DSStatusReadFailed;
	}

	count = sb.st_size / INDEX_ENTRY_SIZE;

	ifp = dsstore_fopen(s, IndexFileName, "r");
	if (ifp == NULL) return DSStatusReadFailed;

	x = IndexNull;

	for (i = 0; i < count; i++)
	{
		if (fread(&e, INDEX_ENTRY_SIZE, 1, ifp) != 1)
			return DSStatusReadFailed;

		e.dsid = ntohl(e.dsid);

		/* Index entries should be sorted */
		if ((i > 0) && (x >= e.dsid)) return DSStatusInvalidStore;
		x = e.dsid;

		where = dsstore_index_lookup(s, e.dsid);
		if (where != IndexNull) return DSStatusDuplicateRecord;

		e.vers = ntohl(e.vers);
		if (e.vers > s->max_vers) s->max_vers = e.vers;

		e.size = ntohl(e.size);
		/* Zero size is impossible */
		if (e.size == 0) return DSStatusInvalidStore;

		e.where = ntohl(e.where);

		dsstore_index_insert(s, &e, 0);
	}

	fclose(ifp);

	return DSStatusOK;
}

/*
 * Internal initializations.
 * - does create and delete disaster recovery
 * - reads the Clean file (contains nichecksum)
 * - reads the Index file
 * - re-creates the Index if necessary
 */
static dsstatus
dsstore_init(dsstore *s)
{
	dsstatus status;
	struct stat sb;
	u_int32_t i, where;
	int write_allowed;
	FILE *f;

	s->max_vers = 0;
	s->nichecksum = 0;

	s->index = NULL;

	s->fetch_count = 0;
	s->save_count = 0;
	s->remove_count = 0;

	write_allowed = dsstore_access_readwrite(s);
	s->dirty = 0;

	/* If Create or Delete file exists, we had a crash */
	if (dsstore_stat(s, CreateFileName, &sb) == 0) s->dirty++;
	if (dsstore_stat(s, DeleteFileName, &sb) == 0) s->dirty++;

	/* Can't start up read-only until someone has done crash-recovery */
	if ((s->dirty != 0) && (!write_allowed)) return DSStatusInvalidStore;

	/* If Clean file is missing or zero size, store is dirty */
	if (dsstore_stat(s, CleanFileName,  &sb) != 0) s->dirty++;
	else if (sb.st_size != sizeof(u_int32_t)) s->dirty++;

	/* If Index file is missing or zero size, store is dirty */
	if (dsstore_stat(s, IndexFileName,  &sb) != 0) s->dirty++;
	else if (sb.st_size == 0) s->dirty++;

	if (s->dirty == 0)
	{
		/* Store appears clean.  Read nichecksum from the Clean file */
		f = dsstore_fopen(s, CleanFileName, "r");
		i = fread(&(s->nichecksum), sizeof(u_int32_t), 1, f);
		fclose(f);
		if (i != 1) return s->dirty++;
	}
	
	if (s->dirty == 0)
	{
		/* Store appears clean.  Read Index file */
		status = dsstore_read_index(s);
		if (status != DSStatusOK)
		{
			/* Index was corrupt.  Clean up from the failed read */
			for (i = 0; i < s->index_count; i++) free(s->index[i]);
			if (s->index != NULL) free(s->index);
			s->index = NULL;
			s->index_count = 0;
			s->dirty++;
		}
	}

	if (s->dirty != 0)
	{
		/* Store is dirty. Read the store to recover the Index */
		/* N.B. index_recovery() sets nichecksum */
		status = index_recovery(s);
		if (status != DSStatusOK) return status;
	}	

	if (write_allowed)
	{
		/* If Temp file exists, we remove it */
		dsstore_unlink(s, TempFileName);
		dsstore_unlink(s, TempIndexFileName);

		/* Delete Index file.  We rewrite it from memory on shutdown. */
		dsstore_unlink(s, IndexFileName);
	}

	/* Check for Create file */
	if (dsstore_stat(s, CreateFileName, &sb) == 0)
	{
		/* Complete create operation */
		status = create_recovery(s);
		if (status != DSStatusOK) return status;
	}

	/* Check for Delete file */
	if (dsstore_stat(s, DeleteFileName, &sb) == 0)
	{
		/* Complete delete operation */
		status = delete_recovery(s);
		if (status != DSStatusOK) return status;
	}

	/* If the store was dirty, check the tree */
	if (s->dirty != 0)
	{
		status = tree_check(s, write_allowed);
		if (status != DSStatusOK) return status;
		
		status = connection_check(s);
		if (status != DSStatusOK) return status;
	}

	/* Check for root */
	if (s->index_count > 0)
	{
		where = dsstore_index_lookup(s, 0);
		if (where == IndexNull) return DSStatusNoRootRecord;
	}

	return DSStatusOK;
}

/*
 * Open an existing Data Store
 */
dsstatus
dsstore_open(dsstore **s, char *dirname, u_int32_t flags)
{
	dsstatus status;
	u_int32_t size, len;

	if (flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_open(s, dirname, flags);

	memset(zero, 0, DataQuantum);

	*s = (dsstore *)malloc(sizeof(dsstore));
	memset(*s, 0, sizeof(dsstore));

	len = strlen(dirname);
	(*s)->dsname = malloc(len + 1);
	memmove((*s)->dsname, dirname, len);
	(*s)->dsname[len] = '\0';

	(*s)->flags = flags;

	status = dsstore_init(*s);
	if (status != DSStatusOK) return status;

	/*
	 * Set cache size to be 10% of the store size, but
	 * not less than the minimum (MIN_CACHE_SIZE), and
	 * not greater than the maximum (MAX_CACHE_SIZE).
	 */
	size = (*s)->index_count / 10;
	if (size < MIN_CACHE_SIZE) size = MIN_CACHE_SIZE;
	if (size > MAX_CACHE_SIZE) size = MAX_CACHE_SIZE;

	(*s)->cache_enabled = 1;
	(*s)->cache = dscache_new(size);

	if (status != DSStatusOK) return status;

	return DSStatusOK;
}

dsstatus
dsstore_close(dsstore *s)
{
	u_int32_t i;
	FILE *f;
	dsstatus status;

	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_close(s);

	/*
	 * Dump memory index to file.
	 */
	status = dsstore_write_index(s);

	if ((status == DSStatusOK) && dsstore_access_readwrite(s))
	{
		/* Write the Clean file */
		f = dsstore_fopen(s, CleanFileName, "w");
		i = fwrite(&(s->nichecksum), sizeof(u_int32_t), 1, f);
		fflush(f);
		fclose(f);
		if (i != 1) status = DSStatusWriteFailed;
	}

	free(s->dsname);

	for (i = 0; i < s->index_count; i++) free(s->index[i]);
	if (s->index != NULL) free(s->index);

	dscache_free(s->cache);
	free(s);

	return status;
}

/*
 * Determines a record's version, quantized size, and index.
 */
static dsstatus
dsstore_index(dsstore *s, u_int32_t dsid, u_int32_t *vers, u_int32_t *size, u_int32_t *where)
{
	store_index_entry_t *e;
	u_int32_t i;

	i = dsstore_index_lookup(s, dsid);
	if (i == IndexNull) return DSStatusInvalidRecordID;

	e = (store_index_entry_t *)s->index[i];
	*vers = e->vers;
	*size = e->size;
	*where = e->where;

	return DSStatusOK;
}

/* 
 * Generate a record ID.
 * Returns smallest unused ID.  ID is added to the memory store,
 * but nothing is written to disk until dsstore_save() is called
 * with the a new record having this ID.
 */
u_int32_t
dsstore_create_dsid(dsstore *s)
{
	u_int32_t i, n;
	store_index_entry_t *e, x;

	if (s == NULL) return IndexNull;

	if (!dsstore_access_readwrite(s)) return IndexNull;

	n = 0;
	for (i = 0; i < s->index_count; i++)
	{
		e = s->index[i];
		if (n != e->dsid) break;
		n++;
	}

	x.dsid = n;
	x.vers = IndexNull;
	x.size = IndexNull;
	x.where = IndexNull;
	dsstore_index_insert(s, &x, 0);

	return n;
}

u_int32_t
dsstore_max_id(dsstore *s)
{
	u_int32_t i, m;
	store_index_entry_t *e;

	if (s == NULL) return IndexNull;

	m = 0;
	for (i = 0; i < s->index_count; i++)
	{
		e = s->index[i];
		if (e->dsid > m) m = e->dsid;
	}

	return m;
}

u_int32_t
dsstore_version(dsstore *s)
{
	if (s == NULL) return IndexNull;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_version(s);

	return s->max_vers;
}

/*
 * Add a new record to the store.
 * Do not use if the record is already in the store!
 */
static dsstatus
store_save_data(dsstore *s, dsrecord *r, dsdata *d)
{
	u_int32_t i, size, where, type, pad;
	store_index_entry_t e;
	off_t offset;
	FILE *f;
	dsstatus status;

	if (d == NULL) return DSStatusOK;

	size = Quantize(dsdata_size(d));
	if (size == 0) return DSStatusInvalidRecord;

	/* Open store file */
	f = dsstore_store_fopen(s, size);
	if (f == NULL) return DSStatusWriteFailed;

	/* Find an empty slot (type == DataTypeNil) in the store file */
	where = IndexNull;
	offset = 0;
	i = 0;
	
	while (1 == fread(&type, sizeof(u_int32_t), 1, f))
	{
		if (type == DataTypeNil)
		{
			where = i;
			break;
		}
		i++;
		offset = offset + size;
		if (fseek(f, offset, SEEK_SET) != 0)
		{
			fclose(f);
			return DSStatusWriteFailed;
		}
	}

	if (where == IndexNull)
	{
		if (fseek(f, 0, SEEK_END) != 0)
		{
			fclose(f);
			return DSStatusWriteFailed;
		}
		where = i;
	}
	else
	{
		offset = where * size;
		if (fseek(f, offset, SEEK_SET) != 0)
		{
			fclose(f);
			return DSStatusWriteFailed;
		}
	}

	/* Remove the Clean file when we dirty the store */
	if (s->dirty == 0)
	{
		s->dirty = 1;
		dsstore_unlink(s, CleanFileName);
	}

	/* write data to store file */
	status = dsdata_fwrite(d, f);
	if (status != DSStatusOK)
	{
		fclose(f);
		return status;
	}

	pad = size - dsdata_size(d);
	if (pad > 0)
	{
		if (fwrite(zero, pad, 1, f) != 1)
		{
			fclose(f);
			return DSStatusWriteFailed;
		}
	}

	fclose(f);

	/* update memory index */
	e.dsid = r->dsid;
	e.vers = r->vers;
	e.size = size;
	e.where = where;
	dsstore_index_insert(s, &e, 1);

	/* Save in cache */
	if (s->cache_enabled == 1) dscache_save(s->cache, r);

	return DSStatusOK;
}

/*
 * 1: Write record to Temp file
 * 2: Rename Temp as Create
 * 3: Delete existing record from store
 * 4: Write new record to store
 * 5: Remove Create file
 */
dsstatus
dsstore_save(dsstore *s, dsrecord *r)
{
	FILE *f;
	dsdata *d;
	dsstatus status;

	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_save(s, r);

	if (!dsstore_access_readwrite(s)) return DSStatusWriteFailed;
	if (r == NULL) return DSStatusOK;

	s->save_count++;

	/* Set record ID if it is a new record */
	if (r->dsid == IndexNull)
		r->dsid = dsstore_create_dsid(s);

	/* Update version numbers */
	s->max_vers++;
	r->vers = s->max_vers;
	r->serial++;

	/* Update nichecksum */
	s->nichecksum += ((r->dsid + 1) * r->serial);

	d = dsrecord_to_dsdata(r);
	if (d == NULL) return DSStatusInvalidRecord;

	/* Write record to Temp file */
	f = dsstore_fopen(s, TempFileName, "w");
	if (f == NULL)
	{
		dsdata_release(d);
		return DSStatusWriteFailed;
	}

	/* Remove the Clean file when we dirty the store */
	if (s->dirty == 0)
	{
		s->dirty = 1;
		dsstore_unlink(s, CleanFileName);
	}

	status = dsdata_fwrite(d, f);
	fclose(f);
	if (status != DSStatusOK)
	{
		dsdata_release(d);
		dsstore_unlink(s, TempFileName);
		return status;
	}

	/* Rename Temp file as Create file */
	status = dsstore_rename(s, TempFileName, CreateFileName);
	if (status != DSStatusOK)
	{
		dsdata_release(d);
		return status;
	}

	/* Delete record from store */
	status = dsstore_remove(s, r->dsid);
	if (status == DSStatusInvalidPath)
	{
	}
	else if (status != DSStatusOK)
	{
		dsdata_release(d);
		return status;
	}

	/* Write data to store */
	status = store_save_data(s, r, d);
	dsdata_release(d);
	if (status != DSStatusOK) return status;

	/* Remove Create file */
	dsstore_unlink(s, CreateFileName);
	return DSStatusOK;
}

/*
 * Like dsstore_save() but does not write temporary files nor
 * does it update the record serial number.
 */
dsstatus
dsstore_save_copy(dsstore *s, dsrecord *r)
{
	dsdata *d;
	dsstatus status;

	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_save(s, r);

	if (!dsstore_access_readwrite(s)) return DSStatusWriteFailed;
	if (r == NULL) return DSStatusOK;

	s->save_count++;

	/* Set record ID if it is a new record */
	if (r->dsid == IndexNull)
		r->dsid = dsstore_create_dsid(s);

	/* Update version numbers */
	s->max_vers++;
	r->vers = s->max_vers;

	/* Update nichecksum */
	s->nichecksum += ((r->dsid + 1) * r->serial);

	d = dsrecord_to_dsdata(r);
	if (d == NULL) return DSStatusInvalidRecord;

	/* Remove the Clean file when we dirty the store */
	if (s->dirty == 0)
	{
		s->dirty = 1;
		dsstore_unlink(s, CleanFileName);
	}

	/* Delete record from store */
	status = dsstore_remove(s, r->dsid);
	if (status == DSStatusInvalidPath)
	{
	}
	else if (status != DSStatusOK)
	{
		dsdata_release(d);
		return status;
	}

	/* Write data to store */
	status = store_save_data(s, r, d);
	dsdata_release(d);
	return status;
}

dsstatus
dsstore_save_attribute(dsstore *s, dsrecord *r, dsattribute *a, u_int32_t asel)
{
	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_save_attribute(s, r, a, asel);

	return dsstore_save(s, r);
}

dsstatus
dsstore_authenticate(dsstore *s, dsdata *user, dsdata *password)
{
	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_authenticate(s, user, password);

	return DSStatusOK;
}

/*
 * 1: Write record id to Delete file
 * 2: delete record from store
 * 3: delete Delete file
 */
dsstatus
dsstore_remove(dsstore *s, u_int32_t dsid)
{
	u_int32_t i, size, where;
	off_t offset;
	int status;
	store_index_entry_t *e;
	char *z, fstore[64];
	FILE *f;
	struct stat sb;
	dsrecord *r;

	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_remove(s, dsid);

	if (!dsstore_access_readwrite(s)) return DSStatusWriteFailed;

	i = dsstore_index_lookup(s, dsid);
	if (i == IndexNull) return DSStatusOK;

	s->remove_count++;

	/* Delete from cache */
	if (s->cache_enabled == 1) dscache_remove(s->cache, dsid);

	e = s->index[i];
	size = e->size;
	where = e->where;

	dsstore_index_delete(s, dsid);

	if ((size == 0) || (size == IndexNull)) return DSStatusOK;

	f = dsstore_fopen(s, DeleteFileName, "w");
	if (f == NULL) return DSStatusWriteFailed;

	/* Remove the Clean file when we dirty the store */
	if (s->dirty == 0)
	{
		s->dirty = 1;
		dsstore_unlink(s, CleanFileName);
	}

	status = fwrite(&dsid, sizeof(u_int32_t), 1, f);
	fclose(f);
	if (status != 1)
	{
		dsstore_unlink(s, DeleteFileName);
		return DSStatusWriteFailed;
	}

	sprintf(fstore, "%s.%d", StoreFileBase, size);
	if (0 != dsstore_stat(s, fstore, &sb))
	{
		/* Store.<size> doesn't exist.  This should never happen. */
		dsstore_unlink(s, DeleteFileName);
		return DSStatusWriteFailed;
	}

	/* Open store file */
	f = dsstore_store_fopen(s, size);
	if (f == NULL)
	{
		dsstore_unlink(s, DeleteFileName);
		return DSStatusWriteFailed;
	}

	offset = where * size;
	status = fseek(f, offset, SEEK_SET);
	if (status != 0)
	{
		fclose(f);
		dsstore_unlink(s, DeleteFileName);
		return DSStatusWriteFailed;
	}

	/* Read record and update nichecksum */
	r = dsrecord_fread(f);
	s->nichecksum -= ((dsid + 1) * r->serial);
	dsrecord_release(r);

	status = fseek(f, offset, SEEK_SET);
	z = malloc(size);
	memset(z, 0, size);
	status = fwrite(z, size, 1, f);
	free(z);
	fclose(f);

	dsstore_unlink(s, DeleteFileName);

	if (status != 1) return DSStatusWriteFailed;
	return DSStatusOK;
}

/* 
 * Fetch a record from the store
 */
dsrecord *
dsstore_fetch(dsstore *s, u_int32_t dsid)
{
	u_int32_t vers;
	u_int32_t size, where;
	off_t offset;
	dsstatus status;
	dsrecord *r;
	FILE *f;

	if (s == NULL) return NULL;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_fetch(s, dsid);

	s->fetch_count++;

	/* Check the cache first */
	if (s->cache_enabled == 1)
	{
		r = dscache_fetch(s->cache, dsid);
		if (r != NULL) return r;
	}
	
	status = dsstore_index(s, dsid, &vers, &size, &where);
	if (status != DSStatusOK) return NULL;

	/* Open store file */
	f = dsstore_store_fopen(s, size);
	if (f == NULL) return NULL;

	offset = where * size;
	if (fseek(f, offset, SEEK_SET) != 0)
	{
		fclose(f);
		return NULL;
	}

	r = dsrecord_fread(f);
	fclose(f);

	/* Save in cache */
	if (s->cache_enabled == 1) dscache_save(s->cache, r);
	return r;
}

/* 
 * Get record's version, serial, and parent.
 */
dsstatus
dsstore_vital_statistics(dsstore *s, u_int32_t dsid, u_int32_t *vers, u_int32_t *serial, u_int32_t *super)
{
	u_int32_t v, size, where;
	off_t offset;
	dsstatus status;
	dsrecord *r;
	FILE *f;

	if (s == NULL) return DSStatusInvalidStore;

	if (vers != NULL) *vers = IndexNull;
	if (serial != NULL) *serial = IndexNull;
	if (super != NULL) *super = IndexNull;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_vital_statistics(s, dsid, vers, serial, super);

	/* Check the cache */
	if (s->cache_enabled == 1)
	{
		r = dscache_fetch(s->cache, dsid);
		if (r != NULL) 
		{
			if (vers != NULL) *vers = r->vers;
			if (serial != NULL) *serial = r->serial;
			if (super != NULL) *super = r->super;

			dsrecord_release(r);
			return DSStatusOK;
		}
	}
	
	status = dsstore_index(s, dsid, &v, &size, &where);
	if (status != DSStatusOK) return status;

	/* Open store file */
	f = dsstore_store_fopen(s, size);
	if (f == NULL) return DSStatusReadFailed;

	offset = where * size;
	if (fseek(f, offset, SEEK_SET) != 0)
	{
		fclose(f);
		return DSStatusReadFailed;
	}

	status = dsrecord_fstats(f, &where, vers, serial, super);
	fclose(f);

	return status;
}

/*
 * Find a child record with the specified key (or meta-key) and value.
 * Input parameters: dsid is the ID of the parent record. key is the
 * attribute key or meta-key (as determined by asel), and val is the 
 * value to match.  The ID of the first record that matches is returned
 * in "match".
 */
dsstatus
dsstore_match(dsstore *s, u_int32_t dsid, dsdata *key, dsdata *val, u_int32_t asel, u_int32_t *match)
{
	dsrecord *r, *k;
	dsattribute *a, *a_index_key;
	u_int32_t i, j, index_this;
	dsdata *d_name, *d_index_key;
	dsindex_key_t *kx;
	dsindex_val_t *vx;

	*match = IndexNull;

	if (s == NULL) return DSStatusInvalidStore;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_match(s, dsid, key, val, asel, match);

	r = dsstore_fetch(s, dsid);
	if (r == NULL) return DSStatusInvalidPath;
	if (r->sub_count == 0)
	{
		dsrecord_release(r);
		return DSStatusOK;
	}

	/*
	 * If the record has an index for this key, use it to find a match.
	 */
	kx = dsindex_lookup_key(r->index, key);
	if (kx != NULL)
	{
		vx = dsindex_lookup_val(kx, val);
		if (vx != NULL)
		{
			if (vx->dsid_count > 0) *match = vx->dsid[0];
		}

		dsrecord_release(r);
		return DSStatusOK;
	}

	/*
	 * Not indexed, so we have to read the child records to look for a match.
	 * While reading the child records we create an index for the "name" key,
	 * and any key listed as a value of the record's "index_key" meta-attribute.
	 */
	d_name = cstring_to_dsdata(NAME_NAME);
	d_index_key = cstring_to_dsdata(NAME_INDEX_KEY);
	a_index_key = dsrecord_attribute(r, d_index_key, SELECT_META_ATTRIBUTE);
	dsdata_release(d_index_key);

	r->index = dsindex_new();
	dsindex_insert_key(r->index, d_name);
	if (a_index_key != NULL)
	{
		for (i = 0; i < a_index_key->count; i++)
			dsindex_insert_key(r->index, a_index_key->value[i]);
	}

	for (i = 0; i < r->sub_count; i++)
	{
		k = dsstore_fetch(s, r->sub[i]);
		if (k == NULL) return DSStatusInvalidPath;

		for (j = 0; j < k->count; j++)
		{
			index_this = 0;
			a = k->attribute[j];
			if (dsdata_equal(d_name, a->key)) index_this = 1;
			else if (dsattribute_index(a_index_key, a->key) != IndexNull) index_this = 1;

			if (index_this == 1)
				dsindex_insert_attribute(r->index, a, r->sub[i]);

			if (*match == IndexNull)
			{
				if (dsdata_equal(key, a->key))
				{
					if (dsattribute_index(a, val) != IndexNull)
						*match = r->sub[i];
				}
			}
		}

		dsrecord_release(k);
	}

	dsattribute_release(a_index_key);
	dsdata_release(d_name);
	dscache_save(s->cache, r);
	dsrecord_release(r);
	return DSStatusOK;
}

u_int32_t
dsstore_record_version(dsstore *s, u_int32_t dsid)
{
	dsstatus status;
	u_int32_t vers, size, where;

	if (s == NULL) return IndexNull;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_record_version(s, dsid);

	status = dsstore_index(s, dsid, &vers, &size, &where);
	if (status != DSStatusOK) return IndexNull;
	return vers;
}

u_int32_t
dsstore_record_serial(dsstore *s, u_int32_t dsid)
{
	dsstatus status;
	u_int32_t serial;

	if (s == NULL) return IndexNull;
	
	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_record_serial(s, dsid);

	status = dsstore_vital_statistics(s, dsid, NULL, &serial, NULL);
	if (status != DSStatusOK) return IndexNull;
	return serial;
}

u_int32_t
dsstore_record_super(dsstore *s, u_int32_t dsid)
{
	dsstatus status;
	u_int32_t super;

	if (s == NULL) return IndexNull;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_record_super(s, dsid);

	status = dsstore_vital_statistics(s, dsid, NULL, NULL, &super);
	if (status != DSStatusOK) return IndexNull;
	return super;
}

u_int32_t
dsstore_nichecksum(dsstore *s)
{
	if (s == NULL) return 0;
	return s->nichecksum;
}

void
dsstore_flush_cache(dsstore *s)
{
	if (s == NULL) return;
	dscache_flush(s->cache);
}

/* Resets all version numbers to zero. */
void
dsstore_reset(dsstore *s)
{
	store_index_entry_t *e;
	dsrecord *r;
	int i, ce;

	if (s == NULL) return;
	if (!dsstore_access_readwrite(s)) return;

	ce = s->cache_enabled;
	s->cache_enabled = 0;
	dscache_flush(s->cache);

	for (i = 0; i < s->index_count; i++)
	{
		e = (store_index_entry_t *)s->index[i];
		e->vers = 0;
		r = dsstore_fetch(s, e->dsid);
		if (r == NULL) continue;
		s->max_vers = IndexNull;
		dsstore_save(s, r);
	}

	s->cache_enabled = ce;
}

dsrecord *
dsstore_statistics(dsstore *s)
{
	dsrecord *r;
	dsattribute *a;
	dsdata *d;
	char str[64];

	if (s == NULL) return NULL;

	if (s->flags & DSSTORE_FLAGS_REMOTE_NETINFO)
		return nistore_statistics(s);

	r = dsrecord_new();

	d = cstring_to_dsdata("checksum");
	a = dsattribute_new(d);
	dsdata_release(d);
	sprintf(str, "%u", s->nichecksum);
	d = cstring_to_dsdata(str);
	dsattribute_append(a, d);
	dsdata_release(d);
	dsrecord_append_attribute(r, a, 0);
	dsattribute_release(a);
	
	d = cstring_to_dsdata("version");
	a = dsattribute_new(d);
	dsdata_release(d);
	sprintf(str, "%u", s->max_vers);
	d = cstring_to_dsdata(str);
	dsattribute_append(a, d);
	dsdata_release(d);
	dsrecord_append_attribute(r, a, 0);
	dsattribute_release(a);
	
	d = cstring_to_dsdata("max_dsid");
	a = dsattribute_new(d);
	dsdata_release(d);
	sprintf(str, "%u", dsstore_max_id(s));
	d = cstring_to_dsdata(str);
	dsattribute_append(a, d);
	dsdata_release(d);
	dsrecord_append_attribute(r, a, 0);
	dsattribute_release(a);
	
	d = cstring_to_dsdata("fetch_count");
	a = dsattribute_new(d);
	dsdata_release(d);
	sprintf(str, "%u", s->fetch_count);
	d = cstring_to_dsdata(str);
	dsattribute_append(a, d);
	dsdata_release(d);
	dsrecord_append_attribute(r, a, 0);
	dsattribute_release(a);
	
	d = cstring_to_dsdata("save_count");
	a = dsattribute_new(d);
	dsdata_release(d);
	sprintf(str, "%u", s->save_count);
	d = cstring_to_dsdata(str);
	dsattribute_append(a, d);
	dsdata_release(d);
	dsrecord_append_attribute(r, a, 0);
	dsattribute_release(a);
	
	d = cstring_to_dsdata("remove_count");
	a = dsattribute_new(d);
	dsdata_release(d);
	sprintf(str, "%u", s->remove_count);
	d = cstring_to_dsdata(str);
	dsattribute_append(a, d);
	dsdata_release(d);
	dsrecord_append_attribute(r, a, 0);
	dsattribute_release(a);
	
	return r;
}	
