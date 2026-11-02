/*============================================================================

  cat utility

  main.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <conio.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libbearos/bearos_term.h>

/*============================================================================

  cat_stream 

============================================================================*/
int cat_stream (FILE *f)
  {
  int bufsiz;
  if (isatty (fileno (f)))
    bufsiz = 1; 
  else
    bufsiz = 512; // Or some number >> 1 

  char buff[bufsiz];

  int n_read = fread (buff, 1, bufsiz, f);
  while (n_read > 0 && !ferror (f) && !ferror (stdout))
    {
    fwrite (buff, n_read, 1, stdout);
    n_read = fread (buff, 1, bufsiz, f);
    };

  if (ferror (stdout) || ferror (f)) 
    return -1;
  else
    return 0;
  }


/*============================================================================

  cat_file

============================================================================*/
int cat_file (const char *pathname)
  {
  int ret = 0;
  FILE *f = fopen (pathname, "r");
  if (f)
    {
    ret = cat_stream (f);
    fclose (f);
    }
  else
    {
    fprintf (stderr, "%s: can't open for reading: %s\n", pathname,
      strerror (errno));
    ret = errno;
    } 
  return ret;
  }


/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  int ret = 0;
  if (argc == 1)
    cat_stream (stdin);
  else
    {
    for (int i = 1; i < argc; i++)
      {
      cat_file (argv[1]);
      }
    }

  return ret;
  }

