/*============================================================================

  fatfshandler.h

  A handler for the FAT filesystem, that most defers to ChaN's FAT handler
  for embedded systems.

  This module must be created using `struct blockops` that provides access
  to the storage device that contains the filesystem. Other being limited
  to 512-byte sectors (which are u

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "blockops.h"
#include "fsops.h"

struct _FatFsHandler;
typedef struct _FatFsHandler FatFsHandler;

extern void fatfshandler_destroy (FatFsHandler *self); 

/** Return an FSOps structure appopriate for this particular handler.
    The caller must store and free it -- it is dynamically generated. */
extern struct FSOps *fatfshandler_get_fsops (FatFsHandler *self); 

extern int fatfshandler_mount (FatFsHandler *self);

/** Create a new FAT handler on the block device defined by `bops`. 
    NOTE `bops` must be dynamically created, and this object will
    own it, and free it when it, itself, is destroyed
*/
extern FatFsHandler *fatfshandler_new (struct BlockOps *bops);


