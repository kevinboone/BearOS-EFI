/*============================================================================

  devfs.h

  /dev is a filesystem handled by the generic pseudo-filesystem 
  PseudoFsHandler. All this module does is to provide an implementation 
  of a root-directory populator for the pseudo-filesystem. And all
  we need to do that is to get the list of devices from the device manager.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "list.h"
#include "fileops.h"

extern int devfs_populate_root (List *entries);
extern struct FileOps *devfs_open_file (const char *name);

