# TO-DO list (minor-ish points only)

- The logic of finding the fsops structure from a pathname is very ugly, and
  not robust. See comments in `kernel/src/mounts.c`

- Implement return of major/minor device numbers in stat(), and use these
  when searching for a device handler when given a `/dev/xxx` filename. 
  The current implementation assumes that all device handlers have 
  files of the same name in `/dev`.

- In the short term, we need a way to manage the keyboard layout. BearOS
  currently only supports US English keyboard layouts. I don't know if
  EFI itself provides anything to help with this, or whether we need
  to do the mapping programmatically.

- Prevent volume being unmounted by `umount` when files are still open

- There should be a command to clear the console screen, short of full
  ANSI terminal support. There is already a syscall for this.

- Use `-gc-sections`, etc., to ensure there is no unused code in the kernel
  (utilities already have this)

- `ls` command needs to be expanded

- Detect ctrl-c in keyboard input and output, and use it to exit a running
  program

- At some point, we'll need proper ANSI terminal support in the console,
  for ease of porting Linux code. 

- There is a fixed, arbitrary buffer size in myprintf() (`compat_stdio.c`) and
  maybe elsewhere. This needs to be replaced by code that can handle
  aribtrary-length output.

- The `bosh` shell is very rudimentary, and needs to be expanded 
