/*============================================================================

  BOSH shell utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <libbearos/bearos_defs.h>
#include <libbearos/bearos_term.h>
#include <libbearos/bearos_exec.h>
#include <libbearos/list.h>
#include <libbearos/path.h>
#include <libbearos/string8.h>
#include "cmds.h"
#include "exec.h"
#include "script.h"

// FWD
static void show_usage_header (FILE *f, const char *argv0);
static void show_version (const char *argv0);
static void show_help (FILE *f, const char *argv0);

/*============================================================================

  do_interactive 

============================================================================*/
static int do_interactive (const char *argv0)
  {
  int ret = 0;
  BOOL quit = FALSE;
  while (!quit)
    {
    char cwd[PATH_MAX];
    if (getcwd (cwd, PATH_MAX - 1) != 0)
      strcpy (cwd, "?");
 
    fputs (cwd, stdout);
    fputs (" > ", stdout);
    fflush (stdout);

    char *line = NULL;
    size_t n;

    int r = getline (&line, &n, stdin);
    if (r > 1)
      {
      line[r - 1] = 0; // TODO
      ret = script_parse_and_run_command (line, &quit);
      }
    else
      {
      // Empty line -- nothing to do
      }

    free (line);
    }
  return ret;
  }


/*============================================================================

  do_script

============================================================================*/
static int do_script (const char *argv0, const char *script)
  {
  return script_load_and_run (argv0, script);
  }


/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  int opt;
  while ((opt = getopt(argc, argv, "?V")) != -1) 
    {
    switch (opt) 
      {
      case '?':
        show_help (stdout, argv[0]);
        exit (EXIT_SUCCESS);
      case 'V':
        show_version (argv[0]);
        exit (EXIT_SUCCESS);
      default: /* '?' */
        show_usage_header (stderr, argv[0]);
        exit (EXIT_FAILURE);
        }
      }
  
  int ret = 0;

  if (argc - optind > 0)
    {
    for (int i = optind; i < argc && ret == 0; i++)
      ret = do_script (argv[0], argv[i]);
    }
  else
    {
    ret = do_interactive (argv[0]);
    }

  return ret;
  }

/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?V] [script]\n", argv0);
  }

/*============================================================================

  show_help

============================================================================*/
static void show_help (FILE *f, const char *argv0)
  {
  show_usage_header (f, argv0);
  fprintf (f, "    -?     show this summary\n");
  fprintf (f, "    -V     show version\n");
  }


/*============================================================================

  show_version

============================================================================*/
static void show_version (const char *argv0)
  {
  printf ("%s version %s\n", argv0, VERSION);
  }



