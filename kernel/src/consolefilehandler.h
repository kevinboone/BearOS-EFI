/*============================================================================

  consolefilehandler.h

  A provider of `struct fileops` for the console. On EFI, we'll use the
  EFI API to write directly to the console. On Linux, we'll just forward
  to the `write()` and `read()` implementations in the standard library.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "fileops.h"

struct _ConsoleFileHandler;
typedef struct _ConsoleFileHandler ConsoleFileHandler;

extern void               consolefilehandler_close_and_cleanup 
                              (ConsoleFileHandler *self);
extern void                consolefilehandler_destroy (ConsoleFileHandler *self);
extern struct FileOps     *consolefilehandler_get_fileops (ConsoleFileHandler *self);
extern ConsoleFileHandler *consolefilehandler_new (void);

