## BearOS program loading

At present, BearOS can run programs in ELF format, built for position-independent
code and a flat memory model.

When loading a program, BearOS loads the entire ELF file into memory, as a
single operation. There is no concept of virtual memory or memory pages. BearOS
allocates a block of memory from EFI that is large enough to accommodate the
whole ELF file. Naturally, applications will use less physical RAM (which is
all BearOS understands) if you strip debug information and comments from the
binary at build time.

The application build process is expected to write the program's entry point
address into the ELF header _as an offset from the start of the code_.  Again,
there's no virtual memory, so the BearOS loader expects to be able to work out
where to start the program as a physical address in RAM. See the utilities in
the `programs/` directory for guidance on how to build a BearOS-compatible
binary on Linux.

BearOS runs the program by making a subroutine call to its entry point, 
and the application is expected to execute a `ret` at some point, to return
to the kernel. In practice, you'll need to put the BearOS start-up code
(`start/build/start.o`) at the start of the binary, and have this
as the entry point.

The BearOS kernel passes four arguments to the subroutine at the program's
entry point, using the standard X86 ABI. These are, in order:

- A pointer to a `BearosContext` object, which the application should store,
  for future use.
- The standard `argc` and `argv`, comprising the application's arguments, and
- An environment pointer, which may be NULL. 

The arguments are stored by the start-up code, and made available to the
BearOS standard C library, in a way that should be transparent to the
application developer. 

