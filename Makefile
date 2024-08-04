
include boot/Makefile
include kernel/Makefile

SHELL_OFFSET = 201 

# Disk binary
disk.bin:
	dd if=/dev/zero of=disk.bin count=10000

# Build the os image
os-image: boot.bin kernel.bin disk.bin
	cat $^ > os-image

shell: os-image
	$(MAKE) -C user
	dd if=user/shell.bin of=os-image bs=512 seek=$(SHELL_OFFSET) conv=notrunc


all: os-image shell

run: all
	qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image,format=raw

clean:
	$(MAKE) -C user clean
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis os-image
