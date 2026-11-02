/*============================================================================

  compat_malloc.h

  Implementations of malloc() and free() which, in an EFI build, are just
  wrappers around EFI API calls. On Linux, of course, we can use the
  malloc() and free() from the standard library, but we have to be careful
  not to use these function names internally, or there will be a clash.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"
extern void free (void *s);
extern void *malloc (size_t length); 
extern void *realloc (void *src, size_t l);

