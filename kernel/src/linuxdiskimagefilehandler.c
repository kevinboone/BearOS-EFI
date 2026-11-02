/*============================================================================

  linuxdiskimagefilehandler.c

  A block device handler for the Linux virtual root image; 
  only needed on Linux builds.
  This class delegates to the ordinary standard C library to its access
  to the image.

  In principle, this class handles both file-type and block-type manipulation,
  but no account has so far been taken of trying to do both in the same
  session. We need to do more to manage the file pointer to the underlying
  Linux image file.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifdef LINUX
#include "blockops.h"
#include "compat_fcntl.h"
#include "../../lib/include/stddef.h"
#include "../../lib/include/errno_numbers.h"
#include "compat_malloc.h"
#include "compat_stdio.h"
#include "compat_string.h"
#include "compat_unistd.h"
#include "linuxdiskimagefilehandler.h"
#include "fileops.h"
#include "process.h"
#include "procmgr.h"

#define SECTOR_SIZE 512

struct _LinuxDiskImageFileHandler
  {
  int f;
  UTF8 *path;
  };

struct LinuxDiskImageFopsData
  {
  struct FileOps *fops;
  // We will use the block operations to do file operations. So we need
  //   to open a struct BlocksOps whenever we open the handler as
  //   a file -- and clean it up when the file closes.
  struct BlockOps *bops;
  off_t pos;
  };

extern time_t global_boot_time;

/*============================================================================

  FWD 

============================================================================*/
int linuxdiskimage_file_handler_struct_close_and_cleanup (struct FileOps *fs);
int linuxdiskimage_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff);
int linuxdiskimage_file_handler_struct_ioctl (struct FileOps *fs, int fd, 
        long request, intptr_t arg1, intptr_t args, intptr_t arg3);
ssize_t linuxdiskimage_file_handler_struct_lseek (struct FileOps *fs, 
        off_t offset, int whence);
ssize_t linuxdiskimage_file_handler_struct_read (struct FileOps *fs, 
        void *buff, size_t count);
ssize_t linuxdiskimage_file_handler_struct_read_sectors (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors);
ssize_t linuxdiskimage_file_handler_struct_write (struct FileOps *fs, 
        const void *buff, size_t count);
ssize_t linuxdiskimage_file_handler_struct_write_sectors (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors);

/*============================================================================

  Externs -- declared here so we don't have to #include anything that
  might clash 

============================================================================*/
extern int close (int fd);
extern int open (char *path, int mode);
extern ssize_t read (int fd, void *buffer, size_t count);
extern ssize_t write (int fd, const void *buffer, size_t count);
extern off_t lseek (int fd, off_t offset, int whence);

/*============================================================================

  linuxdiskimagefilehandler_get_blockops

============================================================================*/
void linuxdiskimagefilehandler_close_and_cleanup 
        (LinuxDiskImageFileHandler *self)
  {
  linuxdiskimagefilehandler_destroy (self);
  }


/*============================================================================

  linuxdiskimagefilehandler_destroy

============================================================================*/
void linuxdiskimagefilehandler_destroy (LinuxDiskImageFileHandler *self)
  {
  if (self->f >= 0) close (self->f);
  free (self->path);
  free (self);
  }


/*============================================================================

  linuxdiskimagefilehandler_get_blockops

============================================================================*/
struct BlockOps *linuxdiskimagefilehandler_get_blockops 
        (LinuxDiskImageFileHandler *self)
  {
  struct BlockOps *bs = malloc (sizeof (struct BlockOps));
  bs->refs = 1;
  bs->read_sectors = linuxdiskimage_file_handler_struct_read_sectors;
  bs->write_sectors = linuxdiskimage_file_handler_struct_write_sectors;
  bs->data = self;
  return bs;
  }

/*============================================================================

  linuxdiskimagefilehandler_get_fileops

============================================================================*/
struct FileOps *linuxdiskimagefilehandler_get_fileops 
        (LinuxDiskImageFileHandler *self)
  {
  struct FileOps *fs = malloc (sizeof (struct FileOps));
  fs->refs = 1; // ??? 

  struct LinuxDiskImageFopsData *fopsData = malloc 
    (sizeof (struct LinuxDiskImageFopsData));
  fopsData->fops = fs;
  fopsData->pos = 0;
  fopsData->bops = linuxdiskimagefilehandler_get_blockops (self);

  fs->data = fopsData;
  fs->close_and_cleanup = linuxdiskimage_file_handler_struct_close_and_cleanup;
  fs->fstat = linuxdiskimage_file_handler_struct_fstat;
  fs->ioctl = linuxdiskimage_file_handler_struct_ioctl;
  fs->lseek = linuxdiskimage_file_handler_struct_lseek;
  fs->read = linuxdiskimage_file_handler_struct_read;
  fs->write = linuxdiskimage_file_handler_struct_write;
  return fs;
  }


