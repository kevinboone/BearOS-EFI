/*============================================================================

  compat_stdlib.h

  realpath() function, etc 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

/** realpath() behaves as in Linux, and expects POSIX-style paths. This
    function can fail. If it does, it returns NULL, and sets errno. 
    The `out` argument is a pointer to a buffer whose size is at
    least PATH_MAX bytes. On success, the function returns the
    value of `out`. */
extern char *realpath (const char *path, char *out);

