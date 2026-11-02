/*============================================================================

  blockops.h

  All block device handlers provide an implementation of `struct BlockOps`,
  which exposes methods for manipulating blocks. Note that this struct is
  _not_ and object, as understood in the BearOS implementation -- it is
  just a structure. Cleaning it up amounts to calling free() on it.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/stdint.h"
#include "../../lib/include/stddef.h"

struct BlockOps;

// read and write functions take an offset in sectors, and return
//   the number of sectors read or written. The offset is also in sectors, 
//   not bytes. The buffer must be large enough 
//   for num_sectors*sector_size bytes.
typedef ssize_t (*BlockOpsReadSectorsFn) (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors);

typedef ssize_t (*BlockOpsWriteSectorsFn) (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors);

struct BlockOps
 {
 void *data;
 int refs;
 BlockOpsReadSectorsFn read_sectors;
 BlockOpsWriteSectorsFn write_sectors;
 };


