/*============================================================================

  procfs.h

  /proc is a filesystem handled by the generic pseudo-filesystem 
  PseudoFsHandler. All this module does is to provide an implementation 
  of a root-directory populator for the pseudo-filesystem. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "list.h"
#include "fileops.h"

extern int procfs_populate_root (List *entries);
extern struct FileOps *procfs_open_file (const char *name);


