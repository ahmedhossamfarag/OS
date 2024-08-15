## UEFI run
run: kernel_o
	$(MAKE) -f Makefile.img

kernel_o:
	$(MAKE) -C kernel

## BIOS run
bios_run: os-image filesystem
	qemu-system-x86_64 -monitor stdio -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image,format=raw

boot_bin:
	$(MAKE) -C bios.boot

kernel_bin:
	$(MAKE) -C kernel kernel_bin

disk_bin:
	dd if=/dev/zero of=disk.bin count=10000

os-image: boot_bin kernel_bin disk_bin
	cat bios.boot/boot.bin kernel/kernel.bin disk.bin > os-image

ROOT_DIR = 201

filesystem: os-image
	$(MAKE) -C user
	dd if=user/filesystem of=os-image bs=512 seek=${ROOT_DIR} conv=notrunc


## clean
clean:
	$(MAKE) -C user clean
	$(MAKE) -f Makefile.img clean
	find . -name '*.o' -type f -delete
	find . -name '*.dis' -type f -delete
	find . -name '*.bin' -type f -delete
	rm -fr os-image