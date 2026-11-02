/*============================================================================

  string.c

  General string handling functions, broadly matching those in the
  C standard library.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_string.h" 
#include "compat_malloc.h" 
#include "../../lib/include/errno_numbers.h"

/*============================================================================

  memcpy 

  No implementation, so long as we can use GCC's built-in one

============================================================================*/
//void *memcpy (void *dest, const void *src, size_t n)
//  {
//  }


/*============================================================================

  memcmp

  Note that we are, for some reason, unable to use GCC's built-in memcmp()
  when building for EFI. So we need to provide one.

============================================================================*/
int memcmp (const void *p1, const void *p2, size_t n)
  {
  register const unsigned char *s1 = (const unsigned char *) p1;
  register const unsigned char *s2 = (const unsigned char *) p2;
  unsigned register char c1, c2;

  do
    {
    c1 = (unsigned char) *s1++;
    c2 = (unsigned char) *s2++;
    } while (c1 == c2 && --n);

  return c1 - c2;
  }


/*============================================================================

  memset 

============================================================================*/
#ifdef LINUX
// For the EFI built, this function already exists in libefi
void *memset (void *dest, int c, size_t n)
  {
  for (register int i = 0; i < n; i++)
    ((char *)dest)[i] = c;
  return dest;
  }
#endif

/*============================================================================

  strchr

============================================================================*/
char *strchr (const char *s, int c)
  {
  char *p = (char *)s;
  while (*p != 0)
    {
    if (*p == c) return p;
    p++;
    }
 
  return 0;
  }


/*============================================================================

  strcmp

============================================================================*/
int strcmp (const char *p1, const char *p2)
  {
  register const unsigned char *s1 = (const unsigned char *) p1;
  register const unsigned char *s2 = (const unsigned char *) p2;
  unsigned register char c1, c2;

  do
    {
    c1 = (unsigned char) *s1++;
    c2 = (unsigned char) *s2++;
    if (c1 == '\0')
      return c1 - c2;
    } while (c1 == c2);

  return c1 - c2;
  }


/*============================================================================

  strncmp

============================================================================*/
int strncmp (const char *p1, const char *p2, size_t n)
  {
  register const unsigned char *s1 = (const unsigned char *) p1;
  register const unsigned char *s2 = (const unsigned char *) p2;
  unsigned register char c1, c2;

  do
    {
    c1 = (unsigned char) *s1++;
    c2 = (unsigned char) *s2++;
    if (c1 == '\0')
      return c1 - c2;
    } while (c1 == c2&& (--n > 0));

  return c1 - c2;
  }


/*============================================================================

  strcpy

============================================================================*/
char *strcpy (char *dst, const char *src)
  {
  int l = strlen (src);
  for (register int i = 0; i < l; i++)
    {
    *dst = *src;
    src++; dst++;
    }
  *dst = 0;
  return dst;
  }

/*============================================================================

  strncpy

============================================================================*/
char *strncpy (char *dst, const char *src, size_t n)
  {
  int l = strlen (src);
  for (register int i = 0; i < l && i < n; i++)
    {
    *dst = *src;
    src++; dst++;
    }
  *dst = 0;
  return dst;
  }

/*============================================================================

  strdup 

============================================================================*/
char *strdup (const char *s)
  {
  int l = strlen (s);
  char *ret = malloc (l + 1);
  strcpy (ret, s); 
  ret[l] = 0;
  return ret;
  }

/*============================================================================

  strlen

============================================================================*/
long unsigned int strlen (const char *s)
  {
  register int l = 0;
  while (*s)
    {
    l++;
    s++;
    }
  return l;
  }

/*============================================================================

  strerror

============================================================================*/
char *strerror (int errnum)
  {
  switch (errnum)
    {
    case 0: return "OK";
    case EIO: return "I/O error";
    case ENOTDIR: return "not a directory";
    case ENOEXEC: return "executable in wrong format";
    case EINVAL: return "invalid parameter";
    case ENOENT: return "not found";
    case ENODEV: return "device error";
    case EROFS: return "read only";
    case ENOMEM: return "out of memory or resources";
    case ENOSPC: return "out of space";
    case ENOMEDIUM: return "no medium in drive";
    case EMEDIUMTYPE: return "incorrect medium type, or medium changed";
    case ETIMEDOUT: return "no response within timeout";
    case EPROTO: return "protocol error";
    case EPERM: return "permission denied";
    case EACCES: return "access denied";
    case EBADF: return "bad file descriptor";
    case ENFILE: return "too many open files";
    }  
  return "unknown error";
  }


/*============================================================================

  strstr

============================================================================*/
char *strstr  (const char *whole, const char *part)
  {
  register const char *p1, *p2;
  register char c;

  if (!(c = *part++)) return (char *)whole;

  for (;;) 
    {
    for (; *whole != c; ++whole) 
       {
       if (!*whole) return 0;
       if (*++whole == c) break;
       if (!*whole) return 0;
       }
     for (p1 = ++whole, p2 = part; (*p1 == *p2) && *p2;) ++p1, ++p2;
     if (!*p2) return (char *)--whole;
    }
  }


