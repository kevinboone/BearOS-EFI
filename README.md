# BearOS/EFI

Version 0.1a, November 2026

*NOTE...*

This is very much a work in progress. Collaboration is welcome. As a result of
various conversations I have had with other retro-computing enthusiasts, I am
publishing this before it is remotely complete. However, I don't expect it
ever to be more than a curiosity. 

## What is this?

BearOS ("Basic Embedded All-purpose Retro Operating System") is a
proof-of-concept, single-tasking, single-user operating system. It embodies
the operating system design philosophy of 1980s microcomputers, but extended
to modern hardware. This version, in particular, runs on 64-bit x86
computers with EFI firmware (which, in practice, means nearly all modern
PCs and laptops). EFI is already quite powerful, and has built-in support
for many kinds of hardware that applications will use, including
storage devices. Using EFI means that the operating system doesn't
have to re-invent all the hardware support. 

For this reason, and others, BearOS is hugely simpler than any modern operating
system and, in some respects, faster. The kernel is much less than a
megabyte in size.  It boots in a fraction of a second and, because all storage
operations are synchronous, it can safely be shut down by switching off the
power. It provides no security or process segregation of any kind. While this
is a serious disadvantage in many scenarios, it avoids all the overheads of
security context switching that create bottlenecks in modern systems. 

BearOS/EFI doesn't have, or need, a separate bootloader. In fact, 
the entire installation can be thought of as an extended EFI bootloader. 

BearOS loads and runs programs in Linux ELF format, but with a specific
code layout. This allows BearOS programs to be compiled using the 
same tools (`gcc`, etc) as Linux. 

As a proof-of-concept BearOS is, of course, very limited. It provides a 
console environment in which you can run a Unix-like command line, and 
a few basic utilities. A design goal is for the C programming API to be
sufficiently Unix-like that it is relatively straightforward to port
Linux (console) application to BearOS. To that end, BearOS has its
own standard C library, developed alongside the kernel. Its root filesystem
layout is broadly Linux-like.

The similarities with Linux are for familiarity and ease of development --
BearOS internally has _nothing whatever_ in common with Linux.

## Linux test harness

BearOS can be built as a Linux application for testing purposes --
see `docs/LINUX_TEST.md` for more information about this. The Linux
test version uses a disk image as the root filesystem, rather than
the EFI block device driver. In other respects, it tries to exercise
the same code as the EFI version. This can be quite useful, for 
finding memory leaks and dangling pointers.

## Stuff that works

- BearOS boots from a USB stick, or a DVD, to a prompt. BearOS/EFI can be
  installed on a hard disk but not, as yet, using its own tools.

- On boot, BearOS enumerates block storage devices and partitions 
  exposed by EFI, and
  creates block devices for them in the `/dev` directory. These can be
  mounted, as in Linux, using the `mount` command.

## Stuff that doesn't work

Please see BUGS.txt

## Major limitations

- The only filesystem type BearOS supports at present is VFAT (with long filenames).
  VFAT has no notion of file ownership or attributes which, in a way, makes it
  a good match for BearOS, which has no notion of user identify. However, it's
  a poor filesystem in many ways.

- BearOS only supports the hardware that EFI supports. It doesn't have a device
  driver infrastructure of its own. While EFI's hardware support is quite
  extensive it doesn't extend, for example, to audio devices. 

## Stuff that I haven't even thought about yet

- Graphical user interface
- Networking
- Audio (probably will never be supported)


