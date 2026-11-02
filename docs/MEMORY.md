## BearOS memory management

BearOS is an EFI application -- a giant bootloader, if you will -- and its
entire access to hardware (at present) is through EFI. This includes memory.
In the BearOS kernel, the equivalent of the conventional `malloc()/free()`
calls are `mymalloc()`, and `myfree()`, which are simply wrappers around the
EFI `PoolAllocate()` and `PoolFree()` functions. 

Application programs, including the built-in utilities, use `malloc()` and
`free()` from the BearOS standard C library. These are implemented as syscalls
to the BearOS kernel, which does the same EFI stuff as the kernel itself. 

Because BearOS has no memory management or memory protection, there's nothing
to stop an application reading or writing any byte in memory it chooses, over
the whole address range. However, an application writing over the kernel or
shell is unlikely to end well. 

A Linux program can act as if it has the whole address space to itself, because
the memory management infrastructure creates the illusion that it does. There
is no equivalent in BearOS of the Linux/Unix `sbrk()` function -- applications
_must_ manage memory in blocks obtained from the standard C library.

The EFI memory management functions are limited: there's not way, for example,
to re-size an allocated memory block. The BearOS standard library implements
`realloc()` by creating a new memory block on the necessary size, copying the
old memory block to the new one, and then freeing the old one. This is
transparent to the application, but it's very inefficient to expand a memory
block byte-by-byte.



