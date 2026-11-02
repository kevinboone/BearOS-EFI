/*============================================================================

  syscall.c

  Implementation of syscall() compatibility method.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "defs.h"
#include "../../lib/include/stddef.h"
#include "../../lib/include/stdint.h"

#ifdef LINUX
#include "../../lib/include/bearos/apidefs.h"
#else
#include "bearos/apidefs.h"
#endif

#include "compat_fcntl.h"
#include "compat_stdio.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "compat_unistd.h"
#include "compat_utime.h"
#include "../../lib/include/asm/utime.h"
#include "../../lib/include/asm/vfs.h"
#include "../../lib/include/errno_numbers.h"
#include "mounts.h"
#include "myioctl.h"
#include "process.h"
#include "procmgr.h"
#include "reboot.h"

extern intptr_t mysyscall (BearosContext *context, 
    long number, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4,
    intptr_t arg5, intptr_t arg6)
  {
  int64_t ret = 0;
  if (number == BEAROS_SYSCALL_CONOUT)
     {
     const char *s = (const char *)arg1;
     myprintf ("%s", s);
     }
  else if (number == BEAROS_SYSCALL_GETCWD)
     {
     char *buff = (char *)arg1;
     int maxlen = arg2;
     strncpy (buff, (char *)process_get_cwd (context->process), maxlen); 
     }
  else if (number == BEAROS_SYSCALL_READ)
     {
     int fd = (int)arg1;
     void *buff = (char *)arg2;
     size_t maxlen = arg3;
     ret = process_read_fd (context->process, fd, buff, maxlen);
     }
  else if (number == BEAROS_SYSCALL_WRITE)
     {
     int fd = (int)arg1;
     void *buff = (char *)arg2;
     size_t maxlen = arg3;
     ret = process_write_fd (context->process, fd, buff, maxlen);
     }
  else if (number == BEAROS_SYSCALL_MALLOC)
     {
     size_t size = (size_t)arg1;
     ret = (int64_t) malloc (size);
     }
  else if (number == BEAROS_SYSCALL_FREE)
     {
     void *mem = (void *)arg1;
     free (mem);
     ret = 0;
     }
  else if (number == BEAROS_SYSCALL_REALLOC)
     {
     void *mem = (void *)arg1;
     size_t size = (size_t)arg2;
     ret = (int64_t) realloc (mem, size);
     }
  else if (number == BEAROS_SYSCALL_IOCTL)
     {
     int fd = arg1;
     long request = arg2;
     intptr_t r1 = arg3;
     intptr_t r2 = arg4;
     intptr_t r3 = arg5;
     ret = myioctl (context->process, fd, request, r1, r2, r3);
     }
  else if (number == BEAROS_SYSCALL_EXEC)
    {
    const UTF8 *path = (const UTF8 *)arg1;
    int argc = (int)arg2;
    char **argv = (char **)arg3;
    char **envp = (char **)arg4;
    int flags = (int)arg5;

    ret = procmgr_run_binary_or_script (path, argc, argv, envp, flags);
    }
  else if (number == BEAROS_SYSCALL_SETCWD)
    {
    UTF8 *cwd = (UTF8 *)arg1;
    ret = process_set_cwd (procmgr_get_current_process(), cwd); 
    }
  else if (number == BEAROS_SYSCALL_OPEN)
    {
    UTF8 *path = (UTF8 *)arg1;
    int mode = arg2;
    ret = myopen ((char *)path, mode); 
    }
  else if (number == BEAROS_SYSCALL_CLOSE)
    {
    int fd = arg1;
    ret = myclose (fd); 
    }
  else if (number == BEAROS_SYSCALL_FSTAT)
    {
    int fd = arg1;
    struct stat *sb = (struct stat *)arg2;
    ret = myfstat (fd, sb); 
    }
  else if (number == BEAROS_SYSCALL_STATFS)
    {
    UTF8 *pathname = (UTF8 *)arg1;
    struct statfs *sb = (struct statfs *)arg2;
    ret = mystatfs ((const char *)pathname, sb); 
    }
  else if (number == BEAROS_SYSCALL_REBOOT)
    {
    int cmd = arg1;
    ret = myreboot (cmd);
    }
  else if (number == BEAROS_SYSCALL_UNLINK)
    {
    const char *pathname = (const char *)arg1;
    ret = myunlink (pathname);
    }
  else if (number == BEAROS_SYSCALL_MKDIR)
    {
    const char *pathname = (const char *)arg1;
    int mode = (int)arg2;
    ret = mymkdir (pathname, mode);
    }
  else if (number == BEAROS_SYSCALL_RMDIR)
    {
    const char *pathname = (const char *)arg1;
    ret = myrmdir (pathname);
    }
  else if (number == BEAROS_SYSCALL_UTIME)
    {
    UTF8 *pathname = (UTF8 *)arg1;
    struct utimbuf *ub = (struct utimbuf *)arg2;
    ret = myutime ((const char *)pathname, ub); 
    }
  else if (number == BEAROS_SYSCALL_LSEEK)
    {
    int fd = (int)arg1;
    off_t offset = (off_t)arg2;
    int whence = (int)arg3; 
    ret = mylseek (fd, offset, whence);
    }
  else if (number == BEAROS_SYSCALL_MOUNT)
    {
    const char *source = (const char*)arg1;
    const char *target = (const char*)arg2;
    const char *type = (const char*)arg3;
    unsigned long flags = arg4;
    void *data = (void *)arg5; 
    ret = mounts_mount (source, target, type, flags, data);
    }
  else if (number == BEAROS_SYSCALL_UMOUNT)
    {
    const char *target = (const char*)arg1;
    ret = mounts_umount_by_mount_point ((UTF8 *)target);
    }
  else
    {
    set_errno (ENOTSUP); // Undefined syscall number
    ret = -1;
    }
  context->_errno = errno;
  return ret;
  }

