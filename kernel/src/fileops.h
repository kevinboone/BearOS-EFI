/*============================================================================

  fileops.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/stdint.h"
#include "../../lib/include/stddef.h"
#include "compat_stat.h" 

struct FileOps;

typedef int (*FileOpsCloseAndCleanupFn) (struct FileOps *fileops);
typedef int (*FileOpsIoctlFn) (struct FileOps *fs, int fd, long request,
                     intptr_t arg1, intptr_t args, intptr_t arg3);
typedef ssize_t (*FileOpsReadFn) (struct FileOps *fileops, 
    void *buff, size_t count);
typedef ssize_t (*FileOpsWriteFn) (struct FileOps *fs, const void *buff, size_t count);
typedef int     (*FileOpsFstatFn) (struct FileOps *fs, struct stat *statbuff);
typedef off_t   (*FileOpsLseekFn) (struct FileOps *fs, off_t offset, int whence); 


struct FileOps
 {
 void *data;
 FileOpsCloseAndCleanupFn close_and_cleanup;
 FileOpsFstatFn fstat;
 FileOpsLseekFn lseek;
 FileOpsIoctlFn ioctl;
 FileOpsReadFn  read;
 FileOpsWriteFn write;
 int refs;
 };

