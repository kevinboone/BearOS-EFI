/*============================================================================

  fcntl.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <bearos/apidefs.h>
#include <fcntl.h>
#include <syscall.h>

int open (const char *path, int flags)
  {
  return syscall (BEAROS_SYSCALL_OPEN, (intptr_t)path, flags, 0, 0, 0, 0);
  }


