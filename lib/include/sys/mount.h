/*============================================================================

  mount.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int mount (const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 void *data);

int umount (const char *target);

#ifdef __cplusplus
}
#endif


