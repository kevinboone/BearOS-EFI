/*============================================================================

  reboot.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <asm/reboot.h>
#include <bearos/apidefs.h>
#include <stdio.h>
#include <sys/syscall.h>


/*============================================================================

  reboot

============================================================================*/
int reboot (int cmd)
  {
  return syscall (BEAROS_SYSCALL_REBOOT, cmd, 0, 0, 0, 0, 0);
  }


