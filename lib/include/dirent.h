/*============================================================================

  dirent.h 

  BearOS alternative to the conventional opendir(), etc 

============================================================================*/

#pragma once

#include <asm/dirent.h>

typedef struct _DIR DIR;

#ifdef __cplusplus
extern "C" {
#endif

extern DIR    *opendir (const char *pathname);
extern int     closedir (DIR *dir);
extern struct  dirent *readdir (DIR *dir);

#ifdef __cplusplus
}
#endif


