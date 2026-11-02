/*============================================================================

  devfs.c

  /dev is a filesystem handled by the generic pseudo-filesystem 
  PseudoFsHandler. 

  See devfs.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_limits.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "defs.h"
#include "devfs.h"
#include "devmgr.h"
#include "../../lib/include/errno_numbers.h"
#include "list.h"
#include "process.h"
#include "procmgr.h"


/*============================================================================

  devfs_populate_root

============================================================================*/
int devfs_populate_root (List *entries)
  {
  devmgr_get_device_names (entries);
  return 0;
  }


/*============================================================================

  devfs_open_file

============================================================================*/
struct FileOps *devfs_open_file (const char *name)
  {
  int fd = process_get_next_fd (procmgr_get_current_process());
  if (fd < 0)
    {
    set_errno (ENFILE); 
    return NULL;
    }

  struct FileOps *ret = devmgr_open_as_file_by_name ((char *)name); 
  if (ret)
    {
    process_set_fileops (procmgr_get_current_process(), fd, ret);
    ret->refs = 1;
    return ret;
    }

  set_errno (ENOENT);
  return NULL;
  }


