# Filesystems

BearOS/EFI currently has three types of filesystems:

- Pseudo-filesystems, that include their own block device (where needed).
  For example, `/dev`, `/proc`
- VFAT filesystems, which may be on a physical drive (`/`) or on a 
  memory block device (`/tmp`)

On an EFI system, BearOS gets the list of VFAT partitions from the EFI firmware, and
creates a device in `/dev` for each one. These devices have names like `disk03part06`.
The boot device always gets an entry `/dev/root`, and gets mounted on `/`. 

The `mount` command can be used, as on Linux, to mount a block device in `/dev`
on a directory. `umount` also works, except that there's no real checking whether
the device is in use.

The `/tmp` filesystem is 4M in size. This is hard coded (in `kernel/src/defaults.h`).
It's just large enough to test that files can be copied on an off, and to provide
a place for temporary files, for applications that really need them.

