/*============================================================================

  compat_stdlib.c

  Implementation of realpath() function, etc 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "defs.h"
#include "compat_stdio.h"
#include "compat_malloc.h"
#include "compat_stdlib.h"
#include "compat_unistd.h"
#include "compat_string.h"
#include "compat_limits.h"
#include "../../lib/include/errno_numbers.h"
#include "process.h"
#include "procmgr.h"

/*============================================================================

  realpath

============================================================================*/
char *realpath (const char *path, char *got_path)
  {
  // TODO

  if (path == 0) 
    {
    set_errno (EINVAL);
    return NULL;
    }
  if (*path == '\0') 
    {
    set_errno (ENOENT);
    return NULL;
    }

  int path_len = strlen (path);
  if (path_len >= PATH_MAX - 2) 
    {
    set_errno (ENAMETOOLONG);
    return NULL;
    }

  char copy_path[PATH_MAX];
  char *max_path, *new_path, *allocated_path;

  strcpy (copy_path + (PATH_MAX-1) - path_len, path);
  path = copy_path + (PATH_MAX-1) - path_len;
  allocated_path = got_path ? NULL : (got_path = malloc(PATH_MAX));
  max_path = got_path + PATH_MAX - 2; /* points to last non-NUL char */
  new_path = got_path;

  if (*path != '/') 
    {
    /* Relative pathname : add cwd */
    if (!getcwd (new_path, PATH_MAX - 1))
      goto err; // errno should have been set
    new_path += strlen (new_path);
    if (new_path[-1] != '/')
      *new_path++ = '/';
    } 
  else 
    {
    *new_path++ = '/';
    path++;
    }

  while (*path != '\0') 
    {
    // Ignore stray / 
    if (*path == '/') 
      {
      path++;
      continue;
      }
    // Ignore "."
    if (*path == '.') 
      {
      if (path[1] == '\0' || path[1] == '/') 
        {
        path++;
        continue;
        }
      if (path[1] == '.') 
        {
        if (path[2] == '\0' || path[2] == '/')  
          {
	  path += 2;
	  // Ignore ".." at root
	  if (new_path == got_path + 1)
	    continue;
	  // Handle ".." by backing up
	  while ((--new_path)[-1] != '/');
	  continue;
	  }
        }
      }
    while (*path != '\0' && *path != '/') 
      {
      if (new_path > max_path) 
        {
        set_errno (ENAMETOOLONG);
        goto err;
        }
      *new_path++ = *path++;
      }
    *new_path++ = '/';
    }

    // Delete trailing slash unless its a lone slash
    if (new_path != got_path + 1 && new_path[-1] == '/')
      new_path--;

  *new_path = '\0';
  return got_path;

err:
  if (allocated_path) free (allocated_path);
  return NULL;
  }

