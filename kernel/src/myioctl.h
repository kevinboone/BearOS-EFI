/*============================================================================

  myioctl.h

  Implementation of the ioctl() compatibility function.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "../../lib/include/stddef.h"
#include "process.h" 

extern int myioctl (Process *process, int fd, unsigned long request, 
    intptr_t arg1, intptr_t arg2, intptr_t arg3);



