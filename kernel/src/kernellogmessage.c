/*============================================================================

  kernellogmessage.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_time.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "kernellogmessage.h"

/*============================================================================

  kernellogmessage_destroy

============================================================================*/
extern void kernellogmessage_destroy (KernelLogMessage *self)
  {
  if (self->text) free (self->text);
  free (self);
  }


/*============================================================================

  kernellogmessage_new

============================================================================*/
KernelLogMessage *kernellogmessage_new (time_t timestamp, 
    KernelLogLevel level, const char *text)
  {
  KernelLogMessage *self = malloc (sizeof (KernelLogMessage));
  self->text = strdup (text);
  self->level = level;
  self->timestamp = timestamp;
  return self;
  }


/*============================================================================

  kernellogmessage_to_text

============================================================================*/
char *kernellogmessage_to_text (KernelLogMessage *self, int flags)
  {
  const char *text = self->text;
  int l = strlen (text);
  char *ret = malloc (l + 10);

  switch (self->level)
    {
    case ERROR: strcpy (ret, "ERROR "); break; 
    case INFO:  strcpy (ret, "INFO  "); break;
    case WARN:  strcpy (ret, "WARN  "); break;
    case DEBUG: strcpy (ret, "DEBUG "); break;
    default:    strcpy (ret, "???   "); break;
    }

  strcpy (ret + 6, text);

  return ret; 
  }


