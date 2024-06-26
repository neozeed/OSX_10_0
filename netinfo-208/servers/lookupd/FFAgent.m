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

/*
 * FFAgent.m
 *
 * Flat File agent for lookupd
 *
 * Copyright (c) 1995, NeXT Computer Inc.
 * All rights reserved.
 * Written by Marc Majka
 */

#import "FFAgent.h"
#import <NetInfo/system_log.h>
#import <stdio.h>
#import <sys/types.h>
#import <sys/param.h>
#import <sys/stat.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>
#import <string.h>
#import <stdlib.h>
#import <NetInfo/dsutil.h>

#define BUFSIZE 8192

extern char *nettoa(unsigned long);

static FFAgent *_sharedFFAgent = nil;

char *categoryFilename[] =
{
#ifdef _UNIX_BSD_43_
	"passwd",
#else
	"master.passwd",
#endif
	"group",
	"hosts",
	"networks",
	"services",
	"protocols",
	"rpc",
	"fstab",
	"printcap",
	"bootparams",
	"bootptab",
	"aliases",
	NULL,
	"ethers",
	"netgroup",
	NULL,
	NULL
};

@implementation FFAgent

- (FFAgent *)init
{
	if (didInit) return self;

	[super init];

	stats = [[LUDictionary alloc] init];
	[stats setBanner:"FFAgent statistics"];
	[stats setValue:"Flat_File" forKey:"information_system"];
	etcDir = copyString("/etc");
	parser = [[FFParser alloc] init];
	[parser setBanner:"FFAgent flat file parser"];

	return self;

	threadLock = syslock_new(0);

	return self;
}

+ (FFAgent *)alloc
{
	if (_sharedFFAgent != nil)
	{
		[_sharedFFAgent retain];
		return _sharedFFAgent;
	}

	_sharedFFAgent = [super alloc];
	_sharedFFAgent = [_sharedFFAgent init];
	if (_sharedFFAgent == nil) return nil;

	system_log(LOG_DEBUG, "Allocated FFAgent 0x%08x\n", (int)_sharedFFAgent);

	return _sharedFFAgent;
}

- (char *)getLineFromFile:(FILE *)fp category:(LUCategory)cat
{
	char s[BUFSIZE];
	char c;
	char *out;
	BOOL getNextLine;
	int len;

    s[0] = '\0';

    fgets(s, BUFSIZE, fp);
    if (s == NULL || s[0] == '\0') return NULL;

	if (s[0] == '#')
	{
		out = copyString("#");
		return out;
	}

	len = strlen(s) - 1;
	s[len] = '\0';

	out = copyString(s);

	/* only printcap, bootparams, and aliases can continue on multiple lines */
	if ((cat != LUCategoryPrinter) &&
		(cat != LUCategoryBootparam) &&
		(cat != LUCategoryAlias))
	{
		return out;
	}

	if (cat == LUCategoryAlias)
	{
		/* alias continues if next line starts with whitespace */
		c = getc(fp);
		while ((c == ' ') || (c == '\t'))
		{
			fgets(s, BUFSIZE, fp);
			if (s == NULL || s[0] == '\0') return out;

			len = strlen(s) - 1;
			s[len] = '\0';
			out = concatString(out, s);

			c = getc(fp);
		}

		/* hit next line - unread a character */
		if (c != EOF) fseek(fp, -1, SEEK_CUR);

		return out;
	}

	/* printcap and bootparams continue if last char is a backslash */
	getNextLine = out[len - 1] == '\\';
	if (getNextLine) out[--len] = '\0';

	while (getNextLine)
	{
		fgets(s, BUFSIZE, fp);
		if (s == NULL || s[0] == '\0') return out;

		len = strlen(s) - 1;
		s[len] = '\0';
		
		getNextLine = s[len - 1] == '\\';
		if (getNextLine) s[--len] = '\0';

		out = concatString(out, s);
	}

	return out;
}

- (LUDictionary *)stamp:(LUDictionary *)item
	file:(char *)name
	time:(long)ts
{
	char str[64];

	[item setAgent:self];
	[item setValue:"Flat_File" forKey:"_lookup_info_system"];

	if (name == NULL) return item;

	[item setValue:name forKey:"_lookup_FF_file"];
	sprintf(str, "%ld", ts);
	[item setValue:str forKey:"_lookup_FF_timestamp"];
	return item;
}

- (BOOL)isValid:(LUDictionary *)item
{
	time_t ts;
	char *fileName;
	char *timeStamp;
	struct stat st;

	if (item == nil) return NO;

	fileName = [item valueForKey:"_lookup_FF_file"];
	if (fileName == NULL) return NO;

	timeStamp = [item valueForKey:"_lookup_FF_timestamp"];
	if (timeStamp == NULL) return NO;

	ts = (time_t)atol(timeStamp);

	if (stat(fileName, &st) < 0) return NO;

	if (ts != st.st_mtime) return NO;

	return YES;
}

