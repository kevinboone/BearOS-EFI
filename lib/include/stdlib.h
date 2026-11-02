/*============================================================================

  sdlib.h 

  BearOS alternative to the conventional stdlib.h

============================================================================*/

#pragma once

#include <stddef.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifdef __cplusplus
extern "C" {
#endif

// Note atoi and atol are equivalent -- both are 32-bit values
int           atoi    (char *s);
long          atol    (char *s);
unsigned      atou    (char *s);
unsigned long atoul   (char *s);
void          exit    (int status); 
void          free    (void *s);
char         *getenv  (const char *name);
void         *malloc  (size_t length); 
void         *realloc (void *s, size_t length); 
/** realpath() behaves as in Linux, and expects POSIX-style paths. This
    function can fail. If it does, it returns NULL, and sets errno. 
    The `out` argument is a pointer to a buffer whose size is at
    least PATH_MAX bytes. On success, the function returns the
    value of `out`. */
extern char *realpath (const char *path, char *out);


#ifdef __cplusplus
}
#endif


