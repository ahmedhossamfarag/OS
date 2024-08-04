
include boot/Makefile
include kernel/Makefile

# Disk binary
disk.bin:
	dd if=/dev/zero of=disk.bin count=10000

# Build the os image
os-image: boot.bin kernel.bin disk.bin
	cat $^ > os-image


all: os-image

run: all
	qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image,format=raw

clean:
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis os-image
