/*============================================================================

  ioctl.c 

  BearOS alternative to the conventional ioctl function

============================================================================*/

#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <bearos/apidefs.h>

/*============================================================================

  ioctl

============================================================================*/
int ioctl (int fd, unsigned long request, intptr_t arg1,
    intptr_t arg2, intptr_t arg3)
  {
  return syscall (BEAROS_SYSCALL_IOCTL, fd, request, arg1, arg2, arg3, 0);
  }







