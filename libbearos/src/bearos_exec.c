/*============================================================================

  bearos_exec.c 

============================================================================*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <bearos/apidefs.h>


/*============================================================================

 bearos_exec_run

============================================================================*/
int bearos_exec_run (const char *path, int argc, char **argv, 
      char **envp, int flags)
  {
  return (int)syscall (BEAROS_SYSCALL_EXEC, (intptr_t)path, 
         argc, (intptr_t)argv, (intptr_t)envp, flags, 0);
  }


