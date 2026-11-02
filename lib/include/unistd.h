/*============================================================================

  unistd.h 

  BearOS alternative to the conventional unistd.h

============================================================================*/

#pragma once

#include <stddef.h>

#ifndef STDIN_FILENO 
#define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO 
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO 
#define STDERR_FILENO 2
#endif

#define SEEK_SET 0
#define SEEK_CUR 1 
#define SEEK_END 2 

extern char **environ;

#ifdef __cplusplus
extern "C" {
#endif

int       close  (int fd);
int       chdir  (const char *path);
char     *getcwd (char *buff, size_t maxlen); 
int       isatty (int fd);
ssize_t   read   (int fd, void *buff, size_t maxlen);
int       rmdir (const char *pathname);

// runv(), etc, are BearOS equivalents of the POSIX fork/exec path. They
//   run a program, leaving the caller to wait for it to finish.
int       runv   (const char *pathname, char *const argv[]);
int       runvp  (const char *file, char *const argv[]);
int       runvpe (const char *file, char *const argv[], char *const envp[]);

off_t     lseek  (int fd, off_t offset, int whence);
int       unlink (const char *pathname);
ssize_t   write  (int fd, const void *buff, size_t maxlen);

#ifdef __cplusplus
}
#endif



