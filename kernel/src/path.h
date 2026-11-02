/*============================================================================

  path.h

  Path-handling functions 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "compat_stat.h"

struct _Path;
typedef struct _Path Path;

#ifdef LINUX
struct stat;
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

// Fill a struct stat, so far as possible with EFI.
extern int path_stat (const Path *self, struct stat *sb);

#ifndef LINUX

// NOTE The following functions are not really Unicode-aware, and need
//  to be completed.

/** Convert a UTF8 path to UFT16, and reverse the / to \. */
extern CHAR16 *path_to_efi (const UTF8 *pathname);
/** Convert an EFI UFT16 path to UTF7, and reverse the \ to /. */
extern UTF8 *path_from_efi (const CHAR16 *pathname);
#endif

