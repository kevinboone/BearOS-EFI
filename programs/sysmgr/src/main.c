/*============================================================================

  sysmgr utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <conio.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <libbearos/bearos_term.h>
#include <string.h>
#include <sys/reboot.h>

/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?V] reboot | shutdown \n", argv0);
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
    char *action = argv[optind];
    if (strcmp (action, "reboot") == 0)
      ret = reboot (BEAROS_REBOOT_RESTART);
    else if (strcmp (action, "shutdown") == 0)
      ret = reboot (BEAROS_REBOOT_SHUTDOWN);
    else
       {
       fprintf (stderr, "%s: %s: unknown action \n", argv[0], action);
       return EXIT_FAILURE;
       }

    if (ret != 0)
      fprintf (stderr, "%s: %s: %s\n", argv[0], action, strerror (errno));
    return ret;
    }
  else
    {
    show_usage_header (stderr, argv[0]);
    return EXIT_FAILURE;
    }
  }


