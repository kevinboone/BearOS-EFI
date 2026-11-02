/*============================================================================

  umount utility

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
int do_umount (const char *argv0, const char *target)
  {
  if (umount (target) == 0)
    return 0;

  const char *msg = strerror (errno);
  fprintf (stderr, "%s: can't unmount %s: %s\n", argv0,
    target, msg);
  return errno;
  } 


/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?V] {mount_point} \n", argv0);
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

  if (argc - optind == 1)
    {
    ret = do_umount (argv[0], argv[optind]);
    }
  else
    {
    show_usage_header (stderr, argv[0]);
    return EXIT_FAILURE;
    }

  return ret;
  }

