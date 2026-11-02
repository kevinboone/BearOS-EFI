/*============================================================================

  device.h

  This object models a particular device, with attributes for the name,
  major/minor numbers, etc. It also contains pointer to the file- 
  and block-level open functions, which various other modules will use
  to manipulate the device.

  The device manager maintains a list of these objects. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "fileops.h"
#include "blockops.h"

struct _Device;
typedef struct _Device Device;

typedef struct FileOps *(*DevOpenAsFileFn) (void *data);
typedef struct BlockOps *(*DevOpenAsBlockFn) (void *data);
typedef void (*DevCloseAndCleanupFn) (void *data);

/** device_destroy just cleans up this Device object, including calling the
    close-and-cleanup function that was stored when the device was created. */
extern void device_destroy (Device *self);

extern const char *device_get_name (const Device *device);

extern Device *device_new (const char *name, int major, int minor, 
    int32_t st_mode, DevOpenAsFileFn open_file_fn, 
    DevOpenAsBlockFn open_block_fn, DevCloseAndCleanupFn close_and_cleanup_fn, 
    void *data);

extern struct FileOps *device_open_as_file (Device *device);
extern struct BlockOps *device_open_as_block (Device *device);