/*============================================================================

  linuxdiskimagefilehandler_new

============================================================================*/
LinuxDiskImageFileHandler *linuxdiskimagefilehandler_new (const UTF8 *path)
  {
  LinuxDiskImageFileHandler *self = malloc (sizeof (LinuxDiskImageFileHandler));
  self->path = (UTF8 *)strdup ((char *)path);
  self->f = -1;
  return self;
  }


/*============================================================================

  linuxdiskimagefilehandler_prepare

============================================================================*/
int linuxdiskimagefilehandler_prepare (LinuxDiskImageFileHandler *self)
  {
  self->f = open ((char *)self->path, O_RDWR);
  if (self->f >= 0) return 0;
  return -1;
  }


/*============================================================================

  linuxdiskimagefilehandler_new

============================================================================*/
int linuxdiskimage_file_handler_struct_close_and_cleanup (struct FileOps *fs)
  {
  struct LinuxDiskImageFopsData *fopsData = fs->data;
  free (fopsData->bops);
  free (fopsData); 
  return 0;
  }


/*============================================================================

  linuxdiskimage_file_handler_fstat

============================================================================*/
int linuxdiskimage_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff)
  {
  statbuff->st_mode = S_IFBLK | 0666;
  statbuff->st_size = 0; // As POSIX 
  statbuff->st_mtime = global_boot_time;
  return 0;
  }


/*============================================================================

  linuxdiskimage_file_handler_ioctl

============================================================================*/
int linuxdiskimage_file_handler_struct_ioctl (struct FileOps *fs, int fd, 
       long request, intptr_t arg1, intptr_t arg2, intptr_t arg3)
  {
  set_errno (EINVAL);
  return -1;
  }


/*============================================================================

  linuxdiskimagefilehandler_struct_lseek

============================================================================*/
ssize_t linuxdiskimage_file_handler_struct_lseek (struct FileOps *fs, 
    off_t offset, int whence) 
  {
  struct LinuxDiskImageFopsData *fopsData = fs->data;
  off_t off;
  if (whence == SEEK_CUR)
    off = offset + fopsData->pos;
  else if (whence == SEEK_END)
    {
    // At present, we don't know how long the block device is.
    set_errno (ENOTSUP);
    return -1;
    } 
  else
    off = offset;
  fopsData->pos = off;
  return off;
  }


/*============================================================================

  linuxdiskimagefilehandler_struct_read

  // TODO TODO TODO deal with situation where len > SECTOR_SIZE

============================================================================*/
ssize_t linuxdiskimage_file_handler_struct_read (struct FileOps *fs, 
    void *buff, size_t len)
  {
  struct LinuxDiskImageFopsData *fopsData = fs->data;
  off_t pos = fopsData->pos;
  off_t start_source_sector = pos / SECTOR_SIZE;
  int start_pos_in_sector = pos % SECTOR_SIZE;

  BYTE sec_buff[SECTOR_SIZE];

  size_t bytes_read = 0;
  off_t out_pos = 0;

  off_t source_sector = start_source_sector;
  int pos_in_sector = start_pos_in_sector;
  int rem_read = len - bytes_read;

  ssize_t sec_read = linuxdiskimage_file_handler_struct_read_sectors 
          (fopsData->bops, sec_buff, source_sector, 1); 

  bytes_read += sec_read * SECTOR_SIZE;

  memcpy (buff + out_pos, sec_buff + pos_in_sector, rem_read); 

  if (bytes_read > len)
    {
    fopsData->pos += len; 
    return len;
    }
  else
    {
    fopsData->pos += bytes_read; 
    return bytes_read;
    }
  }


/*============================================================================

  linuxdiskimagefilehandler_struct_read_sectors

============================================================================*/
ssize_t linuxdiskimage_file_handler_struct_read_sectors (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors)
  {
  LinuxDiskImageFileHandler *self = (LinuxDiskImageFileHandler *)bs->data;
  size_t byte_count = num_sectors * SECTOR_SIZE;
  size_t byte_offset = offset * SECTOR_SIZE;
  if (lseek (self->f, byte_offset, 0) >= 0)
    {
    int bytes_read = read (self->f, buff, byte_count); 
    if (bytes_read >= 0) 
      return bytes_read / SECTOR_SIZE;
    else
      return -1;
    }
  else
    return -1;
  }


/*============================================================================

  linuxdiskimagefilehandler_write

============================================================================*/
ssize_t linuxdiskimage_file_handler_struct_write (struct FileOps *fs, 
    const void *buff, size_t len)
  {
  return -1;
  }


/*============================================================================

  linuxdiskimagefilehandler_struct_write_sectors

============================================================================*/
ssize_t linuxdiskimage_file_handler_struct_write_sectors (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors)
  {
  LinuxDiskImageFileHandler *self = (LinuxDiskImageFileHandler *)bs->data;
  size_t byte_count = num_sectors * SECTOR_SIZE;
  size_t byte_offset = offset * SECTOR_SIZE;
  if (lseek (self->f, byte_offset, 0) >= 0)
    {
    int bytes_written = write (self->f, buff, byte_count); 
    if (bytes_written >= 0) 
      return bytes_written / SECTOR_SIZE;
    else
      return -1;
    }
  else
    return -1;
  }

#endif


