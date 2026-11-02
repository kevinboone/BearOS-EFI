/*============================================================================

  devmgr.h

  The device manager -- essentially a List of Device objects. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "device.h"
#include "fileops.h"
#include "list.h"

struct _DevMgr;
typedef struct _DevMgr DevMgr;

// Global device manager
extern DevMgr *devmgr;

extern void devmgr_cleanup (void); 

extern void devmgr_init (void); 

/** Add a device to the devmgr's list. This action does not imply any change
    of control -- the caller retains ownership of the device, and should
    only shut it down once it has removed the device from the devmgr.

    However, DevMgr does take control of the Device _object_, and will 
    destroy it when it shuts down, or when the Device is removed from
    the list of devices. 
 */
extern void devmgr_add_device (Device *device);

/** Populates a List of UTF8 strings. */
extern void devmgr_get_device_names (List *names);


/** Get a FileOps for the named device, if it is registered. If not, 
    returns NULL and sets errno. The caller owns the returned
    FileOps, and should call its close_and_cleanup method when done with it.
*/
extern struct FileOps *devmgr_open_as_file_by_name (const char *name);

extern struct BlockOps *devmgr_open_as_block_by_name (const char *name);

extern void devmgr_scan_storage (void);

