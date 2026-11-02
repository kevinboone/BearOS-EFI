START     = ../../start/build/start.o
LIBC      = ../../lib/build/bearos_libc.a
LIBCINCS  = ../../lib/include
LIBBEAROS = ../../libbearos/build/libbearos.a
LIBBEAROSINCS = ../../libbearos/include
CFLAGS    = -nostdinc -nostdlib -nostartfiles -ffreestanding -fPIC -Wall -ffunction-sections -fdata-sections
LDFLAGS   = --no-dynamic-linker -T ../bearos.lds -pie --gc-sections

# Work out the objects and sources from the src/ directory, rather than specifying
#  them explicitly in the Makefile
SOURCES        := $(sort $(shell find src/ -type f -name *.c))
OBJECTS        := $(patsubst src/%,build/%,$(SOURCES:.c=.o))

all: $(TARGET)

build/%.o: src/%.c
	@mkdir -p build/
	gcc -I $(LIBCINCS) -I $(LIBBEAROSINCS) $(CFLAGS) -DVERSION=\"${VERSION}\" -MD -MF $(@:.o=.deps) -c -o $@ $< 

$(TARGET): $(TARGET).elf
	objcopy --remove-section=.comment --remove-section=.note \
          --remove-section=.debug* $(TARGET).elf $(TARGET) 
	strip $(TARGET) 

$(TARGET).elf: $(START) $(OBJECTS) $(LIB)
	ld $(LDFLAGS) -o $(TARGET).elf $(START) $(OBJECTS) $(LIBC) $(LIBBEAROS)

install:
	cp -p $(TARGET) ../../files/bin/

clean:
	rm -rf $(TARGET) $(TARGET).elf build/ ../../files/bin/$(TARGET)

