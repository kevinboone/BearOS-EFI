/*============================================================================

  sdlio.c 

  BearOS alternative to the conventional stdio functions.

============================================================================*/

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 1
#define NANOPRINTF_IMPLEMENTATION 1

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <bearos/apidefs.h>
#include "nanoprintf.h"

// Size of buffer in FILE
#define FILE_BUF_LEN 128

// Limits for getline and getdelim
#define DEFAULT_LINE_SIZE 128
#define MIN_LINE_SIZE 4

struct _FILE
  {
  int fd;
  char buff [FILE_BUF_LEN + 1];
  int pos;
  int error;
  int eof;
  };

// Define _FILE structs for stdin, etc.

static FILE f0 = 
  {
  0, {}, 0, 0, 0
  };

static FILE f1 = 
  {
  1, {}, 0, 0, 0
  };

static FILE f2 = 
  {
  2, {}, 0, 0, 0
  };

struct _FILE *stdin = &f0;
struct _FILE *stdout = &f1;
struct _FILE *stderr = &f2;


/*============================================================================

  clearerr 

============================================================================*/
void clearerr (FILE *stream)
  {
  stream->error = 0;
  stream->eof = 0;
  }


/*============================================================================

  fclose

============================================================================*/
int fclose (FILE *stream)
  {
  int ret = 0;
  if (stream == NULL)
    {
    errno = EINVAL;
    ret = -1;
    }
  else
    {
    if (stream->fd != 0) 
      {
      ret = close (stream->fd);
      free (stream);
      }
    }
  return ret;
  }

/*============================================================================

  feof 

============================================================================*/
int feof (FILE *stream)
  {
  return stream->eof;
  }


/*============================================================================

  ferror

============================================================================*/
int ferror (FILE *stream)
  {
  return stream->error;
  }


/*============================================================================

  fputc

============================================================================*/
int fputc (int c, FILE *restrict stream)
  {
  return putc (c, stream);
  }

/*============================================================================

  fputs 

============================================================================*/
int fputs (const char *restrict s, FILE *restrict stream)
  {
  int ret = 0;
  int l = strlen (s);
  for (int i = 0; i < l; i++)
    {
    char c = s[i];
    stream->buff [stream->pos] = c;
    stream->pos++;
    if (c == 10) 
      ret = fflush (stream);
    if (stream->pos >= FILE_BUF_LEN)
      ret =  fflush (stream);
    }
  return ret;
  }


/*============================================================================

  fflush 

============================================================================*/
int fflush (FILE *restrict stream)
  {
  int ret = 0;
  if (stream->pos > 0)
    {
    if (write (stream->fd, stream->buff, stream->pos) >= 0)
      ret = 0;
    else
      ret = EOF;
    }
  stream->pos = 0;
  return ret;
  }

/*============================================================================

  fgetc 

============================================================================*/
int fgetc (FILE *stream)
  {
  int ret;
  char buff[1];
  buff[0] = 0;
  if (fread (buff, 1, 1, stream) <= 0)
    ret = EOF;
  else
    ret = buff[0];
  return ret;
  }


/*============================================================================

  fileno

============================================================================*/
int fileno (FILE *stream)
  {
  return stream->fd;
  }


/*============================================================================

  fread

============================================================================*/
size_t fread (void *ptr, size_t size, size_t nmemb,
                    FILE *restrict stream)
  {
  int items = 0;
  size_t off = 0;
  int done = 0;
  for (int i = 0; i < nmemb && !done; i++)
    {
    size_t n_read = read (stream->fd, (void *)((char *)ptr + off), size); 
    if (n_read < 0)
      {
      stream->error = 1;
      done = 1;
      }
    else if (n_read < size)
      {
      stream->eof = 1;
      done = 1;
      }
    else
      items++;

    off += size;
    }
  return items; 
  }


/*============================================================================

  fwrite

============================================================================*/
size_t fwrite (const void *ptr, size_t size, size_t nmemb,
                    FILE *restrict stream)
  {
  int items = 0;
  size_t off = 0;
  int done = 0;
  for (int i = 0; i < nmemb && !done; i++)
    {
    size_t n_written = write (stream->fd, (void *)((char *)ptr + off), size); 
    if (n_written < 0)
      {
      stream->error = 1;
      done = 1;
      }
    else if (n_written < size)
      {
      stream->eof = 1;
      done = 1;
      }
    else
      items++;

    off += size;
    }
  return items; 
  }


/*============================================================================

  getc

============================================================================*/
int getc (FILE *stream)
  {
  return fgetc (stream);
  }

/*============================================================================

  vpputc

  Helper function for npf_vprint, which exects its character output function
  to return nothing. Ordinary putc returns an int

============================================================================*/
static void vpputc (int c, void *data)
  {
  putc (c, (FILE *)data);
  }


