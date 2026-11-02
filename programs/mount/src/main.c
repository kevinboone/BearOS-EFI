/*============================================================================

  mount utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include "../../lib/include/bearos/defaults.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/mount.h>

/*============================================================================

  do_mount 

============================================================================*/
int do_mount (const char *argv0, const char *source, const char *target,
      const char *type, unsigned long flags)
  {
  if (mount (source, target, type, flags, NULL) == 0)
    return 0;

  const char *msg = strerror (errno);
  fprintf (stderr, "%s: can't mount %s on %s: %s\n", argv0,
    source, target, msg);
  return errno;
  } 


/*============================================================================

  do_show_mounts

  We'll just dump the contents of /proc/mounts for now

============================================================================*/
void do_show_mounts (const char *argv0)
  {
  FILE *f = fopen (BEAROS_PROC_MOUNTS, "r");
  if (f)
    {
    char *line = NULL;
    size_t n = 0;
    while (getline (&line, &n, f) > 0)
      {
      printf ("%s", line);
      free (line);
      line = NULL;
      }
    if (line) free (line);
    fclose (f);
    }
  else
    {
    fprintf (stderr, "%s: can't open " BEAROS_PROC_MOUNTS ": %s\n",
      argv0, strerror (errno));
    }
  }


/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?V] [-o options] {source} {mount_point} \n", argv0);
  }

/*============================================================================

  show_help

============================================================================*/
static void show_help (FILE *f, const char *argv0)
  {
  show_usage_header (f, argv0);
  fprintf (f, "    -?     show this summary\n");
  fprintf (f, "    -V     show version\n");
//TODO
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

  int ret;
  unsigned long flags = 0; // TODO

  if (argc - optind == 2)
    {
    char *type = "auto"; // TODO
    ret = do_mount (argv[0], argv[optind], argv[optind + 1], type, flags);
    }
  else if (argc - optind == 1)
    {
    fprintf (stderr, 
        "Sorry: mounting using a single argument is not yet supported\n");
    return EXIT_FAILURE;
    }
  else
    {
    do_show_mounts (argv[0]);
    return EXIT_SUCCESS;
    }

  return ret;
  }

