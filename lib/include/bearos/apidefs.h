/*============================================================================

  apidefs.h 

  Defines the interface between the "kernel" and the programs it
  runs.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <stdint.h>

// Console output string, no redirection
#define BEAROS_SYSCALL_CONOUT      0 
// Get current directory 
#define BEAROS_SYSCALL_GETCWD      1 
// read file or device 
#define BEAROS_SYSCALL_READ        2 
// write file or device 
#define BEAROS_SYSCALL_WRITE       3 
// Get a directory entry (only one, not like the Linux version) 
//#define BEAROS_SYSCALL_GETDENTS    4 // No longer used
// Allocate memory
#define BEAROS_SYSCALL_MALLOC      5
// Free memory
#define BEAROS_SYSCALL_FREE        6
// Reallocate memory
#define BEAROS_SYSCALL_REALLOC     7
// ioctl() 
#define BEAROS_SYSCALL_IOCTL       8
// execute process
#define BEAROS_SYSCALL_EXEC        9
// Change directory 
#define BEAROS_SYSCALL_SETCWD     10 
// Open a file 
#define BEAROS_SYSCALL_OPEN       11 
// Close a file 
#define BEAROS_SYSCALL_CLOSE      12 
// fstat a file 
#define BEAROS_SYSCALL_FSTAT      13 
// statfs a filesystem, given a file 
#define BEAROS_SYSCALL_STATFS     14 
// Reboot, suspend, etc
#define BEAROS_SYSCALL_REBOOT     15
// Delete file 
#define BEAROS_SYSCALL_UNLINK     16
// Create directory 
#define BEAROS_SYSCALL_MKDIR      17
// Delete directory 
#define BEAROS_SYSCALL_RMDIR      18
// Set file timestamps 
#define BEAROS_SYSCALL_UTIME      19
// Seek within file 
#define BEAROS_SYSCALL_LSEEK      20 
// Mount a filesystem 
#define BEAROS_SYSCALL_MOUNT      21 
// Unmount a filesystem 
#define BEAROS_SYSCALL_UMOUNT     22 

struct _BearosContext;
struct _Process;

typedef int64_t (*BearosSyscallFn) (struct _BearosContext *context, 
  long number, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4,
  intptr_t arg5, intptr_t arg6);

typedef struct _BearosContext 
  {
  BearosSyscallFn syscall_fn; 
  intptr_t exit_jmp_buf[5];
  struct _Process *process;
  int _errno;
  } BearosContext;

typedef int (*BearosStartFn) (const BearosContext *context, 
  int args, char **argv, char **envp);

extern BearosContext *bearos_context;

