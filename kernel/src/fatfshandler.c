/*============================================================================

  fatfshandler.c

  A handler for the FAT filesystem, that mostly defers to ChaN's FAT handler
  for embedded systems.
  
  See fatshandler.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "compat_dirent.h"
#include "compat_fcntl.h"
#include "compat_limits.h"
#include "compat_malloc.h"
#include "compat_string.h"
#include "compat_stdio.h"
#include "compat_string.h"
#include "compat_unistd.h"
#include "devmgr.h"
#include "ff.h"
#include "fatfshandler.h"
#include "fsops.h"
#include "interface_ff.h"
#include "kernelringbuffer.h"
#include "../../lib/include/errno_numbers.h"
#include "../../lib/include/asm/vfs.h"
#include "procmgr.h"
#include "../../lib/include/stddef.h"
#include "list.h"


/** Definition of FatFSHandler attributes. */
struct _FatFsHandler
  {
  int pdrv; // FatFS drive ID, 0-9
  char fat_root[10]; // Root path, e.g., "0://"
  FATFS *fatfs; 
  struct BlockOps *bops; // Block device that backs this filesystem
  };

/** Serve as the `data` attribute for the `struct fileops` for a regular file */
struct FatFsFileData
  {
  FatFsHandler *owner;
  FIL fil;
  char fat_path[PATH_MAX];
  };

/** Serve as the `data` attribute for the `struct fileops` for a regular directory */
struct FatFsDirData
  {
  FatFsHandler *owner;
  DIR dirp;
  char fat_path[PATH_MAX];
  };

// FWD
static struct FileOps *fatfshandler_open (void *data, 
                       const UTF8 *path, int mode);
static int    fatfs_fops_dir_fstat (struct FileOps * fops, 
                       struct stat *statbuff);
static ssize_t fatfs_fops_dir_read (struct FileOps * fops, 
                       void *buff, size_t count);
static int     fatfs_fops_file_close_and_cleanup (struct FileOps *fops);
static int     fatfs_fops_file_fstat (struct FileOps * fops, 
                       struct stat *statbuff);
static off_t   fatfs_fops_file_lseek (struct FileOps *fs, 
                       off_t offset, int whence); 
static ssize_t fatfs_fops_file_read (struct FileOps * fops, 
                       void *buffer, size_t count);
static ssize_t fatfs_fops_file_write (struct FileOps * fops, 
                       const void *buffer, size_t count);
static int     fatfshandler_mkdir (void *data, const UTF8 *path, int mode);
static int     fatfshandler_statfs (void *data, struct statfs *sb);
static int     fatfshandler_unlink (void *data, const UTF8 *path);
static int     fatfshandler_utime  (void *data, const UTF8 *path,
                       const struct utimbuf *buff);
static int     fatfshandler_umount (void *data);


/*============================================================================

  fatfshandler_destroy

============================================================================*/
void fatfshandler_destroy (FatFsHandler *self)
  {
  if (self->fatfs) 
    {
    f_unmount (self->fat_root);
    free (self->fatfs);
    }
  interface_ff_remove_mapping (self->pdrv);
  if (self->bops) free (self->bops);
  free (self);
  }


/*============================================================================

  fatfshandler_fops_dir_close_and_cleanup

============================================================================*/
int fatfs_fops_dir_close_and_cleanup (struct FileOps *fops)
  {
  struct FatFsDirData *data = (struct FatFsDirData *)fops->data;
  f_closedir (&data->dirp);
  free (data);
  return 0;
  }


/*============================================================================

  fatfshandler_fops_dir_fstat

============================================================================*/
int fatfs_fops_dir_fstat (struct FileOps * fops, 
      struct stat *sb)
  {
  // We don't need to know anything about this _particular_ directory.
  // On FAT filesystem, all directories have the same POSIX attributes.
  sb->st_mode = S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR |
         S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;
  sb->st_size = 0;
  sb->st_mtime = 0;
  return 0;
  }


