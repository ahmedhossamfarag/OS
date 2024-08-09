
BOOT_DIR = $(wildcard *.asm)


all: boot.bin


# Build the boot binary file
boot.bin: boot.asm ${BOOT_DIR}
	nasm $< -f bin -o $@

