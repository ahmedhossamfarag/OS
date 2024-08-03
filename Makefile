C_SOURCES = $(shell find kernel -name '*.c')
HEADERS = $(shell find kernel -name '*.h')
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(shell find kernel -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

GCC_INCLUDE := -Ikernel/include/base -Ikernel/include/drivers -Ikernel/include/process -Ikernel/include/lib -Ikernel/include/syscall

BOOT_DIR = $(wildcard boot/*.asm)


%.o : %.c ${HEADERS}
	gcc -m32 -ffreestanding -c -fno-pie ${GCC_INCLUDE} $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

# Build the boot binary file
boot.bin: boot/boot.asm ${BOOT_DIR}
	nasm $< -f bin -o $@

# Build the kernel binary
kernel.bin: kernel/kernel_entry.o ${C_OBJ} ${ASM_OBJ}
	ld -m elf_i386 -o $@ -T link.ld $^ --oformat binary

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
