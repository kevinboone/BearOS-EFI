/*============================================================================

  string.h 

  BearOS alternative to the conventional string.h

============================================================================*/

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char  *strcat   (char *dst, const char *src);
extern size_t strlen   (const char *s);
extern int    strcmp   (const char *a, const char *b);
extern int    strncmp  (const char *a, const char *b, size_t n);
extern char  *strcpy   (char *dst, const char *src);
extern char  *strncpy  (char *dst, const char *src, size_t n);
extern char  *strchr   (const char *s, int c);
extern char  *strrchr  (const char *s, int c);
extern char  *strdup   (const char *s);
extern char  *strerror (int errnum);
extern char  *strtok   (char *str, const char *delim);
extern char  *strtok_r (char *str, const char *delim, char **saveptr);
extern void  *memset   (void *s, int c, size_t n);
extern void  *memcpy   (void *dest, const void *src, size_t n);
extern int    memcmp   (const void *s1, const void *s2, size_t n);

#ifdef __cplusplus
}
#endif


