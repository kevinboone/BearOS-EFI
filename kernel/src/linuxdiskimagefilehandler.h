/*============================================================================

  linuxdiskimagefilehandler.h

  A file handler for the Linux virtual root filesystem.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"
#include "fileops.h"

struct _LinuxDiskImageFileHandler;
typedef struct _LinuxDiskImageFileHandler LinuxDiskImageFileHandler;

extern void linuxdiskimagefilehandler_close_and_cleanup 
        (LinuxDiskImageFileHandler *self);

extern void linuxdiskimagefilehandler_destroy (LinuxDiskImageFileHandler *self);

extern struct BlockOps *linuxdiskimagefilehandler_get_blockops 
        (LinuxDiskImageFileHandler *self);

extern struct FileOps *linuxdiskimagefilehandler_get_fileops 
  (LinuxDiskImageFileHandler *self);

extern LinuxDiskImageFileHandler *linuxdiskimagefilehandler_new 
  (const UTF8 *filename);

extern int linuxdiskimagefilehandler_prepare (LinuxDiskImageFileHandler *self);

