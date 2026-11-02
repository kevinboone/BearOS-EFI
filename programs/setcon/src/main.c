/*============================================================================

  setcon utility

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
#include <string.h>
#include <libbearos/bearos_term.h>

/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s {-s mode} {-l} [-?V]\n", argv0);
  }

/*============================================================================

  show_help

============================================================================*/
static void show_help (FILE *f, const char *argv0)
  {
  show_usage_header (f, argv0);
  fprintf (f, "    -?     show this summary\n");
  fprintf (f, "    -l     list modes\n");
  fprintf (f, "    -s N   set mode N\n");
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

  list_modes 

============================================================================*/
static void list_modes (const char *argv0)
  {
  if (isatty (STDIN_FILENO))
    {
    int num_modes = bearos_term_get_modes (STDOUT_FILENO);
    if (num_modes > 0)
      {
      int mode = bearos_term_get_current_mode (STDOUT_FILENO);
      for (int i = 0; i < num_modes; i++)
	{
	int r; int c;
	int ret = bearos_term_get_mode_size (STDOUT_FILENO, i, &r, &c);
	if (ret == 0)
	  printf ("%3d %3d x %3d %s\n", i, r, c, i == mode ? "(current)" : "");
	}
      exit (EXIT_SUCCESS);
      }
    else
      {
      fprintf (stderr, "%s: mode information is not available\n", argv0);
      exit (EXIT_FAILURE);
      }
    }
  else
    {
    fprintf (stderr, "%s: standard out is not a TTY\n", argv0);
    exit (EXIT_FAILURE);
    }
  }


/*============================================================================

  set_mode 

============================================================================*/
static void set_mode (const char *argv0, int mode)
  {
  if (isatty (STDIN_FILENO))
    {
    if (bearos_term_set_mode (STDOUT_FILENO, mode) == 0)
      {
      exit (EXIT_SUCCESS);
      }
    else
      {
      fprintf (stderr, "%s: can't set mode: %s\n", argv0, 
         strerror (errno));
      exit (EXIT_FAILURE);
      }
    }
  else
    {
    fprintf (stderr, "%s: standard out is not a TTY\n", argv0);
    exit (EXIT_FAILURE);
    }
  }


/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  int opt;
  while ((opt = getopt(argc, argv, "?lVs:")) != -1) 
    {
    switch (opt) 
      {
      case '?':
        show_help (stdout, argv[0]);
        exit (EXIT_SUCCESS);
      case 'l':
        list_modes (argv[0]); 
        break;
      case 's':
        set_mode (argv[0], atoi (optarg)); 
        break;
      case 'V':
        show_version (argv[0]);
        exit (EXIT_SUCCESS);
      default: /* '?' */
        show_usage_header (stderr, argv[0]);
        exit (EXIT_FAILURE);
        }
      }

  show_usage_header (stderr, argv[0]);
  return EXIT_FAILURE;
  }


