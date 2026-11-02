# Syscalls on Bearos/EFI

BearOS is a single-tasking, single-user platform. All code -- kernel and
application -- runs at EFI's `TPL_APPLICATION` privilege level. This means that
all code has complete, unrestricted access to hardware, and can only be
interrupted by a driver's hardware interrupt. 

Since there is no distinction in privilege between kernel code and application
code, BearOS applications do not need to use a specific kernel trap to
enter the kernel. Naturally, this is very bad for security -- there's nothing
to stop applications interfering with one another, writing arbitrary data
to storage, or just writing over the kernel's memory. However, working
this way is simple, and fast.

Therefore, to call into the kernel, BearOS applications just make an ordinary
subroutine call, and the kernel code executes a `ret` to return to the
application. To avoid the kernel needing to expose hundreds of functions to the
application, it exposes its functionality to the application using a syscall
mechanism. In practice, therefore, the BearOS interface to the kernel looks
rather like that of Linux: the application selects the kernel function it wants
as a numeric value, and calls the syscall passing that number, and any
arguments the function takes. 

The application knows where the kernel's syscall handler is in memory because
the kernel passes to the application a pointer to a `BearosContext` structure.
The first memory of this structure is the address of the syscall function.

The syscall mechanism is completely transparent to the application developer,
as it is on Linux, because the actual syscalls are wrapped up in the functions
of the standard C library.

I can't stress to strongly that BearOS has the same security model as
MS/DOS -- that is, none whatsoever. 

