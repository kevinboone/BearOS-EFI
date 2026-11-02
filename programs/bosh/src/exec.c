/*============================================================================

  BOSH shell utility

  exec.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <libbearos/bearos_defs.h>
#include <libbearos/bearos_term.h>
#include <libbearos/bearos_exec.h>
#include <libbearos/list.h>
#include <libbearos/path.h>
#include <libbearos/string8.h>

/*============================================================================

  exec_run_command 

============================================================================*/
int exec_run_command (const char *cmd, char **argv) 
  {
  int ret = runvp (cmd, argv);
  if (errno != 0)
    {
    fprintf (stderr, "%s: %s\n", cmd, strerror (errno));
    ret = errno;
    }
  return ret;
  }



