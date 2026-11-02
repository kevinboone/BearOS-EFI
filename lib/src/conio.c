/*============================================================================

  conio.c

  Implementation of conio functions 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/
#include <sys/syscall.h> 
#include "bearos/apidefs.h"

void console_puts (char *s)
  {
  syscall (BEAROS_SYSCALL_CONOUT, (intptr_t)s, 0, 0, 0, 0, 0);
  }

