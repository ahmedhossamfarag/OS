C_SOURCES = $(wildcard *.c source/base/*.c source/drivers/*.c source/process/*.c source/lib/*.c source/syscall/*.c source/filesystem/*.c)
HEADERS = $(shell find . -name '*.h')
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(shell find . -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

GCC_INCLUDE := -Iinclude/base -Iinclude/drivers -Iinclude/process -Iinclude/lib -Iinclude/syscall


%.o : %.c ${HEADERS}
	i686-elf-gcc $(GCC_INCLUDE) -c $< -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

%.o : %.asm
	i686-elf-as $< -o $@

all: kernel.o

# Build the kernel binary
kernel.o: kernel_entry.o ${C_OBJ} ${ASM_OBJ}
	i686-elf-gcc -T link.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc


clean:
	find . -name '*.o' -type f -delete

