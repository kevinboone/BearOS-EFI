/*============================================================================

  compat_fcntl.c

  See compat_dirent.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/
#include "defs.h"
#include "compat_fcntl.h"
#include "compat_malloc.h"
#include "compat_stdio.h"
//#include "efiutil.h"
//#include "efifiles.h"
//#include "efifilehandler.h"
#include "fileops.h"
#include "../../lib/include/errno_numbers.h"
#include "mounts.h"
#include "path.h"
#include "process.h"
#include "procmgr.h"

int myopen (const char *path, int flags)
  {
  int ret;
 
  Path *p = path_new ((UTF8 *)path);
  if (path_canonicalize (p) == 0)
    {
    UTF8 *rem_path = NULL;
    struct FSOps *fsops = mounts_get_fsops_by_path (path_cstr (p), &rem_path);
    if (fsops)
      {
      PROC_BEGIN_SYNC
      int fd = process_get_next_fd (procmgr_get_current_process());
      if (fd >= 0)
        {
        struct FileOps *fileops = fsops->open (fsops, rem_path, flags);  

        if (fileops)
          {
          process_set_fileops (procmgr_get_current_process(), fd, fileops);
          ret = fd;
          }
        else
          ret = -1;
        }
      else
        ret = -1; // errno already set
      if (rem_path) free (rem_path);
      PROC_END_SYNC
      }
    else
      {
      ret = -1; // errno already set
      }
    }    
  else
    ret = -1;

  path_destroy (p);

  return ret;
  }

