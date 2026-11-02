# Some general notes on BearOS

Here are some miscellaneous notes, in no particular order.

## Kernel ring buffer

BearOS implements a ring buffer for kernel diagnostics with a default
size of 100 messages. Newer messages replace the oldest ones. 
There is no `dmesg` command as Linux has, but you can do

`cat /proc/dmesg`

to see the latest messages. 

## Seeing mounted filesystems

The list of mounts is at `/proc/mounts`, in broadly the same format as
Linux uses. If you run `mount` without arguments, it just dumps this
pseudo-file to the console.

## Case sensitivity

Broadly, BearOS is case-sensitive in the same places that Linux is.

Be aware that filenames are somewhat case-insensitive on FAT filesystems. 
You can create a file with a lower-case name, but an application can
open it using an upper-case or mixed-case equivalent.

This behaviour could be tidied up in the kernel, but Linux handles
FAT filesystem the same way.  

## Time-zone support

There is none at present. You can treat file timestamps as local times or GMT
times as you see fit, but mixing these is likely to be problematic.
Daylight-savings time is basically ignored.  Eventually we will have a
time-zone database. 

## PATH support

When searching for a file to execute, the BearOS shell will look at the `PATH`
environment variable, as Linux shells do. Unlike Linux, the BearOS shell will
run a command in the current directory.

