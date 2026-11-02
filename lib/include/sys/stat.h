/*============================================================================

  stat.h

  Definition of the stat() functions, and all the constants that go with them.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <asm/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

int fstat (int fd, struct stat *restrict statbusf);
int mkdir (const char *pathname, int mode);
int stat  (const char *restrict pathname, struct stat *restrict statbusf);

#ifdef __cplusplus
}
#endif


