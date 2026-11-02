/*============================================================================

  compat_dirent.h

  Implementations of opendir(), readdir(), and closedir(). Directory
  listing works in BearOS by opening a file that happens to be a
  directory, and reading it in blocks of size `sizeof struct mydirent`. 
  There is no equivalent of the Linux `getdents` syscall, because
  reading a directory will always return exactly enough data for a 
  single `struct mydirent`. In Linux, we can read from a directory
  in blocks larger than a single `struct dirent`.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/asm/dirent.h"

/** Struct DIR is returned by opendir(). It contains the file handle,
    and also a static struct dirent. readdir() populates this dirent
    structure, and returns a pointer to it to the caller. 

    NOTE: we can't just call it DIR as in POSIX, because this name 
    clashes with DIR in ff.h. Grr...
*/
typedef struct _MYDIR
  {
  int fd;
  struct dirent entry;
  } MYDIR;

// Prototypes as in Linux
extern int closedir (MYDIR *dirp);
extern MYDIR *opendir (const char *path);
extern struct dirent *readdir (MYDIR *dirp);

