/*============================================================================

  efiutil.c

  EFI stuff that doesn't fit anywhere else.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifndef LINUX

#include <efi/efi.h>
#include <efi/efilib.h>
#include "datetimeutil.h" 
#include "efiutil.h" 
#include "compat_malloc.h" 
#include "compat_stdio.h" 
#include "compat_string.h" 
#include "compat_time.h" 
#include "../../lib/include/errno.h"
#include "path.h" 

/*============================================================================

  efi_util_current_time

============================================================================*/
time_t efi_util_current_time (void)
  {
  EFI_TIME t;
  uefi_call_wrapper (RT->GetTime, 2, &t, NULL);
  time_t now = efi_util_time_to_epoch (&t);
  return now;
  }


/*============================================================================

  efi_util_status_to_errno

============================================================================*/
EFI_HANDLE efi_util_get_handle_from_path (const char *path)
  {
  EFI_HANDLE ret = 0;

  EFI_GUID bp_guid = EFI_BLOCK_IO_PROTOCOL_GUID;

  EFI_HANDLE *handle_buffer = NULL;
  UINTN handle_count = 0;

  EFI_STATUS err = uefi_call_wrapper (BS->LocateHandleBuffer, 5, ByProtocol,
     &bp_guid, NULL, &handle_count, &handle_buffer);
    
  if (err == 0)
    { 
    for (int i = 0; i < handle_count && ret == 0; i++)
      {
      EFI_DEVICE_PATH *dp = DevicePathFromHandle (handle_buffer[i]);
      if (dp)
        {
        CHAR16 *_dps = DevicePathToStr (dp);
        UTF8 *dps = path_from_efi (_dps);
        free (_dps);
	if (strcmp ((char *)dps, path) == 0) 
	  {
	  ret = handle_buffer[i];
	  }
        free (dps);
        }
      }

    FreePool (handle_buffer);
    }

  return ret;
  }


/*============================================================================

  efi_util_status_to_errno

============================================================================*/
int efi_util_status_to_errno (EFI_STATUS status)
  {
  if (status == EFI_SUCCESS) return 0;
  if (status == EFI_UNSUPPORTED ) return ENOTSUP;
  if (status == EFI_INVALID_PARAMETER) return EINVAL;
  if (status == EFI_LOAD_ERROR) return EINVAL;
  if (status == EFI_LOAD_ERROR) return ENOEXEC;
  if (status == EFI_UNSUPPORTED) return EINVAL;
  if (status == EFI_BAD_BUFFER_SIZE) return EINVAL;
  if (status == EFI_DEVICE_ERROR) return ENODEV;
  if (status == EFI_WRITE_PROTECTED) return EROFS;
  if (status == EFI_OUT_OF_RESOURCES) return ENOMEM;
  if (status == EFI_VOLUME_FULL) return ENOSPC;
  if (status == EFI_NO_MEDIA) return ENOMEDIUM;
  if (status == EFI_MEDIA_CHANGED) return EMEDIUMTYPE;
  if (status == EFI_NOT_FOUND) return ENOENT;
  if (status == EFI_ACCESS_DENIED) return EPERM;
  if (status == EFI_NO_RESPONSE) return ETIMEDOUT;
  if (status == EFI_ICMP_ERROR) return EPROTO;
  if (status == EFI_TFTP_ERROR) return EPROTO;
  if (status == EFI_PROTOCOL_ERROR) return EPROTO;
  if (status == EFI_SECURITY_VIOLATION) return EPERM;
  return -1; 
  }


/*============================================================================

  efi_util_time_to_epoch

============================================================================*/
time_t efi_util_time_to_epoch (const EFI_TIME *efitime)
  {
  // TODO correct for TZ and DST
  int yday = datetimeutil_get_yday 
    (efitime->Month, efitime->Day, efitime->Year);

  long e = datetimeutil_ydhms_diff (efitime->Year, yday, 
    efitime->Hour, efitime->Minute, efitime->Second, 1970, 0, 0, 0, 0);

  return e; 
  }

#endif
