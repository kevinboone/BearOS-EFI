/*============================================================================

  time.h

  Implementation of gmtime() and some other fundamental time-handling
  functions.

  Some of this code was derived from the Michael G Schwern' y2038 project,
  and is copyright (c)2007-2010, released under the MIT licence.

  https://github.com/evalEmpire/y2038/

  Otherwise, copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <time.h>

static struct tm tm;

static const char days_in_month[2][12] = 
  {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  };

static const short julian_days_by_month[2][12] = 
  {
  {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
  {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
  };

static const short length_of_year[2] = {365, 366};
static const long years_in_gregorian_cycle = 400;
#define days_in_gregorian_cycle ((365 * 400) + 100 - 4 + 1)

/* Number of days since epoch on Jan 1st, 2008 GMT */
#define CHEAT_DAYS (1199145600 / 24 / 60 / 60)
#define CHEAT_YEARS 108

#define IS_LEAP(n) ((!(((n) + 1900) % 400) || \
    (!(((n) + 1900) % 4) && (((n) + 1900) % 100))) ? 1 : 0)

#define WRAP_TIME64(a, b, m) ((a) = ((a) < 0) ? ((b)--, (a) + (m)) : (a))

struct tm * gmtime64_r (const long *in_time, struct tm *p)
  {
  int v_tm_sec, v_tm_min, v_tm_hour, v_tm_mon, v_tm_wday;
  long v_tm_tday;
  int leap;
  long m;
  long time;
  long year = 70;
  long cycles = 0;

  if (!in_time || !p)
    return 0;

  time = *in_time;

  v_tm_sec = (int)(time % 60);
  time /= 60;
  v_tm_min = (int)(time % 60);
  time /= 60;
  v_tm_hour = (int)(time % 24);
  time /= 24;
  v_tm_tday = time;

  WRAP_TIME64 (v_tm_sec, v_tm_min, 60);
  WRAP_TIME64 (v_tm_min, v_tm_hour, 60);
  WRAP_TIME64 (v_tm_hour, v_tm_tday, 24);

  v_tm_wday = (int)((v_tm_tday + 4) % 7);
  if (v_tm_wday < 0)
    v_tm_wday += 7;
  m = v_tm_tday;

  if (m >= CHEAT_DAYS)
  {
    year = CHEAT_YEARS;
    m -= CHEAT_DAYS;
  }

  if (m >= 0)
  {
    /* Gregorian cycles, this is huge optimization for distant times */
    cycles = m / (long)days_in_gregorian_cycle;
    if (cycles)
    {
      m -= cycles * (long)days_in_gregorian_cycle;
      year += cycles * years_in_gregorian_cycle;
    }

    /* Years */
    leap = IS_LEAP (year);
    while (m >= (long)length_of_year[leap])
    {
      m -= (long)length_of_year[leap];
      year++;
      leap = IS_LEAP (year);
    }

    /* Months */
    v_tm_mon = 0;
    while (m >= (long)days_in_month[leap][v_tm_mon])
    {
      m -= (long)days_in_month[leap][v_tm_mon];
      v_tm_mon++;
    }
  }
  else
  {
    year--;

    /* Gregorian cycles */
    cycles = (m / (long)days_in_gregorian_cycle) + 1;
    if (cycles)
    {
      m -= cycles * (long)days_in_gregorian_cycle;
      year += cycles * years_in_gregorian_cycle;
    }

    /* Years */
    leap = IS_LEAP (year);
    while (m < (long)-length_of_year[leap])
    {
      m += (long)length_of_year[leap];
      year--;
      leap = IS_LEAP (year);
    }

    /* Months */
    v_tm_mon = 11;
    while (m < (long)-days_in_month[leap][v_tm_mon])
    {
      m += (long)days_in_month[leap][v_tm_mon];
      v_tm_mon--;
    }
    m += (long)days_in_month[leap][v_tm_mon];
  }

  p->tm_year = (int)year;

  if (p->tm_year != year)
  {
    return 0;
  }

  /* At this point m is less than a year so casting to an int is safe */
  p->tm_mday = (int)m + 1;
  p->tm_yday = julian_days_by_month[leap][v_tm_mon] + (int)m;
  p->tm_sec = v_tm_sec;
  p->tm_min = v_tm_min;
  p->tm_hour = v_tm_hour;
  p->tm_mon = v_tm_mon;
  p->tm_wday = v_tm_wday;

  return p;
  }

/*============================================================================

  gmtime

============================================================================*/
struct tm *gmtime (const time_t *timep)
  {
  gmtime64_r (timep, &tm);
  return &tm;
  }

/*============================================================================

  time

============================================================================*/
time_t time (time_t *t)
  {
  // TODO
  return 0;
  }



