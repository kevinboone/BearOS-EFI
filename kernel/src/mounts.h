/*============================================================================

  mounts.h

  Manages the mount table.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"
#include "fsops.h"

struct _Mount;
typedef struct _Mount Mount;

extern void mounts_cleanup (void);
extern void mounts_init (void);

/** Gets the FSOps structure that handles `path`, and writes `rem_path`
    to indicate the path relative to the root. Caller must free `rem_path`
    if this function returns non-zero. On failure, errno set most likely
    to ENOENT. 

    If rem_path == NULL, then the remaining path is not calculated,
    and no memory is allocated.

    NOTE: path must be a canonical, absolute path!
*/
extern struct FSOps *mounts_get_fsops_by_path (const UTF8 *path, 
      UTF8 **rem_path);

/** Gets the mount table in text form, to be presented as /proc/mounts. 
    Caller should free the text. */
extern char *mounts_get_text_form (void);

extern int mounts_mount (const char *source, const char *target, const char *type,
      unsigned long flags, const void *data);

extern int mounts_umount_by_mount_point (UTF8 *mount_point);