/*============================================================================

  fopen

============================================================================*/
FILE *fopen (const char *pathname, const char *mode)
  {
  FILE *ret = NULL;
  int flags = O_RDONLY;
  if (strcmp (mode, "r") == 0) flags = O_RDONLY;
  else if (strcmp (mode, "r+") == 0) flags = O_RDWR;
  else if (strcmp (mode, "w") == 0) flags = O_WRONLY;
  else if (strcmp (mode, "w+") == 0) flags = O_RDWR | O_TRUNC | O_CREAT;
  else if (strcmp (mode, "a") == 0) flags = O_WRONLY |  O_CREAT | O_APPEND;
  else if (strcmp (mode, "a+") == 0) flags = O_RDWR |  O_CREAT | O_APPEND;
  int fd = open (pathname, flags);
  if (fd >= 0)
    {
    FILE *f = malloc (sizeof (FILE));
    if (f)
      { 
      f->buff[0] = 0;
      f->pos = 0;
      f->error = 0;
      f->eof = 0;;
      f->fd = fd;
      ret = f;
      } 
    else
      errno = ENOMEM; 
    }

  return ret;
  }


/*============================================================================

  fprintf 

============================================================================*/
int fprintf (FILE *restrict stream, const char *restrict fmt, ...)
  {
  va_list val;
  va_start (val, fmt);
  int ret = npf_vpprintf (vpputc, stream, fmt, val);
  va_end (val);
  return ret;
  }


/*============================================================================

  getdelim

============================================================================*/
ssize_t getdelim (char **restrict lineptr, size_t *restrict n,
          int delim, FILE *restrict stream)
  {
  char *buf;
  char *ptr;
  size_t newsize, numbytes;
  int pos;
  int ch;
  int cont;

  if (stream == NULL || lineptr == NULL || n == NULL)
    {
    errno = EINVAL;
      return -1;
    }

  buf = *lineptr;
  if (buf == NULL || *n < MIN_LINE_SIZE) 
    {
    buf = (char *)realloc (*lineptr, DEFAULT_LINE_SIZE);
    if (buf == NULL)
      {
      return -1;
      }
    *lineptr = buf;
    *n = DEFAULT_LINE_SIZE;
    }

  numbytes = *n;
  ptr = buf;

  cont = 1;

  while (cont)
    {
    /* fill buffer - leaving room for nul-terminator */
    while (--numbytes > 0)
      {
      if ((ch = getc (stream)) == EOF)
        {
	cont = 0;
        break;
        }
      else 
        {
        *ptr++ = ch;
        if (ch == delim)
          {
          cont = 0;
          break;
          }
        }
      }

    if (cont)
       {
       // Buffer is too small so reallocate
       pos = ptr - buf;
       newsize = (*n << 1);
       buf = realloc (buf, newsize);
       if (buf == NULL)
         {
         cont = 0;
         break;
         }

       // After reallocating, continue in new buffer 
       *lineptr = buf;
       *n = newsize;
       ptr = buf + pos;
       numbytes = newsize - pos;
       }
    }

  if (ptr == buf)
    return -1;

  *ptr = '\0';
  return (ssize_t)(ptr - buf);
  }

/*============================================================================

  getline 

============================================================================*/
ssize_t getline (char **restrict lineptr, size_t *restrict n,
                       FILE *restrict stream)
  {
  return getdelim (lineptr, n, '\n', stream);
  }



/*============================================================================

  printf 

============================================================================*/
int printf (const char *restrict fmt, ...)
  {
  va_list val;
  va_start (val, fmt);
  int ret = npf_vpprintf (vpputc, stdout, fmt, val);
  va_end (val);
  return ret;
  }


/*============================================================================

 puts 

============================================================================*/
int puts (const char *s)
  {
  int ret = 0;
  int l = strlen (s);
  if (fwrite (s, l, 1, stdout) >= 0)
    if (fwrite ("\n", 1, 1, stdout) >= 0)
      ret = 0;
    else
      ret = -1;
  else
    ret = -1;
  return ret; 
  }


/*============================================================================

 putc 

============================================================================*/
int putc (int c, FILE *stream)
  {
  if (stream->pos >= FILE_BUF_LEN)
    {
    fflush (stream);
    }
  stream->buff [stream->pos] = (char)c;
  stream->pos++;
  if (c == 10)
    {
    fflush (stream);
    }
  return 0; // TODO
  }


/*============================================================================

  snprintf

============================================================================*/
int snprintf (char *buff, size_t n, const char *restrict fmt, ...)
  {
  va_list val;
  va_start (val, fmt);
  int rv = npf_vsnprintf (buff, n, fmt, val); 
  va_end (val);
  return rv;
  }


/*============================================================================

  stdio_init

============================================================================*/
void stdio_init (void)
  {
  stdin = &f0;
  stdout = &f1;
  stderr = &f2;
  }


/*============================================================================

  stdio_cleanup 

============================================================================*/
void stdio_cleanup (void)
  {
  fflush (stdout); 
  fflush (stderr); 
  }

