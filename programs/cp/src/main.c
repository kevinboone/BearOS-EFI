/*============================================================================

  cp utility

  main.c

  cp file1 file2 directory/   -- OK
  cp file1 file2 file3        -- no: last arg must be directory with multiple
                                 sources
  cp file1 file2              -- OK single source 
  cp file1                    -- no: no target

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <libbearos/bearos_defs.h>
#include <libbearos/path.h>
#include <libgen.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>


// FWD
static int copy_file_to_file (const UTF8 *source, 
        const UTF8 *dest_file, BOOL verbose, BOOL preserve);
static int do_copy_file (const UTF8 *source, const UTF8 *dest, BOOL preserve);

/*============================================================================

  copy_file_to_dir 

============================================================================*/
static int copy_file_to_dir (const UTF8 *source, 
    const UTF8 *dest_dir, BOOL verbose, BOOL preserve)
  {
  UTF8 *source_copy = (UTF8 *)strdup ((char *)source);

  UTF8 *source_base = (UTF8 *)basename ((char *)source_copy);

  Path *dest_path = path_new ((UTF8 *)dest_dir);
  path_append (dest_path, (UTF8 *)source_base);
  
  return copy_file_to_file (source, path_cstr (dest_path), verbose, preserve);
  
  path_destroy (dest_path);
  free (source_copy);

  return 0;
  }


/*============================================================================

  copy_file_to_file

============================================================================*/
static int copy_file_to_file (const UTF8 *source, 
        const UTF8 *dest_file, BOOL verbose, BOOL preserve)
  {
  int ret = 0;

  if (verbose)
    printf ("Copying '%s' to '%s'\n", source, dest_file);
  
  Path *source_p = path_new (source);

  if (path_is_directory (source_p))
    {
    fprintf (stderr, "Sorry, copying directories is not yet supported\n");
    ret = -1;
    }
  else
    {
    ret = do_copy_file (source, dest_file, preserve);
    }

  path_destroy (source_p);

  return ret;
  }


/*============================================================================

  do_copy_attributes

============================================================================*/
void do_copy_attributes (const UTF8 *source, const UTF8 *dest)
  {
  struct stat sb;
  if (stat ((char *)source, &sb) == 0)
    {
    struct utimbuf ub;
    ub.modtime = sb.st_mtime;
    if (utime ((char *)dest, &ub) != 0)
      {
      fprintf (stderr, "Can't set modification time on %s: %s\n",
         dest, strerror (errno));
      }
    }
  else
    {
    // What to do here? It would be off for stat() to fail after we've read
    //   the file for copying
    }
  }

/*============================================================================

  do_copy_file 

============================================================================*/
static int do_copy_file (const UTF8 *source, const UTF8 *dest, BOOL preserve)
  {
  int ret = 0;
  char buffer[4096];

  int fin = open ((char *)source, O_RDONLY);
  if (fin >= 0)
    {
    int fout = open ((char *)dest, O_WRONLY | O_TRUNC | O_CREAT);
    if (fout >= 0)
      {
      BOOL done = FALSE;
      while (!done)
        {
        int n_read = read (fin, buffer, sizeof (buffer));
        if (n_read < 0)
          {
          ret = -1; done = TRUE;
          fprintf (stderr, "%s: read failed: %s\n", strerror (errno)); 
          }
        else if (n_read > 0)
          {
          int n_written = write (fout, buffer, n_read);
          if (n_written != n_read)
            {
            fprintf (stderr, "%s: wrote %d bytes, expected %d",
              dest, n_written, n_read);
            ret = -1; done = TRUE;
            }
          }
        else done = TRUE;
        }

      close (fout);
      }
    else
      {
      fprintf (stderr, "%s: can't write: %s\n", dest, strerror (errno));
      ret = -1; 
      }
    close (fin);
    if (preserve)
      do_copy_attributes (source, dest);
    }
  else
    {
    fprintf (stderr, "%s: can't read: %s\n", source, strerror (errno));
    ret = -1;
    }
  
  return ret;
  }


/*============================================================================

  show_usage_header 

============================================================================*/
static void show_usage_header (FILE *f, const char *argv0)
  {
  fprintf (f, "Usage: %s [-?Vv] {directories...} \n", argv0);
  }

/*============================================================================

  show_help

============================================================================*/
static void show_help (FILE *f, const char *argv0)
  {
  show_usage_header (f, argv0);
  fprintf (f, "    -?     show this summary\n");
  fprintf (f, "    -p     preserve timestamps\n");
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
  BOOL preserve = FALSE;

  int opt;
  while ((opt = getopt(argc, argv, "?pVv")) != -1) 
    {
    switch (opt) 
      {
      case '?':
        show_help (stdout, argv[0]);
        exit (EXIT_SUCCESS);
      case 'V':
        show_version (argv[0]);
        exit (EXIT_SUCCESS);
      case 'p':
        preserve = TRUE;
        break; 
      case 'v':
        verbose = TRUE;
        break; 
      default: /* '?' */
        show_usage_header (stderr, argv[0]);
        exit (EXIT_FAILURE);
        }
      }

  if (argc - optind >= 2)
    {
    int ret = 0;

    int n_sources = argc - optind - 1;

    BOOL last_is_dir = FALSE;

    const UTF8 *last_path = (UTF8 *)argv[argc - 1];

    Path *p = path_new ((UTF8 *)last_path);
    if (path_is_directory (p))
      last_is_dir = TRUE;

    BOOL ok = FALSE;
    if (n_sources == 1)
      ok = TRUE;
    else
      {
      if (last_is_dir)
        ok = TRUE;
      else
        {
        fprintf (stderr, 
          "%s: With multiple sources, last argument must be an existing directory\n",
          argv[0]);
        ok = FALSE;
        }  
      }

    if (ok)
      {
      if (n_sources == 1 && last_is_dir)
        {
        ret = copy_file_to_dir ((UTF8 *)argv[optind], last_path, verbose, preserve);
        }
      else if (n_sources == 1)
        {
        ret = copy_file_to_file ((UTF8 *)argv[optind], last_path, verbose, preserve);
        } 
      else
        {
        for (int i = optind; i < argc - 1; i++)
           ret = copy_file_to_dir ((UTF8 *)argv[i], last_path, verbose, preserve);
        }
      }
    else
      ret = EXIT_FAILURE;

    path_destroy (p);

    return ret;
    }
  else
    {
    show_usage_header (stderr, argv[0]);
    return EXIT_FAILURE;
    }
  }


