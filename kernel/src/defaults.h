/*============================================================================

  defaults.c

  Various default values for the BearOS kernel

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

// Program the kernel runs on boot
#define DEFAULT_INIT "/bin/bosh"

// Size of the temporary directory's filesystem, in bytes
#define TMPDIR_SIZE (4096*1024)
