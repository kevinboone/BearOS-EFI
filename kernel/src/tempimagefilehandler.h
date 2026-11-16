
/*============================================================================

  tempimagefilehandler.h

  A file handler for the Linux virtual root filesystem.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"
#include "fileops.h"

struct _TempImageFileHandler;
typedef struct _TempImageFileHandler TempImageFileHandler;

extern void tempimagefilehandler_close_and_cleanup 
        (TempImageFileHandler *self);

extern void tempimagefilehandler_destroy (TempImageFileHandler *self);

extern struct BlockOps *tempimagefilehandler_get_blockops 
        (TempImageFileHandler *self);

extern struct FileOps *tempimagefilehandler_get_fileops 
  (TempImageFileHandler *self);

extern TempImageFileHandler *tempimagefilehandler_new 
  (size_t size);

extern int tempimagefilehandler_prepare (TempImageFileHandler *self);

