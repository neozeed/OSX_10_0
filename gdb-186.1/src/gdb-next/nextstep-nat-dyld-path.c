#include "nextstep-nat-dyld-path.h"

#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>

#include "defs.h"
#include "inferior.h"
#include "environ.h"

#define assert CHECK_FATAL

/* Declarations of functions used only in this file. */

static char *build_suffix_name (const char *name, const char *suffix);
static char *search_for_name_in_path (const char *name, const char *path, 
                                    const char *suffix);
static const char *look_back_for_slash (const char *name, const char *p);
static const char *get_framework_pathname (const char *name, const char *type, int with_suffix);

/* look_back_for_slash() is passed a string name and an end point in name to
   start looking for '/' before the end point.  It returns a pointer to the
   '/' back from the end point or NULL if there is none. */

static const char *look_back_for_slash (const char *name, const char *p)
{
  for (p = p - 1; p >= name; p--) {
    if (*p == '/')
      return p;
  }
  return NULL;
}
/* build_suffix_name returns the proper suffix'ed name for NAME,
   putting SUFFIX before .dylib, if it is the suffix for NAME,
   and just appending it otherwise.  The return value is malloc'ed,
   and it is up to the caller to free it.  If SUFFIX is NULL, then
   this returns NULL.  */

static char *
build_suffix_name (const char *name, const char *suffix)
{
  size_t suffixlen = strlen (suffix);
  size_t namelen = strlen(name);
  char *name_with_suffix;

  if (suffixlen > 0) 
    {
      char *tmp;
      name_with_suffix = xmalloc (namelen + suffixlen + 1);
      if (namelen < 7)
        tmp = NULL;
      else
        tmp = strrchr(name, '.');
        
      if (tmp != NULL && strcmp (tmp, ".dylib") == 0) 
        {
          int baselen = namelen - 6;
          memcpy (name_with_suffix, name, baselen);
          tmp = name_with_suffix + baselen; 
          memcpy (tmp, suffix, suffixlen);
          tmp += suffixlen;
          memcpy (tmp, ".dylib", 6);
          *(tmp + 6) = '\0';
        }
      else
        {
          memcpy (name_with_suffix, name, namelen);
          tmp = name_with_suffix + namelen;
          memcpy (tmp, suffix, suffixlen);
          *(tmp + suffixlen) = '\0';
        }
      return name_with_suffix;
    }
  else
    {
      return NULL;
    }
}

/* search_for_name_in_path() is used in searching for name in the
  DYLD_LIBRARY_PATH or the DYLD_FRAMEWORK_PATH.  It is passed a name
  and a path and returns the name of the first combination that exist
  or NULL if none exists.  */

static char *
search_for_name_in_path
(const char *name, const char *path, const char *suffix)
{
  char *dylib_name;
  char *name_with_suffix;
  int name_with_suffix_len;
  const char *p, *cur;
  size_t curlen;
  size_t namelen;
  size_t pathlen;
  struct stat stat_buf;

  namelen = strlen (name);
  pathlen = strlen (path);

  /* Prebuild the name with suffix */
  if (suffix)
    {
      name_with_suffix = build_suffix_name (name, suffix);
      name_with_suffix_len = strlen (name_with_suffix);
      dylib_name = xmalloc (name_with_suffix_len + pathlen + 2);
    }
  else
    {
      name_with_suffix = NULL;
      name_with_suffix_len = 0;
      dylib_name = xmalloc (namelen + pathlen + 2);
    }
  

  /* Now cruise on through the path, trying the name_with_suffix, and then
     the name, with each path element */
               
  cur = path;
  
  for (;;) {

    p = strchr (cur, ':');
    if (p == NULL) {
      p = strchr (cur, '\0');
    }
    assert (p != NULL);
    curlen = p - cur;
    
    /* Skip empty path elements... */
    
    if (curlen != 0) {
      memcpy (dylib_name, cur, curlen);
      dylib_name[curlen] = '/';
    
      if (name_with_suffix != NULL)
        {
          memcpy (dylib_name + curlen + 1, name_with_suffix, name_with_suffix_len);
          dylib_name[curlen + 1 + name_with_suffix_len] = '\0';
          if (stat(dylib_name, &stat_buf) == 0) 
            {
               free (name_with_suffix);
               return dylib_name;
            }
        }
      
      memcpy (dylib_name + curlen + 1, name, namelen);
      dylib_name[curlen + 1 + namelen] = '\0';

        if (stat (dylib_name, &stat_buf) == 0) 
          {
            if (name_with_suffix)
              free (name_with_suffix);
            return dylib_name;
          }
      }
    
    if (*p == '\0') {
      break;
    }
    cur = p + 1;
    if (*cur == '\0') {
      break;
    }
  }

  free (dylib_name);
  if (name_with_suffix)
    free (name_with_suffix);
  
  return NULL;
}

