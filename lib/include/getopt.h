/*============================================================================

  getopt.h 

  Simple BearOS implementation of getopt().

============================================================================*/

#pragma once

extern char *optarg;
extern int optind, opterr, optopt;

#ifdef __cplusplus
extern "C" {
#endif

int getopt (int argc, char *const argv[], const char *optstring);

#ifdef __cplusplus
}
#endif


