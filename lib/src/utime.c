/*============================================================================

  utime.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <bearos/apidefs.h> 
#include <syscall.h> 
#include <time.h>
#include <utime.h>


/*============================================================================

  utime

============================================================================*/
int utime (const char *filename, const struct utimbuf *buf)
  {
  return syscall (BEAROS_SYSCALL_UTIME, (intptr_t)filename, 
         (intptr_t)buf, 0, 0, 0, 0);
  }



