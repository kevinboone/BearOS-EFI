# Makefile for Bearos/EFI 
# Copyright (c)2026 Kevin Boone, GPLv3.0

all: bearos.iso 

kernel/build/bearos.efi:
	make -C kernel

start_:
	make -C start 

libc_:
	make -C lib

libbearos_:
	make -C libbearos

programs_: start_ libc_ libbearos_
	make -C programs 

img/bearos.img: libc_ start_ kernel/build/bearos.efi programs_ rootfs_ 
	make -C programs install
	mkdir -p img
	rm -f img/bearos.img
	dd if=/dev/zero of=img/bearos.img bs=1M count=4
	mformat -i img/bearos.img -v EFIBOOT
	mmd -i img/bearos.img ::/EFI ::/EFI/BOOT
	mcopy -i img/bearos.img kernel/build/bearos.efi ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i img/bearos.img files/* ::/

rootfs_:
	mkdir -p files/
	cp -vax rootfs/* files

bearos.iso: img/bearos.img
	mkdir -p dummy/
	cp img/bearos.img dummy/
	xorriso -as mkisofs -V hello -o bearos.iso -eltorito-platform efi -e bearos.img -no-emul-boot dummy/

clean:
	make -C kernel clean
	make -C start clean
	make -C programs clean
	make -C lib clean
	make -C libbearos clean
	rm -rf bearos.iso dummy/ files/ img/

