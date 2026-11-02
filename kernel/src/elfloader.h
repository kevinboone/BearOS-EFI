/*============================================================================

  elfloader.h

  Loads an ELF executable into memory, preparatory to running it.

  BearOS executables are position-independent, monolithic code blobs.
  Loading such an executable amounts to little more than reserving
  memory for it, and reading the entire ELF file into memory. We do have
  to parse the ELF header, not only to ensure that we're dealing with
  a valid ELF file, but to find the program entry point.

  Working this way will get us the ELF header in memory, and probably
  some other unnecessary stuff, as well as the code. But that's a small
  price to pay for simplicity, and no need for memory management.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "list.h"
#include "process.h"

struct _ElfLoader;
typedef struct _ElfLoader ElfLoader;

/** Check that the ELF header of the given file is valid, and suitable
    for this architecture. Returns zero on success, and sets errno on
    failure. */
extern int elfloader_check (const ElfLoader *self);

extern void elfloader_destroy (ElfLoader *self);

extern void *elfloader_get_start (const ElfLoader *self);

/** Reads the ELF file into memory, and calculates the program entry 
    point from the header. */
extern void *elfloader_load (ElfLoader *self);

extern ElfLoader *elfloader_new (const UTF8 *path);

/** Starts execution of a program loaded by elfloader_load. Don't 
    call this until load() has returned 0. */
int elfloader_run (const ElfLoader *self, Process *process, const List *args);

