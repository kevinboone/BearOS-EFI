/*============================================================================

  main.c

  Main REPL loop.

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifdef LINUX
#else
#include <efi/efi.h>
#include <efi/efilib.h>
EFI_HANDLE efi_boot_dev_handle;
#endif

#include "defs.h" 
#include "blockops.h" 
#include "compat_malloc.h" 
#include "compat_stdio.h" 
#include "compat_stat.h" 
#include "compat_time.h" 
#include "console.h" 
#include "consolefilehandler.h" 
#include "device.h" 
#include "devmgr.h" 
#include "fileops.h" 
#include "fsops.h" 
#include "kernelringbuffer.h"
#include "list.h" 
#include "mounts.h" 
#include "path.h" 
#include "procmgr.h" 
#include "process.h" 
#include "shell.h" 

#include "ff.h"

Process *current_process = NULL;
ConsoleFileHandler *cfh = NULL;

time_t global_boot_time;

int main (int argc, char **argv);

/*============================================================================

 cleanup_console

============================================================================*/
void cleanup_console (void)
  {
  //if (cfh)
  //  consolefilehandler_destroy (cfh);
  }


/*============================================================================

  efi_main() is the entry point for the UEFI program. It is called by the
  crt.o stub appropriate for this kind of program, which is part of
  gnu-efi

  FWIW efi_main() gets passed a handle to the disk image from which the
  program was loaded, and also a reference to the EFI system table. This
  reference sets up the standard "ST", "BS", etc., API structures that 
  will be used extensively.

============================================================================*/

#ifndef LINUX
EFI_STATUS EFIAPI efi_main (EFI_HANDLE image_handle, 
    EFI_SYSTEM_TABLE *system_table) 
  {
  InitializeLib (image_handle, system_table);

  EFI_LOADED_IMAGE *loaded_image = NULL;
  EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  uefi_call_wrapper (BS->HandleProtocol, 3,
      image_handle, &lip_guid, (void **) &loaded_image);

  efi_boot_dev_handle = loaded_image->DeviceHandle;

  // Turn off the hardware watchdog timer
  uefi_call_wrapper (BS->SetWatchdogTimer, 4, 0, 0, 0, "");

  main (0, NULL);

  // We never get here
  return EFI_SUCCESS;
  }
#endif

/*============================================================================

 init_console 

============================================================================*/
void init_console (void)
  {
  cfh = consolefilehandler_new();
  Device *console = device_new ("console", 5, 1, S_IFCHR, 
    (DevOpenAsFileFn)consolefilehandler_get_fileops, NULL, 
      (DevCloseAndCleanupFn)consolefilehandler_close_and_cleanup, cfh);
  devmgr_add_device (console);
  }

/*============================================================================

 init_block_devs

============================================================================*/
void init_block_devs (void)
  {
  devmgr_scan_storage ();
  }


/*============================================================================

  main

============================================================================*/
int main (int argc, char **argv)
  {
  global_boot_time = mytime (NULL);

  kernelringbuffer_init();
  kernelringbuffer_log (INFO, "Starting");

  console_clear();

  devmgr_init();
  init_console();
  init_block_devs();
  procmgr_init();
  mounts_init();

  myprintf ("*** BearOS/X64 ***\n");

  Process *process = procmgr_create_root_process (shell_run);

  kernelringbuffer_log (INFO, "Kernel init complete: starting root process");
  procmgr_run_process (process, 0, NULL);

  // We won't normally get here

  procmgr_process_finished (process);

  mounts_cleanup();
  procmgr_cleanup();
  
  cleanup_console();
  devmgr_cleanup();

  kernelringbuffer_cleanup();
  }


