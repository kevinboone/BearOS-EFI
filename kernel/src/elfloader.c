/*============================================================================

  elfloader.c

  Loads an ELF executable into memory, preparatory to running it.

  See elfloader.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "../../lib/include/stdint.h"
#include "defs.h"
#include "compat_elf.h"
#include "compat_fcntl.h"
#include "compat_malloc.h"
#include "compat_stat.h"
#include "compat_string.h"
#include "compat_stdio.h"
#include "compat_unistd.h"
#include "elfloader.h"
#include "list.h"
#include "../../lib/include/errno_numbers.h"
#include "path.h"
#include "process.h"
#include "procmgr.h"
#include "syscall.h"
#include "string8.h"

//#define DEBUG_LOADER

/*============================================================================

Object attribute structure

============================================================================*/
struct _ElfLoader
  {
  Path *path;
  // Program entry point 
  BYTE *entry;
  };

/*============================================================================

  elfloader_destroy

============================================================================*/
void elfloader_destroy (ElfLoader *self)
  {
  path_destroy (self->path);
  free (self);
  }


/*============================================================================

  elfloader_check

============================================================================*/
int elfloader_check (const ElfLoader *self)
  {
  ELFHeader elf_header;
  int ret = 0;
  
#ifdef DEBUG_LOADER
    myprintf ("Trying to open path %s\n", path_cstr (self->path));
#endif

  int f = myopen ((char *)path_cstr (self->path), O_RDONLY);
  if (f < 0) return -1; // errno will have been set

  int n = myread (f, &elf_header, sizeof (elf_header));
  if (n == sizeof (elf_header))
    {
#ifdef DEBUG_LOADER
    myprintf ("Read ELF header\n");
#endif

    if (elf_header.magic[1] == 'E' && elf_header.magic[2] == 'L' 
            && elf_header.magic[3] == 'F' && elf_header.endian == 1)
      {
      // Right now, we only support X64.
#ifdef __x86_64__
      if (elf_header.e_machine == EM_X86_64)
        {
#ifdef DEBUG_LOADER
        myprintf ("ELF arch matches, entry point = %x\n", elf_header.e_entry);
#endif
        ret = 0;
        }
      else 
        {
#ifdef DEBUG_LOADER
        myprintf ("Wrong ELF arch\n");
#endif
        set_errno (ENOEXEC); 
        ret = -1;
        }
#else
      // Eventually, check other architectures here
#endif
      }
    else
      { 
#ifdef DEBUG_LOADER
      myprintf ("No ELF signature\n");
#endif
      set_errno (ENOEXEC); 
      ret = -1;
      }
    }
  else
    {
#ifdef DEBUG_LOADER
    myprintf ("Can't read ELF header\n");
#endif
    set_errno (ENOEXEC); 
    ret = -1;
    }

  myclose (f);

  return ret;
  }

/*============================================================================

  elfloader_get_start

============================================================================*/
void *elfloader_get_start (const ElfLoader *self)
  {
  return self->entry;
  }

/*============================================================================

  elfloader_load

============================================================================*/
void *elfloader_load (ElfLoader *self)
  {
  if (elfloader_check (self) != 0) return NULL; // errno should have been set

  void *ret = 0;
  struct stat sb;
  mystat ((char *)path_cstr (self->path), &sb); // This shouldn't fail by this point 
  int64_t len = sb.st_size;
#ifdef DEBUG_LOADER
  myprintf ("ELF size is %lld\n", len);
#endif

  ret = malloc (len);
  if (ret)
    {
    int f = myopen ((char *)path_cstr (self->path), O_RDONLY); 
    // We already checked we could open
    int64_t nread = myread (f, ret, len);
    (void)nread;
    ELFHeader *elf_header = (ELFHeader *)ret;
    int64_t entry = elf_header->e_entry;
    self->entry = ret + ELF_GEN_OFFSET + entry;
#ifdef DEBUG_LOADER
    myprintf ("nread=%lld entry=%llx real entry=%llx\n", nread, entry, self->entry);
#endif
    myclose (f);
    }
  else
    {
    set_errno (ENOMEM);
    }

  return ret;
  }


/*============================================================================

  elfloader_new

============================================================================*/
ElfLoader *elfloader_new (const UTF8 *path)
  {
  ElfLoader *self = malloc (sizeof (ElfLoader));
  self->path = path_new (path);
  path_canonicalize (self->path); 
  return self;
  }

/*============================================================================

  elfloader_run

============================================================================*/
int elfloader_run (const ElfLoader *self, Process *process, const List *args)
  {
#ifdef DEBUG_LOADER
  myprintf ("about to run\n");
#endif
  BearosStartFn start = (BearosStartFn)self->entry; 
#ifdef DEBUG_LOADER
  myprintf ("start is %p\n", start);
#endif
  BearosContext bc;
  bc.syscall_fn = mysyscall;
#ifdef DEBUG_LOADER
  myprintf ("syscall is %p\n", bc.syscall_fn);
#endif
  bc.process = process;

  int argc = list_length (args);
  char **argv = malloc ((argc + 1) * sizeof (char *));
  for (int i = 0; i < argc; i++)
    {
    argv[i] = strdup ((char *)string8_cstr (list_get (args, i)));
    }
  argv[argc] = NULL; // Just to be safe -- some programs do look for this NULL

#ifdef DEBUG_LOADER
  myprintf ("about to call start()\n");
#endif
  int ret = start (&bc, argc, argv, NULL); // TODO
#ifdef DEBUG_LOADER
  myprintf ("called start()\n");
#endif

  for (int i = 0; i < argc; i++)
    {
    free (argv[i]);
    }
  free (argv);

  return ret; 
  }


