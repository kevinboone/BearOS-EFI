/*============================================================================

  compat_unistd.h

  Implementations of fundamental file-handling functions. All are wrappers
  around methods in the filesystem handlers that are exponse in the
  relevant `struct fops` from that handler. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#define SEEK_SET 0
#define SEEK_CUR 1 
#define SEEK_END 2 

#include "compat_limits.h" 
#include "../../lib/include/stddef.h"
#include "../../lib/include/stdint.h"

extern int      myclose  (int fd);
extern char    *getcwd   (char *buf, size_t bufsize);
extern off_t    mylseek  (int fd, off_t offset, int whence);
extern ssize_t  myread   (int fd, void *buf, size_t count);
extern int      myrmdir  (const char *path);
extern ssize_t  mywrite  (int fd, void *buf, size_t count);
extern int      myunlink (const char *path);
