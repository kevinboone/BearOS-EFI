/*============================================================================

  datetimeutil.h

  General utilities for converting date/time values

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once
#include "defs.h"
#include "compat_time.h"

extern BOOL datetimeutil_is_leap_year (int year);

/** Get the difference in seconds between two timestamps (presumed
    to be in the same timezone. */
extern long datetimeutil_ydhms_diff (long year1, long yday1, int hour1, int min1, 
            int sec1, int year0, int yday0, int hour0, int min0, int sec0);

/** Get the day of the year from m/d/y. */
extern int datetimeutil_get_yday (int mon, int day, int year);

/** Convert an expanded date/time into a Unix epoch time. */
extern time_t datetimeutil_ymdhms_to_epoch (unsigned year, unsigned month, 
         unsigned day, unsigned hour, unsigned min, unsigned sec, 
         int utc_off_min);

