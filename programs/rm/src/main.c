/*============================================================================

  rm utility

  main.c

  // TODO -- recursive

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <libbearos/bearos_defs.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>


/*============================================================================

  do_rm 

============================================================================*/
static int do_rm (const char *argv0, const char *pathname, BOOL verbose)
  {
  if (verbose)
    printf ("Deleting %s\n", pathname);
  int ret = unlink (pathname);
  if (ret < 0)
    {
    fprintf (stderr, "%s: can't delete: %s\n", pathname, strerror (errno));
    return errno;
    }
  return 0;
  } 


/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?V] {files...} \n", argv0);
  }

/*============================================================================

  show_help

============================================================================*/
static void show_help (FILE *f, const char *argv0)
  {
  show_usage_header (f, argv0);
  fprintf (f, "    -?     show this summary\n");
  fprintf (f, "    -V     show version\n");
  fprintf (f, "    -v     verbose\n");
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
  BOOL verbose = FALSE;

  int opt;
  while ((opt = getopt(argc, argv, "?Vv")) != -1) 
    {
    switch (opt) 
      {
      case '?':
        show_help (stdout, argv[0]);
        exit (EXIT_SUCCESS);
      case 'V':
        show_version (argv[0]);
        exit (EXIT_SUCCESS);
      case 'v':
        verbose = TRUE;
        break;
      default: /* '?' */
        show_usage_header (stderr, argv[0]);
        exit (EXIT_FAILURE);
        }
      }

  int ret = EXIT_SUCCESS;
  if (argc - optind > 0)
    {
    for (int i = optind; i < argc; i++)
      ret = do_rm (argv[0], argv[i], verbose);
    }
  else
    {
    fprintf (stderr, "%s: no files specified\n", argv[0]);
    show_usage_header (stderr, argv[0]);
    return EXIT_FAILURE;
    }
  return ret;
  }


