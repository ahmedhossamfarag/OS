# ROOT_DIR = 201

# run: all
# 	qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image,format=raw

# # Boot binary
# boot/boot:
# 	$(MAKE) -C boot


# # Kenel binary	
# kernel/kernel:
# 	$(MAKE) -C kernel

# # Disk binary
# disk.bin:
# 	dd if=/dev/zero of=disk.bin count=10000


# # Build the os image
# os-image: boot/boot kernel/kernel disk.bin
# 	cat boot/boot.bin kernel/kernel.bin disk.bin > os-image

# # Attach file system
# filesystem: os-image
# 	$(MAKE) -C user
# 	dd if=user/filesystem of=os-image bs=512 seek=${ROOT_DIR} conv=notrunc


# all: os-image #filesystem


# clean:
# 	$(MAKE) -C user clean
# 	find . -name '*.o' -type f -delete
# 	find . -name '*.so' -type f -delete
# 	find . -name '*.bin' -type f -delete
# 	rm -fr *.bin *.dis os-image

OS_NAME := os-image
ISO := $(OS_NAME).iso
VM_NAME := $(OS_NAME)_VM

run: all
	# qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image -bios /usr/share/ovmf/OVMF.fd
	# Check if the VM exists, if not create it
	VBoxManage showvminfo $(VM_NAME) || VBoxManage createvm --name $(VM_NAME) --ostype Other --register
	# Modify VM settings
	VBoxManage modifyvm $(VM_NAME) --memory 2024 --boot1 dvd --cpus 4
	# Attach the ISO to the VM
	VBoxManage storagectl $(VM_NAME) --name "IDE Controller" --add ide
	VBoxManage storageattach $(VM_NAME) --storagectl "IDE Controller" --port 0 --device 0 --type dvddrive --medium $(ISO)
	# Start the VM
	VBoxManage startvm $(VM_NAME)

kernel/kernel:
	$(MAKE) -C kernel

kernel-image: kernel/kernel
	cp kernel/kernel.o iso/boot/$@

os-image: kernel-image
	rm -fr $@
	grub-mkrescue -o $@.iso iso

all: os-image

clean:
	$(MAKE) -C user clean
	find . -name '*.o' -type f -delete
	find . -name '*.so' -type f -delete
	find . -name '*.bin' -type f -delete
	rm -fr *.bin *.dis os-image iso/boot/kernel-image