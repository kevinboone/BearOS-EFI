/*============================================================================

  procmgr.h

  Process manager. This class maintains the list of running processes,
  and provides ways to manage processes. There should only be one
  instance of this class, so the constructor and destructor are not
  exposed. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "process.h"
#include "../../lib/include/bearos/apidefs.h"

struct ProcMgr;
typedef struct _ProcMgr ProcMgr;

// Global process manager
extern ProcMgr *procmgr;

extern void procmgr_cleanup (void); 

/** Create a child process from a block of memory containing its code. 
  Sets errno on failure. */
extern Process *procmgr_create_child_process (const Process *parent, 
  void *prog, BearosStartFn entry, int flags);

/** Loads an ELF file into memory and runs its code as a process. 
   Sets errno on failure. */ 
Process *procmgr_create_child_from_elf (const Process *parent, 
  const UTF8 *filename, int flags);

/** Create the root process, which will be the parent, direct or indirect,
   of all subsequent processes. Sets errno on failure, but it's unlikly
   that a failure here will be recoverable. */
extern Process *procmgr_create_root_process (BearosStartFn entry);

/** Get the current process. Of necessity, any user-level code that calls
   this function will get a reference to its own process. */
extern Process *procmgr_get_current_process (void);

extern void procmgr_init (void); 

void procmgr_process_finished (Process *process);

/** Run a program, which may be an executable or, if the file starts with a
   'shebang' line, a script. */
int procmgr_run_binary_or_script (const UTF8 *path, int argc, char **argv, 
      char **envp, int flags);

/** Run a process, using the code and entry point stored in the 
    process previously. */
int procmgr_run_process (Process *process, int argc, char **argv);

/** Set the current process to a different one. Needless to say, user-level
    code shouldn't be calling this explicitly. */
void procmgr_set_current_process (Process *process);

