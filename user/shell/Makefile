IUSER_LIBS = -I../lib/include
CUSER_LIBS = $(wildcard ../lib/*.c++)
USER_LIBS_OBJ = ${CUSER_LIBS:.c++=.o}

C_SOURCES = $(shell find . -name '*.c++')
HEADERS = $(shell find . -name '*.h')
C_OBJ = ${C_SOURCES:.c++=.o}

ASM_SOURCES = $(shell find . -name '*.asm')
ASM_OBJ = ${ASM_SOURCES:.asm=.o}


GCC_INCLUDE := ${IUSER_LIBS} -Iinclude

%.o : %.c++ ${HEADERS}
	g++ -m32 -nostdlib -nodefaultlibs -c ${GCC_INCLUDE} $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

all: shell.o


shell.o: start.o ${C_OBJ} ${ASM_OBJ} ${USER_LIBS_OBJ}
	g++ -m32 -nostdlib -nodefaultlibs -nostartfiles -o $@ $^ -z noexecstack
