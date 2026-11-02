/*============================================================================

  process.c

  Models a running process. See process.h for more details.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_limits.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "compat_stdio.h"
#include "consolefilehandler.h"
#include "devmgr.h"
#include "fileops.h"
#include "../../lib/include/errno_numbers.h"
#include "../../lib/include/bearos/apidefs.h"
#include "list.h"
#include "path.h"
#include "process.h"
#include "procmgr.h"
#include "syscall.h"

struct _Process
  {
  BearosStartFn fn;
  UTF8 cwd[PATH_MAX];
  int _errno;
  struct FileOps *file_ops[NFILES_MAX];
  char **argv;
  int argc;
  char **envp; 
  BearosContext bc;
  void *mem_base;
  };


/*============================================================================

  process_clone

============================================================================*/
Process *process_clone (const struct _Process *process)
  {
  Process *self = malloc (sizeof (Process));

  // It makes no sense to clone the memory associated with the running
  ///  parent process. Well, not in BearOS, anyway. We don't support fork()
  self->mem_base = NULL;

  self->fn = process->fn;
  self->argv = NULL; 
  self->argc = 0;
  self->envp = NULL;

  if (process->envp)
    process_set_and_copy_envp (self, process->envp);
  
  strcpy ((char *)self->cwd, (char *)process->cwd);

  self->_errno = process->_errno;

  for (int i = 0; i < NFILES_MAX; i++)
    {
    struct FileOps *fs = process->file_ops[i];
    if (fs)
      {
      self->file_ops[i] = fs;
      self->file_ops[i]->refs++; // Let's think about this...
      }
    else
      { 
      self->file_ops[i] = NULL;
      }
    }
  return self;
  }


/*============================================================================

  process_close_file

============================================================================*/
int process_close_fd (Process *self, int fd)
  {
  int ret = -1;
  struct FileOps *fs = self->file_ops[fd];
  if (fs)
    {
    fs->refs--;
    if (fs->refs == 0)
      {
      //myprintf ("fd %d ready to close\n", fd);
      ret = fs->close_and_cleanup (fs);
      free (fs);
      }
    self->file_ops[fd] = NULL;
    }
  return ret;
  }

/*============================================================================

  process_close_all_files

============================================================================*/
static void process_close_all_files (Process *self)
  {
  for (int i = 0; i < NFILES_MAX; i++)
    {
    process_close_fd (self, i);
    }
  }

/*============================================================================

  process_destroy_environment

============================================================================*/
static void process_destroy_environment (Process *self)
  {
  if (self->envp)
    {
    for (char **env = self->envp; *env != 0; env++)
      {
      char *t = *env;
      free (t); 
      }
    free (self->envp);
    }
  self->envp = NULL;
  }

/*============================================================================

  process_destroy

============================================================================*/
void process_destroy (Process *self)
  {
  process_close_all_files (self);
  if (self->argv)
    {
    for (int i = 0; i < self->argc; i++)
      free (self->argv[i]);
    free (self->argv);
    } 

  if (self->mem_base) free (self->mem_base);
  process_destroy_environment (self);
  free (self);
  }

/*============================================================================

  process_get_cwd

============================================================================*/
const UTF8 *process_get_cwd (const Process *self)
  {
  return self->cwd;
  }

/*============================================================================

  process_get_errno

============================================================================*/
int process_get_errno (const Process *self)
  {
  return self->_errno;
  }

/*============================================================================

  process_get_errno

============================================================================*/
struct FileOps *process_get_fileops (Process *self, int fd)
  {
  if (fd < 0 || fd >= NFILES_MAX)
    {
    process_set_errno (self, EBADF);
    return NULL;
    }

  struct FileOps *fs = self->file_ops[fd];
  if (fs == NULL)
    {
    process_set_errno (self, EBADF);
    return NULL;
    }

  return fs;
  }

/*============================================================================

  process_get_next_fd

============================================================================*/
int process_get_next_fd (const Process *self)
  {
  for (int i = 0; i < NFILES_MAX; i++)
    {
    struct FileOps *fs = self->file_ops[i];
    if (!fs) return i;
    }

  return -1;
  set_errno (ENFILE);
  }

/*============================================================================

  process_ioctl_fd

============================================================================*/
int process_ioctl_fd (Process *self, int fd,
        long request, intptr_t arg1, intptr_t arg2, intptr_t arg3)
  {
  struct FileOps *fs = process_get_fileops (self, fd);
  if (fs == NULL) return -1; // errno has been set
  if (fs->ioctl)
    {
    int ret = fs->ioctl (fs, fd, request, arg1, arg2, arg3);
    return ret;
    }
  set_errno (ENOTSUP);
  return -1;
  }


