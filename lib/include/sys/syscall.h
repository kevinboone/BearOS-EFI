/*============================================================================

  syscall.h 

============================================================================*/

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

long syscall (long number, intptr_t arg1, intptr_t arg2, intptr_t arg3,
        intptr_t arg4, intptr_t arg5, intptr_t arg6);

#ifdef __cplusplus
}
#endif



