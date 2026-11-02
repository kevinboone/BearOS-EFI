/*============================================================================

  rmdir utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/*============================================================================

  do_rmdir 

============================================================================*/
int do_rmdir (const char *argv0, const char *pathname)
  {
  if (rmdir (pathname) == 0)
    return 0;
  fprintf (stderr, "%s: can't delete: %s\n", pathname, strerror (errno));
  return errno;
  }

/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?V] {directories...} \n", argv0);
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

  if (argc - optind > 0)
    {
    int ret = 0;
    for (int i = optind; i < argc; i++)
      ret = do_rmdir (argv[0], argv[1]);
    return ret;
    }
  else
    {
    fprintf (stderr, "%s: no directories specified\n", argv[0]);
    show_usage_header (stderr, argv[0]);
    return EXIT_FAILURE;
    }
  }