- (LUDictionary *)itemWithKeys:(char **)keys
	values:(char **)values
	category:(LUCategory)cat
{
	char *fname;
	FILE *fp;
	char *line;
	LUDictionary *item = nil;
	char **vals;
	char fpath[MAXPATHLEN + 1];
	int i, len;
	BOOL match;
	struct stat st;
	long ts;

	fname = categoryFilename[cat];
	if (fname == NULL) return nil;

	ts = 0;

	if (etcDir != NULL)
	{
		sprintf(fpath, "%s/%s", etcDir, fname);

		if (stat(fpath, &st) < 0) return nil;
		ts = st.st_mtime;
	}

	syslock_lock(threadLock); // locked [[[[[

	if (etcDir != NULL) fp = fopen(fpath, "r");
	else fp = stdin;

	if (fp == NULL)
	{
		syslock_unlock(threadLock); // ] unlocked
		return nil;
	}

	/* bootptab entries start after a "%%" line */
	if (cat == LUCategoryBootp)
	{
		while (NULL != (line = [self getLineFromFile:fp category:cat]))
		{
			if (!strncmp(line, "%%", 2)) break;
			freeString(line);
			line = NULL;
		}
		if (line == NULL)
		{
			fclose(fp);
			syslock_unlock(threadLock); // ] unlocked
			return nil;
		}

		freeString(line);
		line = NULL;
	}

	len = listLength(keys);
	if (listLength(values) != len)
	{
		fclose(fp);
		syslock_unlock(threadLock); // ] unlocked
		return nil;
	}

	while (NULL != (line = [self getLineFromFile:fp category:cat]))
	{
		if (line[0] == '#') 
		{
			freeString(line);
			line = NULL;
			continue;
		}

		item = [parser parse_A:line category:cat];

		freeString(line);
		line = NULL;

		if (item == nil) continue;

		match = YES;

		for (i = 0; (i < len) && match; i++)
		{
			vals = [item valuesForKey:keys[i]];
			if (vals == NULL)
			{
				match = NO;
				continue;
			}

			if (listIndex(values[i], vals) == IndexNull)
			{
				match = NO;
				continue;
			}
		}

		if (match)
		{
			fclose(fp);
			syslock_unlock(threadLock); // ] unlocked

			return [self stamp:item file:fpath time:ts];
		}

		[item release];
		item = nil;
	}

	fclose(fp);
	syslock_unlock(threadLock); // ] unlocked
	return nil;
}

- (LUDictionary *)itemWithKey:(char *)key
	value:(char *)val
	category:(LUCategory)cat
{
	LUDictionary *item;
	char **k = NULL;
	char **v = NULL;

	k = appendString(key, k);
	v = appendString(val, v);

	item = [self itemWithKeys:k values:v category:(LUCategory)cat];
	if (cat == LUCategoryAlias) [item addValue:"1" forKey:"alias_local"];

	freeList(k);
	freeList(v);

	return item;
}

- (LUArray *)allItemsWithCategory:(LUCategory)cat
{
	char *fname;
	FILE *fp;
	char *line;
	LUDictionary *item;
	LUDictionary *vstamp;
	LUArray *all;
	char fpath[MAXPATHLEN + 1];
	char scratch[MAXPATHLEN + 1];
	struct stat st;
	long ts;

	fname = categoryFilename[cat];
	if (fname == NULL) return nil;

	ts = 0;

	if (etcDir != NULL)
	{
		sprintf(fpath, "%s/%s", etcDir, fname);

		if (stat(fpath, &st) < 0) return nil;
		ts = st.st_mtime;
	}

	syslock_lock(threadLock); // locked [[[[[

	if (etcDir != NULL) fp = fopen(fpath, "r");
	else fp = stdin;

	if (fp == NULL)
	{
		syslock_unlock(threadLock); // ] unlocked
		return nil;
	}

	/* bootptab entries start after a "%%" line */
	if (cat == LUCategoryBootp)
	{
		while (NULL != (line = [self getLineFromFile:fp category:cat]))
		{
			if (!strncmp(line, "%%", 2)) break;
			freeString(line);
			line = NULL;
		}
		if (line == NULL)
		{
			fclose(fp);
			syslock_unlock(threadLock); // ] unlocked
			return nil;
		}

		freeString(line);
		line = NULL;
	}

	all = [[LUArray alloc] init];

	vstamp = [[LUDictionary alloc] init];
	sprintf(scratch, "FFAgent validation %s %s", fpath, ctime(&ts));
	[vstamp setBanner:scratch];
	[self stamp:vstamp file:fpath time:ts];
	[all addValidationStamp:vstamp];
	[vstamp release];

	while (NULL != (line = [self getLineFromFile:fp category:cat]))
	{
		if (line[0] == '#')
		{
			freeString(line);
			line = NULL;
			continue;
		}

		item = [parser parse_A:line category:cat];

		freeString(line);
		line = NULL;
		if (item == nil) continue;
		if (cat == LUCategoryAlias) [item addValue:"1" forKey:"alias_local"];
		[all addObject:[self stamp:item file:fpath time:ts]];
		[item release];
	}

	fclose(fp);
	syslock_unlock(threadLock); // ] unlocked
	return all;
}

