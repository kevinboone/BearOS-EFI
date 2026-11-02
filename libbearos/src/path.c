/*============================================================================

  path.c

  Path handling functions.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

/*
#include "defs.h" 
#include "string8.h" 
#include "list.h" 
#include "compat_limits.h" 
#include "compat_stdlib.h" 
#include "compat_stdio.h" 
*/
#include <libbearos/bearos_defs.h> 
#include <libbearos/path.h> 
#include <libbearos/string8.h> 
#include <limits.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/stat.h> 

struct _Path
  {
  String8 *path; 
  };

/*============================================================================

  path_append

============================================================================*/
extern void path_append (Path *self, const UTF8 *p)
  {
  if (p[0])
    {
    if (p[0] == '/')
      {
      // Note that this is safe on a multibyte string, since the / byte 
      //   can't appear anywhere in UTF8 except as itself
      string8_destroy (self->path);
      self->path = string8_new ((UTF8 *)p);
      }
    else
      {
      const UTF8 *orig = string8_cstr (self->path);
      if (strcmp ((char *)orig, "/") == 0)
        {
        // Root is a special case
        string8_append (self->path, p);
        }
      else
        {
        int l = strlen ((char *)orig);
        if (orig[l - 1] == '/')
          {
          string8_append (self->path, p);
          }
        else
          {
          string8_append (self->path, (UTF8 *)"/");
          string8_append (self->path, p);
          }
        }
      }
    }
  // Otherwise, path is empty
  }

/*============================================================================

  path_canonicalize

============================================================================*/
int path_canonicalize (Path *self)
  {
  int ret;
  UTF8 *s = (UTF8 *)realpath ((char *)path_cstr (self),  0);
  if (s)
    {
    string8_destroy (self->path);
    self->path = string8_new (s);
    free (s);
    ret = 0;
    }
  else
    {
    // errno should have been set
    ret = -1;
    } 
  return ret;
  }

/*============================================================================

  path_destroy

============================================================================*/
void path_destroy (Path *self)
  {
  if (self)
    {
    if (self->path)
      string8_destroy (self->path); 
    free (self);
    }
  }

/*============================================================================

  string8_cstr

============================================================================*/
UTF8 *path_cstr (const Path *self)
  {
  return string8_cstr (self->path);
  }

/*============================================================================

  path_is_directory  

============================================================================*/
BOOL path_is_directory (const Path *self)
  {
  struct stat sb;
  if (stat ((char *)path_cstr (self), &sb) != 0)
    return FALSE;

  return (sb.st_mode & S_IFMT) == S_IFDIR; 
  }

/*============================================================================

  path_is_file

============================================================================*/
BOOL path_is_file (const Path *self)
  {
  struct stat sb;
  if (stat ((char *)path_cstr (self), &sb) != 0)
    return FALSE;

  return (sb.st_mode & S_IFMT) == S_IFREG;
  }

/*============================================================================

  path_stat

============================================================================*/
extern int path_stat (const Path *self, struct stat *sb)
  {
  return stat ((char *)path_cstr (self), sb);
  }

/*============================================================================

  string8_new

============================================================================*/
Path *path_new (const UTF8 *s)
  {
  Path *self = malloc (sizeof (Path));
  self->path = string8_new (s); 
  return self;
  }

/*============================================================================

  path_to_efi 

============================================================================*/
#ifndef LINUX
CHAR16 *path_to_efi (const UTF8 *pathname)
  {
  // TODO TODO TODO -- proper UTF8 -> UTF16 conversion
  int l = strlen ((char *)pathname);
  CHAR16* ret = malloc ((l + 1) * sizeof (CHAR16));
  for (int i = 0; i < l; i++) 
    {
    // Ugh!
    if (pathname[i] == '/')
      ret[i] = '\\';
    else 
      ret[i] = pathname[i];
    }
  ret [l] = 0;
  return ret;
  }
#endif


/*============================================================================

  path_from_efi 

============================================================================*/
#ifndef LINUX
extern UTF8 *path_from_efi (const CHAR16 *pathname)
  {
  const CHAR16 *s = pathname;
  UTF8 out[PATH_MAX];
  UTF8 *r = out;
  while (*s)
    {
    int c = *s;
    if (c == '\\')
      *r = '/'; 
    else
      *r = (UTF8)*s; // URGH!
    s++;
    r++;
    }
  *r = 0;
  return (UTF8 *)strdup ((char *)out);
  }
#endif


