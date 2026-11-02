/*============================================================================

  getopt.c 

  BearOS alternative to the conventional ioctl function

============================================================================*/

#include <getopt.h>
#include <stdio.h>
#include <string.h>

char *optarg;
int optind = 1, opterr, optopt, optpos, optreset = 0;

//#define optpos __optpos

/*============================================================================

  __getopt_msg

============================================================================*/
static void __getopt_msg (char *a, char *b, char *c, int l)
  {
  FILE *f = stderr;
  fputs (a, f);
  fwrite (b, strlen(b), 1, f);
  fwrite (c, 1, l, f);
  fputc ('\n', f);
  }


/*============================================================================

  __getopt_is_switch

============================================================================*/
static int __getopt_is_switch (char c)
  {
  return (c == '-');
  }

/*============================================================================

  getopt 

============================================================================*/
int getopt (int argc, char *const argv[], const char *optstring)
  {
  char *optchar;

  if (!optind || optreset)
    {
    optreset = 0;
    optpos = 0;
    optind = 1;
    }

 if (optind >= argc || !argv[optind])
    return -1;

  if (!__getopt_is_switch (argv[optind][0]))
  /*if (argv[optind][0] != '-' && argv[optind][0] != '/') */
    {
    if (__getopt_is_switch (optstring[0]))
      {
      optarg = argv[optind++];
      return 1;
      }
    return -1;
    }

  if (!argv[optind][1])
    return -1;

  if (__getopt_is_switch (argv[optind][1]) && !argv[optind][2])
    return optind++, -1;

  int k;
  int c;

  if (!optpos) optpos++;
  c = argv[optind][optpos], k = 1;
  optchar = argv[optind] + optpos;
  optopt = c;
  optpos += k;

  if (!argv[optind][optpos])
    {
    optind++;
    optpos = 0;
    }

  if (__getopt_is_switch (optstring[0]))
    optstring++;

  int i = 0; 
  int d, l;

  do 
    {
    d = optstring[i], l = 1;
    if (l > 0) i+=l; else i++;
    } while (l && d != c && d != 0);

  if (optstring[i] == ':') 
    {
    if (optstring[i+1] == ':') optarg = 0;
    else if (optind >= argc) 
      {
      if (optstring[0] == ':') return ':';
      if (opterr) __getopt_msg (argv[0],
                                ": option requires an argument: ",
        optchar, k);
      return '?';
      }

    if (optstring[i+1] != ':' || optpos)
      {
      optarg = argv[optind++] + optpos;
      optpos = 0;
      }
    }

  return c;
  }



