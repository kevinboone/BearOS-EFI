# Makefile for efitest library
# Copyright (c)2026 Kevin Boone, GPLv3.0

TARGET         = build/bearos_libc.a

LIB_INCS       = include

CFLAGS         = -I $(LIB_INCS) -fpic -fno-builtin-fputs -fno-builtin-printf -Wall

# Work out the objects and sources from the src/ directory, rather than specifying
#  them explicitly in the Makefile
SOURCES        := $(sort $(shell find src/ -type f -name *.c))
OBJECTS        := $(patsubst src/%,build/%,$(SOURCES:.c=.o))

all: $(TARGET) 

build/%.o: src/%.c
	@mkdir -p build/
	gcc $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $< 

$(TARGET): $(OBJECTS) 
	echo $(OBJECTS)
	ar qf $(TARGET) $(OBJECTS)	

clean:
	rm -rf build/

