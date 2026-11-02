TARGET   = testprog

CFLAGS   = -nostdlib -nostartfiles -ffreestanding -fPIC 
LIB_INCS = ../lib/include 

build/start.o: src/start.c
	mkdir -p build/
	gcc -I ../include -I $(LIB_INCS) $(CFLAGS) -o build/start.o -c src/start.c

clean:
	rm -rf build 
