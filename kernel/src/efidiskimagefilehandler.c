/*============================================================================

  efidiskimagefilehandler.c

  This is a handler for block storage devices exposed as EFI handles.

  See efidiskimagefilehandler.h for more information.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifndef LINUX
#include <efi/efi.h>
#include <efi/efilib.h>
#include "blockops.h"
#include "compat_fcntl.h"
#include "../../lib/include/errno_numbers.h"
#include "compat_malloc.h"
#include "compat_stdio.h"
#include "compat_string.h"
#include "compat_unistd.h"
#include "efidiskimagefilehandler.h"
#include "efiutil.h"
#include "fileops.h"
#include "process.h"
#include "procmgr.h"

// TODO should we try to account for different sector sizes?
#define SECTOR_SIZE 512

struct _EFIDiskImageFileHandler
  {
  EFI_HANDLE     efi_device;
  EFI_BLOCK_IO  *bio;
  int32_t        media_id;
  };

extern time_t global_boot_time;

struct EFIDiskImageFopsData
  {
  struct FileOps *fops;
  // We will use the block operations to do file operations. So we need
  //   to open a struct BlocksOps whenever we open the handler as
  //   a file -- and clean it up when the file closes.
  struct BlockOps *bops;
  off_t pos;
  };


/*============================================================================

  FWD 

============================================================================*/
int efidiskimage_file_handler_struct_close_and_cleanup (struct FileOps *fs);
int efidiskimage_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff);
int efidiskimage_file_handler_struct_ioctl (struct FileOps *fs, int fd, 
        long request, intptr_t arg1, intptr_t args, intptr_t arg3);
ssize_t efidiskimage_file_handler_struct_lseek (struct FileOps *fs, 
        off_t offset, int whence);
ssize_t efidiskimage_file_handler_struct_read (struct FileOps *fs, 
        void *buff, size_t count);
ssize_t efidiskimage_file_handler_struct_read_sectors (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors);
ssize_t efidiskimage_file_handler_struct_write (struct FileOps *fs, 
        const void *buff, size_t count);
ssize_t efidiskimage_file_handler_struct_write_sectors (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors);


/*============================================================================

  efidiskimagefilehandler_get_blockops

============================================================================*/
void efidiskimagefilehandler_close_and_cleanup 
        (EFIDiskImageFileHandler *self)
  {
  efidiskimagefilehandler_destroy (self);
  }


/*============================================================================

  efidiskimagefilehandler_destroy

============================================================================*/
void efidiskimagefilehandler_destroy (EFIDiskImageFileHandler *self)
  {
  if (self->efi_device >= 0) free (self->efi_device);
  free (self);
  }


/*============================================================================

  efidiskimagefilehandler_get_blockops

============================================================================*/
struct BlockOps *efidiskimagefilehandler_get_blockops 
        (EFIDiskImageFileHandler *self)
  {
  struct BlockOps *bs = malloc (sizeof (struct BlockOps));
  bs->refs = 1;
  bs->read_sectors = efidiskimage_file_handler_struct_read_sectors;
  bs->write_sectors = efidiskimage_file_handler_struct_write_sectors;
  bs->data = self;
  return bs;
  }

/*============================================================================

  efidiskimagefilehandler_get_fileops

============================================================================*/
struct FileOps *efidiskimagefilehandler_get_fileops 
        (EFIDiskImageFileHandler *self)
  {
  struct FileOps *fs = malloc (sizeof (struct FileOps));
  fs->refs = 1; // ??? 

  struct EFIDiskImageFopsData *fopsData = malloc 
    (sizeof (struct EFIDiskImageFopsData));
  fopsData->fops = fs;
  fopsData->pos = 0;
  fopsData->bops = efidiskimagefilehandler_get_blockops (self);

  fs->data = fopsData;
  fs->close_and_cleanup = efidiskimage_file_handler_struct_close_and_cleanup;
  fs->fstat = efidiskimage_file_handler_struct_fstat;
  fs->ioctl = efidiskimage_file_handler_struct_ioctl;
  fs->lseek = efidiskimage_file_handler_struct_lseek;
  fs->read = efidiskimage_file_handler_struct_read;
  fs->write = efidiskimage_file_handler_struct_write;
  return fs;
  }


/*============================================================================

  efidiskimagefilehandler_new

============================================================================*/
EFIDiskImageFileHandler *efidiskimagefilehandler_new (EFI_HANDLE efi_device)
  {
  EFIDiskImageFileHandler *self = malloc (sizeof (EFIDiskImageFileHandler));
  self->efi_device = efi_device;
  return self;
  }


