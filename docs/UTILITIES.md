# BearOS utilities

BearOS has a bunch of utilities in the `bin/` directory of the root
filesystem. These have the same names as popular Linux utilities -- `cat`, 
`rm`, `cp`, etc. -- and behave in broadly the same way. However,
the BearOS versions are much more limited that the Linux utilities of
the same name. As the BearOS standard C library expands, I hope that it
will become possible simply to build the Linux versions for BearOS, which
is why I don't want to expend too much effort on the built-in utilities.

BearOS has a couple of utilities of its own.

### setcon

This utilities lists (`-l`) and sets (`-s`) the console mode, that is,
the number of rows and columns. Computers vary in how they respond to
changing the mode. Some respond by resizing the graphical display, 
changing the type size appropriately. This is the ideal behaviour.
Some, however, just change the size of a screen window in which
text appears -- The VirtualBox EFI support does this. While this might
be useful for testing, it doesn't make the text any easier or harder
to read.

There's not much that can be done about this, until BearOS gets its
own graphical console.

