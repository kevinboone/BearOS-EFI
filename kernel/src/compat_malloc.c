/*============================================================================

  compat_malloc.c

  See compat_malloc.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifdef LINUX
#else
#include <efi/efi.h>
#include <efi/efilib.h>
#include "defs.h"
#endif

#include "compat_malloc.h"
#include "compat_string.h"

#ifndef LINUX
void *malloc (size_t l)
  {
  return AllocatePool (l);
  }

void free (void *s)
  {
  return FreePool (s);
  }

// Ugly as it is, this is the best we can do in EFI :/
void *realloc (void *src, size_t l)
  {
  void *new_mem = malloc (l); 
  // This copy will overrun the source, if l is larger than
  //   the original allocation. Although wasteful, this won't
  //   cause a problem in EFI
  memcpy (new_mem, src, l);
  free (src);
  return new_mem;
  }

#endif

