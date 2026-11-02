/*============================================================================

  compat_utime.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/asm/time.h"
#include "../../lib/include/asm/utime.h"

extern int myutime (const char *path, const struct utimbuf *ub);