/* get_framework_pathname() is passed a name of a dynamic library and
   returns a pointer to the start of the framework name if one exist or
   NULL none exists.  A framework name can take one of the following two
   forms: Foo.framework/Versions/A/Foo Foo.framework/Foo Where A and Foo
   can be any string.  */

static const char *
get_framework_pathname (const char *name, const char *type, int with_suffix)
{
  const char *foo, *a, *b, *c, *d, *suffix;
  unsigned long l, s;

  /* pull off the last component and make foo point to it */
  a = strrchr(name, '/');
  if(a == NULL)
    return(NULL);
  if(a == name)
    return(NULL);
  foo = a + 1;
  l = strlen(foo);
	
  /* look for suffix if requested starting with a '_' */
  if(with_suffix){
    suffix = strrchr(foo, '_');
    if(suffix != NULL){
      s = strlen(suffix);
      if(suffix == foo || s < 2)
	suffix = NULL;
      else
	l -= s;
    }
  }

  /* first look for the form Foo.framework/Foo */
  b = look_back_for_slash(name, a);
  if(b == NULL){
    if(strncmp(name, foo, l) == 0 &&
       strncmp(name + l, type, sizeof(type)-1 ) == 0)
      return(name);
    else
      return(NULL);
  }
  else{
    if(strncmp(b+1, foo, l) == 0 &&
       strncmp(b+1 + l, type, sizeof(type)-1 ) == 0)
      return(b+1);
  }

  /* next look for the form Foo.framework/Versions/A/Foo */
  if(b == name)
    return(NULL);
  c = look_back_for_slash(name, b);
  if(c == NULL ||
     c == name ||
     strncmp(c+1, "Versions/", sizeof("Versions/")-1) != 0)
    return(NULL);
  d = look_back_for_slash(name, c);
  if(d == NULL){
    if(strncmp(name, foo, l) == 0 &&
       strncmp(name + l, type, sizeof(type)-1 ) == 0)
      return(name);
    else
      return(NULL);
  }
  else{
    if(strncmp(d+1, foo, l) == 0 &&
       strncmp(d+1 + l, type, sizeof(type)-1 ) == 0)
      return(d+1);
    else
      return(NULL);
  }
}

void dyld_library_basename (const char *path, const char **s, unsigned int *len, int *is_framework, int *is_bundle)
{
  const char *p = NULL;
  const char *q = NULL;

  if (is_framework != NULL) {
    *is_framework = 0;
  }
  if (is_bundle != NULL) {
    *is_bundle = 0;
  }

  p = get_framework_pathname (path, ".framework/", 1);
  if (p != NULL) {

    q = strrchr (path, '/');
    assert (q != NULL);
    assert (*q++ == '/');
    *s = q;
    *len = strlen (q);
    if (is_framework != NULL) {
      *is_framework = 1;
    }
    if (is_bundle != NULL) {
      *is_bundle = 0;
    }

    return;
  }

  p = get_framework_pathname (path, ".bundle/", 1);
  if (p != NULL) {

    q = strrchr (path, '/');
    assert (q != NULL);
    assert (*q++ == '/');
    *s = q;
    *len = strlen (q);
    if (is_framework != NULL) {
      *is_framework = 0;
    }
    if (is_bundle != NULL) {
      *is_bundle = 1;
    }

    return;
  }

  q = strrchr (path, '/');
  if (q != NULL) {
    assert (*q++ == '/');
    *s = q;
    *len = strlen (q);
    return;
  }

  *s = path;
  *len = strlen (path);
  return;
}

