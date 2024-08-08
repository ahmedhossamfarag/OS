ROOT_DIR = 201

run: all
	qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image,format=raw

# Boot binary
boot/boot:
	$(MAKE) -C boot


# Kenel binary	
kernel/kernel:
	$(MAKE) -C kernel

# Disk binary
disk.bin:
	dd if=/dev/zero of=disk.bin count=10000


# Build the os image
os-image: boot/boot kernel/kernel disk.bin
	cat boot/boot.bin kernel/kernel.bin disk.bin > os-image

# Attach file system
filesystem: os-image
	$(MAKE) -C user
	dd if=user/filesystem of=os-image bs=512 seek=${ROOT_DIR} conv=notrunc


all: os-image #filesystem


clean:
	$(MAKE) -C user clean
	find . -name '*.o' -type f -delete
	find . -name '*.so' -type f -delete
	find . -name '*.bin' -type f -delete
	rm -fr *.bin *.dis os-image
