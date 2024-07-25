C_SOURCES = $(wildcard kernel/*.c kernel/source/base/*.c kernel/source/drivers/*.c kernel/source/process/*.c kernel/source/lib/*.c kernel/source/syscall/*.c kernel/source/filesystem/*.c)
HEADERS = $(wildcard kernel/include/base/*.h kernel/include/drivers/*.h kernel/include/process/*.h kernel/include/lib/*.h kernel/include/syscall/*.h)
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(wildcard kernel/source/base/*.asm kernel/source/drivers/*.asm kernel/source/process/*.asm kernel/source/lib/*.asm kernel/source/syscall/*.asm)
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
	qemu-system-x86_64 -smp 4 -m 2048 -drive file=os-image,format=raw

clean:
	rm -fr *.bin *.o kernel/*.o kernel/source/base/*.o kernel/source/drivers/*.o kernel/source/process/*.o kernel/source/lib/*.o kernel/source/syscall/*.o kernel/source/filesystem/*.o *.dis os-image
