# BearOS start code

This directory contains the source for the BearOS library start-up code. This
can't be included in the main (libc) library, because the `.o` file must
specifically be linked as the first object file in the executable.

