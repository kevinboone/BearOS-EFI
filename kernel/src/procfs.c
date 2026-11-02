/*============================================================================

  procfs.c

  /proc is a filesystem handled by the generic pseudo-filesystem 
  PseudoFsHandler. 

  See procfs.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_limits.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "defs.h"
#include "fileops.h"
#include "procfs.h"
#include "kernelringbuffer.h"
#include "../../lib/include/errno_numbers.h"
#include "list.h"
#include "mounts.h"
#include "process.h"
#include "procmgr.h"
#include "textpseudofile.h"

//FWD
static struct FileOps *procfs_open_mounts (void);
static struct FileOps *procfs_open_dmesg (void);

/*============================================================================

  procfs_open_as_file_by_name

============================================================================*/
static struct FileOps *procfs_open_as_file_by_name (char *name)
  {
  if (strcmp (name, "mounts") == 0)
    return procfs_open_mounts ();
  else if (strcmp (name, "dmesg") == 0)
    return procfs_open_dmesg ();

  return NULL;
  }


/*============================================================================

  procfs_open_dmesg

============================================================================*/
static struct FileOps *procfs_open_dmesg (void)
  {
  char *text = kernelringbuffer_get_text_form(); 
  TextPseudoFile *tsf = textpseudofile_new (text); 
  free (text);
  return textpseudofile_get_fops (tsf);
  }


/*============================================================================

  procfs_open_mounts

============================================================================*/
static struct FileOps *procfs_open_mounts (void)
  {
  char *text = mounts_get_text_form(); 
  TextPseudoFile *tsf = textpseudofile_new (text); 
  free (text);
  return textpseudofile_get_fops (tsf);
  }


/*============================================================================

  procfs_populate_root

============================================================================*/
int procfs_populate_root (List *entries)
  {
  list_append (entries, strdup ("dmesg"));
  list_append (entries, strdup ("mounts"));
  return 0;
  }


/*============================================================================

  procfs_open_file

============================================================================*/
struct FileOps *procfs_open_file (const char *name)
  {
  int fd = process_get_next_fd (procmgr_get_current_process());
  if (fd < 0)
    {
    set_errno (ENFILE); 
    return NULL;
    }

  struct FileOps *ret = procfs_open_as_file_by_name ((char *)name); 
  if (ret)
    {
    process_set_fileops (procmgr_get_current_process(), fd, ret);
    ret->refs = 1;
    return ret;
    }

  set_errno (ENOENT);
  return NULL;
  }



