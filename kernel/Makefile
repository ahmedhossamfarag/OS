C_SOURCES = $(wildcard kernel/*.c kernel/source/base/*.c kernel/source/drivers/*.c kernel/source/process/*.c kernel/source/lib/*.c kernel/source/syscall/*.c kernel/source/filesystem/*.c)
HEADERS = $(shell find kernel -name '*.h')
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(shell find kernel -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

GCC_INCLUDE := -Ikernel/include/base -Ikernel/include/drivers -Ikernel/include/process -Ikernel/include/lib -Ikernel/include/syscall


%.o : %.c ${HEADERS}
	gcc -m32 -ffreestanding -c -fno-pie ${GCC_INCLUDE} $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

# Build the kernel binary
kernel.bin: kernel/kernel_entry.o ${C_OBJ} ${ASM_OBJ}
	ld -m elf_i386 -o $@ -T link.ld $^ --oformat binary

all: kernel.bin
