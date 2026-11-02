/*============================================================================

  kernelringbuffer.h

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once
#include "kernellogmessage.h"

struct _KernelRingBuffer;
typedef struct _KernelRingBuffer KernelRingBuffer;

extern char *kernelringbuffer_get_text_form (void);
extern void  kernelringbuffer_init (void);
extern void  kernelringbuffer_log (KernelLogLevel level, const char *fmt,...);
extern void  kernelringbuffer_cleanup (void);

