/*============================================================================

  compat_stat.c

  Implementation of the stat() and fstat() functions. In practice, these
  functions just forward the request to the relevant filesystem handler
  via the open file table for the current process.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "defs.h"
#include "compat_stat.h"
#include "compat_time.h"
#include "compat_malloc.h"
#include "compat_stdio.h"
#include "compat_unistd.h"
#include "../../lib/include/asm/vfs.h"
#include "../../lib/include/errno_numbers.h"
#include "../../lib/include/stdint.h"
#include "compat_fcntl.h"
#include "mounts.h"
#include "path.h"
#include "process.h"
#include "procmgr.h"

/*============================================================================

  myfstat

  Get a `struct fileops` for the given file handle, using the open file
  table in the current process. Then call fstat() on the `fops` object,
  if it provides one. 

============================================================================*/
int myfstat (int fd, struct stat *statbuff)
  {
  int ret = 0;
  struct FileOps *fs = process_get_fileops (procmgr_get_current_process(), fd);
  if (fs)
    {
    if (fs->fstat)
      {
      ret = fs->fstat (fs, statbuff);
      }
    else
      {
      // Handler does not provide an 'fstat' implementation
      set_errno (EINVAL);
      ret = -1;
      }
    }
  else
    ret = -1; // errno already set
  return ret;
  }


/*============================================================================

  mystat

  To implement mystat(), we open the file/directory for reading, and then
  call myfstat() on it.

============================================================================*/
int mystat (const char *pathname, struct stat *statbuff)
  {
  int ret = 0;
  int fd = myopen (pathname, O_RDONLY);
  if (fd >= 0)
    {
    ret = myfstat (fd, statbuff);
    myclose (fd);
    }
  else 
    {
    ret = -1; // errno already set
    }
  return ret;
  }


/*============================================================================

  mystatfs

============================================================================*/
int mystatfs (const char *pathname, struct statfs *statbuff)
  {
  struct FSOps *fsops = mounts_get_fsops_by_path ((UTF8 *)pathname, NULL);
  int ret;
  if (fsops->statfs)
    {
    ret = fsops->statfs (fsops, statbuff);
    }
  else
    {
    set_errno (ENOTSUP);
    ret = -1;
    }
  return ret;
  }


/*============================================================================

  mymkdir

============================================================================*/
int mymkdir (const char *pathname, int mode)
  {
  int ret = 0;
  Path *p = path_new ((UTF8 *)pathname);
  if (path_canonicalize (p) == 0)
    {
    UTF8 *rem_path = NULL;
    struct FSOps *fsops = mounts_get_fsops_by_path 
      ((UTF8 *)path_cstr (p), &rem_path);
    if (fsops->mkdir)
      {
      ret = fsops->mkdir (fsops, rem_path, mode);
      if (rem_path) free (rem_path);
      }
    else
      {
      set_errno (ENOTSUP);
      ret = -1;
      }
    }
  else
    ret = -1;
  path_destroy (p);
  return ret;
  }



