C_SOURCES = $(wildcard kernel/*.c kernel/source/base/*.c kernel/source/drivers/*.c kernel/source/lib/*.c)
HEADERS = $(wildcard kernel/include/base/*.h kernel/include/drivers/*.h kernel/include/lib/*.h)
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(wildcard kernel/source/base/*.asm kernel/source/drivers/*.asm kernel/source/lib/*.asm)
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

GCC_INCLUDE := -Ikernel/include/base -Ikernel/include/drivers -Ikernel/include/lib

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

# Build the os image
os-image: boot.bin kernel.bin
	cat $^ > os-image


all: os-image

run: all
	qemu-system-x86_64 -drive file=os-image,format=raw

clean:
	rm -fr *.bin *.o kernel/*.o kernel/include/*.o *.dis os-image
