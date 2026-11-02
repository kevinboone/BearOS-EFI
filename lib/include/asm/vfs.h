/*============================================================================

  vfs.h 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <stddef.h>
#include <stdint.h>

// Type IDs for various filesystems, included for Linux compatibility.

#define MSDOS_SUPER_MAGIC  0x4d44
#define PROC_SUPER_MAGIC   0x9fa0
#define TMPFS_SUPER_MAGIC  0x1021944

typedef __PTRDIFF_TYPE__ ssize_t; // WHY? It's in stddef.h

struct statfs 
  {
  int32_t f_type;
  ssize_t f_bsize;
  ssize_t f_blocks;
  ssize_t f_bfree;;
  };

