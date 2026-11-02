/*============================================================================

  shell.c

  Load and run the default init process (which might just be a shell). 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "defs.h" 
#include "compat_stdio.h" 
#include "defaults.h" 
#include "elfloader.h" 
#include "procmgr.h"
#include "shell.h"

/*============================================================================

  shell_run

============================================================================*/
int shell_run (const BearosContext *bc, int argc, char **argv, char **envp)
  {
  char *exec_path = DEFAULT_INIT;

  Process *p = procmgr_create_child_from_elf (procmgr_get_current_process(),
     (UTF8 *)exec_path, 0);
  if (p)
    {
    procmgr_run_process (p, 0, NULL);
    procmgr_process_finished (p);
    myprintf ("System halted\n");
    }
  else
    myprintf ("Can't run %s -- panic!\n", DEFAULT_INIT);

  return 0;
  }