/*============================================================================

  fatfshandler_fops_dir_read

============================================================================*/
ssize_t fatfs_fops_dir_read (struct FileOps * fops, 
      void *buff, size_t count)
  {
  struct FatFsDirData *data = (struct FatFsDirData *)fops->data;
  FILINFO fno;
  int err = f_readdir (&data->dirp, &fno);
  if (err != 0)
    {
    set_errno (interface_ff_fresult_to_errno (err));
    return -1;
    }
  
  if (fno.fname[0] == 0) // End of dir
    return 0;

  // TODO check buffer size "count"
  struct dirent *de = (struct dirent *)buff;
  strncpy (de->d_name, fno.fname, sizeof (de->d_name)); 
  return sizeof (struct dirent);
  }


/*============================================================================

  fatfshandler_fops_file_close_and_cleanup

============================================================================*/
int fatfs_fops_file_close_and_cleanup (struct FileOps *fops)
  {
  struct FatFsFileData *data = (struct FatFsFileData *)fops->data;
  f_close (&data->fil);
  free (data);
  return 0;
  }


/*============================================================================

  fatfshandler_fops_file_fstat

============================================================================*/
int fatfs_fops_file_fstat (struct FileOps * fops, 
      struct stat *sb)
  {
  struct FatFsFileData *data = (struct FatFsFileData *)fops->data;
  //FatFsHandler *self = data->owner; 
  FILINFO fno;

  int err = f_stat (data->fat_path, &fno);
  if (err == 0)
    {
    sb->st_size = fno.fsize;
    sb->st_mode = S_IFREG | S_IRUSR | S_IWUSR | S_IXUSR |
         S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;
    sb->st_mtime = interface_ff_fattime_to_epoch (fno.fdate << 16 | fno.ftime);
    return 0;
    }
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    return -1;
    }
  }


/*============================================================================

  fatfs_fops_file_write

============================================================================*/
static ssize_t fatfs_fops_file_write (struct FileOps * fops, 
      const void *buff, size_t count)
  {
  struct FatFsFileData *data = (struct FatFsFileData *)fops->data;
  unsigned nwritten = 0;
  int err = f_write (&data->fil, buff, count, &nwritten); 
  if (err == 0)
    {
    return nwritten;
    }
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    return -1;
    }
  }


/*============================================================================

  fatfs_fops_file_lseek

============================================================================*/
static off_t fatfs_fops_file_lseek (struct FileOps *fs, 
                       off_t offset, int whence)
  {
  struct FatFsFileData *data = (struct FatFsFileData *)fs->data;

  FIL *fil = &data->fil;
  FSIZE_t current_pos = fil->fptr;
  FSIZE_t ofs = 0;
  if (whence == SEEK_CUR)
    {
    ofs = current_pos + offset;
    }
  else if (whence == SEEK_END)
    {
    FILINFO info;
    f_stat (data->fat_path, &info); // TODO -- handle error
    ofs = info.fsize - offset;
    }
  else
    {
    ofs = offset;
    }

  FRESULT err = f_lseek (fil, ofs);
  if (err == 0)
    {
    return ofs;
    }
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    return -1;
    }
  }


/*============================================================================

  fatfs_fops_file_read

============================================================================*/
static ssize_t fatfs_fops_file_read (struct FileOps * fops, 
      void *buff, size_t count)
  {
  struct FatFsFileData *data = (struct FatFsFileData *)fops->data;
  unsigned nread = 0;
  int err = f_read (&data->fil, buff, count, &nread); 
  if (err == 0)
    {
    return nread;
    }
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    return -1;
    }
  }


/*============================================================================

  fatfshandler_get_fsops

============================================================================*/
struct FSOps *fatfshandler_get_fsops (FatFsHandler *self)
  {
  struct FSOps *fsops = malloc (sizeof (struct FSOps));
  memset (fsops, 0, sizeof (struct FSOps));
  fsops->data = self;
  fsops->open = fatfshandler_open;
  strcpy ((char *)fsops->type, "vfat");
  fsops->magic = MSDOS_SUPER_MAGIC;
  fsops->mkdir = fatfshandler_mkdir;
  fsops->statfs = fatfshandler_statfs;
  fsops->unlink = fatfshandler_unlink;
  fsops->umount = fatfshandler_umount;
  fsops->utime = fatfshandler_utime;
  // In FF, unlink and rmdir are the same thing
  fsops->rmdir = fatfshandler_unlink;
  return fsops;
  }


