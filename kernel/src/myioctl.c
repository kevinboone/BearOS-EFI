/*============================================================================

  myioctl.c

  Implementation of the ioctl() compatibility function. In practice,
  we delegate to the process, which delegates to the `struct fileops`
  that corresponds to the supplied file handle.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "../../lib/include/stddef.h"
#include "myioctl.h"

/*============================================================================

  myioctl

============================================================================*/
int myioctl (Process *process, int fd, unsigned long request, 
       intptr_t arg1, intptr_t arg2, intptr_t arg3)
  {
  return process_ioctl_fd (process, fd, request, arg1, arg2, arg3);
  }



