/*============================================================================

  shell.h

  Main REPL loop.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/
#pragma once

#include "process.h"
#include "../../lib/include/bearos/apidefs.h"

extern int shell_run (const BearosContext *bc, int argc, char **argv, 
              char **envp);