char *dyld_resolve_image (const struct dyld_path_info *d, const char *dylib_name)
{
  struct stat stat_buf;

  const char *framework_name = NULL;
  const char *framework_name_suffix = NULL;
  const char *library_name = NULL;

  char *framework_path = NULL;

  framework_name = get_framework_pathname (dylib_name, ".framework/", 0);
  framework_name_suffix = get_framework_pathname (dylib_name, ".framework/", 1);

  library_name = strrchr (dylib_name, '/');
  if ((library_name != NULL) && (library_name[1] != '\0')) {
    library_name++;
  } else {
    library_name = dylib_name;
  }

  /* If d->framework_path is set and this dylib_name is a
     framework name, use the first file that exists in the framework
     path, if any.  If none exist, go on to search the
     d->library_path if any.  The first call to get_framework_pathname()
     tries to get a name without a suffix, the second call tries with
     a suffix. */

  if (d->framework_path != NULL) {

    if (framework_name != NULL)
      {
        framework_path = search_for_name_in_path (framework_name, 
                                                  d->framework_path, 
                                                  d->image_suffix);
        if (framework_path != NULL) 
          {
	      return framework_path;
          }
      }
  
    if (framework_name_suffix != NULL) 
      {
        framework_path = search_for_name_in_path (framework_name_suffix, 
                                                  d->framework_path, 
                                                  d->image_suffix);
        if (framework_path != NULL) 
          {
	     return framework_path;
          }
      }
  }

  /* If d->library_path is set, then use the first file that
     exists in the path.  If none exist, use the original name. The
     string d->library_path points to is "path1:path2:path3" and
     comes from the enviroment variable DYLD_LIBRARY_PATH.  */

  if (d->library_path != NULL) 
    {
      framework_path = search_for_name_in_path (library_name, d->library_path,
                                              d->image_suffix);
    if (framework_path != NULL) 
      {
        return framework_path;
      }
  }
  
  /* Now try to open the dylib_name (remembering to try the suffix first).  
     If it fails and we have not previously tried to search for a name then 
     try searching the fall back paths (including the default fall back 
     framework path). */
  
  if (d->image_suffix) 
    {
      char *suffix_name;

      suffix_name = build_suffix_name (dylib_name, d->image_suffix);
      if (stat (suffix_name, &stat_buf) == 0)
        {
          return suffix_name;
        }
      else
        {
          free (suffix_name);
        }
    }

  if (stat (dylib_name, &stat_buf) == 0) 
    {
      return strsave (dylib_name);
    }

  /* First try the the d->fallback_framework_path if that has
     been set (first without a suffix and then with a suffix). */

  if (d->fallback_framework_path != NULL) 
    {

      if (framework_name != NULL)
        {
          framework_path = search_for_name_in_path (framework_name, 
                                                    d->fallback_framework_path, 
                                                    d->image_suffix);
        if (framework_path != NULL) 
          {
	     return framework_path;
          }
        }
  
      if (framework_name_suffix != NULL) 
        {
          framework_path = search_for_name_in_path (framework_name_suffix, 
                                                    d->fallback_framework_path, 
                                                    d->image_suffix);
        if (framework_path != NULL) 
          {
	     return framework_path;
          }
        }
    }

  /* If no new name is still found try d->fallback_library_path if
     that was set.  */

  if (d->fallback_library_path != NULL) 
    {
      framework_path = search_for_name_in_path (library_name, 
                                                d->fallback_library_path, 
                                                d->image_suffix);
      if (framework_path != NULL) 
        {
          return framework_path;
        }
    }

  return NULL;
}

void dyld_init_paths (dyld_path_info *d)
{
  char *home;

  const char *default_fallback_framework_path = 
    "%s/Library/Frameworks:"
    "/Local/Library/Frameworks:"
    "/Network/Library/Frameworks:"
    "/System/Library/Frameworks";

  const char *default_fallback_library_path = 
    "%s/lib:"
    "/usr/local/lib:"
    "/lib:"
    "/usr/lib";

  /* Neither framework path searching nor library insertion is done
     for setuid programs which are not run by the real user.  */

  if ((getuid() == geteuid()) && (getgid() == getegid())) {

    d->framework_path = get_in_environ (inferior_environ, 
                                        "DYLD_FRAMEWORK_PATH");
    if (d->framework_path != NULL) 
      { 
        d->framework_path = strsave (d->framework_path); 
      }

    d->library_path = get_in_environ (inferior_environ, "DYLD_LIBRARY_PATH");
    if (d->library_path != NULL) 
      { 
        d->library_path = strsave (d->library_path); 
      }

    d->fallback_framework_path = get_in_environ (inferior_environ, 
						 "DYLD_FALLBACK_FRAMEWORK_PATH");
    if (d->fallback_framework_path != NULL) 
      { 
        d->fallback_framework_path = strsave (d->fallback_framework_path); 
      }

    d->fallback_library_path = get_in_environ (inferior_environ, 
					       "DYLD_FALLBACK_LIBRARY_PATH");
    if (d->fallback_library_path != NULL) 
      { 
        d->fallback_library_path = strsave (d->fallback_library_path); 
      }

    d->image_suffix = get_in_environ (inferior_environ, "DYLD_IMAGE_SUFFIX");
    if (d->image_suffix != NULL) 
      { 
        d->image_suffix = strsave (d->image_suffix); 
      }

    d->insert_libraries = get_in_environ (inferior_environ, 
					  "DYLD_INSERT_LIBRARIES");
    if (d->insert_libraries != NULL) 
      { 
        d->insert_libraries = strsave (d->insert_libraries); 
      }
  }
  
  home = get_in_environ (inferior_environ, "HOME");
  if (home != NULL) 
    { 
      home = strsave (home);
    }
    
  if (home == NULL) 
    {
      home = strsave ("/");
    }
    
  if (d->fallback_framework_path == NULL) 
    {
      d->fallback_framework_path = 
	xmalloc (strlen (default_fallback_framework_path) 
                                            + strlen (home) + 1);
      sprintf (d->fallback_framework_path, default_fallback_framework_path, 
	       home);
    }

  if (d->fallback_library_path == NULL) 
    {
      d->fallback_library_path = 
	xmalloc (strlen (default_fallback_library_path) 
                                          + strlen (home) + 1);
      sprintf (d->fallback_library_path, default_fallback_library_path, home);
    }
    
}
