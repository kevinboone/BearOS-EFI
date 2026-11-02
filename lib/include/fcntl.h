/*============================================================================

  fcntl.h

  Definition of the open() function, etc 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <asm/fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

int open (const char *path, int flags);

#ifdef __cplusplus
}
#endif




