/*============================================================================

  stat.c 

  BearOS alternative to the conventional stat functions.

============================================================================*/

#include <bearos/apidefs.h> 
#include <fcntl.h> 
#include <sys/stat.h>
#include <unistd.h> 
#include <syscall.h> 
#include <sys/vfs.h> 

/*============================================================================

  fstat

============================================================================*/
int fstat (int fd, struct stat *restrict statbuff)
  {
  return syscall (BEAROS_SYSCALL_FSTAT, (intptr_t)fd, 
         (intptr_t)statbuff, 0, 0, 0, 0);
  }


/*============================================================================

  mkdir 

============================================================================*/
int mkdir (const char *pathname, int mode)
  {
  return syscall (BEAROS_SYSCALL_MKDIR, (intptr_t)pathname, 
         (intptr_t)mode, 0, 0, 0, 0);
  }


/*============================================================================

  stat

============================================================================*/
int stat (const char *restrict pathname, struct stat *restrict statbuff)
  {
  int ret = 0;
  int fd = open (pathname, O_RDONLY);
  if (fd >= 0)
    {
    ret = fstat (fd, statbuff);
    close (fd);
    }
  else 
    {
    ret = -1; // errno already set
    }
  return ret;
  }


/*============================================================================

  stat

============================================================================*/
int statfs (const char *filename, struct statfs *sb)
  {
  return syscall (BEAROS_SYSCALL_STATFS, (intptr_t)filename, 
         (intptr_t)sb, 0, 0, 0, 0);
  }