/*============================================================================

  fatfshandler_mkdir

============================================================================*/
static int fatfshandler_mkdir (void *data, const UTF8 *path, int mode)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  FatFsHandler *self = (FatFsHandler *)fsops->data;

  char fat_path [PATH_MAX];
  mysnprintf (fat_path, PATH_MAX, "%s%s", self->fat_root, path);

  int ret;

  FRESULT err = f_mkdir (fat_path);

  if (err == 0)
    ret = 0;
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    ret = -1;
    }

  return ret;
  }


/*============================================================================

  fatfshandler_mount

============================================================================*/
int fatfshandler_mount (FatFsHandler *self)
  {
  int ret = 0;

  int pdrv = interface_ff_add_mapping (self->bops); 
  self->pdrv = pdrv;
  mysnprintf (self->fat_root, sizeof (self->fat_root), "%d:/", pdrv);
  
  int err = f_mount (self->fatfs, self->fat_root, 1);    
  if (err)
    {
    kernelringbuffer_log (ERROR, "FAT f_mount() failed, error %d", err);
    set_errno (interface_ff_fresult_to_errno (err));
    ret = -1;
    }

  return ret;
  }

/*============================================================================

  fatfshandler_new

============================================================================*/
FatFsHandler *fatfshandler_new (struct BlockOps *bops)
  {
  FatFsHandler *self = malloc (sizeof (FatFsHandler));
  self->fatfs = malloc (sizeof (FATFS)); 
  self->bops = bops;
  return self;
  }


/*============================================================================

  fatfshandler_open

============================================================================*/
static struct FileOps *fatfshandler_open (void *data, 
        const UTF8 *path, int mode)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  FatFsHandler *self = (FatFsHandler *)fsops->data;

  // In FF, for some weird reason "/" is not a directory, and we can't
  //   call f_stat() on it. We must fake directory attributes.
  BOOL force_root = FALSE;
  if (strcmp ((char *)path, "/") == 0) force_root = TRUE;

  char fat_path [PATH_MAX];
  mysnprintf (fat_path, PATH_MAX, "%s%s", self->fat_root, path);

  BOOL is_dir = FALSE;
  BOOL create = FALSE;

  FILINFO fno;
  if (force_root)
    is_dir = TRUE;  // We can only open / as a directory 

  if (mode & O_CREAT) create = TRUE;

  if (!is_dir && !create)
    {
    // We don't need to stat the file if we already know it's a directory,
    //    or we are creating the file
    int err = f_stat (fat_path, &fno);
    if (err == 0)
      {
      is_dir = fno.fattrib & AM_DIR;
      } 
    }

  if (force_root || is_dir)
    {
    DIR dir;
    int err = f_opendir (&dir, fat_path);
    if (err != 0)
      {
      set_errno (interface_ff_fresult_to_errno (err));
      return NULL; 
      }

    PROC_BEGIN_SYNC
    int fd = process_get_next_fd (procmgr_get_current_process());
    if (fd < 0)
      {
      set_errno (ENFILE); 
      PROC_END_SYNC
      return NULL;
      }

    struct FatFsDirData *fdd = malloc (sizeof (struct FatFsDirData));
    fdd->owner = self;
    strncpy (fdd->fat_path, fat_path, sizeof (fdd->fat_path));
    fdd->dirp = dir;

    struct FileOps *fops = malloc (sizeof (struct FileOps));
    fops->data = fdd;
    fops->refs = 1;
    fops->read = fatfs_fops_dir_read;
    fops->fstat = fatfs_fops_dir_fstat;
    fops->close_and_cleanup = fatfs_fops_dir_close_and_cleanup;
    fops->ioctl = NULL;

    process_set_fileops (procmgr_get_current_process(), fd, fops);
    PROC_END_SYNC
    return fops;
    }
  else
    {
    // It can only a regular file -- FAT supports nothing else
    FIL fil;
    BYTE fat_mode = interface_ff_posix_to_fat_mode (mode);

    FRESULT err = f_open (&fil, fat_path, fat_mode);

    if (err != 0)
      {
      set_errno (interface_ff_fresult_to_errno (err));
      return NULL; 
      }

    PROC_BEGIN_SYNC
    int fd = process_get_next_fd (procmgr_get_current_process());
    if (fd < 0)
      {
      set_errno (ENFILE); 
      PROC_END_SYNC
      return NULL;
      }

    struct FatFsFileData *ffd = malloc (sizeof (struct FatFsFileData));
    ffd->owner = self;
    ffd->fil = fil; 
    strncpy (ffd->fat_path, fat_path, sizeof (ffd->fat_path));
    struct FileOps *fops = malloc (sizeof (struct FileOps));
    fops->data = ffd;
    fops->refs = 1;
    fops->close_and_cleanup = fatfs_fops_file_close_and_cleanup;
    fops->fstat = fatfs_fops_file_fstat;
    fops->lseek = fatfs_fops_file_lseek;
    fops->read = fatfs_fops_file_read;
    fops->write = fatfs_fops_file_write;
    fops->ioctl = NULL;

    process_set_fileops (procmgr_get_current_process(), fd, fops);
    PROC_END_SYNC
    return fops;
    }
  return NULL; // Can't get here
  }


