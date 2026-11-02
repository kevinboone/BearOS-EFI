/*============================================================================

  compat_stdio.c

  Implementation of myprint(), which is just a wrapper around the corresponding
  nanoprintf function.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifdef LINUX
#include "../../lib/include/stdint.h"
#include "../../lib/include/stddef.h"
ssize_t write (int fd, const void *buff, size_t n);
#else
#include <efi/efi.h>
#include <efi/efilib.h>
#endif

#include "compat_stdio.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "process.h"
#include "../../lib/include/errno_numbers.h"

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 1

// Compile nanoprintf in this translation unit.
#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"

/*============================================================================

  myprintf

============================================================================*/
int myprintf (const char *format, ...)
  {
  char buffer[1024]; // TODO TODO TODO
  va_list val;
  va_start (val, format);
  int rv = npf_vsnprintf (buffer, sizeof (buffer), format, val);
  va_end (val);
#ifdef LINUX
  write (1, buffer, strlen (buffer));
#else
  Print (L"%a", buffer);  
#endif
  return rv;
  }

/*============================================================================

  mysprintf

============================================================================*/
int mysnprintf (char *s, int n, const char *format, ...)
  {
  va_list val;
  va_start (val, format);
  int rv = npf_vsnprintf (s, n, format, val); 
  va_end (val);
  return rv;
  }


