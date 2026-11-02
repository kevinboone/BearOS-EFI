# Testing BearOS on Linux

See `docs/BUILDING.md` for information on building BearOS for Linux.

The BearOS kernel can run as a Linux application for testing purposes.
It can even load and run BearOS applications, despite that their
memory layout is completely inappropriate for Linux.

_BUT_ it can only run under the control of `valgrind`:

`valgrind kernel/bearos`

I presume that this works because `valgrind` does its own memory protection, 
which is more conducive to the way BearOS works than raw Linux is.
Attempting to run `bearos` without `valgrind` will just result in a 
segfault as soon as it tries to execute the shell.

It's important to understand that BearOS has no support for, or understanding
of, virtual memory. The memory layout of a BearOS application is inimical to
Linux. Why `valgrind` allows it to work, I don't know -- but it's good that it
does.

`valgrind` will detect memory faults and memory leaks in the BearOS kernel, and
will show line numbers if the kernel is build with GCC's `-g` switch. It will
also detect memory faults and leaks in BearOS applications, but it won't be
able to report line numbers, however these are built.



