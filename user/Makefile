IUSER_LIBS = -Ilib/include
CUSER_LIBS = $(wildcard lib/*.c++)
USER_LIBS_OBJ = ${CUSER_LIBS:.c++=.o}

C_SOURCES = $(shell find shell -name '*.c++')
HEADERS = $(shell find . -name '*.h')
C_OBJ = ${C_SOURCES:.c++=.o}

ASM_SOURCES = $(shell find shell -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}


GCC_INCLUDE := ${IUSER_LIBS} -Ishell/include


%.o : %.c ${HEADERS}
	gcc -m32 -c ${GCC_INCLUDE} $< -o $@

%.o : %.c++ ${HEADERS}
	g++ -m32 -c ${GCC_INCLUDE} $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

# Build the kernel binary
shell.bin: shell_entry.o ${C_OBJ} ${ASM_OBJ} ${KERNEL_LIBS_OBJ} ${USER_LIBS_OBJ}
	g++ -m32 -o $@ $^ -z noexecstack

all: shell.bin

clean:
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis
