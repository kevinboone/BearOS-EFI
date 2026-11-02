/*============================================================================

  path.h

  Path-handling functions 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <libbearos/bearos_defs.h> 
#include <sys/stat.h> 

struct _Path;
typedef struct _Path Path;

#ifdef __cplusplus
extern "C" {
#endif

/** Appends a new path component. If p represents an absolute path, that is,
    starts with /, then it replaces the old path. Otherwise it is appended,
    removing any spurious /'s */
extern void path_append (Path *self, const UTF8 *p);

/** Converts the path to absolute canonical form, using realpath(). This
    function can fail, in which case it returns -1 and sets errno. */
extern int path_canonicalize (Path *self);

extern UTF8 *path_cstr (const Path *self);

extern void path_destroy (Path *self);

extern Path *path_new (const UTF8 *s);

// path_is_directory and path_is_file both return false for a file that
//   does not exist. 
extern BOOL path_is_directory (const Path *self);
extern BOOL path_is_file (const Path *self);

// Fill a struct stat 
extern int path_stat (const Path *self, struct stat *sb);

#ifdef __cplusplus
}
#endif




