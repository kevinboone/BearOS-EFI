/*============================================================================

  compat_string.h

  General string handling functions, broadly matching those in the
  C standard library.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"

extern void *memcpy  (void *dest, const void *src, size_t n);
extern void *memset  (void *dest, int c, size_t n);
extern long unsigned int strlen (const char *s);
extern int   strcmp (const char *a, const char *b);
extern int   strncmp (const char *a, const char *b, size_t n);
extern char *strcpy  (char *dst, const char *src);
extern char *strchr  (const char *s, int c);
extern char *strdup  (const char *s);
extern char *strncpy (char *dst, const char *src, size_t n);
extern char *strstr  (const char *whole, const char *part);

/** Note that strerror only returns meaningful text values for the
    EXXX constants that the program actually uses. Some of the EXX
    constants in compat_error.h will result in "unknown error"
    being returned. */
extern char *strerror (int errnum);

