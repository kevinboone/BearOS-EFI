/*============================================================================

  compat_utime.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "../../lib/include/asm/time.h"
#include "../../lib/include/errno_numbers.h"
#include "compat_utime.h"
#include "defs.h"
#include "path.h"
#include "mounts.h"
#include "procmgr.h"

int myutime (const char *pathname, const struct utimbuf *ub)
  {
  int ret = 0;
  Path *p = path_new ((UTF8 *)pathname);
  if (path_canonicalize (p) == 0)
    {
    struct FSOps *fsops = mounts_get_fsops_by_path ((UTF8 *)path_cstr (p), NULL);
    if (fsops->utime)
      {
      ret = fsops->utime (fsops, path_cstr (p), ub);
      }
    else
      {
      set_errno (ENOTSUP);
      ret = -1;
      }
    }
  path_destroy (p);
  return ret;
  }

