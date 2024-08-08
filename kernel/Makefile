C_SOURCES = $(wildcard *.c source/base/*.c source/drivers/*.c source/process/*.c source/lib/*.c source/syscall/*.c source/filesystem/*.c)
HEADERS = $(shell find . -name '*.h')
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(shell find . -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

GCC_INCLUDE := -Iinclude/base -Iinclude/drivers -Iinclude/process -Iinclude/lib -Iinclude/syscall


%.o : %.c ${HEADERS}
	gcc -m32 -ffreestanding -c -fno-pie ${GCC_INCLUDE} $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

all: kernel.bin

# Build the kernel binary
kernel.bin: kernel_entry.o ${C_OBJ} ${ASM_OBJ}
	ld -m elf_i386 -o $@ -T link.ld $^ --oformat binary

