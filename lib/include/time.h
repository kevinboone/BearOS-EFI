/*============================================================================

  time.h

  Definition of gmtime() and struct tm, etc

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <asm/time.h>

#ifdef __cplusplus
extern "C" {
#endif

extern struct tm *gmtime     (const time_t *timep);
extern struct tm *gmtime64_r (const long *in_time, struct tm *p);
extern time_t time           (time_t *t);

#ifdef __cplusplus
}
#endif



