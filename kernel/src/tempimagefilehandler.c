/*============================================================================

  tempimagefilehandler.c

  A block device handler for the temporary directory; 
  used in both EFI and Linux builds.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "blockops.h"
#include "compat_fcntl.h"
#include "../../lib/include/stddef.h"
#include "../../lib/include/errno_numbers.h"
#include "compat_malloc.h"
#include "compat_stdio.h"
#include "compat_string.h"
#include "compat_unistd.h"
#include "defaults.h"
#include "tempimagefilehandler.h"
#include "fileops.h"
#include "process.h"
#include "procmgr.h"

#define SECTOR_SIZE 512

struct _TempImageFileHandler
  {
  uint8_t *mem;
  size_t size;
  };

struct TempImageFileFopsData
  {
  struct FileOps *fops;
  struct BlockOps *bops;
  off_t pos;
  };

extern time_t global_boot_time;

int tempimage_file_handler_struct_close_and_cleanup (struct FileOps *fs); // FWD
int tempimage_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff); // FWD

/*============================================================================

  FWD 

============================================================================*/
ssize_t tempimage_file_handler_struct_read_sectors (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors);
ssize_t tempimage_file_handler_struct_write_sectors (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors);
ssize_t tempimage_file_handler_struct_get_sector_count (struct BlockOps *bs);

/*============================================================================

  tempimagefilehandler_close_and_cleanup

============================================================================*/
void tempimagefilehandler_close_and_cleanup 
        (TempImageFileHandler *self)
  {
  tempimagefilehandler_destroy (self);
  }


/*============================================================================

  tempimagefilehandler_destroy

============================================================================*/
void tempimagefilehandler_destroy (TempImageFileHandler *self)
  {
  if (self->mem) free (self->mem);
  free (self);
  }


/*============================================================================

  tempimagefilehandler_get_blockops

============================================================================*/
struct BlockOps *tempimagefilehandler_get_blockops 
        (TempImageFileHandler *self)
  {
  struct BlockOps *bs = malloc (sizeof (struct BlockOps));
  bs->refs = 1;
  bs->read_sectors = tempimage_file_handler_struct_read_sectors;
  bs->write_sectors = tempimage_file_handler_struct_write_sectors;
  bs->get_sector_count = tempimage_file_handler_struct_get_sector_count;
  bs->data = self;
  return bs;
  }

/*============================================================================

  tempimagefilehandler_get_fileops

============================================================================*/
struct FileOps *tempimagefilehandler_get_fileops 
        (TempImageFileHandler *self)
  {
  struct FileOps *fs = malloc (sizeof (struct FileOps));
  fs->refs = 1; // ??? 

  struct TempImageFileFopsData *fopsData = malloc 
    (sizeof (struct TempImageFileFopsData));
  memset (fopsData, 0, sizeof (struct TempImageFileFopsData));
  fopsData->fops = fs;
  fopsData->pos = 0;
  fopsData->bops = tempimagefilehandler_get_blockops (self);

  fs->data = fopsData;
  fs->fstat = tempimage_file_handler_struct_fstat;
  fs->close_and_cleanup = tempimage_file_handler_struct_close_and_cleanup;
/*
  fs->ioctl = tempimage_file_handler_struct_ioctl;
  fs->lseek = tempimage_file_handler_struct_lseek;
  fs->read = tempimage_file_handler_struct_read;
  fs->write = tempimage_file_handler_struct_write;
*/
  return fs;
  }


/*============================================================================

  tempimagefilehandler_new

============================================================================*/
TempImageFileHandler *tempimagefilehandler_new (size_t size)
  {
  TempImageFileHandler *self = malloc (sizeof (TempImageFileHandler));
  self->mem = NULL;
  self->size = size;
  return self;
  }


/*============================================================================

  tempimagefilehandler_prepare

============================================================================*/
int tempimagefilehandler_prepare (TempImageFileHandler *self)
  {
  self->mem = malloc (self->size);
  return 0;
  }


/*============================================================================

  tempimage_filehandler_struct_close_and_cleanup

============================================================================*/
int tempimage_file_handler_struct_close_and_cleanup (struct FileOps *fs)
  {
  struct TempImageFileFopsData *fopsData = fs->data;
  free (fopsData->bops);
  free (fopsData); 
  return 0;
  }


/*============================================================================

  tempimage_file_handler_fstat

============================================================================*/
int tempimage_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff)
  {
  statbuff->st_mode = S_IFBLK | 0666;
  statbuff->st_size = 0; // As POSIX 
  statbuff->st_mtime = global_boot_time;
  return 0;
  }


/*============================================================================

  tempimagefilehandler_struct_get_sector_count

============================================================================*/
ssize_t tempimage_file_handler_struct_get_sector_count (struct BlockOps *bs)
  {
  TempImageFileHandler *self = (TempImageFileHandler *)bs->data;
  return self->size / SECTOR_SIZE;
  }

/*============================================================================

  tempimagefilehandler_struct_read_sectors

============================================================================*/
ssize_t tempimage_file_handler_struct_read_sectors (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors)
  {
  TempImageFileHandler *self = (TempImageFileHandler *)bs->data;
  size_t byte_count = num_sectors * SECTOR_SIZE;
  size_t byte_offset = offset * SECTOR_SIZE;
  memcpy (buff, self->mem + byte_offset, byte_count);
  return byte_count;
  }


/*============================================================================

  tempimagefilehandler_struct_write_sectors

============================================================================*/
ssize_t tempimage_file_handler_struct_write_sectors (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors)
  {
  TempImageFileHandler *self = (TempImageFileHandler *)bs->data;
  size_t byte_count = num_sectors * SECTOR_SIZE;
  size_t byte_offset = offset * SECTOR_SIZE;
  memcpy (self->mem + byte_offset, buff, byte_count);
  return byte_count;
  }



