
BOOT_DIR = $(wildcard *.asm)


all: boot.bin long_mode.bin


# Build the boot binary file
boot.bin: boot.asm ${BOOT_DIR}
	nasm $< -f bin -o $@

long_mode.bin: switch_to_long_mode.asm ${BOOT_DIR}
	nasm $< -f bin -o $@



