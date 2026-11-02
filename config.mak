# If you want to build for an architecture other than x86_64, be aware that `ARCH` below
#   isn't the only change you'll have to make.
# 1. You'll need to change the compiler CFLAGS in all Makefiles if you want to build
#   for a different bit widht (e.g., -m32)
# 2. You'll need a libefi and libgnuefi appropriate for the architecture
# 3. You'll have to change the boot filename from BOOTX64.EFI to whatever is appropriate
# 4. You'll need a version of `objcopy` that can output EFI for the target architecture.
# In fact, if you want to build a 32-bit version, it's probably easier to use a 32-bit
#   Linux build host (if you can find one).
ARCH=x86_64
#ARCH=ia32

