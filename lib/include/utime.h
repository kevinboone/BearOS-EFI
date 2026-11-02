/*============================================================================

  utime.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <asm/utime.h>

#ifdef __cplusplus
extern "C" {
#endif

int utime (const char *filename, const struct utimbuf *buf);

#ifdef __cplusplus
}
#endif


