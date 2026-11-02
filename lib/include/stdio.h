/*============================================================================

  stdio.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <stddef.h>

struct FILE;
typedef struct _FILE FILE;

#ifndef LINUX
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
#endif

#ifndef EOF
#define EOF -1
#endif

#ifdef __cplusplus
extern "C" {
#endif

void    clearerr (FILE *stream);
int     fclose (FILE *stream);
int     feof (FILE *stream);
int     ferror (FILE *stream);
int     fflush (FILE *restrict stream);
int     fgetc (FILE *stream);
int     fileno (FILE *stream);
FILE   *fopen (const char *pathname, const char *mode);
int     fprintf (FILE *restrict stream, const char *restrict fmt, ...);
int     fputc (int c, FILE *stream);
int     fputs  (const char *restrict s, FILE *restrict stream);
size_t  fread (void *ptr, size_t size, size_t nmemb,
                    FILE *restrict stream);
size_t  fwrite (const void *ptr, size_t size, size_t nmemb,
                    FILE *restrict stream);
int     getc (FILE *stream);
ssize_t getdelim (char **restrict lineptr, size_t *restrict n,
                       int delim, FILE *restrict stream);
ssize_t getline (char **restrict lineptr, size_t *restrict n,
                       FILE *restrict stream);
int     printf (const char *restrict fmt, ...);
int     putc (int c, FILE *stream);
int     puts (const char *s);
int     snprintf (char *buff, size_t n, const char *restrict fmt, ...);

#define putchar(x) fputc(x,stdout)

#ifdef __cplusplus
}
#endif

