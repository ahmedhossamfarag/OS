
BOOT_DIR = $(wildcard *.asm)


%.o : %.asm
	nasm $< -f elf -o $@

all: boot.bin


# Build the boot binary file
boot.bin: boot.asm ${BOOT_DIR}
	nasm $< -f bin -o $@