/*============================================================================

  fatfshandler_statfs

============================================================================*/
static int fatfshandler_statfs (void *data, struct statfs *sb)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  FatFsHandler *self = (FatFsHandler *)fsops->data;

  memset (sb, 0, sizeof (struct statfs)); 

  FATFS *fs;

  uint32_t free_clusters = 0;
  int err = f_getfree (self->fat_root, &free_clusters, &fs); 
  
  int ret = 0;
  if (err == 0)
    {
    int sector_size = FF_MIN_SS; 
    // sector size is not a constant if FF is configured to support a 
    //   range of sectors sizes
    //int sector_size = (int)fs->ssize; // bytes 

    sb->f_type = MSDOS_SUPER_MAGIC;
    sb->f_bsize = sector_size;
    sb->f_blocks = (fs->n_fatent - 2) * fs->csize;
    sb->f_bfree = free_clusters * fs->csize;
    }
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    ret = -1;
    }
  return ret;
  }


/*============================================================================

  fatfshandler_unlink

============================================================================*/
static int fatfshandler_unlink (void *data, const UTF8 *path)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  FatFsHandler *self = (FatFsHandler *)fsops->data;

  char fat_path [PATH_MAX];
  mysnprintf (fat_path, PATH_MAX, "%s%s", self->fat_root, path);

  int ret;

  FRESULT err = f_unlink (fat_path);

  if (err == 0)
    ret = 0;
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    ret = -1;
    }

  return ret;
  }


/*============================================================================

  fatfshandler_utime

============================================================================*/
static int fatfshandler_utime (void *data, const UTF8 *path,
        const struct utimbuf *buff)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  FatFsHandler *self = (FatFsHandler *)fsops->data;

  char fat_path [PATH_MAX];
  mysnprintf (fat_path, PATH_MAX, "%s%s", self->fat_root, path);

  int ret;

  FILINFO fno;
  DWORD fat_datetime = interface_ff_epoch_to_fattime (buff->modtime);
  fno.fdate = fat_datetime >> 16;
  fno.ftime = fat_datetime & 0xFFFF;
  FRESULT err = f_utime (fat_path, &fno);

  if (err == 0)
    ret = 0;
  else
    {
    set_errno (interface_ff_fresult_to_errno (err));
    ret = -1;
    }

  return ret;
  }

/*============================================================================

  fatfshandler_umount

============================================================================*/
static int fatfshandler_umount (void *data)
  {
  struct FSOps *fsops = (struct FSOps *)data;
  FatFsHandler *self = (FatFsHandler *)fsops->data;
  fatfshandler_destroy (self); 
  return 0;
  }



