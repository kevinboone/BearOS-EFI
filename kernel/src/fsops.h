/*============================================================================

  fsops.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"
#include "fileops.h"
#include "../../lib/include/asm/utime.h"
#include "../../lib/include/asm/vfs.h"

typedef int             (*FSMkdirFn)     (void *data, const UTF8 *path, int mode);
typedef struct FileOps *(*FSOpsOpenFn)   (void *data, const UTF8 *path, int mode);
typedef int             (*FSRmdirFn)     (void *data, const UTF8 *path);
typedef int             (*FSStatFsFn)    (void *data, struct statfs *sb);
typedef int             (*FSUmountFn)    (void *data);
typedef int             (*FSUtimeFn)     (void *data, const UTF8 *path,
                           const struct utimbuf *buff);
typedef int             (*FSOpsUnlinkFn) (void *data, const UTF8 *path);

struct FSOps
  {
  void *data;
  FSMkdirFn mkdir; // Create directory 
  FSOpsOpenFn open; // Open a file in this filesystem
  FSRmdirFn rmdir; // Delete directory 
  FSStatFsFn statfs; // Get filesystem information
  FSOpsUnlinkFn unlink; // Delete a file
  FSUmountFn umount; // Unmount filesystem 
  FSUtimeFn utime; // Update file timestamps
  char type[32]; // Type name, e.g., "vfat"
  int32_t magic; // Magic number, for use in statfs(), for Linux compat
  };

 
