# Application exit and interrupt handling

BearOS runs an application using a simple subroutine call to the loaded code in
memory. The program runs until it issues a return-from-subroutine instruction
to return to the kernel (see `docs/PROGRAM_LOADING.md`).


This leaves the problem of handling a simple `exit()` call in the standard
library and, even more so, handling an interrupt that terminates the
program.

The way BearOS currently handles these things is to set a jump point in the
application start-up code (`start/src/start.c`), just before this code calls
the application's `main()`. It uses GCC's `__builtin_setjmp()` to do this,
because the BearOS standard library does not yet have its own `setjmp()`
implementation.

A call to `exit()` results in a `longjmp()` to the jump point, which unwinds
the stack and returns to the kernel.

This is a rather ugly mechanism, but one that suits the single-tasking nature
of BearOS.
