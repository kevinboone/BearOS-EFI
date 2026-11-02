/*============================================================================

  df utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <bearos/defaults.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/vfs.h>
#include <libbearos/string8.h>
#include <libbearos/text.h>

//#define PROCMOUNTS "/proc/mounts"

void do_root (const char *root, const char *source, BOOL human);

/*============================================================================

  df_all 

============================================================================*/
int df_all (const char *argv0, BOOL human)
  {
  FILE *f = fopen (BEAROS_PROC_MOUNTS, "r");
  if (f)
    {
    char *line = NULL;
    size_t n = 0;
    while (getline (&line, &n, f) > 0)
      {
      String8 *line2 = string8_new ((UTF8 *)line);

      List *toks = string8_split (line2, " ");
      int l = list_length (toks);
      if (l > 2)
        {
        const char *root = list_get (toks, 1);
        const char *source = list_get (toks, 0);
        do_root (root, source, human);
        }
      else
        {
        // Silently ignore ill-formed entries in /proc/mounts (but
        //   there shouldn't be any).
        }

      list_destroy (toks);

      string8_destroy (line2);
      free (line);
      line = NULL;
      }
    if (line) free (line);
    fclose (f);
    return 0;
    }
  else
    {
    fprintf (stderr, "%s: can't open " BEAROS_PROC_MOUNTS ": %s\n",
      argv0, strerror (errno));
    return errno;
    }
  
  }


/*============================================================================

  do_root 

============================================================================*/
void do_root (const char *root, const char *source, BOOL human)
  {
  struct statfs sb;
  if (statfs (root, &sb) == 0)
    { 
    if (human)
      {
      ssize_t total = sb.f_blocks * sb.f_bsize;
      ssize_t free = sb.f_bfree * sb.f_bsize;
      ssize_t used = total - free;
      char total_s[12];
      char free_s[12];
      char used_s[12];
      text_humanize_size (total, total_s, sizeof (total_s) - 1);
      text_humanize_size (free, free_s, sizeof (free_s) - 1);
      text_humanize_size (used, used_s, sizeof (used_s) - 1);
      int percent = 0;
      if (total != 0) percent = 100 * used / total;
      printf ("%-20s%+8s%+8s%+8s %3d%%  %s\n", source, total_s, 
        used_s, free_s, percent, root);
      }
    else
      {
      // Sizes in blocks
      ssize_t total = sb.f_blocks;
      ssize_t free = sb.f_bfree;
      ssize_t used = total - free;
      int percent = 0;
      if (total != 0) percent = 100 * used / total;
      printf ("%-20s%8d%8d%8d %3d%%  %s\n", source, total, used, free, percent, root);
      }
    }
  else
    {
    fprintf (stderr, "%s: can't get information: %s\n", 
      root, strerror (errno));
    }
  }


/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?vh]\n", argv0);
  }

/*============================================================================

  show_help

============================================================================*/
static void show_help (FILE *f, const char *argv0)
  {
  show_usage_header (f, argv0);
  fprintf (f, "   -?  show this summary\n");
  fprintf (f, "   -h  human-readable sizes\n");
  fprintf (f, "   -V  show version\n");
  }


/*============================================================================

  show_version

============================================================================*/
static void show_version (const char *argv0)
  {
  printf ("%s version %s\n", argv0, VERSION);
  }


/*============================================================================

  show_header

============================================================================*/
static void show_header (BOOL human)
  {
  printf ("%-20s%+8s%+8s%+8s %+3s %s\n", 
      "Filesystem", "Total", "Used", "Free", "Use%", "Mounted on");
  }


/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  BOOL human = FALSE;
  int opt;

  while ((opt = getopt(argc, argv, "?hV")) != -1) 
    {
    switch (opt) 
      {
      case '?':
        show_help (stdout, argv[0]);
        exit (EXIT_SUCCESS);
      case 'h':
        human = TRUE;
        break;
      case 'V':
        show_version (argv[0]);
        exit (EXIT_SUCCESS);
      default: /* '?' */
        show_usage_header (stderr, argv[0]);
        exit (EXIT_FAILURE);
        }
      }

  int ret = 0;

  show_header (human);
  ret = df_all (argv[0], human);

  return ret;
  }


