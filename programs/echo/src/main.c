/*============================================================================

  echo utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <conio.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libbearos/bearos_term.h>

/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  for (int i = 1; i < argc; i++)
    {
    fputs (argv[i], stdout);
    if (i != argc - 1)
      fputs (" ", stdout);
    }

  fputs ("\n", stdout);
  return 0;
  }