/*============================================================================

  process_init_env

============================================================================*/
void process_init_env (Process *self)
  {
  self->envp = malloc (3 * sizeof (char*));
  self->envp[0] = strdup ("PATH=/bin:/home/bin");
  self->envp[1] = strdup ("HOME=/home"); // TODO??
  self->envp[2] = NULL; 
  }


/*============================================================================

  process_init_std

============================================================================*/
void process_init_std (Process *self)
  {
  struct FileOps *fs = devmgr_open_as_file_by_name ("console");
  self->file_ops[0] = fs;  
  self->file_ops[1] = fs;  
  self->file_ops[2] = fs;  
  fs->refs = 3; // TODO
  }

/*============================================================================

  process_new

============================================================================*/
Process *process_new (BearosStartFn fn, const char **envp)
  {
  Process *self = malloc (sizeof (Process));

  self->mem_base = NULL;
 
  int num_env = 0;
  if (envp)
    {
    for (const char **env = envp; *env != 0; env++)
      num_env++;

    self->envp = malloc ((num_env + 1) * sizeof (char *));

    for (int i = 0; i < num_env; i++)
      self->envp[i] = strdup (envp[i]);
    self->envp[num_env] = NULL;
    }

  self->fn = fn;
  strcpy ((char *)self->cwd, "/");
  self->_errno = 0;
  for (int i = 0; i < NFILES_MAX; i++)
    {
    self->file_ops[i] = NULL;
    }
  return self;
  }

/*============================================================================

  process_read_fd

============================================================================*/
size_t process_read_fd (Process *self, int fd, void *buff, size_t maxlen)
  {
  struct FileOps *fs = process_get_fileops (self, fd);
  if (fs == NULL) return -1; // errno has been set
  if (fs->read)
    return fs->read (fs, buff, maxlen);
  set_errno (ENOTSUP);
  return (size_t)-1;
  }

/*============================================================================

  process_run

============================================================================*/
int process_run (Process *self, int argc, char **argv)
  {
  self->argv = malloc ((argc + 1) * sizeof (char *));
  for (int i = 0; i < argc; i++)
     self->argv[i] = strdup (argv[i]); 
  self->argv[argc] = 0;
  self->argc = argc;

  self->bc.process = self;
  self->bc.syscall_fn = mysyscall;

  int ret = self->fn (&self->bc, self->argc, self->argv, self->envp);
  return ret;
  }

/*============================================================================

  process_set_and_copy_envp 

============================================================================*/
void process_set_and_copy_envp (Process *self, char **envp)
  {
  process_destroy_environment (self);

  int num_env = 0;
  if (envp)
    {
    for (char **env = envp; *env != 0; env++)
      num_env++;

    self->envp = malloc ((num_env + 1) * sizeof (char *));

    for (int i = 0; i < num_env; i++)
      self->envp[i] = strdup (envp[i]);
    self->envp[num_env] = NULL;
    }
  }

/*============================================================================

  process_set_cwd

============================================================================*/
int process_set_cwd (Process *self, UTF8 *cd)
  {
  int ret = 0;
  Path *p = path_new (self->cwd);

  path_append (p, cd);

  if (path_is_directory (p))
    {
    path_canonicalize (p);
    ret = 0;
    }
  else
    {
    self->_errno = ENOTDIR; 
    ret = -1;
    }
  
  if (ret == 0)
    strncpy ((char *)self->cwd, (char *)path_cstr (p), PATH_MAX); 
  
  path_destroy (p);
  return ret;
  }

/*============================================================================

  process_set_entry

============================================================================*/
void process_set_entry (Process *process, BearosStartFn entry)
  {
  process->fn = entry;
  }

/*============================================================================

  process_set_errno

============================================================================*/
void process_set_errno (Process *process, int _errno)
  {
  // Check process is non-null: this method may be called during
  //   shutdown, when there is no current process.
  if (process)
    process->_errno = _errno;
  }

/*============================================================================

  process_set_fileops

============================================================================*/
int process_set_fileops (Process *self, int fd, struct FileOps *fs)
  {
  if (fd < 0 || fd >= NFILES_MAX)
    {
    process_set_errno (self, EBADF);
    return -1;
    }
  self->file_ops[fd] = fs;
  return 0;
  }


/*============================================================================

  process_set_mem_base

============================================================================*/
void process_set_mem_base (Process *process, void *mem_base)
  {
  process->mem_base = mem_base;
  }


/*============================================================================

  process_write_fd

============================================================================*/
size_t process_write_fd (Process *self, int fd, void *buff, size_t maxlen)
  {
  struct FileOps *fs = process_get_fileops (self, fd);
  if (fs == NULL) return -1; // errno has been set
  if (fs->write)
    return fs->write (fs, buff, maxlen);
  set_errno (ENOTSUP);
  return (size_t)-1;
  }