- (const char *)serviceName
{
	return "Flat File";
}

- (const char *)shortName
{
	return "FF";
}

- (void)dealloc
{
	if (stats != nil) [stats release];
	freeString(etcDir);
	etcDir = NULL;
	if (parser != nil) [parser release];
	syslock_free(threadLock);

	system_log(LOG_DEBUG, "Deallocated FFAgent 0x%08x\n", (int)self);

	[super dealloc];

	_sharedFFAgent = nil;
}

- (LUDictionary *)statistics
{
	return stats;
}

- (void)resetStatistics
{
	if (stats != nil) [stats release];
	stats = [[LUDictionary alloc] init];
	[stats setBanner:"FFAgent statistics"];
	[stats setValue:"Flat_File" forKey:"information_system"];
}

- (void)setDirectory:(char *)dir
{
	if (dir == NULL) return;
	freeString(etcDir);
	etcDir = copyString(dir);
}

- (LUArray *)allGroupsWithUser:(char *)name
{
	LUArray *allWithUser;
	LUArray *all;
	LUDictionary *user;
	LUDictionary *group;
	LUDictionary *vstamp;
	char **vals;
	int i, len, nvals;
	char fpath[MAXPATHLEN + 1];
	char scratch[MAXPATHLEN + 1];
	struct stat st;
	long ts;

	all = [self allItemsWithCategory:LUCategoryGroup];
	if (all == nil) return nil;

	len = [all count];
	if (len == 0)
	{
		[all release];
		return nil;
	}

	allWithUser = [[LUArray alloc] init];

	/* first get the user's default group(s) */
	sprintf(fpath, "%s/master.passwd", etcDir);
	if (stat(fpath, &st) < 0) ts = 0;
	else ts = st.st_mtime;

	vstamp = [[LUDictionary alloc] init];
	sprintf(scratch, "FFAgent validation %s %s", fpath, ctime(&ts));
	[vstamp setBanner:scratch];
	[self stamp:vstamp file:fpath time:ts];
	[allWithUser addValidationStamp:vstamp];
	[vstamp release];

	user = [self itemWithKey:"name" value:name category:LUCategoryUser];
	if (user != nil)
	{
		vals = [user valuesForKey:"gid"];
		if (vals != NULL)
		{
			nvals = [user countForKey:"gid"];
			if (nvals < 0) nvals = 0;

			for (i = 0; i < nvals; i++)
			{
				group = [self itemWithKey:"gid" value:vals[i]
					category:LUCategoryGroup];

				if (group == nil) continue;

				if ([allWithUser containsObject:group])
				{
					[group release];
					continue;
				}
				[allWithUser addObject:group];
				[group release];
			}
		}
		[user release];
	}

	/* get groups with this user as a member */
	sprintf(fpath, "%s/group", etcDir);
	if (stat(fpath, &st) < 0) ts = 0;
	else ts = st.st_mtime;

	vstamp = [[LUDictionary alloc] init];
	sprintf(scratch, "FFAgent validation %s %s", fpath, ctime(&ts));
	[vstamp setBanner:scratch];
	[self stamp:vstamp file:fpath time:ts];
	[allWithUser addValidationStamp:vstamp];
	[vstamp release];

	for (i = 0; i < len; i ++)
	{
		group = [all objectAtIndex:i];
		vals = [group valuesForKey:"users"];
		if (vals == NULL) continue;
		if (listIndex(name, vals) == IndexNull)
			continue;

		if ([allWithUser containsObject:group])
			continue;

		[allWithUser addObject:group];
	}

	[all release];

	len = [allWithUser count];
	if (len == 0)
	{
		[allWithUser release];
		allWithUser = nil;
	}

	return allWithUser;
}

- (LUDictionary *)serviceWithName:(char *)name
	protocol:(char *)prot
{
	LUDictionary *item;
	char **k = NULL;
	char **v = NULL;

	k = appendString("name", k);
	v = appendString(name, v);
	if (prot != NULL)
	{
		k = appendString("protocol", k);
		v = appendString(prot, v);
	}
	
	item = [self itemWithKeys:k values:v category:LUCategoryService];

	freeList(k);
	k = NULL;

	freeList(v);
	v = NULL;

	return item;
}

- (LUDictionary *)serviceWithNumber:(int *)number
	protocol:(char *)prot
{
	char str[32];
	LUDictionary *item;
	char **k = NULL;
	char **v = NULL;

	sprintf(str, "%d", *number);

	k = appendString("port", k);
	v = appendString(str, v);
	if (prot != NULL)
	{
		k = appendString("protocol", k);
		v = appendString(prot, v);
	}
	
	item = [self itemWithKeys:k values:v category:LUCategoryService];

	freeList(k);
	k = NULL;

	freeList(v);
	v = NULL;

	return item;
}

@end

