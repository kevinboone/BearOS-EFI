/*============================================================================

  interface_ff.h

  BearOS interface to ChaN's VFAT filesystem module
  Note that the main functions -- disk_write, etc. -- are declared
  in diskio.h. However, these declarations depend on types defined in 
  ff.h. So the order of inclusion is relevant.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "blockops.h"

/** Maximum number of VFAT drives we allow. This number sets the size of 
    the mapping table from FF drive numbers to BlockOps structures. 
    There is no other limitation (that I know of).
*/
#define FF_MAX_DRV 10

// DWORD is the way FF defines a 32-bit quantity, but I don't want to include
//  headers here
#ifndef DWORD
#define DWORD uint32_t
#endif


// Stores the blockops structure in the FF mapping table, and returns the
//   drive number (starting from 0)
extern int    interface_ff_add_mapping       (struct BlockOps *blockops);
extern DWORD  interface_ff_epoch_to_fattime  (time_t epoch);
extern int    interface_ff_fresult_to_errno  (int fresult);
extern BYTE   interface_ff_posix_to_fat_mode (int posix_mode);
extern time_t interface_ff_fattime_to_epoch  (DWORD ftime);
extern void   interface_ff_remove_mapping    (BYTE pdrv);

