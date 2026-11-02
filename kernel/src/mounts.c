/*============================================================================

  mounts.c
  
  Manages the mount table. There's a lot more work to do here.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_limits.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "defs.h"
#include "devfs.h"
#include "procfs.h"
#include "devmgr.h"
#include "../../lib/include/bearos/defaults.h"
#include "../../lib/include/asm/vfs.h"
#include "../../lib/include/errno_numbers.h"
#include "fatfshandler.h"
#include "kernelringbuffer.h"
#include "mounts.h"
#include "path.h"
#include "pseudofshandler.h"
#include "procmgr.h"
#include "../../lib/include/stddef.h"
#include "process.h"
#include "string8.h"

struct _Mount
  {
  char source[32];
  UTF8 mount_point [PATH_MAX];
  struct FSOps *fsops;
  };

Mount mounts[MOUNTS_MAX];

// FWD
static int mounts_get_free_slot();

/*============================================================================

  mounts_add_mount

============================================================================*/
int mounts_add_mount (const char *source, const UTF8* path, 
      struct FSOps *fsops)
  {
  PROC_BEGIN_SYNC
  int n = mounts_get_free_slot();
  if (n >= 0)
    {
    Path *p = path_new (path);
    // The target path will be stored and used in pattern matching against
    //   new paths. So we need the path in a form with no relative bits
    //   or trailing / characters.
    path_canonicalize (p);
    strcpy ((char *)mounts[n].mount_point, (char *)path_cstr (p));
    strncpy (mounts[n].source, source, sizeof mounts[n].source);
    mounts[n].fsops = fsops;
    PROC_END_SYNC
    kernelringbuffer_log (INFO, "Mounted %s on %s", source, path_cstr(p));
    path_destroy (p);
    return 0;
    }
  else
    {
    PROC_END_SYNC
    set_errno (ENOSPC);
    kernelringbuffer_log (ERROR, "Failed to mount %s on %s: too many mounts", 
      source, path);
    return -1;
    }
  }


/*============================================================================

  mounts_cleanup

============================================================================*/
void mounts_cleanup (void)
  {
  for (int i = 0; i < MOUNTS_MAX; i++)
    {
    Mount *m = &mounts[i];
    if (m->source[0])
      {
      mounts_umount_by_mount_point (m->mount_point);
      }
    }
  }


/*============================================================================

  mounts_get_free_slot

============================================================================*/
static int mounts_get_free_slot (void)
  {
  for (int i = 0; i < MOUNTS_MAX; i++)
    {
    Mount *m = &mounts[i];
    if (m->source[0] == 0) return i;
    }
  return -1;
  }


/*============================================================================

  mounts_get_fsops_by_path

============================================================================*/
static struct FSOps *mounts_get_fsops_by_path_on_root (const UTF8 *path, 
    UTF8 **rem_path)
  {
  struct FSOps *ret = 0;
  for (int i = 0; (i < MOUNTS_MAX) && (ret == 0); i++)
    {
    Mount *vol = &mounts[i];
    if (vol->source[0] == 0) continue;

    const UTF8 *cand_root = vol->mount_point; 
    if (strcmp ((char *)cand_root, "/"))
      {
      // This is not the root filesystem
      }
    else
      {
      // This is the root filesystem
      if (strcmp ((char *)path, "/") == 0)
        {
        // User asked for the root of the root filesystem
        if (rem_path)
	  *rem_path = (UTF8 *)strdup ("/");
        ret = vol->fsops;
        }
      else
        {
        if (rem_path)
	  *rem_path = (UTF8 *)strdup ((char *)path);
        ret = vol->fsops;
        }
      }
    }
  return ret;
  }


/*============================================================================

  mounts_get_fsops_by_path

============================================================================*/
static struct FSOps *mounts_get_fsops_by_path_no_root (const UTF8 *path, 
    UTF8 **rem_path)
  {
  struct FSOps *ret = 0;
  for (int i = 0; (i < MOUNTS_MAX) && (ret == 0); i++)
    {
    Mount *vol = &mounts[i];
    if (vol->source[0] == 0) continue;

    const UTF8 *cand_root = vol->mount_point; 
    if (strcmp ((char *)cand_root, "/"))
      {
      // This is not the root filesystem
      char temp[PATH_MAX];
      strcpy (temp, (char *)path); // TODO max len
      // First character of path must by /, because this function
      //   only accepts canonical paths
      char *sep_pos = strchr (temp + 1, '/');
      if (sep_pos)
        {
        // path is /a/b, with a / in it
        *sep_pos = 0;
        }

      if (strcmp ((char *)temp, (char *)vol->mount_point) == 0) 
        {
        const char *rempath = (char *)path + strlen (temp);
        if (rem_path)
          {
          if (rempath[0])
            *rem_path = (UTF8 *)strdup ((char *)path + strlen (temp));
          else
            *rem_path = (UTF8 *)strdup ("/");
          }
        ret = vol->fsops;
        }
      }
    else
      {
      // This is the root filesystem
      }
    }
  return ret;
  }


