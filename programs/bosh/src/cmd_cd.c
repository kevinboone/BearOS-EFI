/*============================================================================

  BOSH shell utility

  cmd_cd.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libbearos/list.h>
#include "cmds.h"

/*============================================================================

  cmd_cd

============================================================================*/
static int cmd_cd_chdir (const char *cwd)
  {
  if (chdir (cwd) == 0)
    return 0;
  else 
    {
    fprintf (stderr, "%s: can't change directory: %s\n", 
      cwd, strerror (errno)); 
    return errno;
    }
  }

/*============================================================================

  cmd_cd

============================================================================*/
int cmd_cd (int argc, char **argv)
  {
  const char *dir;
  if (argc == 1)
    {
    dir = getenv ("HOME");
    if (!dir) dir = "/";
    }
  else
    dir = argv[1];
    
  return cmd_cd_chdir (dir);
  }

