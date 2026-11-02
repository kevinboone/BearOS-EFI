/*============================================================================

  stdlib.c 

  BearOS alternative to the conventional stdlib functions

============================================================================*/

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <bearos/apidefs.h>

static int hexval (int c);

/*============================================================================

  atoi 

============================================================================*/
int atoi (char *s)
  {
  return atol (s);
  }

/*============================================================================

  atol 

============================================================================*/
long atol (char *num)
  {
  long long value = 0;
  int neg = 0;

  // skip leading whitespace
  while (isspace (num[0])) 
    {
    num++;
    }

  if (num[0] == '0' && num[1] == 'x') 
    {
    // hex
    num += 2;
    while (*num && isxdigit (*num))
      value = value * 16 + hexval (*num++);
    } 
  else 
    {
    // decimal
    if (num[0] == '-') 
      {
      neg = 1;
      num++;
      } 
    else if (num[0] == '+') 
      {
      num++;
      }

    while (*num && isdigit (*num))
       value = value * 10 + *num++ - '0';
    }

  if (neg)
    value = -value;

  return (long)value;
  }

/*============================================================================

  atou 

============================================================================*/
unsigned atou (char *num)
  {
  return atoul (num);
  }

/*============================================================================

  atoul

============================================================================*/
unsigned long atoul (char *num)
  {
  unsigned long value = 0;
  if (num[0] == '0' && num[1] == 'x') 
    {
    // hex
    num += 2;
    while (*num && isxdigit(*num))
      value = value * 16 + hexval(*num++);
    } 
  else 
    {
    // decimal
    while (*num && isdigit(*num))
      value = value * 10 + *num++  - '0';
    }

  return value;
  }

/*============================================================================

  exit

============================================================================*/
void exit (int status)
  {
  // Non-local jump to the start point we stored in start() in the
  //   start-up code
  __builtin_longjmp (bearos_context->exit_jmp_buf, 1);
  }

/*============================================================================

  free 

============================================================================*/
void free (void *mem)
  {
  bearos_context->syscall_fn (bearos_context, 
    BEAROS_SYSCALL_FREE, (intptr_t)mem, 0, 0, 0, 0, 0);
  errno = bearos_context->_errno;
  }


/*============================================================================

  getenv 

============================================================================*/
char *getenv  (const char *name)
  {
  if (environ == NULL) return NULL;
  if (name == NULL) return NULL;
  if (name[0] == 0) return NULL;

  for (char **env = environ; *env != 0; env++)
    {
    char *nv = *env;
    char *eqpos = strchr (nv, '=');
    if (eqpos)
      {
      int keylen = eqpos - nv;
      if (strncmp (name, nv, keylen) == 0)
        {
        return eqpos + 1;
        }
      }
    }

  return NULL;
  }


/*============================================================================

  hexval 

============================================================================*/
static int hexval (int c) 
  {
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0;
  }

/*============================================================================

  malloc 

============================================================================*/
void *malloc (size_t size)
  {
  void *ret = (void *)bearos_context->syscall_fn (bearos_context, 
    BEAROS_SYSCALL_MALLOC, (intptr_t)size, 0, 0, 0, 0, 0);
  errno = bearos_context->_errno;
  return ret;
  }


/*============================================================================

  reaalloc 

============================================================================*/
void *realloc (void *s, size_t length)
  {
  void *ret = (void *)bearos_context->syscall_fn (bearos_context, 
    BEAROS_SYSCALL_REALLOC, (intptr_t)s, length, 0, 0, 0, 0);
  errno = bearos_context->_errno;
  return ret;
  }

/*============================================================================

  realpath

============================================================================*/
char *realpath (const char *path, char *got_path)
  {
  // TODO

  if (path == 0) 
    {
    errno = EINVAL;
    return NULL;
    }
  if (*path == '\0') 
    {
    errno = ENOENT;
    return NULL;
    }

  int path_len = strlen (path);
  if (path_len >= PATH_MAX - 2) 
    {
    errno = ENAMETOOLONG;
    return NULL;
    }

  char copy_path[PATH_MAX];
  char *max_path, *new_path, *allocated_path;

  strcpy (copy_path + (PATH_MAX-1) - path_len, path);
  path = copy_path + (PATH_MAX-1) - path_len;
  allocated_path = got_path ? NULL : (got_path = malloc(PATH_MAX));
  max_path = got_path + PATH_MAX - 2; /* points to last non-NUL char */
  new_path = got_path;

  if (*path != '/') 
    {
    /* Relative pathname : add cwd */
    if (!getcwd (new_path, PATH_MAX - 1))
      goto err; // errno should have been set
    new_path += strlen (new_path);
    if (new_path[-1] != '/')
      *new_path++ = '/';
    } 
  else 
    {
    *new_path++ = '/';
    path++;
    }

  while (*path != '\0') 
    {
    // Ignore stray / 
    if (*path == '/') 
      {
      path++;
      continue;
      }
    // Ignore "."
    if (*path == '.') 
      {
      if (path[1] == '\0' || path[1] == '/') 
        {
        path++;
        continue;
        }
      if (path[1] == '.') 
        {
        if (path[2] == '\0' || path[2] == '/')  
          {
	  path += 2;
	  // Ignore ".." at root
	  if (new_path == got_path + 1)
	    continue;
	  // Handle ".." by backing up
	  while ((--new_path)[-1] != '/');
	  continue;
	  }
        }
      }
    while (*path != '\0' && *path != '/') 
      {
      if (new_path > max_path) 
        {
        errno = ENAMETOOLONG;
        goto err;
        }
      *new_path++ = *path++;
      }
    *new_path++ = '/';
    }

    // Delete trailing slash unless its a lone slash
    if (new_path != got_path + 1 && new_path[-1] == '/')
      new_path--;

  *new_path = '\0';
  return got_path;

err:
  if (allocated_path) free (allocated_path);
  return NULL;
  }


