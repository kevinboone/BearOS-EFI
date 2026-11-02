# BearOS known bugs

## Bugs affecting building applications

### STATIC VARIABLES DON'T WORK!

Programs built or BearOS currently can't reliably use static variables whose initial
values are not zero. This, I think, is related to the way in which segments
are grouped when linking. It's not a problem for new code, but could be
nasty when porting existing code. 

## Bugs in the kernel

### The FAT filesystem support fails on certain FAT volumes created using Linux

ChaN's FF driver assumes that the type of FAT (12/16/32) can be determined from
the number of clusters. But, on Linux, `mkfs.vfat` will create FAT32 with only
a small number of clusters. The FF driver incorrectly detected these as FAT16,
and fails because they don't have a fixed number of root directory entries.  I
have inserted a nasty kludge that kind-of works around this limitation, but I'm
not sure how robust it is. Ideally, we should force the right type of FAT
filesystem for the size.

## The FAT filesystem support fails on read-only hardware

This is a problem when the root filesystem is on a truly read-only
device, like a DVD or DVD image. ChaN's FF code allows the low-level block
driver to return an error code WRPRT to indicate a write-protected device
but, so far as I can tell, this code isn't actually detected anywhere.
Writing on a read-only device leads to `f_write()`, etc., returning
a general "I/O error" code, and a _broken filesystem_. We need to think
about checking for read-only devices, and setting some kind of flag on
the filesystem handler. 

