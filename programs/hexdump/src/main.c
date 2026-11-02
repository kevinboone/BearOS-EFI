/*============================================================================

  hexdump utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <errno.h>
#include <fcntl.h>
#include <libbearos/bearos_defs.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>


/*============================================================================

  do_hexdump

============================================================================*/
static int do_hexdump (const char *argv0, const char *pathname, 
        off_t offset, size_t count)
  {
  int ret = 0;
  int line_len = 16; // bytes
  int f = open (pathname, O_RDONLY);
  if (f >= 0)
    {
    size_t total_read = 0;
    BYTE buff[line_len];
    BOOL done = FALSE;
    if (lseek (f, offset, SEEK_SET) >= 0)
      {
      while (!done)
	{
	int n_read = read (f, buff, line_len);
	if (n_read == 0) 
          done = TRUE;
        else if (n_read < 0)
          {
          fprintf (stderr, "%s: error reading: %s\n", 
            pathname, strerror (errno));
          done = TRUE;
          ret = errno;
          }
	if (!done)
	  {
	  int toprint = n_read;
	  printf ("%08X ", offset + total_read);
	  for (int i = 0; i < toprint; i++)
	    {
	    printf ("%02X ", buff[i]);
	    }
	  for (int i = toprint; i < line_len; i++)
	    {
	    printf ("   ");
	    }
	  printf (" ");
	  for (int i = 0; i < toprint; i++)
	    {
	    char c = buff[i];
	    if (c > 32 && c < 126)
	      printf ("%c", c);
	    else
	      printf (".");
	    }
	  printf ("\n");
	  total_read += n_read;
	  if (total_read >= count) done = TRUE;
	  }
        }
      }
    else
      {
      fprintf (stderr, "%s: can't seek to position %ld: %s\n", 
	pathname, offset, strerror (errno));
      ret = errno;
      }
    close (f);
    }
  else
    {
    fprintf (stderr, "%s: can't open for reading: %s\n", 
      pathname, strerror (errno));
    ret = errno;
    }
  return ret;
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
  size_t count = 256;
  off_t offset = 0; 
  int opt;

  while ((opt = getopt(argc, argv, "?Vvn:s:")) != -1) 
    {
    switch (opt) 
      {
      case '?':
        show_help (stdout, argv[0]);
        exit (EXIT_SUCCESS);
      case 'n':
        count = atoul (optarg);
        break;
      case 's':
        offset = atoul (optarg);
        break;
      case 'V':
        show_version (argv[0]);
        exit (EXIT_SUCCESS);
      default: /* '?' */
        show_usage_header (stderr, argv[0]);
        exit (EXIT_FAILURE);
        }
      }

  int ret = EXIT_SUCCESS;
  if (argc - optind > 0)
    {
    for (int i = optind; i < argc; i++)
      ret = do_hexdump (argv[0], argv[i], offset, count);
    }
  else
    {
    fprintf (stderr, "%s: no files specified\n", argv[0]);
    show_usage_header (stderr, argv[0]);
    return EXIT_FAILURE;
    }
  return ret;
  }


