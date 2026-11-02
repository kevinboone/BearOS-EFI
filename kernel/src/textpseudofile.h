/*============================================================================

  textpsedofile.h

  a `struct fops` provider for a read-only text pseudo-file, like /proc/mounts

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#pragma once

struct _TextPseudoFile;
typedef struct _TextPseudoFile TextPseudoFile;

extern void            textpseudofile_destroy (TextPseudoFile *self);
extern struct FileOps *textpseudofile_get_fops (TextPseudoFile *self);
extern TextPseudoFile *textpseudofile_new (const char *text);


