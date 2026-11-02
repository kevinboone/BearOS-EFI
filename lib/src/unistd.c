/*============================================================================

  unistd.c 

  BearOS alternative to the conventional unistd functions

============================================================================*/

//#include <efi/efi.h>
//#include <efi/efilib.h>
#include <asm/termbits.h>
#include <asm/termios.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "bearos/apidefs.h"


/*============================================================================

  close 

============================================================================*/
int close (int fd)
  {
  return syscall (BEAROS_SYSCALL_CLOSE, (intptr_t)fd, 
         0, 0, 0, 0, 0);
  }


/*============================================================================

  getcwd 

============================================================================*/
char *getcwd (char *buff, size_t maxlen)
  {
  return (char *)syscall (BEAROS_SYSCALL_GETCWD, (intptr_t)buff, 
         maxlen, 0, 0, 0, 0);
  }


/*============================================================================

  chdir 

============================================================================*/
int chdir (const char *path)
  {
  return (int)syscall (BEAROS_SYSCALL_SETCWD, (intptr_t)path, 
         0, 0, 0, 0, 0);
  }


/*============================================================================

  find_executable

  Helper for runvpe() etc., that searches the $PATH for a specific executable
  file. 

============================================================================*/
static int find_executable (const char *cmd, char **exec_path, int use_path)
  {
  struct stat sb;
  if (stat (cmd, &sb) == 0)
    {
    // We've given a real file -- no need to check $PATH

    // TODO we should check the executable status, when we ever get
    //   to supporting filesystems that have such a thing. VFAT
    //   does not.
    *exec_path = strdup (cmd);
    return 0;
    }

  if (!use_path) return -1;

  // We won't try expanding $PATH if the command is itself a pathname
  if (strchr (cmd, '/')) return -1;

  const char *path = getenv ("PATH");
  if (path == NULL) return -1; 

  char *path_copy = strdup (path);

  int found = 0;

  char *p = strtok (path_copy, ":");
  while (p && !found) 
    {
    // TODO: tidy this up
    char try[PATH_MAX];
    strcpy (try, p);
    if (p[strlen (p) - 1] != '/')
      strcat (try, "/");
    strcat (try, cmd);

    int ret = find_executable (try, exec_path, 0); 
    if (ret == 0) found = 1; 
  
    p = strtok (NULL, ":");
    } 

  free (path_copy);
 
  if (found) return 0;
  return -1;
  }


/*============================================================================

  isatty 

============================================================================*/
int isatty (int fd)
  {
  struct winsize ws;
  int ret = ioctl (fd, TIOCGWINSZ, (intptr_t)&ws, 0, 0);
  return ret == 0;
  }


/*============================================================================

  read 

============================================================================*/
ssize_t read (int fd, void *buff, size_t maxlen)
  {
  return syscall (BEAROS_SYSCALL_READ, fd, (intptr_t)buff, maxlen, 0, 0, 0);
  }


/*============================================================================

  rmdir 

============================================================================*/
int rmdir (const char *pathname)
  {
  return syscall (BEAROS_SYSCALL_RMDIR, (intptr_t)pathname, 
         0, 0, 0, 0, 0);
  }


/*============================================================================

  _runv 

============================================================================*/
int _runvpe (const char *file, char *const argv[], char *const envp[], 
        int use_path)
  {
  char *exec_path = NULL;
  if (find_executable (file, &exec_path, use_path) == 0)
    {
    int argc = 0;
    char *const *nargv = argv;
    while (*nargv++) argc++;

    int flags = 0;
    int ret = (int)syscall (BEAROS_SYSCALL_EXEC, (intptr_t)exec_path, 
         argc, (intptr_t)argv, (intptr_t)envp, flags, 0);

    free (exec_path);
    return ret;
    } 
  else
    {
    errno = ENOENT;
    return -1;
    }
  }

/*============================================================================

  runv 

============================================================================*/
int runv (const char *pathname, char *const argv[])
  {
  return _runvpe (pathname, argv, environ, 0);
  }

/*============================================================================

  runvp 

============================================================================*/
int runvp (const char *file, char *const argv[])
  {
  return _runvpe (file, argv, environ, 1);
  }

/*============================================================================

  runvpe 

============================================================================*/
int runvpe (const char *file, char *const argv[], char *const envp[])
  {
  return _runvpe (file, argv, envp, 1);
  }


/*============================================================================

  lseek 

============================================================================*/
off_t lseek (int fd, off_t offset, int whence)
  {
  return syscall (BEAROS_SYSCALL_LSEEK, (intptr_t)fd, (intptr_t)offset, 
          (intptr_t)whence, 0, 0, 0);
  }


/*============================================================================

  unlink 

============================================================================*/
int unlink (const char *pathname)
  {
  return syscall (BEAROS_SYSCALL_UNLINK, (intptr_t)pathname, 0, 0, 0, 0, 0);
  }


/*============================================================================

  write 

============================================================================*/
ssize_t write (int fd, const void *buff, size_t maxlen)
  {
  ssize_t ret = syscall (BEAROS_SYSCALL_WRITE, fd, (intptr_t)buff, maxlen, 0, 0, 0);
  return ret;
  }



