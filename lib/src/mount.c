/*============================================================================

  mount.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <bearos/apidefs.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <syscall.h>
#include <sys/mount.h>

/*============================================================================

  mount

============================================================================*/
int mount (const char *source, const char *target,
                 const char *type, unsigned long flags,
                 void *data)
  {
  if (!type || !target || !source) 
    {
    errno = EINVAL;
    return -1;
    }
 
  // The kernel won't check whether the target is a directory that
  //   actually exists, and will cheerfully create an unreachable mount.
  // So let's check it here.

  struct stat sb;
  if (stat (target, &sb) != 0)
    {
    errno = ENOTDIR;
    return -1;
    }

  if (!S_ISDIR (sb.st_mode))
    {
    errno = ENOTDIR;
    return -1;
    }

  int ret = syscall (BEAROS_SYSCALL_MOUNT, (intptr_t)source, (intptr_t)target, 
        (intptr_t)type, flags, (intptr_t)data, 0);

  return ret;
  }


/*============================================================================

  umount

============================================================================*/
int umount (const char *target)
  {
  if (!target) 
    {
    errno = EINVAL;
    return -1;
    }
 
  struct stat sb;
  if (stat (target, &sb) != 0)
    {
    errno = ENOTDIR;
    return -1;
    }

  if (!S_ISDIR (sb.st_mode))
    {
    errno = ENOTDIR;
    return -1;
    }

  int ret = syscall (BEAROS_SYSCALL_UMOUNT, (intptr_t)target, 0, 0, 0, 0, 0);

  return ret;
  }



