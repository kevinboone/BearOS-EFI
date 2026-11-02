/*============================================================================

  devmgr.c

  The device manager -- essentially a List of Device objects. 

  Copyright (c)2026 Kevin Boone, GPLv3.0

============================================================================*/

#ifdef LINUX
#else
#include <efi/efi.h>
#include <efi/efilib.h>
#include "efiutil.h"
#endif

#ifdef LINUX
#include "linuxdiskimagefilehandler.h" 
#else
#include "efidiskimagefilehandler.h" 
#endif

#include "compat_malloc.h"
#include "compat_string.h"
#include "compat_stdio.h"
#include "device.h"
#include "devmgr.h"
#include "../../lib/include/errno_numbers.h"
#include "../../lib/include/stddef.h"
#include "interface_ff.h" 
#include "list.h"
#include "kernelringbuffer.h"
#include "path.h"
#include "procmgr.h"

struct _DevMgr
  {
  List *devices;
  };

DevMgr *devmgr = NULL;

// FWD
static void devmgr_destroy (DevMgr *self);
static DevMgr *devmgr_new (void);
ssize_t write (int fd, void *buff, size_t n);


#ifdef LINUX
LinuxDiskImageFileHandler *ldifh = NULL;
#else
extern EFI_HANDLE efi_boot_dev_handle;
EFIDiskImageFileHandler *edifh = NULL;
#endif

/*============================================================================

  devmgr_add_device

============================================================================*/
void devmgr_add_device (Device *device)
  {
  list_append (devmgr->devices, device);
  }

/*============================================================================

  devmgr_cleanup

============================================================================*/
void devmgr_cleanup (void)
  {
  devmgr_destroy (devmgr);
  }

/*============================================================================

  devmgr_init

============================================================================*/
void devmgr_init (void)
  {
  devmgr = devmgr_new();
  }


/*============================================================================

  devmgr_destroy

============================================================================*/
static void devmgr_destroy (DevMgr *self)
  {
  if (self->devices) 
    {
    list_destroy (self->devices);
    }
  free (self);
  }


/*============================================================================

  devmgr_get_device_by_name

============================================================================*/
Device *devmgr_get_device_by_name (const char *name)
  {
  // Nasty -- we need to be able to open a device by its /dev filename, as
  //   we as its internal name. What we _should_ do is get the major/minor
  //   numbers from the file stat, then search for a device using those.
  // But none of this is implemented yet, so we use this cheap hack, hoping
  //   that all devices are in /dev :/
  if (strncmp (name, "/dev/", 5) == 0)
    name += 5;

  int l = list_length (devmgr->devices); 
  for (int i = 0; i < l; i++)
    {
    Device *device = (Device *)list_get (devmgr->devices, i);
    if (strcmp (name, device_get_name (device)) == 0) return device;
    }
  return NULL;
  }

/*============================================================================

  devmgr_get_device_names

============================================================================*/
void devmgr_get_device_names (List *names)
  {
  int l = list_length (devmgr->devices); 
  for (int i = 0; i < l; i++)
    {
    Device *device = (Device *)list_get (devmgr->devices, i);
    list_append (names, strdup (device_get_name (device))); 
    }
  }


/*============================================================================

  devmgr_open_as_block_by_name

============================================================================*/
struct BlockOps *devmgr_open_as_block_by_name (const char *name)
  {
  Device *device = devmgr_get_device_by_name (name);
  if (device)
    {
    return device_open_as_block (device);
    }
  else
    {
    set_errno (ENOENT);
    return NULL;
    }
  }


/*============================================================================

  devmgr_open_as_file_by_name

============================================================================*/
struct FileOps *devmgr_open_as_file_by_name (const char *name)
  {
  Device *device = devmgr_get_device_by_name (name);
  if (device)
    {
    return device_open_as_file (device);
    }
  else
    {
    set_errno (ENOENT);
    return NULL;
    }
  }


/*============================================================================

  devmgr_new

============================================================================*/
static DevMgr *devmgr_new (void)
  {
  DevMgr *self = malloc (sizeof (DevMgr));
  self->devices = list_create ((ListFreeItemFn)device_destroy);
  return self;
  }


