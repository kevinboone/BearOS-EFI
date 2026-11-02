/*============================================================================

  compat_stat.h

  Implementation of the stat() and fstat() functions. In practice, these
  functions just forward the request to the relevant filesystem handler
  via the open file table for the current process.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "compat_time.h"
#include "../../lib/include/stdint.h"
#include "../../lib/include/asm/stat.h"
#include "../../lib/include/asm/vfs.h"

int myfstat  (int fd, struct stat *statbuff);
int mystat   (const char *pathname, struct stat *statbuff);
int mystatfs (const char *pathname, struct statfs *statbuff);
int mymkdir  (const char *pathname, int mode);


