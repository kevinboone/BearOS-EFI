/*============================================================================

  dirent.h 

  BearOS alternative to the conventional opendir(), etc 

============================================================================*/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// IMPORTANT...
// This structure must be an exact match for the struct MYDIR defined in
//   compat_dirent.h for the kernel. We can use the name DIR in the kernel
//   because (Grr...) this name clashes with the same name in ff.h.

typedef struct _MYDIR
  {
  int fd;
  struct dirent entry;
  } MYDIR;

/*============================================================================

  opendir.h

============================================================================*/
DIR *opendir (const char *pathname)
  {
  MYDIR *ret = NULL; 

  int f = open (pathname, O_RDONLY);

  if (f >= 0)
    {
    struct stat sb;
    if (fstat (f, &sb) == 0)
      {
      if ((sb.st_mode & S_IFMT) == S_IFDIR)
        {
        ret = malloc (sizeof (MYDIR));
        ret->fd = f;
        }
     else
        {
        errno = ENOTDIR;
        close (f);
        }
      }
    }
  else
    {
    // errno already set
    }

  return (DIR *)ret;
  }


/*============================================================================

  closedir

============================================================================*/
int closedir (DIR *_dir)
  {
  int ret = 0;
  MYDIR *dir = (MYDIR *)_dir;
  if (dir->fd >= 0) ret = close (dir->fd);
  free (dir);
  return ret;
  }


/*============================================================================

  readdir

============================================================================*/
struct dirent *readdir (DIR *_dir)
  {
  MYDIR *dir = (MYDIR *)_dir;
  struct dirent de;
  int n = read (dir->fd, (void *)&de, sizeof (de));
  if (n > 0)
    {
    strncpy (dir->entry.d_name, de.d_name, sizeof (dir->entry.d_name));
    return &dir->entry;
    }

  return NULL;
  }



