/*============================================================================

  string.c

  General string handling functions.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include <errno.h> 

/*============================================================================

  strcat

============================================================================*/
char *strcat (char *dst, const char *src)
  {
  strcpy (dst + strlen (dst), src);
  return dst;
  }


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

  strchr

============================================================================*/
char *strrchr (const char *s, int c)
  {
  char *ret = 0;

  do 
    {
    if (*s == c)
      ret = (char*) s;
    } while (*s++);

  return (ret);
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

  strerror

============================================================================*/
char *strerror (int errno)
  {
  switch (errno)
    {
    case 0: return "OK";
    case ENOTSUP: return "unsupported operation";
    case ENOTBLK: return "not a block device";
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
    } while (c1 == c2 && (--n > 0));

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

  strtok

============================================================================*/
char *strtok (char *str, const char *delim)
  {
  static char* last;
  return strtok_r (str, delim, &last);
  }


/*============================================================================

  strtok_r

============================================================================*/
char *strtok_r (char *str, const char *delim, char **saveptr)
  {
  const char *spanp;
  char* tok;
  int c;
  int sc;

  if (str == NULL && (str = *saveptr) == NULL)
    {
    return (NULL);
    }

  cont:

  c = *str++;
  for (spanp = delim; (sc = *spanp++) != 0;)
    {
    if(c == sc)
      {
      goto cont;
      }
    }

  if( c == 0)
    { 
    *saveptr = NULL;
    return (NULL);
    }

  tok = str - 1;

  for (;;)
    {
    c = *str++;
    spanp = delim;
    do
      {
      if ((sc = *spanp++) == c)
        {
        if (c == 0)
          {
          str = NULL;
          }
        else
          {
          str[-1] = 0;
          }
        *saveptr = str;
        return (tok);
        }
      } while (sc != 0);
    }
  // We never get here
  }



