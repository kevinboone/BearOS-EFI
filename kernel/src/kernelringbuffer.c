/*============================================================================

  kernelringbuffer.c

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <stdarg.h> 
#include "compat_stdio.h"
#include "compat_time.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "kernellogmessage.h"
#include "kernelringbuffer.h"
#include "list.h"
#include "nanoprintf.h"
#include "process.h"

struct _KernelRingBuffer
  {
  List *list;
  int len;
  };

static KernelRingBuffer *global_krb = NULL;

//FWD
static void kernelringbuffer_destroy (KernelRingBuffer *self);
static KernelRingBuffer *kernelringbuffer_new (void);


/*============================================================================

  kernelringbuffer_append

============================================================================*/
static void kernelringbuffer_append (KernelRingBuffer *self, 
      KernelLogLevel level, const char *text)
  {
  KernelLogMessage *msg = kernellogmessage_new (mytime (NULL), level, text);

  PROC_BEGIN_SYNC

  list_append (self->list, msg);
  int l = list_length (self->list);
  if (l >= self->len)
    list_remove_at (self->list, 0);
 
  PROC_END_SYNC
  }


/*============================================================================

  kernelringbuffer_cleanup

============================================================================*/
void kernelringbuffer_cleanup (void)
  {
  if (global_krb) kernelringbuffer_destroy (global_krb);
  }


/*============================================================================

  kernelringbuffer_destroy

============================================================================*/
static void kernelringbuffer_destroy (KernelRingBuffer *self)
  {
  if (self->list) list_destroy (self->list);
  free (self);
  }


/*============================================================================

  kernelringbuffer_get_text_form

============================================================================*/
char *kernelringbuffer_get_text_form (void)
  {
  PROC_BEGIN_SYNC

  char *out = malloc(1);
  out[0] = 0;
  int out_len = 0;

  int l = list_length (global_krb->list);

  for (int i = 0; i < l; i++)
    {
    KernelLogMessage *klm = list_get (global_krb->list, i);
    char *text = kernellogmessage_to_text (klm, 0); 
    int text_len = strlen (text);
    out = realloc (out, out_len + text_len + 10); 
    strcpy (out + out_len, text);
    out[out_len + text_len] = '\n';
    out[out_len + text_len + 1] = 0;
    free (text);
    out_len += text_len + 1;
    }

  PROC_END_SYNC
  return out;
  }


/*============================================================================

  kernelringbuffer_log

============================================================================*/
void kernelringbuffer_log (KernelLogLevel level, const char *fmt,...)
  {
  char buffer[1024]; // TODO TODO TODO
  va_list val;
  va_start (val, fmt);
  npf_vsnprintf (buffer, sizeof (buffer), fmt, val);
  va_end (val);
  kernelringbuffer_append (global_krb, level, buffer);
  }


/*============================================================================

  kernelringbuffer_destroy

============================================================================*/
void kernelringbuffer_init (void)
  {
  global_krb = kernelringbuffer_new();
  }


/*============================================================================

  kernelringbuffer_new

============================================================================*/
static KernelRingBuffer *kernelringbuffer_new (void)
  {
  KernelRingBuffer *self = malloc (sizeof (KernelRingBuffer));
  self->list = list_create ((ListFreeItemFn)kernellogmessage_destroy);
  self->len = 10; // TODO
  return self;
  }