/*============================================================================

  efidiskimagefilehandler_prepare

============================================================================*/
int efidiskimagefilehandler_prepare (EFIDiskImageFileHandler *self)
  {
  EFI_GUID bioGuid = BLOCK_IO_PROTOCOL;
  EFI_BLOCK_IO *bio;
  EFI_STATUS status = uefi_call_wrapper (BS->HandleProtocol, 3, 
    self->efi_device, &bioGuid, (void **) &bio);

  if (status == 0)
    {
    self->media_id = bio->Media->MediaId;
    self->bio = bio;
    return 0;
    }

  return -1;
  }


/*============================================================================

  efidiskimagefilehandler_struct_close_and_cleanup

============================================================================*/
int efidiskimage_file_handler_struct_close_and_cleanup (struct FileOps *fs)
  {
  struct EFIDiskImageFopsData *fopsData = fs->data;
  free (fopsData->bops);
  free (fopsData); 
  return 0;
  }


/*============================================================================

  efidiskimage_file_handler_fstat

============================================================================*/
int efidiskimage_file_handler_struct_fstat (struct FileOps * fs, 
      struct stat *statbuff)
  {
  statbuff->st_mode = S_IFBLK | 0666;
  statbuff->st_size = 0; // As POSIX 
  statbuff->st_mtime = global_boot_time;
  return 0;
  }

/*============================================================================

  efidiskimage_file_handler_ioctl

============================================================================*/
int efidiskimage_file_handler_struct_ioctl (struct FileOps *fs, int fd, 
       long request, intptr_t arg1, intptr_t arg2, intptr_t arg3)
  {
  set_errno (EINVAL);
  return -1;
  }


/*============================================================================

  efidiskimagefilehandler_struct_lseek

============================================================================*/
ssize_t efidiskimage_file_handler_struct_lseek (struct FileOps *fs, 
    off_t offset, int whence) 
  {
  struct EFIDiskImageFopsData *fopsData = fs->data;
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

  efidiskimagefilehandler_struct_read

  // TODO TODO TODO deal with situation where len > SECTOR_SIZE

============================================================================*/
ssize_t efidiskimage_file_handler_struct_read (struct FileOps *fs, 
    void *buff, size_t len)
  {
  struct EFIDiskImageFopsData *fopsData = fs->data;
  off_t pos = fopsData->pos;
  off_t start_source_sector = pos / SECTOR_SIZE;
  int start_pos_in_sector = pos % SECTOR_SIZE;

  BYTE sec_buff[SECTOR_SIZE];

  size_t bytes_read = 0;
  off_t out_pos = 0;

  off_t source_sector = start_source_sector;
  int pos_in_sector = start_pos_in_sector;
  int rem_read = len - bytes_read;

  ssize_t sec_read = efidiskimage_file_handler_struct_read_sectors 
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

  efidiskimagefilehandler_struct_read_sectors

============================================================================*/
ssize_t efidiskimage_file_handler_struct_read_sectors (struct BlockOps *bs, 
        void *buff, off_t offset, size_t num_sectors)
  {
  EFIDiskImageFileHandler *self = (EFIDiskImageFileHandler *)bs->data;

  EFI_STATUS status = uefi_call_wrapper (self->bio->ReadBlocks, 5, self->bio, 
        self->media_id, offset, num_sectors * SECTOR_SIZE, buff);
  if (status == 0)
    {
    return num_sectors;
    }
  else
    {
    set_errno (efi_util_status_to_errno (status));
    return -1;
    } 
  }


/*============================================================================

  efidiskimagefilehandler_write


============================================================================*/
ssize_t efidiskimage_file_handler_struct_write (struct FileOps *fs, 
    const void *buff, size_t len)
  {
  return -1;
  }


/*============================================================================

  efidiskimagefilehandler_struct_write_sectors

============================================================================*/
ssize_t efidiskimage_file_handler_struct_write_sectors (struct BlockOps *bs, 
        const void *buff, off_t offset, size_t num_sectors)
  {
  EFIDiskImageFileHandler *self = (EFIDiskImageFileHandler *)bs->data;

  EFI_STATUS status = uefi_call_wrapper (self->bio->WriteBlocks, 5, self->bio, 
        self->media_id, offset, num_sectors * SECTOR_SIZE, buff);

  if (status == 0)
    {
    return num_sectors;
    }
  else
    {
    int _errno = efi_util_status_to_errno (status);
    set_errno (_errno);
    return -1;
    } 
  }

#endif



