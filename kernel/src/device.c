/*============================================================================

  device.c

  This object models a particular device, with attributes for the name,
  major/minor numbers, etc. 

  See device.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_malloc.h"
#include "compat_string.h"
#include "device.h"
#include "../../lib/include/errno_numbers.h"
#include "../../lib/include/stddef.h"
#include "../../lib/include/stdint.h"
#include "procmgr.h"

struct _Device
  {
  char name[256];
  int major;
  int minor;
  int32_t st_mode;
  void *data;
  DevOpenAsFileFn open_file_fn;
  DevOpenAsBlockFn open_block_fn;
  DevCloseAndCleanupFn close_and_cleanup_fn;
  };

/*============================================================================

  device_destroy

============================================================================*/
void device_destroy (Device *self)
  {
  if (self->close_and_cleanup_fn)
    self->close_and_cleanup_fn (self->data);
  free (self);
  }


/*============================================================================

  device_get_name

============================================================================*/
const char *device_get_name (const Device *self)
  {
  return self->name;
  }


/*============================================================================

  device_new

============================================================================*/
Device *device_new (const char *name, int major, int minor, int32_t st_mode, 
      DevOpenAsFileFn open_file_fn, DevOpenAsBlockFn open_block_fn, 
      DevCloseAndCleanupFn close_and_cleanup_fn, void *data) 
  {
  Device *self = malloc (sizeof (Device));
  strncpy (self->name, name, sizeof (self->name));
  self->major = major;
  self->minor = minor;
  self->st_mode = st_mode;
  self->open_file_fn = open_file_fn;
  self->open_block_fn = open_block_fn;
  self->close_and_cleanup_fn = close_and_cleanup_fn;
  self->data = data;
  return self;
  }


/*============================================================================

  device_open_as_block

============================================================================*/
struct BlockOps *device_open_as_block (Device *self)
  {
  if (self->open_block_fn)
    return self->open_block_fn (self->data);
  else
    {
    set_errno (ENOTBLK);
    return NULL;
    }
  }


/*============================================================================

  device_open_as_file

============================================================================*/
struct FileOps *device_open_as_file (Device *self)
  {
  return self->open_file_fn (self->data);
  }


