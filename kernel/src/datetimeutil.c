/*============================================================================

  datetimeutil.c 

  Some general functions for handling dates and times.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "../../lib/include/stdint.h"
#include "../../lib/include/errno_numbers.h"
#include "datetimeutil.h"

#define TM_YEAR_BASE 1900

static int month_offsets[] = {0,31,59,90,120,151,181,212,243,273,304,334};
   

/*============================================================================

  shr 

============================================================================*/
static long shr (long a, int b)
  {
  long one = 1;
  return (-one >> 1 == -1
	  ? a >> b
	  : (a + (a < 0)) / (one << b) - (a < 0));
  }


/*============================================================================

  timeutil_get_yday

============================================================================*/
int datetimeutil_get_yday (int mon, int day, int year)
  {
  static const int days[2][13] = 
     {
     {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
     {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
     };

  int leap = datetimeutil_is_leap_year (year);
  return days[leap][mon] + day;
  }

/*============================================================================

  datetimeutil_is_leap_year 

============================================================================*/
BOOL datetimeutil_is_leap_year (int year)
  {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
  }


/*============================================================================

  datetimeutil_ydhms_diff 

============================================================================*/
long datetimeutil_ydhms_diff (long year1, long yday1, int hour1, int min1, 
       int sec1, int year0, int yday0, int hour0, int min0, int sec0)
  {
  int a4 = shr (year1, 2) + shr (TM_YEAR_BASE, 2) - ! (year1 & 3);
  int b4 = shr (year0, 2) + shr (TM_YEAR_BASE, 2) - ! (year0 & 3);
  int a100 = (a4 + (a4 < 0)) / 25 - (a4 < 0);
  int b100 = (b4 + (b4 < 0)) / 25 - (b4 < 0);
  int a400 = shr (a100, 2);
  int b400 = shr (b100, 2);
  int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);
  long years = year1 - year0;
  long days = 365 * years + yday1 - yday0 + intervening_leap_days;
  long hours = 24 * days + hour1 - hour0;
  long minutes = 60 * hours + min1 - min0;
  long seconds = 60 * minutes + sec1 - sec0;
  return seconds;
  }


/*============================================================================

  datetimeutil_ydhms_diff 

============================================================================*/
time_t datetimeutil_ymdhms_to_epoch (unsigned year, unsigned month, 
         unsigned day, unsigned hour, unsigned min, unsigned sec, 
         int utc_off_min)
  {
  if (month > 12 || month < 1) return (time_t) -1; // Out of bounds will crash

  // TODO -- we should check the other arguments

  int leap_year_count = year / 4;

  int epoch_day_number = month_offsets [month - 1];
  if ((year % 4 == 0) && (year % 100 != 0))
    {
    if (epoch_day_number < 32)
      {
      epoch_day_number--;
      }
    }

  epoch_day_number = (year - 1970) * 365 + leap_year_count
	  + epoch_day_number + day - 493;

  unsigned int time_of_day_secs = (unsigned int)(hour * 3600) +
      (unsigned int)(min * 60) + (unsigned int)sec;

  time_t ret = (time_t)epoch_day_number * (time_t)86400
     + (time_t)time_of_day_secs - (utc_off_min * 60);

  return ret;
  }