/*============================================================================

  devmgr_scan_storage

  // TODO -- deal with repeated scans

============================================================================*/
void devmgr_scan_storage (void)
  {
#ifdef LINUX
  ldifh = linuxdiskimagefilehandler_new ((UTF8 *)"linuxdisk.img");
  if (linuxdiskimagefilehandler_prepare (ldifh) != 0)
    {
    write (2, "Can't initialize Linux disk image\n", 34);
    return;
    }

  Device *did = device_new ("root", 8, 1, S_IFBLK, 
    (DevOpenAsFileFn)linuxdiskimagefilehandler_get_fileops, 
      (DevOpenAsBlockFn)linuxdiskimagefilehandler_get_blockops, 
        (DevCloseAndCleanupFn)linuxdiskimagefilehandler_close_and_cleanup,
          ldifh);
  devmgr_add_device (did);
#else

  edifh = efidiskimagefilehandler_new (efi_boot_dev_handle);
  if (efidiskimagefilehandler_prepare (edifh) != 0)
    {
    write (2, "Can't initialize EFI disk image\n", 34);
    return;
    }

  Device *did = device_new ("root", 8, 1, S_IFBLK, 
    (DevOpenAsFileFn)efidiskimagefilehandler_get_fileops, 
      (DevOpenAsBlockFn)efidiskimagefilehandler_get_blockops, 
         (DevCloseAndCleanupFn)efidiskimagefilehandler_close_and_cleanup, edifh);
  devmgr_add_device (did);

  List *device_list = list_create (free);
  List *partition_list = list_create (free);

  EFI_GUID bp_guid = EFI_BLOCK_IO_PROTOCOL_GUID;

  EFI_HANDLE *handle_buffer = NULL;
  UINTN handle_count = 0;

  EFI_STATUS err = uefi_call_wrapper (BS->LocateHandleBuffer, 5, ByProtocol, 
     &bp_guid, NULL, &handle_count, &handle_buffer);


  if (err == 0)
    { 
     
    // First we iterate the block devices, looking for those that are _not_ partitions.
    // We'll put them into device_list as raw devices.
    for (int i = 0; i < handle_count; i++)
      {
      EFI_BLOCK_IO  *blk_io;
      EFI_DEVICE_PATH *dp = DevicePathFromHandle (handle_buffer[i]);
      CHAR16 *_dps = DevicePathToStr (dp);
      UTF8 *dps = path_from_efi (_dps);

      uefi_call_wrapper (ST->BootServices->HandleProtocol,
		3, handle_buffer[i] , &bp_guid, &blk_io);

      if (!blk_io->Media->LogicalPartition)
        list_append (device_list, dps); 
      else
        list_append (partition_list, dps); 
      
      free (_dps);
      }

    if (handle_buffer)  FreePool (handle_buffer);
    
    int num_devs = list_length (device_list);
    int num_parts = list_length (partition_list);

    for (int dev = 0; dev < num_devs; dev++)
      {
      const char *root_dev_name = list_get (device_list, dev);
      for (int part = 0; part < num_parts; part++)
        {
        const char *part_dev_name = list_get (partition_list, part);

        if (strstr (part_dev_name, root_dev_name) == part_dev_name)
          {
          char name[100];
          mysnprintf (name, sizeof (name) - 1, "disk%02dpart%02d", dev, part);
          kernelringbuffer_log (INFO, "%s is %s", name, part_dev_name);
          EFI_HANDLE handle = efi_util_get_handle_from_path (part_dev_name);
          if (handle)
            {
            EFIDiskImageFileHandler *edifh = 
                efidiskimagefilehandler_new (handle);
            if (efidiskimagefilehandler_prepare (edifh) == 0)
              {
	      Device *did = device_new (name, 8, part, S_IFBLK, 
		(DevOpenAsFileFn)efidiskimagefilehandler_get_fileops, 
		  (DevOpenAsBlockFn)efidiskimagefilehandler_get_blockops, 
		     (DevCloseAndCleanupFn)
                        efidiskimagefilehandler_close_and_cleanup, edifh);
	      devmgr_add_device (did);
              }
            else
              {
              efidiskimagefilehandler_destroy (edifh);
              kernelringbuffer_log (ERROR,   
                 "Can't initialize block device at %s", part_dev_name);
              }
            }
          else
            {
            // What can we do? Device handle has disappeared
            }
          }
        }
      }
    }

  list_destroy (device_list);
  list_destroy (partition_list);

#endif
  }


