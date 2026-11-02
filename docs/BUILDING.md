# Building BearOS/EFI

## Prerequisites

BearOS/EFI is designed to be compiled on a 64-bit Linux system using `gcc`
and associated tooling.

In addition to the `gcc`, `make`, etc., you'll need a recent `objcopy`,
`gnu-efi` development materials (`apt install gnu-efi`, or build it from
source), and `xorriso` for making the DVD image. 

## Building a bootable disk image

`make` should result in the generation of `bearos.iso`, which can be burned
to a DVD or flashed to a USB stick. The build process compiles the kernel,
standard C library, and utilities, and assembles the EFI system partition and
root file system. It then invokes `xorriso` to make an El-Torrito bootable
DVD/USB image.

## Building the Linux test version

There is a specific Makefile for this, so just

`make -f Makefile.linux`.

Note that only the kernel is different for the EFI and Linux versions -- the
standard library and utilities are not platform-dependent. 

The Linux build results in an executable `bearos`, and a disk image `linuxdisk.img`.

## Build order

If you want to build parts of the installation, please bear in mind the
dependencies.

The kernel needs to be built first (`make -C kernel`), then the standard C
library (`make -C lib`), then the application start-up code (`make -C start`).
 All the utility programs depend on the BearOS runtime
library (`make -C libbearos`), so the utilities should be built last (`make -C
programs`).

To work on the utilities separately from the rest of the system, you can `make
-C programs` and then `make -f Makefile.linux installimage` for the Linux test
version, or `make bearos.iso` for the bootable EFI version. 

Each of the utilities has its own `Makefile` but, after a successful compile,
you'll still need to build the bootable image or Linux disk image.

