/*============================================================================

  efiutil.h

  EFI stuff that doesn't fit anywhere else.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifndef LINUX

#include <efi/efi.h>
#include <efi/efilib.h>
#include "compat_time.h"

/** Gets a device handle matching the device path. Why does gnu-efi not
    already provide such a fundamental procedure? Perhaps it does,
    and I just couldn't find it :/ */
extern EFI_HANDLE efi_util_get_handle_from_path (const char *path);

/** Convert an EFI_STATUS value into a Linux-like errno, for compatibility
    with Linux code. */
extern int efi_util_status_to_errno (EFI_STATUS status);

/** Convert an EFI_TIME structure (which has a similar function to
    POSIX "struct tm" into a time_t. */
extern time_t efi_util_time_to_epoch (const EFI_TIME *efitime);

/** Like the Unix "time(NULL)". */
extern time_t efi_util_current_time (void);

#endif
