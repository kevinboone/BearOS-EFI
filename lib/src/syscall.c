/*============================================================================

  syscall.c 

  BearOS alternative to the conventional syscall function

============================================================================*/

#include <errno.h>
#include <stddef.h>
#include <sys/syscall.h>
#include "bearos/apidefs.h"

/*============================================================================

  syscall

============================================================================*/
long syscall (long number, intptr_t arg1, intptr_t arg2, intptr_t arg3,
        intptr_t arg4, intptr_t arg5, intptr_t arg6)
  {
  intptr_t ret = bearos_context->syscall_fn (bearos_context, 
    number, arg1, arg2, arg3, arg4, arg5, arg6); 
  errno = bearos_context->_errno;

  return (long)ret;
  }





