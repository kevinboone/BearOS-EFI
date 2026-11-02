/*============================================================================

  interface_ff.c 

  BearOS interface to ChaN's VFAT filesystem module

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include "../../lib/include/stdint.h"
#include "../../lib/include/errno_numbers.h"
#include "blockops.h"
#include "compat_fcntl.h"
#include "compat_time.h"
#include "datetimeutil.h"
#include "defs.h"
#include "interface_ff.h"
#include "ff.h"
#include "diskio.h"
// procmgr defines errno
#include "procmgr.h" 

/** Mapping between FatFS drive numbers (0-10) and block devices. */
static struct BlockOps *ffmap[FF_MAX_DRV];

//FWD
DRESULT interface_ff_errno_to_dresult (int _errno);
struct BlockOps *interface_ff_get_blockops_for_drv (BYTE pdrv);

/*============================================================================

  disk_initialize

============================================================================*/
DSTATUS disk_initialize (BYTE pdrv)
  {
  return 0;
  }


/*============================================================================

  disk_status

============================================================================*/
DSTATUS disk_status (BYTE pdrv)
  {
  return 0; // May be more to do here
  }


/*============================================================================

  disk_read

============================================================================*/
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
  {
  struct BlockOps *bo = interface_ff_get_blockops_for_drv (pdrv);
  ssize_t n_read = bo->read_sectors (bo, buff, sector, count);
  if (n_read < 0)
    return interface_ff_errno_to_dresult (errno);
  else
    return RES_OK;
  }


/*============================================================================

  disk_write

============================================================================*/
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
  {
  struct BlockOps *bo = interface_ff_get_blockops_for_drv (pdrv);
  ssize_t n_written = bo->write_sectors (bo, buff, sector, count);
  if (n_written < 0)
    return interface_ff_errno_to_dresult (errno);
  else
    return RES_OK;
  }


/*============================================================================

  disk_ioctl

============================================================================*/
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
  {
  return 0; // Probably more to do here
  }


/*============================================================================

  get_fattime 

============================================================================*/
DWORD get_fattime (void) 
  {
  time_t now = mytime (NULL);
  struct tm *stm = gmtime (&now);

  return (DWORD)(stm->tm_year - 80) << 25 |
           (DWORD)(stm->tm_mon + 1) << 21 |
           (DWORD)stm->tm_mday << 16 |
           (DWORD)stm->tm_hour << 11 |
           (DWORD)stm->tm_min << 5 |
           (DWORD)stm->tm_sec >> 1;
  }


/*============================================================================

  interface_ff_add_mapping 

============================================================================*/
int interface_ff_add_mapping (struct BlockOps *blockops)
  {
  for (int i = 0; i < FF_MAX_DRV; i++)
    {
    if (ffmap[i] == NULL)
      {
      ffmap[i] = blockops;
      return i;
      }
    }
  return 0xFF;
  }


/*============================================================================

  interface_ff_epoch_to_fattime

============================================================================*/
DWORD interface_ff_epoch_to_fattime (time_t epoch)
  {
  struct tm *stm = gmtime (&epoch);

  int year = stm->tm_year + 1900;
  int month = stm->tm_mon + 1;
  int mday = stm->tm_mday;
  int hour = stm->tm_hour;
  int min = stm->tm_min;
  int sec = stm->tm_sec;

  WORD fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
  WORD ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);
  
  return fdate << 16 | ftime;
  }

/*============================================================================

  interface_ff_errno_to_dresult

============================================================================*/
DRESULT interface_ff_errno_to_dresult (int _errno)
  {
  switch (_errno)
    {
    case 0: return 0;
    case EIO: return RES_ERROR;
    case EROFS: return RES_WRPRT;
    case ETIMEDOUT: return RES_NOTRDY;
    default: return RES_ERROR; 
    }
  }


/*============================================================================

  interface_ff_fresult_to_errno

============================================================================*/
int interface_ff_fresult_to_errno (int fresult)
  {
  switch (fresult)
    {
    case 0: return 0;
    case FR_DISK_ERR: return EIO;
    case FR_NOT_READY: return ETIMEDOUT;
    case FR_NO_FILE: return ENOENT;
    case FR_NO_PATH: return ENOENT;
    case FR_INVALID_NAME: return EINVAL;
    case FR_DENIED: return EPERM;
    case FR_EXIST: return EPERM;
    case FR_INVALID_OBJECT: return EINVAL;
    case FR_WRITE_PROTECTED: return EROFS;
    case FR_INVALID_DRIVE: return EINVAL;
    case FR_NO_FILESYSTEM: return EINVAL;
    case FR_TIMEOUT: return ETIMEDOUT;
    case FR_NOT_ENOUGH_CORE: return ENOMEM;
    case FR_TOO_MANY_OPEN_FILES: return ENFILE;
    case FR_INVALID_PARAMETER: return EINVAL;
    default: return EIO; 
    }
  }


/*============================================================================

  interface_ff_fattime_to_epoch

============================================================================*/
time_t interface_ff_fattime_to_epoch (DWORD ftime)
  {
  int sec = (ftime & 0x1F) << 1;
  ftime >>= 5;
  int min = (ftime & 0x3F);
  ftime >>= 6;
  int hour = (ftime & 0x1F);
  ftime >>= 5;
  int day = (ftime & 0x1F);
  ftime >>= 5;
  int month = (ftime & 0x0F);
  ftime >>= 4;
  int year = (int)ftime;

  return datetimeutil_ymdhms_to_epoch (year + 1980, month,
      day, hour, min, sec, 0);

  }


/*============================================================================

  interface_ff_get_blockops_for_drv

============================================================================*/
struct BlockOps *interface_ff_get_blockops_for_drv (BYTE pdrv)
  {
  if (pdrv > FF_MAX_DRV) 
    return NULL;
  return ffmap[pdrv];
  }


/*============================================================================

  interface_ff_posix_to_fat_mode

============================================================================*/
BYTE interface_ff_posix_to_fat_mode (int posix_mode)
  {
  BYTE ret = 0;
  // We can't do anything with the high-numbered flags, like O_BINARY.
  // So, for now, we just mask them out.
  posix_mode &= 0xFFFF;
  if (posix_mode == O_RDONLY) ret |= FA_READ;
  if (posix_mode & O_WRONLY) ret |= FA_WRITE;
  if (posix_mode & O_RDWR) ret |= (FA_READ | FA_WRITE);
  if (posix_mode & O_APPEND) ret |= FA_OPEN_APPEND;
  if (posix_mode & O_CREAT) ret |= FA_OPEN_ALWAYS;
  if (posix_mode & O_TRUNC) ret |= FA_CREATE_ALWAYS;
  return ret;
  }


/*============================================================================

  interface_ff_remove_mapping

============================================================================*/
void interface_ff_remove_mapping (BYTE pdrv)
  {
  if (pdrv > FF_MAX_DRV) return;
  ffmap[pdrv] = NULL;
  }


