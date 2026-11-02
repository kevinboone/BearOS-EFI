/*============================================================================

  ioctl.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int ioctl (int fd, unsigned long request, intptr_t arg1,
   intptr_t arg2, intptr_t arg3);

#ifdef __cplusplus
}
#endif



