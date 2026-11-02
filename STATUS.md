# BearOS/EFI project status

BearOS boots to a command line from a USB stick on real hardware, or a DVD
image in a VirtualBox VM, or as a Linux application for testing purposes.

The basic root filesystem forms part of the boot image. It's possible to mount
other disk partitions on arbitrary directories, so long as they are in VFAT
format -- presently BearOS supports no other filesystem.  The mount/umount
logic is broadly completed, but it is presently possible to unmount a
filesystem which has open files on it, which is catastrophic. 

Block and character devices are exposed in `/dev/`, as they are on Linux.
However, BearOS has as yet no way to add or remove devices at runtime -- all
the hardware scanning happens at boot time.

There is a rudimentary `/proc` filesystem; for example, you can see mounted
partitions using `cat /proc/mounts`. 

There is a growing C standard library, which is broadly POSIX-compliant, within
the limitations of BearOS itself. Because BearOS is single-tasking, there is
not, and never will be, equivalents of the POSIX `fork()/exec()` calls.
However, there is a BearOS-specific alternative that applications can use to
run other applications. 

The BearOS "BOSH" shell is very rudimentary, but can load and run applications
and a few built-in commands (like `cd`). It can run a script line-by-line.
There is no support for redirection of `stdout`, etc., although the
infrastructure to do this is partially implemented in the kernel. 

The biggest problem for BearOS is support for hardware -- as it is, I imagine,
for every new operating system. At present, I've been working on the assumption
that BearOS will use only hardware managed by EFI. Most (all?) EFI
implementations have drivers for screen and keyboard, pointer devices, and most
common block-storage devices including removable ones. Some have support for
TCP/IP networking, but probably not over WiFi.  So far as I know, no EFI
implementation has support for audio.

To go beyond EFI's hardware support means creating an entire device-driver
infrastructure for BearOS, including control of interrupts and ports.  This is
going to be a colossal amount of work, and I'm not convinced at this point that
I have the enthusiasm for it.  
 



