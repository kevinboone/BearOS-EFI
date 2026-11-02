/*============================================================================

  compat_time.h

  Implementation of gmtime() and some other fundamental time-handling
  functions.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/asm/time.h"

extern struct tm *gmtime (const time_t *timep);
extern time_t mytime (time_t *t);
