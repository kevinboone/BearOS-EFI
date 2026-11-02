/*============================================================================

  reboot.c

  Implementation of myreboot() 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#include <efi/efi.h>
#include <efi/efilib.h>
#include "acpi.h"
#include "defs.h"
#include "../../lib/include/bearos/apidefs.h"
#include "../../lib/include/errno_numbers.h"
#include "../../lib/include/bearos/apidefs.h"
#include "../../lib/include/asm/reboot.h"
#include "procmgr.h"

/*============================================================================

  myreboot

============================================================================*/
int myreboot (int cmd)
  {
#ifdef LINUX
  set_errno (ENOTSUP);
  return -1;
#else
  switch (cmd)
    {
    case BEAROS_REBOOT_RESTART:
      RT->ResetSystem (EfiResetCold, 0, 0, NULL);
      return 0;

    case BEAROS_REBOOT_SHUTDOWN:
      return acpi_shutdown();

    default: 
      {
      set_errno (EINVAL);
      return -1;
      }
    }
#endif
  }


