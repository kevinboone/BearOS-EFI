/*============================================================================

  reboot.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include <asm/reboot.h>

#ifdef __cplusplus
extern "C" {
#endif

/** `cmd` is one of the BEAROS_REBOOT_XXX constants defined in asm/reboot.h. */
int reboot (int cmd);

#ifdef __cplusplus
}
#endif



