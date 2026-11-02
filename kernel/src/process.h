/*============================================================================

  process.h

  Models a running process. The process has a working directory, an 
  environment, an error code (errno), and a table of open files.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "defs.h"
#include "fileops.h"
#include "../../lib/include/bearos/apidefs.h"

struct _Process;

typedef struct _Process Process;

/** Duplicate this process. `struct fileops` entries in the open file table
    are not copied, but their references are increased by one. */
extern Process *process_clone (const struct _Process *process);

/** Close a file in the open file table of this process. */
extern int process_close_fd (Process *self, int fd);

extern void process_destroy (Process *self);

/** Get the working directory for this process. */
extern const UTF8 *process_get_cwd (const Process *self);

/** Get the error code associated with this process. */
extern int process_get_errno (const Process *self);

/** Get the next available file handle. Returns -1 and sets errno if
    the table is full. Callers should be aware that, in principle, 
    another part of the code could also be working on the file
    table, and sould lock accordingly. */
extern int process_get_next_fd (const Process *self);

/** Call ioctl() on a file in this processes's file table. */
extern int process_ioctl_fd (Process *self, int fd,
        long request, intptr_t arg1, intptr_t arg2, intptr_t arg3); 

/** Initialize the environment for this process. */
extern void process_init_env (Process *self);

/** Create open files for stdin, stdout, and stderr for this process. */
extern void process_init_std (Process *self);

/** Note that the process will copy the supplied environment. */
extern Process *process_new (BearosStartFn fn, const char **envp);

/** Call read() on a file in this processes's file table. */
extern size_t process_read_fd (Process *self, int fd, 
        void *buff, size_t maxlen);

/** Run the process with supplied argv. */
extern int process_run (Process *self, int argc, char **argv);

/** Sets the environment after a clone() call. clone() will give a child
    process the parent's environment, but a caller may prefer to set
    a different one. This method frees any existing env memory. */
extern void process_set_and_copy_envp (Process *self, char **envp);

extern int process_set_cwd (Process *process, UTF8 *cwd);

/** Sets the point in memory at which the program was loaded. The 
    program entry point will generally _not_ be this address. */
extern void process_set_mem_base (Process *process, void *mem_base);

/** Sets the address at which to start execution. Generally it will
    be an address withing the block that starts with mem_base. */
extern void process_set_entry (Process *process, BearosStartFn entry);

/** Sets the current error code. Callers should probably use the
    `set_errno` macro to call this, since the implementation
    may change. */
extern void process_set_errno (Process *process, int _errno);

/** Call write() on a file in this processes's file table. */
extern size_t process_write_fd (Process *self, int fd, 
        void *buff, size_t maxlen);

/** Note: get_filestruct() can set errno. */
extern struct FileOps *process_get_fileops (Process *self, int fd);
extern int process_set_fileops (Process *self, int fd, struct FileOps *fs);

#define errno process_get_errno(procmgr_get_current_process())
#define set_errno(n) process_set_errno(procmgr_get_current_process(),n)

// We demarcate critical sections that may not be interrupted, in the
//   expectation that one day there might be a way to interrupt them.
// Right now, the implementation is empty.
#define PROC_BEGIN_SYNC
#define PROC_END_SYNC

