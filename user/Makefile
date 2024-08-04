
ILIBS = -I../kernel/include/lib
CLIBS = $(wildcard ../kernel/source/lib/*.c)
LIBS_OBJ = ${CLIBS:.c=.o}

C_SOURCES = $(shell find shell -name '*.c')
HEADERS = $(shell find shell -name '*.h')
C_OBJ = ${C_SOURCES:.c=.o}

ASM_SOURCES = $(shell find shell -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}


GCC_INCLUDE := $(ILIBS) -Iuser/shell


%.o : %.c ${HEADERS}
	gcc -m32 -ffreestanding -c -fno-pie ${GCC_INCLUDE} $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

# Build the kernel binary
shell.bin: shell_entry.o ${C_OBJ} ${ASM_OBJ} ${LIBS_OBJ}
	ld -m elf_i386 -o $@ -Ttext 0 $^ --oformat binary

all: shell.bin

clean:
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis
