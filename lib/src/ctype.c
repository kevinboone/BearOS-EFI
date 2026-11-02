/*============================================================================

  ctype.c 

  BearOS alternative to the conventional stdio functions.

============================================================================*/

#include <ctype.h>

int isspace (int c)
  {
  if (c == ' ') return 1;
  if (c == '\f') return 1;
  if (c == '\n') return 1;
  if (c == '\r') return 1;
  if (c == '\t') return 1;
  if (c == '\v') return 1;
  return 0;
  }

int isdigit (int c)
  {
  return (c >= '0' && c <= '9');
  }

int isxdigit (int c)
  {
  if (isdigit (c)) return 1;
  if (c >= 'a' && c <= 'f') return 1;
  if (c >= 'A' && c <= 'F') return 1;
  return 0;
  }
