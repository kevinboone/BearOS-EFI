/*============================================================================

  efidiskimagefilehandler.h

  This is a handler for block storage devices exposed as EFI handles.

  In principle, this class handles both file-type and block-type manipulation,
  but no account has so far been taken of trying to do both in the same
  session. We need to do more to manage the file pointer to the underlying
  block device.

  This class uses EFI's block storage API, even though EFI notionally
  supports direct file access on FAT partitions. EFI file handling could
  be implemented as its own file handler, with its own `struct fileops`.
  However, EFI's FAT support is very limited -- it doesn't even provide
  a way to create a new directory. So we'll work at the block level.
  This also has the advantage of being more portable to other firmware,
  that doesn't support EFI. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#ifndef LINUX
#include <efi/efi.h>

#include "fileops.h"

struct _EFIDiskImageFileHandler;
typedef struct _EFIDiskImageFileHandler EFIDiskImageFileHandler;

void efidiskimagefilehandler_close_and_cleanup 
        (EFIDiskImageFileHandler *self);

extern void efidiskimagefilehandler_destroy (EFIDiskImageFileHandler *self);

extern struct BlockOps *efidiskimagefilehandler_get_blockops 
        (EFIDiskImageFileHandler *self);

extern struct FileOps *efidiskimagefilehandler_get_fileops 
  (EFIDiskImageFileHandler *self);

extern EFIDiskImageFileHandler *efidiskimagefilehandler_new 
  (EFI_HANDLE efi_device);

extern int efidiskimagefilehandler_prepare (EFIDiskImageFileHandler *self);

#endif
