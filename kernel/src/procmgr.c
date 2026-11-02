/*============================================================================

  procmgr.c

  Process manager. See procmgr.h for more details. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "defs.h" 
#include "compat_fcntl.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "compat_stdio.h"
#include "compat_unistd.h"
#include "kernelringbuffer.h"
#include "procmgr.h"
#include "process.h"
#include "elfloader.h"

struct _ProcMgr
  {
  Process *current_process;
  };

ProcMgr *procmgr = NULL;

// FWD
static void procmgr_destroy (ProcMgr *self);
static ProcMgr *procmgr_new (void);

/*============================================================================

  procmgr_cleanup

============================================================================*/
void procmgr_cleanup (void)
  {
  procmgr_destroy (procmgr);
  }


/*============================================================================

  procmgr_create_process

============================================================================*/
Process *procmgr_create_child_process (const Process *parent, 
          void *prog, BearosStartFn entry, int flags)
  {
  Process *p = process_clone (parent);
  if (p)
    {
    process_set_mem_base (p, prog);
    process_set_entry (p, entry);
    }
  return p;
  }


/*============================================================================

  procmgr_create_child_from_elf 

============================================================================*/
Process *procmgr_create_child_from_elf (const Process *parent, 
          const UTF8 *filename, int flags)
  {
  Process *ret = NULL;
  ElfLoader *elf = elfloader_new ((UTF8 *)filename);
  void *prog = elfloader_load (elf);
  if (prog)
    {
    BearosStartFn start = (BearosStartFn) elfloader_get_start (elf);
    // The child process will own memory 'prog' and will free it on exit
    ret = procmgr_create_child_process (parent, prog, start, flags);
    }
  else
    {
    // errno should have been set
    }
  elfloader_destroy (elf);
  //free (exec_path);
  return ret;
  }


/*============================================================================

  procmgr_create_root_process

============================================================================*/
Process *procmgr_create_root_process (BearosStartFn entry)
  {
  kernelringbuffer_log (INFO, "Creating the root process");
  Process *process = process_new (entry, NULL);
  process_init_std (process);
  process_init_env (process);
  return process;
  }


/*============================================================================

  procmgr_get_current_process

============================================================================*/
Process *procmgr_get_current_process (void)
  {
  return procmgr->current_process;
  }

/*============================================================================

  procmgr_init

============================================================================*/
void procmgr_init (void)
  {
  kernelringbuffer_log (INFO, "Initializing process manager");
  procmgr = procmgr_new();
  }


/*============================================================================

  procmgr_destroy

============================================================================*/
static void procmgr_destroy (ProcMgr *self)
  {
  // TODO
  free (self);
  }

/*============================================================================

  procmgr_new

============================================================================*/
static ProcMgr *procmgr_new (void)
  {
  ProcMgr *self = malloc (sizeof (ProcMgr));
  self->current_process = NULL;
  return self;
  }


/*============================================================================

  procmgr_process_finished

============================================================================*/
void procmgr_process_finished (Process *process)
  {
  // TODO
  process_destroy (process);
  }


/*============================================================================

  procmgr_run_binary_or_script

============================================================================*/
int procmgr_run_binary_or_script (const UTF8 *path, int argc, char **argv, 
      char **envp, int flags)
  {
  int ret = 0;
  char *script_runner = NULL;
  char buff[256];

  int f = myopen ((const char *)path, O_RDONLY);
  if (f >= 0)
    {
    memset (buff, 0, sizeof (buff)); 
    myread (f, buff, sizeof (buff) - 1);
    if (strncmp ((char *)buff, "#!/", 3) == 0)
      {
      char *retpos = strchr (buff, '\n');
      if (retpos)
        {
        *retpos = 0;
        script_runner = buff + 2;
        }
      }
    myclose (f);
    }
  else
    return -1; // errno should have been set

  if (script_runner)
    {
    int newargc = argc + 1;
    char **newargv = malloc ((newargc + 1) * sizeof (char *));
    newargv[0] = strdup (script_runner);
    newargv[1] = strdup ((char *)path);
    for (int i = 2; i < argc; i++)
      newargv[i + 2] = strdup (argv[i]);
    newargv[newargc] = NULL; 
 
    ret = procmgr_run_binary_or_script ((UTF8 *)newargv[0], newargc, 
            newargv, envp, flags); 

    for (int i = 0; i < newargc; i++)
      free (newargv[i]);
    free (newargv);
    }
  else
    {
    Process *p = procmgr_create_child_from_elf (procmgr_get_current_process(), 
             (UTF8 *)path, flags);
    if (p)
       {
       if (envp) process_set_and_copy_envp (p, envp);
       ret = procmgr_run_process (p, argc, argv);
       procmgr_process_finished (p);
       // errno MUST be set zero if we successfully loaded and ran a process.
       // The exit code from the process reflects its internal success/failure
       //   state; errno is only used if we couldn't run anything.
       set_errno (0);
       }
    else 
       {
       // errno should have been set
       ret = -1;
       }
    }
  return ret;
  }

/*============================================================================

  procmgr_run_process

============================================================================*/
int procmgr_run_process (Process *process, int argc, 
      char **argv)
  {
  Process *old = procmgr_get_current_process();
  procmgr_set_current_process (process);
  int ret = process_run (process, argc, argv);
  procmgr_set_current_process (old);
  return ret;
  }


/*============================================================================

  procmgr_set_current_process

============================================================================*/
void procmgr_set_current_process (Process *process)
  {
  procmgr->current_process = process;
  }

