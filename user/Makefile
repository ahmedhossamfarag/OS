UEFI_ROOTLBA = 0x20000
UEFI_DATALBA = 0x20002

BIOS_ROOTLBA = 201
BIOS_DATALBA = 203



all: filesystem


fsmap : fsmap.asm
	nasm -DRLBA=$(UEFI_ROOTLBA) -DDLBA=$(UEFI_DATALBA) $< -f bin -o $@.bin

bios_fsmap : fsmap.asm
	nasm -DRLBA=$(BIOS_ROOTLBA) -DDLBA=$(BIOS_DATALBA) $< -f bin -o $@.bin



# shell
shell/shell_o:
	$(MAKE) -C shell


# filesystem
filesystem: fsmap shell/shell_o
	dd if=fsmap.bin of=filesystem bs=512 seek=0 conv=notrunc
	dd if=shell/shell.o of=filesystem bs=512 seek=2 conv=notrunc

bios_filesystem: bios_fsmap shell/shell_o
	dd if=bios_fsmap.bin of=filesystem bs=512 seek=0 conv=notrunc
	dd if=shell/shell.o of=filesystem bs=512 seek=2 conv=notrunc


clean:
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis filesystem