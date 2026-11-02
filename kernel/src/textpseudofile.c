/*============================================================================

  textpsedofile.c

  a `struct fops` provider for a read-only text pseudo-file, like /proc/mounts

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_limits.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "defs.h"
#include "fileops.h"
#include "../../lib/include/errno_numbers.h"
#include "process.h"
#include "procmgr.h"
#include "textpseudofile.h"

extern time_t global_boot_time;

struct _TextPseudoFile 
  {
  char *text;
  };


struct TPFData
  {
  char *text;
  TextPseudoFile *owner;
  int len;
  int pos;
  };


/*============================================================================

  textpseudofile_destroy

============================================================================*/
void textpseudofile_destroy (TextPseudoFile *self)
  {
  if (self->text) free (self->text);
  free (self);
  }

/*============================================================================

  textpseudofile_fops_close_and_cleanup

============================================================================*/
static int textpseudofile_fops_close_and_cleanup (struct FileOps *fops)
  {
  struct TPFData *tpf_data = (struct TPFData *)fops->data;
  textpseudofile_destroy (tpf_data->owner);
  if (tpf_data->text) free (tpf_data->text);
  free (tpf_data);
  return 0;
  }


/*============================================================================

  textpseudofile_fops_fstat

============================================================================*/
static int textpseudofile_fops_fstat (struct FileOps *fops, struct stat *sb)
  {
  memset (sb, 0, sizeof (struct stat));
  sb->st_mtime = global_boot_time;
  sb->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;
  return 0;
  }


/*============================================================================

  textpseudofile_fops_read

============================================================================*/
static ssize_t textpseudofile_fops_read (struct FileOps *fops, 
        void *buff, size_t count)
  {
  struct TPFData *tpf_data = (struct TPFData *)fops->data;

  if (tpf_data->pos >= tpf_data->len) return 0; // EOF

  int n = count;
  if (tpf_data->pos + n >= tpf_data->len)
    n = tpf_data->len - tpf_data->pos;
  strncpy (buff, tpf_data->text + tpf_data->pos, n);
  tpf_data->pos += n;
  
  return n;
  }



/*============================================================================

  textpseudofile_get_fops

============================================================================*/
struct FileOps *textpseudofile_get_fops (TextPseudoFile *self)
  {
  struct FileOps *fops = malloc (sizeof (struct FileOps));
  memset (fops, 0, sizeof (struct FileOps));
  fops->read = textpseudofile_fops_read;
  fops->fstat = textpseudofile_fops_fstat;
  fops->close_and_cleanup = textpseudofile_fops_close_and_cleanup;
  fops->data = self;
  fops->ioctl = NULL;

  struct TPFData *tpf_data = malloc (sizeof (struct TPFData));
  tpf_data->text = strdup (self->text);
  tpf_data->len = strlen (tpf_data->text);
  tpf_data->pos = 0;
  tpf_data->owner = self;

  fops->data = tpf_data;

  return fops;
  }


/*============================================================================

  textpseudofile_new

============================================================================*/
TextPseudoFile *textpseudofile_new (const char *text)
  {
  TextPseudoFile *self = malloc (sizeof (TextPseudoFile));
  self->text = strdup (text);
  return self;
  }


