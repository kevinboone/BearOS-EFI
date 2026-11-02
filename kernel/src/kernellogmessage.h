/*============================================================================

  kmsg.h

  Kernel log messages

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

#include "compat_time.h"

typedef enum _KernelLogLevel
  {
  ERROR = 0,
  WARN = 1,
  INFO = 2,
  DEBUG = 3 
  } KernelLogLevel;


typedef struct _KernelLogMessage
  {
  time_t timestamp;
  KernelLogLevel level;
  char *text;
  } KernelLogMessage;


extern void kernellogmessage_destroy (KernelLogMessage *self);

extern KernelLogMessage *kernellogmessage_new (time_t timestamp, 
  KernelLogLevel level, const char *text);

extern char *kernellogmessage_to_text (KernelLogMessage *self, int flags);