/*============================================================================

  mounts_get_fsops_by_path

  This logic is ugly and broken. The problem is that a path starting
  /foo/bar could match the root directory /, even if there is a /foo
  that is a mount point. We really ought to sort the mount table by
  length of mount point, and try matching from the longest first.
  Instead, I've adopted the ugly hack of cheching the root fs only
  if all the other mount points fail. This logic will collapse if
  we have a mount on /foo/ and another on /foo/bar.

============================================================================*/
struct FSOps *mounts_get_fsops_by_path (const UTF8 *path, 
    UTF8 **rem_path)
  {
  if (strlen ((char *)path) < 1) 
    {
    set_errno (ENOENT); // Nobody can handle an empty path
    return 0;
    }

  struct FSOps *ret = mounts_get_fsops_by_path_no_root (path, rem_path);
  if (ret)
    {
    // Nothing to do -- we found it
    }
  else
    {
    // Try on the root fs if nothing else
    ret = mounts_get_fsops_by_path_on_root (path, rem_path);
    }

  if (ret == NULL)
    set_errno (ENOENT);

  return ret;
  }


/*============================================================================

  mounts_get_text_form

============================================================================*/
char *mounts_get_text_form (void)
  {
  PROC_BEGIN_SYNC
  String8 *s = string8_new_empty();
  for (int i = 0; i < MOUNTS_MAX; i++)
    {
    const char *source = mounts[i].source;
    if (source[0] == 0) continue;
    const UTF8 *mount_point = mounts[i].mount_point;     
    const char *type = mounts[i].fsops->type;
    string8_append (s, (UTF8 *)source);
    string8_append (s, (UTF8 *)" ");
    string8_append (s, mount_point);
    string8_append (s, (UTF8 *)" ");
    string8_append (s, (UTF8 *)type);
    
    // TODO 
    string8_append (s, (UTF8 *)"\n");
    }
  char *ret = strdup ((char *)string8_cstr (s));
  string8_destroy (s);
  PROC_END_SYNC
  return ret; 
  }


/*============================================================================

  mounts_init

  Create the mount points we really can't do without

============================================================================*/
void mounts_init (void)
  {
  kernelringbuffer_log (INFO, "Mounting core filesystems"); 
  mounts_mount ("proc", BEAROS_PROC, "proc", 0, NULL);
  mounts_mount ("dev", "/dev", "dev", 0, NULL);
  mounts_mount ("/dev/root", "/", "vfat", 0, NULL);
  }


/*============================================================================

  mounts_mount

============================================================================*/
int mounts_mount (const char *source, const char *target, const char *type,
      unsigned long flags, const void *data)
  {
  int ret = 0;

  Path *p = path_new ((UTF8 *)target);

  kernelringbuffer_log (INFO, "Mounting %s on %s, type %s", source,
    target, type);

  // May as well try VFAT, as we don't support anything else yet
  if (strcmp (type, "auto") == 0) type = "vfat"; //TODO

  // We can't test whether the target is a directory when the system is
  //   starting, because there's no root directory to test it against.
  //if (path_is_directory (p))
    {
    if (strcmp (type, "proc") == 0)
      {
      PseudoFsHandler *proc_fs_handler = pseudofshandler_new 
          (procfs_populate_root, procfs_open_file, "proc", 
              PROC_SUPER_MAGIC); 
      struct FSOps *fsops = fsops = pseudofshandler_get_fsops (proc_fs_handler);
      ret = mounts_add_mount ("proc", (UTF8 *)target, fsops);
      }
    else if (strcmp (type, "dev") == 0)
      {
      PseudoFsHandler *dev_fs_handler = pseudofshandler_new (devfs_populate_root, 
	devfs_open_file, "dev", TMPFS_SUPER_MAGIC); 
      struct FSOps *fsops = pseudofshandler_get_fsops (dev_fs_handler);
      ret = mounts_add_mount ("dev", (UTF8 *)target, fsops);
      }
    else if (strcmp (type, "vfat") == 0)
      {
      struct BlockOps *ldbops = devmgr_open_as_block_by_name (source);
      if (ldbops)
	{
        PROC_BEGIN_SYNC
	FatFsHandler *fatfs_handler = fatfshandler_new (ldbops); 
        if (fatfshandler_mount (fatfs_handler) == 0)
          {
	  struct FSOps *fsops = fatfshandler_get_fsops (fatfs_handler);
	  ret = mounts_add_mount (source, (UTF8 *)target, fsops);
          }
        else
          {
          // errno should have been set
          fatfshandler_destroy (fatfs_handler);
          ret = -1;
          }
        PROC_END_SYNC
	}
      else
        {
        kernelringbuffer_log (ERROR, "Not a valid device: %s", source);
        // errno should have been set
        ret = -1;
        }
      }
    else
      {
      kernelringbuffer_log (ERROR, "Unknown filesystem type: %s", type);
      set_errno (EINVAL);
      ret = -1;
      }
    }
  //else
  //  {
  //  ret = -1;
  //  set_errno (ENOTDIR);
  //  }

  path_destroy (p);
  return ret;
  }


/*============================================================================

  mounts_umount_by_mount

============================================================================*/
int mounts_umount_by_mount_point (UTF8 *mount_point)
  {
  for (int i = 0; i < NVOL_MAX; i++)
    {
    Mount *vol = &mounts[i];
    if (vol->source[0] == 0) continue;
    const UTF8 *cand_root = vol->mount_point; 
    if (strcmp ((char *)cand_root, (char *)mount_point) == 0)
      {
      if (vol->fsops->umount (vol->fsops) == 0)
        {
        vol->source[0] = 0;
        free (vol->fsops);
        vol->fsops = NULL;
        vol->mount_point[0] = 0;
        return 0;
        }
      else
        return -1; // errno already set
      }
    }

  // not a mount point
  set_errno (EINVAL);
  return -1;
  }



