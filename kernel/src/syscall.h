/*============================================================================

  syscall.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifdef LINUX
#include "../../lib/include/bearos/apidefs.h"
#else
#include "bearos/apidefs.h"
#endif

extern intptr_t mysyscall (BearosContext *context, 
    long number, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4,
    intptr_t arg5, intptr_t arg6);

