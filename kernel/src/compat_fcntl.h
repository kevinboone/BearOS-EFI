/*============================================================================

  compat_fcntl.h

  Definition of the myopen() function, which is the back-end for the
  open() syscall. 

  In practice, what myopen() does is to open the file via the mount
  table, which provides a `struct fileops` that is particular to the
  mount. `mounts_get_fsops_by_path()` takes a generic POSIX path,
  and strips off that part which defines the mount, leaving a path
  that can be used with the `struct fileops` calls on that mount.

  As part of the opening logic, we need to get a new file handle from 
  the current process, and then store the fileops object against that
  handle. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <asm/fcntl.h>

/** open() behaves as on Linux, except that we don't provide the 3-parameter
    version that includes file modes. VFAT filesystems don't really support
    any of these modes. */
int myopen (const char *pathname, int flags);


