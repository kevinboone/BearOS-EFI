/*============================================================================

  compat_stdio.h

  Definition of myprintf().

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/stdint.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef LINUX
typedef struct _FILE FILE;
#endif

extern int myprintf (const char *format, ...);
extern int mysnprintf (char *s, int n, const char *format, ...);

