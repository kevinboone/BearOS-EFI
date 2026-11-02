/*============================================================================

  compat_unistd.c

  Implementations of fundamental file-handling functions. All are wrappers
  around methods in the filesystem handlers that are exponse in the
  relevant `struct fops` from that handler. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "defs.h"
#include "compat_malloc.h"
#include "compat_unistd.h"
#include "compat_string.h"
#include "compat_stdio.h"
//#include "efifiles.h"
//#include "efiutil.h"
#include "mounts.h"
#include "../../lib/include/errno_numbers.h"
#include "path.h"
#include "process.h"
#include "procmgr.h"

/*============================================================================

  myclose 

============================================================================*/
int myclose (int fd)
  {
  return process_close_fd (procmgr_get_current_process(), fd);
  }

/*============================================================================

  getcwd

============================================================================*/
char *getcwd (char *buf, size_t bufsize)
  {
  strncpy (buf, 
          (char *)process_get_cwd (procmgr_get_current_process()), bufsize); 
  return buf;
  }


/*============================================================================

  mylseek 

============================================================================*/
off_t mylseek (int fd, off_t offset, int whence)
  {
  int ret = 0;
  struct FileOps *fs = process_get_fileops (procmgr_get_current_process(), fd);
  if (fs)
    {
    if (fs->lseek)
      ret = fs->lseek (fs, offset, whence);
    else
      {
      // File handler did not provide an 'lseek' implementation
      set_errno (ENOTSUP);
      ret = -1;
      }
    }
  else
    ret = -1;
  return ret;
  }


/*============================================================================

  myread 

============================================================================*/
ssize_t myread (int fd, void *buf, size_t count)
  {
  int ret = 0;
  struct FileOps *fs = process_get_fileops (procmgr_get_current_process(), fd);
  if (fs)
    {
    if (fs->read)
      ret = fs->read (fs, buf, count);
    else
      {
      // File handler did not provide a 'read' implementation
      set_errno (ENOTSUP);
      ret = -1;
      }
    }
  else
    ret = -1;
  return ret;
  }


/*============================================================================

  myrmdir

============================================================================*/
int myrmdir (const char *pathname)
  {
  int ret = 0;
  Path *p = path_new ((UTF8 *)pathname);
  if (path_canonicalize (p) == 0)
    {
    UTF8 *rem_path = NULL;
    struct FSOps *fsops = mounts_get_fsops_by_path (path_cstr (p), &rem_path);
    if (fsops->rmdir)
      {
      ret = fsops->rmdir (fsops, rem_path);
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


/*============================================================================

  myunlink

============================================================================*/
int myunlink (const char *pathname)
  {
  int ret = 0;
  Path *p = path_new ((UTF8 *)pathname);
  if (path_canonicalize (p) == 0)
    {
    UTF8 *rem_path = NULL;
    struct FSOps *fsops = mounts_get_fsops_by_path (path_cstr (p), &rem_path);
    if (fsops)
      {
      if (fsops->unlink)
        {
        ret = fsops->unlink (fsops, rem_path);
        }
      else
        {
        ret = -1;
        set_errno (ENOTSUP);
        }
      }
    else
      {
      ret = -1;
      set_errno (ENOENT);
      }
    if (rem_path) free (rem_path);
    }
  path_destroy (p);
  return ret;
  }


/*============================================================================

  mywrite 

============================================================================*/
ssize_t mywrite (int fd, void *buf, size_t count)
  {
  int ret = 0;
  struct FileOps *fs = process_get_fileops (procmgr_get_current_process(), fd);
  if (fs)
    {
    if (fs->write)
      ret = fs->write (fs, buf, count);
    else
      {
      // File handler did not provide a 'write' implementation
      ret = -1;
      set_errno (EINVAL);
      }
    }
  else
    ret = -1;
  return ret;
  }


