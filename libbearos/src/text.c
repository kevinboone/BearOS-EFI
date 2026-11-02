/*============================================================================

  textutil.c

  General text formatting functions. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <stdio.h>

/*============================================================================

  text_humanize_size

============================================================================*/
void text_humanize_size (long int n, char *buffer, int max)
  {
  char *size_suffix[] = {"", "K", "M", "G", "T"};

  char suffixes = sizeof(size_suffix) / sizeof(size_suffix[0]);
  int i = 0;
  double float_n = n;
  if (n > 1024) 
    {
    for (i = 0; (n / 1024) > 0 && i < suffixes - 1; i++, n /= 1024)
    float_n = n / 1024.0;
    }

  snprintf (buffer, max, "%6.1lf%s", float_n, size_suffix[i]);
  }


