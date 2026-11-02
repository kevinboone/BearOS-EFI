/*============================================================================

  vfs.h 

============================================================================*/

#pragma once

#include <asm/vfs.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int statfs (const char *filename, struct statfs *sb);

#ifdef __cplusplus
}
#endif



